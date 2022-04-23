FROM influxdb:latest

RUN apt update -y && apt install cron unzip -y
RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-aarch64.zip" -o "awscliv2.zip" && unzip awscliv2.zip && ./aws/install

COPY influxdb-to-s3.sh /usr/bin/influxdb-to-s3.sh

ENTRYPOINT ["/usr/bin/influxdb-to-s3.sh"]
CMD ["startcron"]
