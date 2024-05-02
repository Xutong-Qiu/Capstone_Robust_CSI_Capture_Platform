#!/bin/bash

rm -r PiCode

# Ask the user for the IP address
echo "Enter the IP address for live streaming:"
read live_ip
live_ip=${live_ip:-"192.168.51.200"} 
echo "Enter sleep time:"
read sleep_time
sleep_time=${sleep_time:-"10"}
# Create the PiCode directory if it doesn't exist
mkdir -p ./PiCode

start_port=1230
# Copy the script 5 times with different names
for i in {1..8}; do
  cp localsave.sh "./PiCode/${i}localsave.sh"

  new_port=$((start_port + i))

  sed -i '' "s/PORT_PLACEHOLDER/$new_port/" "./PiCode/${i}localsave.sh"
  sed -i '' "s/IP_ADDRESS_PLACEHOLDER/$live_ip/" "./PiCode/${i}localsave.sh"
  sed -i '' "s/SLEEP_TIME_PLACEHOLDER/$sleep_time/" "./PiCode/${i}localsave.sh"
done

echo "Scripts have been customized and copied to ./PiCode."