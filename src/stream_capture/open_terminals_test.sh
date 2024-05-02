#!/bin/bash
osascript <<EOF
tell application "Terminal"
    set pi1Listening to do script "socat TCP-LISTEN:1234,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test1.pcap"
    delay 1 # Give the window a second to open
end tell
EOF
echo "start listening Pi1..."
sleep 1

osascript <<EOF
tell application "Terminal"
    set pi2Listening to do script "socat TCP-LISTEN:12345,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test2.pcap"
    delay 1 # Give the window a second to open
end tell
EOF
echo "start listening Pi2..."

osascript <<EOF
tell application "Terminal"
    set pi1 to do script "ssh pi@192.168.51.125 \\"cd /jffs/nexmon/; sudo ./quick_setup_livestream_5GHz.sh 36 80 1 1 JCAS3\\""
end tell
EOF
echo "Pi1 starts capturing..."

osascript <<EOF
tell application "Terminal"
    set pi2 to do script "ssh pi@192.168.51.98 \\"cd /jffs/nexmon/; sudo ./quick_setup_livestream_5GHz.sh 36 80 1 1 JCAS3\\""
end tell
EOF

echo "Pi2 starts capturing..."

sleep 5
echo "Terminating..."
killall Terminal


