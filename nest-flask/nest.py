import requests
import time

import os

class Nest:
    def __init__(self, client_id, client_secret, code):
        self.__client_id = client_id
        self.__client_secret = client_secret
        self.__code = code
        self.__access_token = None
        self.__refresh_token = os.environ.get("REFRESH_TOKEN")
        self.__device_name = os.environ.get("DEVICE_NAME")

        self.__access_token_last_update = 0
        self.__expires_in = 0

    def __get_token(self):
        params = (
            ('client_id', self.__client_id),
            ('client_secret', self.__client_secret),
            ('code', self.__code),
            ('grant_type', 'authorization_code'),
            ('redirect_uri', 'https://www.google.com'),
        )

        response = requests.post('https://www.googleapis.com/oauth2/v4/token', params=params)

        response_json = response.json()

        self.__access_token = response_json['token_type'] + ' ' + str(response_json['access_token'])
        self.__refresh_token = response_json['refresh_token']

    def __refresh_access_token(self):
        if ( (int(time.time()) - self.__access_token_last_update) / 60 < self.__expires_in ):
            return

        params = (
            ('client_id', self.__client_id),
            ('client_secret', self.__client_secret),
            ('refresh_token', self.__refresh_token),
            ('grant_type', 'refresh_token'),
        )

        response = requests.post('https://www.googleapis.com/oauth2/v4/token', params=params)

        response_json = response.json()
        self.__access_token = response_json['token_type'] + ' ' + response_json['access_token']
        self.__expires_in = response_json['expires_in']
        self.__access_token_last_update = int(time.time())

    def get_stats(self):
        self.__refresh_access_token()

        url_get_device = 'https://smartdevicemanagement.googleapis.com/v1/' + self.__device_name

        headers = {
            'Content-Type': 'application/json',
            'Authorization': self.__access_token,
        }

        response = requests.get(url_get_device, headers=headers)

        response_json = response.json()
        
        return {
            'humidity': response_json['traits']['sdm.devices.traits.Humidity']['ambientHumidityPercent'],
            'temperature': response_json['traits']['sdm.devices.traits.Temperature']['ambientTemperatureCelsius'],
            'status': response_json['traits']['sdm.devices.traits.ThermostatHvac']['status'],
            'setpoint': response_json['traits']['sdm.devices.traits.ThermostatTemperatureSetpoint'].get('heatCelsius', None),
            'eco': response_json['traits']['sdm.devices.traits.ThermostatEco']['mode']
        }

    def set_eco_on(self):
        return self.__set_eco_mode('MANUAL_ECO')

    def set_eco_off(self):
        return self.__set_eco_mode('OFF')

    def __set_eco_mode(self, mode):
        self.__refresh_access_token()
        
        url_set_mode = 'https://smartdevicemanagement.googleapis.com/v1/' + self.__device_name + ':executeCommand'

        headers = {
            'Content-Type': 'application/json',
            'Authorization': self.__access_token,
        }

        data = '{"command" : "sdm.devices.commands.ThermostatEco.SetMode", "params" : {"mode" : "' + mode + '"} }'

        response = requests.post(url_set_mode, headers=headers, data=data)

        return response.json()
