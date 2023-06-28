import sys
import configparser
import logging
import json
import time

import tinytuya
import paho.mqtt.client as mqtt

class MQTT:
    client: mqtt.Client
    address: str
    topic: str

    def __init__(self, address: str, topic: str):
        self.address = address
        self.topic = topic

        self.client = mqtt.Client("tuya-mqtt")
        self.client.connect(self.address)

    def send(self, msg):
        self.client.publish(self.topic, msg)

class Tuya:
    client: tinytuya.OutletDevice

    def __init__(self, name: str, ip: str, dev_id: str, local_key: str):
        self.name = name

        logging.info(f"Connecting to sensor {name} @ {ip}")

        self.client = tinytuya.OutletDevice(
            dev_id=dev_id,
            address=ip,
            local_key=local_key,
            version=3.3)

    def read(self):
        data = self.client.status()

        logging.info(f"Received from {self.name} - {data}")

        payload = {
            'name': self.name,
            'cur_current': data['dps']['18'],
            'cur_power': data['dps']['19'] / 10.0,
            'cur_voltage': data['dps']['20'] / 10.0
        }

        return json.dumps(payload)

def create_sensors_from_config():
    tuya_parser = configparser.ConfigParser()
    tuya_parser.read('devices.conf')

    sensors = []
    for device_name in tuya_parser.sections():
        ip = tuya_parser[device_name]['ip']
        dev_id = tuya_parser[device_name]['dev_id']
        local_key = tuya_parser[device_name]['local_key']

        sensors.append(Tuya(device_name, ip, dev_id, local_key))

    return sensors

def mqtt_from_config():
    conf_parser = configparser.ConfigParser()
    conf_parser.read('mqtt.conf')

    broker = conf_parser['MQTT']['broker']
    topic = conf_parser['MQTT']['topic']

    return MQTT(broker, topic)

def main():
    logging.basicConfig(stream=sys.stdout, level=logging.INFO)

    mqtt = mqtt_from_config()
    sensors = create_sensors_from_config()

    while True:
        for sensor in sensors:
            try:
                data = sensor.read()
                logging.info("Sending data over mqtt - " + data)

                mqtt.send(data)
            except:
                logging.info("Error reading data from sensors. Will exit")
                sys.exit(13)
        
        time.sleep(5)

if __name__ == '__main__':
    # The script is being run as the main program
    main()