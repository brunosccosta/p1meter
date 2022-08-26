FROM python:3.7

RUN apt-get update
RUN pip3 install oci paho-mqtt

COPY . /app/
WORKDIR /app

ENTRYPOINT /usr/local/bin/python3 main.py