# Usage Instructions

## Prerequisite

This project requires the `libssh` C++ library and C++11 or later.

- **On MacOS**: Install `libssh` using Homebrew: `brew install libssh`
    Install `pv` using Homebrew: `brew install pv`

## Usage

### Running the Capture Script

1. Use any third-party terminal application (ensure its process name is not "Terminal").
2. Run the script `./capture_csi.sh`. If you need to change the capture length, modify the `sleep` command within `capture_csi.sh`.

### Configuring SSH Hosts

To add, delete, or modify SSH host configurations:

1. Open `constants.h`.
2. Modify the `num_host` variable to reflect the number of hosts you are configuring.
3. Update the `addrs`, `usernames`, and `keys` variables with the corresponding information for each host.
4. In `capture_csi.sh`, adjust the range of the for loop to match the number of hosts specified by `num_host`.
