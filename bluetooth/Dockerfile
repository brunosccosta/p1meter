FROM python:3.7

RUN apt-get update

RUN apt-get install -y cron bluez bluetooth
RUN pip3 install bluepy paho-mqtt btlewrap

COPY xiaomi-ble-mqtt/ /app/
COPY docker_entrypoint.sh docker_entrypoint.sh

ENTRYPOINT sh docker_entrypoint.sh

