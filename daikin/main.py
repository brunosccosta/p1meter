import os
import re
import time
import json
import configparser

import paho.mqtt.client as mqtt
from daikin import DaikinApi

class Daikin:
    api: DaikinApi
    uuid: str
    name: str

    def __init__(self, api:DaikinApi, uuid: str, name: str):
        self.api = api
        self.uuid = uuid
        self.name = name

    def read(self):
        response = self.api.getCloudDeviceDetails()
        
        data = {'name': self.name }
        for dev_data in response or []:
            if dev_data["id"] != self.uuid:
                continue

            mp_data = dev_data["managementPoints"][1]

            data['status'] = mp_data["onOffMode"]["value"]

            operation = mp_data["operationMode"]["value"]
            data['operation'] = operation

            data['eco'] = mp_data["econoMode"]["value"]
            data['powerful'] = mp_data["powerfulMode"]["value"]
            data['streamer'] = mp_data["streamerMode"]["value"]
            
            sensors = mp_data["sensoryData"]["value"]
            data['outdoor_temp'] = sensors["outdoorTemperature"]["value"]
            data['room_temp'] = sensors["roomTemperature"]["value"]
            data['room_hum'] = sensors["roomHumidity"]["value"]

            setpoints = mp_data["temperatureControl"]["value"]["operationModes"]
            if operation in setpoints:
                data['setpoint'] = setpoints[operation]["setpoints"]["roomTemperature"]["value"]

        return json.dumps(data)

class MQTT:
    client: mqtt.Client
    address: str
    topic: str

    def __init__(self, address: str, topic: str):
        self.address = address
        self.topic = topic

        self.client = mqtt.Client("daikin-mqtt")
        self.client.connect(self.address)

    def send(self, msg):
        self.client.publish(self.topic, msg)

def read_value_or_env_variable(value):
    matches = re.finditer(r"^\${(.+)}$", value)
    for match in matches:
        return os.environ[match.group(1)]
    return value

def mqtt_from_config():
    conf_parser = configparser.ConfigParser()
    conf_parser.read('mqtt.conf')

    broker = conf_parser['MQTT']['broker']
    topic = conf_parser['MQTT']['topic']

    return MQTT(broker, topic)

mqtt = mqtt_from_config()
config_parser = configparser.ConfigParser()
config_parser.read('app.conf')

username = config_parser['app']['username']
password = config_parser['app']['password']
uuid = config_parser['app']['uuid']
name = config_parser['app']['name']
polling = int(config_parser['app']['polling_seconds'])

username = read_value_or_env_variable(username)
password = read_value_or_env_variable(password)

api = DaikinApi(username, password)
api.retrieveAccessToken()

airco = Daikin(api, uuid, name)

while True:
    try:
        mqtt.send(airco.read())
    except:
        logging.info("Error reading data from daikin. Will exit")
        sys.exit(13)
    
    time.sleep(polling)