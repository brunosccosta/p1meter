FROM python:3.7

RUN apt-get update

RUN apt-get install -y cron bluez bluetooth libbluetooth-dev
RUN pip3 install bluepy paho-mqtt requests pybluez pycryptodomex

COPY MiTemperature2 /app/
COPY docker_entrypoint.sh docker_entrypoint.sh

ENTRYPOINT sh docker_entrypoint.sh
