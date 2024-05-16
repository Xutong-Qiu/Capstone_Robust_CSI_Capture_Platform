#!/bin/sh

# ./nexutil -Ieth6 -s500 -b -l34 -v `./mcp -c 100/80 -C 1 -N 1 -m a0:36:bc:b2:0b:94 -b 0x80`

echo "Run ./quick_set_livestream.sh channel bandwidth rxcore rxnss [macaddr] [frametype]"

IFACE5GHZ="eth5"

CHAN=$1
if [ "$CHAN" = "" ]; then
  echo "Run ./quick_set_livestream.sh channel bandwidth rxcore rxnss macaddr frametype"
  exit 1
fi

BW=$2
if [ "$BW" = "" ]; then
  echo "Missing bandwidth [20|40|80]"
  exit 1
fi

case "$BW" in
20)
  ;;
40)
  ;;
80)
  ;;
*)
  echo "Invalid bandwidth"
  exit 1
esac

RXCORE=$3
if [ "$RXCORE" = "" ]; then
  echo "Missing rxcore mask: set to combination (or) of values [1|2|4|8]"
  exit 1
fi
if [ "$RXCORE" -lt "1" -o "$RXCORE" -gt "15" ]; then
  echo "Invalid rxcore"
  exit 1
fi

RXNSS=$4
if [ "$RXNSS" = "" ]; then
  echo "Missing rxnss mask: set to combination (or) of values [1|2|4|8]"
  exit 1
fi
if [ "$RXNSS" -lt "1" -o "$RXNSS" -gt "15" ]; then
  echo "Invalid rxnss"
  exit 1
fi

MACADDR=$5
if [ "$MACADDR" = "" ]; then
  echo "Monitor frames from all MAC Addresses"
fi

case "$MACADDR" in
"JCAS1")
  MACADDR="a0:36:bc:b2:1d:c8"
  echo "JCAS1 MACADDR: $MACADDR"
  ;;
"JCAS3")
  MACADDR="a0:36:bc:b2:0b:90"
  echo "JCAS3 MACADDR: $MACADDR"
  ;;
# *)
  # echo "MACADDR: $MACADDR"
esac

FRAMETYPE=$6
if [ "$FRAMETYPE" = "" ]; then
  echo "Monitor all frame types"
fi
echo "FRAMETYPE: empty-all; 0x80-Beacon; 0x08-BroadcastData; 0x88-QoSData; "
echo "FRAMETYPE: 0x54-NDPAnnouncement; 0x94-BlockAck; 0xb4-RequestToSend"


# ./nexutil -Ieth6 -s500 -b -l34 -v `./mcp -c $CHAN/BW -C 1 -N 1 -m a0:36:bc:b2:0b:94 -b 0x80`
# ./nexutil -Ieth6 -s500 -b -l34 -v `./mcp -c $CHAN/$BW -C $RXCORE -N $RXNSS -m a0:36:bc:b2:1d:cc -b 0x80`
# ./nexutil -I ${IFACE5GHZ} -s500 -b -l34 -v `./mcp -c $CHAN/$BW -C $RXCORE -N $RXNSS -m $MACADDR -b $FRAMETYPE`

# base command
CMD="./mcp -c $CHAN/$BW -C $RXCORE -N $RXNSS"

# Append optional parameters if they are set
[ -n "$MACADDR" ] && CMD="$CMD -m $MACADDR"
[ -n "$FRAMETYPE" ] && CMD="$CMD -b $FRAMETYPE"

echo $CMD
CMDoutput=$(eval $CMD)
echo "    Output: $CMDoutput"

echo "./nexutil -I ${IFACE5GHZ} -s500 -b -l34 -v $CMDoutput"
./nexutil -I ${IFACE5GHZ} -s500 -b -l34 -v $CMDoutput

# enable monitor mode
echo "/usr/sbin/wl -i ${IFACE5GHZ} monitor 1"
/usr/sbin/wl -i ${IFACE5GHZ} monitor 1 

# live stream data
# LIVEIPADDR=$7
# if [ "$LIVEIPADDR" = "" ]; then
  # echo "No destination IP addr of live streaming"
  # exit 1
# fi
echo "./tcpdump -i ${IFACE5GHZ} -l -w - | socat - TCP:192.168.51.200:202 &"
echo ""

./tcpdump -i ${IFACE5GHZ} -l -w - | socat - TCP:192.168.51.200:202 &

