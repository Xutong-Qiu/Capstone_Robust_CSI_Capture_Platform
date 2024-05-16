# Capstone_Robust_CSI_Capture_Platform

## AC86U router 

### Environment Setup – AC86U Router

First, install Asus Merlin-WRT version 384.19_0 on your RT-AC86U.

In the latest firmware, SCP support has been removed due to security reasons. This makes the old Nexmon installation instructions invalid. To use, a USB flash drive formatted into an AC86U-supported format needs to be used. This will be mounted as an external drive for the router. We will install a package manager on it from the following link: [Gist by 1951FDG](https://gist.github.com/1951FDG/3cada1211df8a59a95a8a71db6310299). Once installed, run `opkg install openssh-sftp-server` to install SFTP to enable SCP.

Note that the Nexmon CSI uses Python 2, which is no longer supported by Linux. We need to specify in the header: `#!/usr/bin/env python2`
