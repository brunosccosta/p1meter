FROM python:3.7

RUN apt-get update
RUN pip3 install oic paho-mqtt requests

COPY . /app/
WORKDIR /app

ENTRYPOINT /usr/local/bin/python3 main.py