#!/bin/bash

set -e

service dbus start
bluetoothd &

/usr/local/bin/python3 /app/LYWSD03MMC.py --atc \
	--influxdb 1 \
	--watchdogtimer 5 \
	--mqttconfigfile /conf/mqtt.ini \
	--devicelistfile /conf/devices.ini \
	--onlydevicelist \
	--battery
