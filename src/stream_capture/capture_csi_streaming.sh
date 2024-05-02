#!/bin/bash
output_dir="${PWD}/outputs"

make 
rm ./outputs/*.pcap
rm ./outputs/*.txt

# exit 0

startPort=1230
for i in {1..8}; do
    # Define the output file based on the iteration
    outputFile="$output_dir/sniffer${i}.pcap"
    windowName="capture${i}"
    port=$((startPort + i))
    osascript <<EOF

tell application "Terminal"
    set windowName to do script "socat TCP-LISTEN:$port,reuseaddr - | pv -trab > $outputFile"
    delay 0.5
end tell
EOF
done

date
./ssh_execute

#change here to modify capture length
sleep 23

date
echo ""
echo "Terminating and saving .pcap files..."
killall Terminal

echo "Output..."
./read_output

# grep "captured" outputs/output*.txt
for i in {1..8}; do
    printf "Sniffer${i}: "
    awk 'NR==3 {printf "Start time:%s ", $2}' "${output_dir}/output$((i)).txt"
    awk 'NR==11 {printf "End time:%s;  ", $2}' "${output_dir}/output$((i)).txt"
    grep "captured" outputs/output${i}.txt
done


# unused test code
# socat TCP-LISTEN:1235,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test1.pcap&
# socat_pid1=$!
# socat TCP-LISTEN:1236,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test2.pcap&
# socat_pid2=$!
# socat TCP-LISTEN:1237,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test3.pcap&
# socat_pid3=$!
# socat TCP-LISTEN:1238,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test4.pcap&
# socat_pid4=$!
# socat TCP-LISTEN:1239,reuseaddr - > /Users/qiuxutong/Desktop/Capstone/test5.pcap&
# socat_pid5=$!
# sleep 2

# echo "here"
# kill $socat_pid1
# kill $socat_pid2
# kill $socat_pid3
# kill $socat_pid4
# kill $socat_pid5