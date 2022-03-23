FROM python:3.7

RUN apt-get update

RUN pip3 install requests paho-mqtt
COPY main.py /app/main.py

ENTRYPOINT python /app/main.py
