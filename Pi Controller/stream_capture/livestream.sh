#!/bin/sh

echo "tcpdump -i wlan0 port 5500 -l -w - | socat - TCP:IP_ADDRESS_PLACEHOLDER:PORT_PLACEHOLDER"
echo ""

date +"%Y-%m-%d %H:%M:%S.%3N"

tcpdump -i wlan0 port 5500 -l -w - | socat - TCP:IP_ADDRESS_PLACEHOLDER:PORT_PLACEHOLDER &

ps aux | grep -E 'socat|tcpdump'

sleep SLEEP_TIME_PLACEHOLDER
pkill tcpdump
date +"%Y-%m-%d %H:%M:%S.%3N"

sleep 1
ps aux | grep -E 'socat|tcpdump'