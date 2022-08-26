FROM python:3.7

RUN apt-get update
RUN pip3 install paho-mqtt PyP100

COPY . /app/
WORKDIR /app

ENTRYPOINT /usr/local/bin/python3 tapo-to-mqtt.py