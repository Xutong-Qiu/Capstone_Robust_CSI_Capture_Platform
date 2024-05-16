#!/bin/sh

SLEEP="usleep 200000"

echo "Removing firmware"
/sbin/rmmod dhd
$SLEEP

echo "Reinserting firmware"
/sbin/insmod /jffs/nexmon/dhd.ko

echo "Bring up interfaces"
wl -i eth5 up
wl -i eth5 radio on
wl -i eth5 country US
ifconfig eth5 up

echo "services-start: Nexmon Setup Complete" > /dev/kmsg

