# Usage Instructions

## AC86U router setup

First, install Asus Merlin-WRT version 384.19_0 on your RT-AC86U.

In the latest firmware, SCP support has been removed due to security reasons. This makes the old Nexmon installation instructions invalid. To use, a USB flash drive formatted into an AC86U-supported format needs to be used. This will be mounted as an external drive for the router. We will install a package manager on it from the following link: [Gist by 1951FDG](https://gist.github.com/1951FDG/3cada1211df8a59a95a8a71db6310299). Once installed, run `opkg install openssh-sftp-server` to install SFTP to enable SCP.

Note that the Nexmon CSI uses Python 2, which is no longer supported by Linux. We need to specify in the header: `#!/usr/bin/env python2`

Then, you may choose one of the following options to patch the router:

### Option 1: Using pre-built binaries
Copy all the contents in scp folder to the `/jffs/` partition of your router: `scp ./nexmonster-rtac86u/* admin@<routers-ip>:/jffs/`
Make nexutil, makecsiparams, and tcpdump executable: `ssh admin@<routers-ip> "/bin/chmod +x /jffs/nexutil /jffs/mcp /jffs/tcpdump "`

### Option 2: Building the binaries from sources if you need to modify the source code
Please follow the instructions here: [link](https://github.com/nexmonster/nexmon_csi/discussions/7). But change the replace the original csi_extractor.c with the one provided in this repo under `src`

## Usage

### Quick setup
- On your router, run ```./reload_5GHz.sh``` to load the patched firmware into the system
- On your client, run ```socat TCP-LISTEN:[PORT],reuseaddr - > test.pcap``` to start listening. Change the port as needed.

- On the router:
  - Modify the /quick_set_livestream.sh file to set up the live-streaming parameter: [Desktop_IPaddr] is the desktop's LAN IP address and [PORT] is the same port as we set above.
    ```
    ./tcpdump -i ${IFACE5GHZ} port 5500 -l -w - | socat - TCP:[Desktop_IPaddr]:[PORT]
    ```
  - Configure the channel, bandwidth, rxcore (rx antennas, set to combination (or) of values [1|2|4|8]), rxnss (spatial streams), [macaddr] (mac address filter), [frametype] (frame control type filter):
    ```
    ./quick_setup_livestream_5GHz.sh channel bandwidth rxcore rxnss [macaddr] [frametype]
    ```
    Some examples:
    - ```./quick_setup_livestream_5GHz.sh 36 80 1 1```
    - ```./quick_setup_livestream_5GHz.sh 36 80 3 1 11:22:33:44:55:66 0x80```
