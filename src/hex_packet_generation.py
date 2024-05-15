from scapy.all import *
from scapy.layers.dot11 import Dot11

def create_80211_packet(filename):
    # Specify MAC addresses
    src_mac = '02:00:00:00:01:00'  # Example source MAC
    dst_mac = '02:00:00:00:02:00'  # Example destination MAC
    bssid = '02:00:00:00:03:00'    # Example BSSID

    # Create the 802.11 frame
    # 'type=2' for Data frames, 'subtype=0' for Data subtype
    dot11 = Dot11(type=2, subtype=0, addr1=dst_mac, addr2=src_mac, addr3=bssid)

    # Define a payload containing bytes 0x11 0x12
    byte_payload = bytes([0x80, 0x00, 0x00, 0x00, 
                          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x36, 
                          0xbc, 0xb2, 0xde, 0x14, 0xa0, 0x36, 0xbc, 0xb2, 
                          0xde, 0x14, 0x00, 0x00
                          ])

    # Combine the 802.11 header with the payload
    packet =  Raw(load=byte_payload)
    
    wrpcap(filename, packet)
    
create_80211_packet("80211_packet_with_hex_payload.pcap")

