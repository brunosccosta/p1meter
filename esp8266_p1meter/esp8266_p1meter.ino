#include <Arduino.h>

#include <Ticker.h>
#include <ArduinoJson.h>

#include <WiFiManager.h>
#include <ESP8266WiFi.h>

#include <WebSocketsServer.h>   //https://github.com/Links2004/arduinoWebSockets/tree/async

#include <MQTTClient.h>

#include "settings.h"

// * Initiate led blinker library
Ticker ticker;

WiFiClient net;

// * Initiate MQTT client
MQTTClient client = MQTTClient(1024);
IPAddress mqttIp(192, 168, 86, 7);

WebSocketsServer webSocket = WebSocketsServer(81);

String telegram = "";         // a string to hold incoming data

// **********************************
// * Ticker (System LED Blinker)    *
// **********************************

// * Blink on-board Led
void tick()
{
    // * Toggle state
    int state = digitalRead(LED_BUILTIN);    // * Get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);       // * Set pin to the opposite state
}

// **********************************
// * MQTT                           *
// **********************************

void send_data_to_broker() {
    Serial.println("Sending data..");

    StaticJsonDocument<350> doc;

    doc["consumption_low_tarif"] = CONSUMPTION_LOW_TARIF;
    doc["consumption_high_tarif"] = CONSUMPTION_HIGH_TARIF;
    doc["returndelivery_low_tarif"] = RETURNDELIVERY_LOW_TARIF;
    doc["returndelivery_high_tarif"] = RETURNDELIVERY_HIGH_TARIF;
    doc["actual_consumption"] = ACTUAL_CONSUMPTION;
    doc["actual_returndelivery"] = ACTUAL_RETURNDELIVERY;

    doc["l1_instant_power_usage"] = L1_INSTANT_POWER_USAGE;
    doc["l2_instant_power_usage"] = L2_INSTANT_POWER_USAGE;
    doc["l3_instant_power_usage"] = L3_INSTANT_POWER_USAGE;
    doc["l1_instant_power_current"] = L1_INSTANT_POWER_CURRENT;
    doc["l2_instant_power_current"] = L2_INSTANT_POWER_CURRENT;
    doc["l3_instant_power_current"] = L3_INSTANT_POWER_CURRENT;
    doc["l1_voltage"] = L1_VOLTAGE;
    doc["l2_voltage"] = L2_VOLTAGE;
    doc["l3_voltage"] = L3_VOLTAGE;

    doc["gas_meter_m3"] = GAS_METER_M3;

    doc["actual_tarif_group"] = ACTUAL_TARIF;
    doc["short_power_outages"] = SHORT_POWER_OUTAGES;
    doc["long_power_outages"] = LONG_POWER_OUTAGES;
    doc["short_power_drops"] = SHORT_POWER_DROPS;
    doc["short_power_peaks"] = SHORT_POWER_PEAKS;

    String buffer;
    serializeJson(doc, buffer);

    client.publish(MQTT_PUBLISH_TOPIC, buffer);
    // String debug = String(result) + " - " + client.returnCode() + " - " + client.lastError() + "\n\n";
    // webSocket.broadcastTXT(debug);
    webSocket.broadcastTXT(buffer);
}

// **********************************
// * P1                             *
// **********************************

float getValue(char startchar, char endchar)
{
    int start = telegram.lastIndexOf(startchar);
    int end = telegram.lastIndexOf(endchar);

    if (start >= 0 && end >= 0) {
        String value = telegram.substring(start + 1, end);

        if (endchar == '*') {
            return (1000 * value.toFloat());
        }
        else if (endchar == ')') {
            return value.toFloat();
        }
    }

    return 0;
}

