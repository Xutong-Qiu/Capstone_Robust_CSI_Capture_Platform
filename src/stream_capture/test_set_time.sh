#!/bin/bash

# Get the current time plus 5 seconds (rounded to the next minute) on macOS
# Note: Adding 1 minute to ensure the command is scheduled in the future, even after accounting for script execution and SSH connection time
target_time=$(date -v+1M +"%H:%M")

# Remote command to execute. Adjust the output file path as needed.
output_file="/home/qiuxutong/Desktop/test.txt"
remote_command="echo 'ls > $output_file' | at $target_time"

# SSH and execute the command on the remote host
ssh qiuxutong@192.168.0.28 "$remote_command"

echo "Scheduled 'ls' command on qiuxutong@192.168.0.28 to run at $target_time"
echo "Output will be stored in $output_file on the remote host."


# #!/bin/bash
# target_time=$(date -v+5S +"%H:%M:%S")

# ssh qiuxutong@192.168.0.28 'sleep $(echo "$(date -d "09:01:07" +%s) - $(date +%s)" | bc); <your_command_here>' &
# ssh raspi2 'sleep $(echo "$(date -d "09:01:07" +%s) - $(date +%s)" | bc); <your_command_here>' &

