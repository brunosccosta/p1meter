#!/bin/bash

set -e

export CRON=${CRON:-"* * * * *"}

service dbus start
bluetoothd &

echo "$CRON /usr/local/bin/python3.7 /app/data-read.py >> /var/log/cron.log 2>&1" > /etc/cron.d/data-read

cat /etc/cron.d/data-read

# Apply cron job
crontab /etc/cron.d/data-read

# Create the log file to be able to run tail
touch /var/log/cron.log

# cat /var/spool/cron/crontabs/root
cron && tail -f /var/log/cron.log
