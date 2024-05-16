#!/bin/sh

SLEEP="usleep 200000"

echo "Removing firmware"
/sbin/rmmod dhd
$SLEEP

echo "Reinserting firmware"
/sbin/insmod /jffs/nexmon/dhd.ko

echo "Bring up interfaces"
wl -i eth6 up
wl -i eth6 radio on
wl -i eth6 country US
ifconfig eth6 up

echo "services-start: Nexmon Setup Complete" > /dev/kmsg

