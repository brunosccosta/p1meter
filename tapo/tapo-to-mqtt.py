
import time
import json
import re
import os
import sys
import configparser
import logging

import paho.mqtt.client as mqtt
from PyP100 import PyP110

class Tapo:
    name: str
    address: str
    username: str
    password: str
    p110: PyP110.P110

    def __init__(
        self,
        name: str,
        address: str,
        username: str,
        password: str):
        self.name = name
        self.address = address
        self.username = username
        self.password = password

        self.__connect()

    def __connect(self):
        logging.info("Connecting to Tapo on %s - %s:%s", self.address, self.username, self.password)
        self.p110 = PyP110.P110(self.address, self.username, self.password)

        self.p110.handshake()
        self.p110.login()

    def read(self):
        result = self.p110.getEnergyUsage()
        power = result['result']['current_power']
        
        data = {'name': self.name, 'power': power}
        return json.dumps(data)

class MQTT:
    client: mqtt.Client
    address: str
    topic: str

    def __init__(self, address: str, topic: str):
        self.address = address
        self.topic = topic

        self.client = mqtt.Client("tapo-mqtt")
        self.client.connect(self.address)

    def send(self, msg):
        self.client.publish(self.topic, msg)

def read_value_or_env_variable(value):
    matches = re.finditer(r"^\${(.+)}$", value)
    for match in matches:
        return os.environ[match.group(1)]
    return value

def create_sensors_from_config():
    tapo_parser = configparser.ConfigParser()
    tapo_parser.read('devices.conf')

    sensors = []
    for device_name in tapo_parser.sections():
        username = tapo_parser[device_name]['username'] or tapo_parser['DEFAULT']['username']
        password = tapo_parser[device_name]['password'] or tapo_parser['DEFAULT']['password']
        address = tapo_parser[device_name]['address']

        username = read_value_or_env_variable(username)
        password = read_value_or_env_variable(password)

        sensors.append(Tapo(device_name, address, username, password))

    return sensors

def mqtt_from_config():
    conf_parser = configparser.ConfigParser()
    conf_parser.read('mqtt.conf')

    broker = conf_parser['MQTT']['broker']
    topic = conf_parser['MQTT']['topic']

    return MQTT(broker, topic)

mqtt = mqtt_from_config()
sensors = create_sensors_from_config()

while True:
    for sensor in sensors:
        try:
            mqtt.send(sensor.read())
        except:
            logging.info("Error reading data from sensors. Will exit")
            sys.exit(13)
    
    time.sleep(5)