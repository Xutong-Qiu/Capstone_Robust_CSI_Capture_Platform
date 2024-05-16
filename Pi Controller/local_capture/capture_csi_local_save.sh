#!/bin/bash
output_dir="${PWD}/outputs"

rm ./outputs/*.pcap
rm ./outputs/*.txt


addr=(101 102 103 104 105 106 107 108)
# Use a for loop to iterate through the array indices
for i in "${!addr[@]}"; do
    ssh pi@192.168.51.${addr[$i]} "sudo ./localsave.sh $1 > output.txt 2>&1" &
done

# change here to modify capture length
sleep $(($1 + 5))

# Use a for loop to iterate through the array indices
for i in "${!addr[@]}"; do
    # Define the output file based on the iteration
    outputFile="$output_dir/output$(($i + 1)).pcap"
    # Use the address from the array and the correct outputFile path in the scp command
    scp pi@192.168.51.${addr[$i]}:output.pcap "$outputFile"
    outputFile="$output_dir/output$(($i + 1)).txt"
    scp pi@192.168.51.${addr[$i]}:output.txt "$outputFile"
done

# display result
for i in {1..8}; do
    # printf "Sniffer${i}: "
    # awk 'NR==3 {printf "Start time:%s ", $2}' "${output_dir}/output$((i)).txt"
    # awk 'NR==11 {printf "End time:%s;  ", $2}' "${output_dir}/output$((i)).txt"
    # grep "captured" outputs/output${i}.txt
    start_time=$(sed -n '3p' "${output_dir}/output$((i)).txt" | cut -d' ' -f2)
    end_time=$(sed -n '11p' "${output_dir}/output$((i)).txt" | cut -d' ' -f2)
    captured=$(grep "captured" "${output_dir}/output${i}.txt")

    echo -n "Sniffer${i}: "
    echo -n "Start time:${start_time} "
    echo -n "End time:${end_time};  "
    echo "${captured}"
done