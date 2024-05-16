from scapy.all import *
from scapy.layers.dot11 import Dot11

def create_80211_packet(filename):
    # Specify MAC addresses
    src_mac = 'a0:36:bc:b2:de:14'  # Example source MAC
    dst_mac = 'ff:ff:ff:ff:ff:ff'  # Example destination MAC
    bssid = 'a0:36:bc:b2:de:14'    # Example BSSID

    # Create the 802.11 frame
    # 'type=2' for Data frames, 'subtype=0' for Data subtype
    dot11 = Dot11(type=0, subtype=8, addr1=dst_mac, addr2=src_mac, addr3=bssid)

    # Define a simple payload
    payload = Raw(load="Hello, this is a test payload!")

    # Combine the 802.11 header with the payload
    packet = dot11 #/ payload

    # Write the packet to a PCAP file
    wrpcap(filename, packet)

# Use the function to create a PCAP file
create_80211_packet("80211_packet.pcap")
