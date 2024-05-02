#!/bin/bash

set_up_destination_dir="./quick_setup.sh"
localsave_destination_dir="./localsave.sh"
addr=(101 102 103 104 105 106 107 108)
channels=(36 3 149 7 36 3 149 7)
bandwidths=(80 20 80 20 80 20 80 20)
tx_routers=(JCAS3-5G JCAS3-2.4G JCAS4 injector JCAS3-5G JCAS3-2.4G JCAS4 injector)
for i in "${!addr[@]}"; do
    host_ip="192.168.51.${addr[$i]}"
    set_up_file_path="./pi_quick_setup.sh"
    localsave_file_path="./localsave.sh"
    # Copy Capture setup file
    scp "$set_up_file_path" "pi@${host_ip}:${set_up_destination_dir}" 
    # Copy local save file
    scp "$localsave_file_path" "pi@${host_ip}:${localsave_destination_dir}"
    echo "File has been copied to ${host_ip}"
    ssh pi@${host_ip} "chmod 777 quick_setup.sh; sudo ./quick_setup.sh ${channels[$i]} ${bandwidths[$i]} 1 1 ${tx_routers[$i]}; chmod 777 localsave.sh"
    echo "${host_ip} has been setup"
    echo ""
done


# 192.168.51.91  1231
#192.168.51.195 1232
#192.168.51.196 1233
#192.168.51.148 1234
#192.168.51.167 1235
#192.168.51.161 1236