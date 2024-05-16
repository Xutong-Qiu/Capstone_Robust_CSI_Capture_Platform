#!/bin/sh

echo "tcpdump -i wlan0 dst port 5500 -vv -w output.pcap"
echo ""

date +"%Y-%m-%d %H:%M:%S.%3N"

tcpdump -i wlan0 dst port 5500 -vv -w output.pcap &

sleep $1
pkill tcpdump
date +"%Y-%m-%d %H:%M:%S.%3N"