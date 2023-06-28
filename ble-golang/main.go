package main

import (
	"bufio"
	"context"
	"encoding/binary"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/go-ble/ble"
	"github.com/go-ble/ble/examples/lib/dev"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

type SensorData struct {
	Name string  `json:"sensor"`
	Temp float64 `json:"temperature"`
	Hum  float64 `json:"humidity"`
	Batv float64 `json:"battery"`
}

type SensorProcessor struct {
	processedTimes map[string]time.Time
	mu             sync.Mutex
}

const Sensor = "LYWSD03MMC"
const TelinkVendorPrefix = "a4:c1:38"

var EnvironmentalSensingUUID = ble.UUID16(0x181a)
var XiaomiIncUUID = ble.UUID16(0xfe95)

var timeInSeconds = 5

var processor = SensorProcessor{
	processedTimes: make(map[string]time.Time),
}

// Create a map to store the sensor data
var sensorMap = make(map[string]string)

var mqttClient MQTT.Client

func decodeSign(i uint16) int {
	if i < 32768 {
		return int(i)
	} else {
		return int(i) - 65536
	}
}

func (sp *SensorProcessor) ProcessData(data []byte, frameMac string, rssi int) {
	// Process the string
	if len(data) != 13 {
		return
	}

	mac := fmt.Sprintf("%X", data[0:6])

	if mac != frameMac {
		return
	}

	sp.mu.Lock()
	defer sp.mu.Unlock()

	// Check the last processed time for the given string
	lastProcessedTime, exists := sp.processedTimes[frameMac]
	if exists && time.Since(lastProcessedTime).Seconds() < float64(timeInSeconds) {
		return
	}

	temp := float64(decodeSign(binary.BigEndian.Uint16(data[6:8]))) / 10.0
	hum := float64(data[8])
	batv := float64(binary.BigEndian.Uint16(data[10:12])) / 1000.0

	name, exists := sensorMap[mac]
	if exists {
		sensorData := &SensorData{
			Name: name,
			Temp: temp,
			Hum:  hum,
			Batv: batv,
		}

		writeToMqtt(sensorData)
	} else {
		log.Println("unknown device: ", mac)
	}

	// Update the processed time for the string
	sp.processedTimes[frameMac] = time.Now()
}

func writeToMqtt(data *SensorData) {
	jsonData, err := json.Marshal(data)
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	// Publish a message
	topic := "ATCThermometer"
	payload := string(jsonData)
	qos := 1
	retained := false

	token := mqttClient.Publish(topic, byte(qos), retained, payload)
	token.Wait()

	// Check if there was an error publishing the message
	if token.Error() != nil {
		log.Fatal(token.Error())
	} else {
		fmt.Printf("Message published to topic: %s\n", topic)
	}
}

func advHandler(a ble.Advertisement) {
	mac := strings.ReplaceAll(strings.ToUpper(a.Addr().String()), ":", "")

	for _, sd := range a.ServiceData() {
		if sd.UUID.Equal(EnvironmentalSensingUUID) {
			processor.ProcessData(sd.Data, mac, a.RSSI())
		}
	}
}

func connectToMqtt() {
	// Create a new MQTT client
	opts := MQTT.NewClientOptions()
	opts.AddBroker("tcp://localhost:1883")
	opts.SetClientID("ble-mqtt")

	// Create the MQTT client instance
	mqttClient = MQTT.NewClient(opts)

	// Connect to the MQTT broker
	if token := mqttClient.Connect(); token.Wait() && token.Error() != nil {
		log.Fatal(token.Error())
	}
}

func readConfig() {
	// Open the file
	file, err := os.Open("ble-devices.ini")
	if err != nil {
		fmt.Println("Error:", err)
		return
	}
	defer file.Close()

	// Create a scanner to read the file line by line
	scanner := bufio.NewScanner(file)
	var currentKey string

	for scanner.Scan() {
		line := scanner.Text()

		if strings.HasPrefix(line, "[") && strings.HasSuffix(line, "]") {
			// Extract the MAC address from the line
			mac := strings.TrimPrefix(line, "[")
			mac = strings.TrimSuffix(mac, "]")

			// Remove colons from the MAC address
			mac = strings.ReplaceAll(mac, ":", "")

			// Set the MAC address as the current key
			currentKey = mac
		} else if strings.HasPrefix(line, "sensorname=") {
			// Extract the sensor name from the line
			sensorName := strings.TrimPrefix(line, "sensorname=")

			// Add the sensor name to the map with the current key
			sensorMap[currentKey] = sensorName
		}
	}

	// Check if any errors occurred while scanning the file
	if err := scanner.Err(); err != nil {
		fmt.Println("Error:", err)
		return
	}

	// Print the sensor map
	for mac, sensorName := range sensorMap {
		fmt.Printf("MAC: %s, Sensor Name: %s\n", mac, sensorName)
	}
}

func main() {
	deviceID := flag.Int("i", 0, "use device hci`N`")
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr,
			"Usage: %s [FLAGS...] [MACS TO POLL...]\n", os.Args[0])
		flag.PrintDefaults()
	}
	flag.Parse()

	device, err := dev.NewDevice("default", ble.OptDeviceID(*deviceID))
	if err != nil {
		log.Fatal("oops: ", err)
	}

	readConfig()
	connectToMqtt()

	ble.SetDefaultDevice(device)

	ctx := ble.WithSigHandler(context.Background(), nil)

	telinkVendorFilter := func(a ble.Advertisement) bool {
		return strings.HasPrefix(a.Addr().String(), TelinkVendorPrefix)
	}
	err = ble.Scan(ctx, true, advHandler, telinkVendorFilter)
	if err != nil {
		log.Fatal("oops: %s", err)
	}
}