void decode_telegram()
{
    // 1-0:1.8.1(000992.992*kWh)
    // 1-0:1.8.1 = Elektra verbruik laag tarief (DSMR v4.0)
    if (telegram.startsWith("1-0:1.8.1"))
    {
        CONSUMPTION_LOW_TARIF = getValue('(', '*');
    }

    // 1-0:1.8.2(000560.157*kWh)
    // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v4.0)
    if (telegram.startsWith("1-0:1.8.2"))
    {
        CONSUMPTION_HIGH_TARIF = getValue('(', '*');
    }
	
    // 1-0:2.8.1(000560.157*kWh)
    // 1-0:2.8.1 = Elektra teruglevering laag tarief (DSMR v4.0)
    if (telegram.startsWith("1-0:2.8.1"))
    {
        RETURNDELIVERY_LOW_TARIF = getValue('(', '*');
    }

    // 1-0:2.8.2(000560.157*kWh)
    // 1-0:2.8.2 = Elektra teruglevering hoog tarief (DSMR v4.0)
    if (telegram.startsWith("1-0:2.8.2"))
    {
        RETURNDELIVERY_HIGH_TARIF = getValue('(', '*');
    }

    // 1-0:1.7.0(00.424*kW) Actueel verbruik
    // 1-0:1.7.x = Electricity consumption actual usage (DSMR v4.0)
    if (telegram.startsWith("1-0:1.7.0"))
    {
        ACTUAL_CONSUMPTION = getValue('(', '*');
    }

    // 1-0:2.7.0(00.000*kW) Actuele teruglevering (-P) in 1 Watt resolution
    if (telegram.startsWith("1-0:2.7.0"))
    {
        ACTUAL_RETURNDELIVERY = getValue('(', '*');
    }

    // 1-0:21.7.0(00.378*kW)
    // 1-0:21.7.0 = Instantaan vermogen Elektriciteit levering L1
    if (telegram.startsWith("1-0:21.7.0"))
    {
        L1_INSTANT_POWER_USAGE = getValue('(', '*');
    }

    // 1-0:41.7.0(00.378*kW)
    // 1-0:41.7.0 = Instantaan vermogen Elektriciteit levering L2
    if (telegram.startsWith("1-0:41.7.0"))
    {
        L2_INSTANT_POWER_USAGE = getValue('(', '*');
    }

    // 1-0:61.7.0(00.378*kW)
    // 1-0:61.7.0 = Instantaan vermogen Elektriciteit levering L3
    if (telegram.startsWith("1-0:61.7.0"))
    {
        L3_INSTANT_POWER_USAGE = getValue('(', '*');
    }

    // 1-0:31.7.0(002*A)
    // 1-0:31.7.0 = Instantane stroom Elektriciteit L1
    if (telegram.startsWith("1-0:31.7.0"))
    {
        L1_INSTANT_POWER_CURRENT = getValue('(', '*');
    }
    // 1-0:51.7.0(002*A)
    // 1-0:51.7.0 = Instantane stroom Elektriciteit L2
    if (telegram.startsWith("1-0:51.7.0"))
    {
        L2_INSTANT_POWER_CURRENT = getValue('(', '*');
    }
    // 1-0:71.7.0(002*A)
    // 1-0:71.7.0 = Instantane stroom Elektriciteit L3
    if (telegram.startsWith("1-0:71.7.0"))
    {
        L3_INSTANT_POWER_CURRENT = getValue('(', '*');
    }

    // 1-0:32.7.0(232.0*V)
    // 1-0:32.7.0 = Voltage L1
    if (telegram.startsWith("1-0:32.7.0"))
    {
        L1_VOLTAGE = getValue('(', '*');
    }
    // 1-0:52.7.0(232.0*V)
    // 1-0:52.7.0 = Voltage L2
    if (telegram.startsWith("1-0:52.7.0"))
    {
        L2_VOLTAGE = getValue('(', '*');
    }   
    // 1-0:72.7.0(232.0*V)
    // 1-0:72.7.0 = Voltage L3
    if (telegram.startsWith("1-0:72.7.0"))
    {
        L3_VOLTAGE = getValue('(', '*');
    }

    // 0-1:24.2.1(150531200000S)(00811.923*m3)
    // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter
    if (telegram.startsWith("0-1:24.2.1"))
    {
        GAS_METER_M3 = getValue('(', '*');
    }

    // 0-0:96.14.0(0001)
    // 0-0:96.14.0 = Actual Tarif
    if (telegram.startsWith("0-0:96.14.0"))
    {
        ACTUAL_TARIF = getValue('(', ')');
    }

    // 0-0:96.7.21(00003)
    // 0-0:96.7.21 = Aantal onderbrekingen Elektriciteit
    if (telegram.startsWith("0-0:96.7.21"))
    {
        SHORT_POWER_OUTAGES = getValue('(', ')');
    }

    // 0-0:96.7.9(00001)
    // 0-0:96.7.9 = Aantal lange onderbrekingen Elektriciteit
    if (telegram.startsWith("0-0:96.7.9"))
    {
        LONG_POWER_OUTAGES = getValue('(', ')');
    }

    // 1-0:32.32.0(00000)
    // 1-0:32.32.0 = Aantal korte spanningsdalingen Elektriciteit in fase 1
    if (telegram.startsWith("1-0:32.32.0"))
    {
        SHORT_POWER_DROPS = getValue('(', ')');
    }

    // 1-0:32.36.0(00000)
    // 1-0:32.36.0 = Aantal korte spanningsstijgingen Elektriciteit in fase 1
    if (telegram.startsWith("1-0:32.36.0"))
    {
        SHORT_POWER_PEAKS = getValue('(', ')');
    }
}

void read_p1_hardwareserial()
{
    while (Serial.available()) {
        char inChar = (char) Serial.read();
        if (inChar == '\n') {
            decode_telegram();
            telegram = "";
            return;
        } else {
            telegram += inChar;
        }
    }
}

// **********************************
// * Setup Main                     *
// **********************************

void setup()
{
    // Setup a hw serial connection for communication with the P1 meter and logging (not using inversion)
    Serial.begin(BAUD_RATE);

    // * Set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    // * Start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(0.6, tick);

    // * WiFiManager local initialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // * Reset settings - uncomment for testing
    // wifiManager.resetSettings();

    // * Fetches SSID and pass and tries to connect
    // * Reset when no connection after 10 seconds
    if (!wifiManager.autoConnect())
    {
        Serial.println(F("Failed to connect to WIFI and hit timeout"));

        // * Reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(WIFI_TIMEOUT);
    }

    // * If you get here you have connected to the WiFi
    Serial.println(F("Connected to WIFI..."));

    // Connect to the MQTT broker
    client.begin(mqttIp, net);

    while (!client.connect(THINGNAME)) {
        Serial.println(client.lastError());
        delay(400);
    }

    if(!client.connected()){
        Serial.println("MQTT Timeout!");
        return;
    }

    Serial.println("MQTT Connected!");

    Serial.println(F("Setting up websocket..."));
    webSocket.begin();

    // * Keep LED off
    ticker.detach();
    digitalWrite(LED_BUILTIN, HIGH);

    telegram.reserve(1024);

    Serial.println("Setup done!");
}

// **********************************
// * Loop                           *
// **********************************

void loop()
{    
    long now = millis();

    webSocket.loop();
    
    read_p1_hardwareserial();

    if (now - LAST_UPDATE_SENT > UPDATE_INTERVAL) {
        send_data_to_broker();
        LAST_UPDATE_SENT = millis();
    }
}
