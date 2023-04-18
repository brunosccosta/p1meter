FROM python:3.7

RUN apt-get update

RUN pip3 install requests flask beautifulsoup4

COPY . /app/
ENTRYPOINT python /app/app.py
