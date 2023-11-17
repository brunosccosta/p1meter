FROM python:3.7

RUN apt-get update

RUN pip3 install tinytuya flask

COPY . /app/
WORKDIR /app

ENTRYPOINT python /app/app.py
