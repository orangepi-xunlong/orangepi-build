#!/bin/bash

MAC_BEF=$(cat /sys/class/net/lan0/address |cut -b -6)
MAC=$(cat /sys/class/net/lan0/address |cut -b 7-)
MAC=${MAC//:/""}
MAC=$((16#$MAC))
MAC=$(($MAC-1))
MAC=`printf %x $MAC`
NUM=`expr 8 - ${#MAC}`

while [ $NUM -ne 0 ]
do
        MAC=0$MAC
        let "NUM--"
done

echo ${MAC} >> /usr/local/test.log
echo ${MAC_BEF} >> /usr/local/test.log
MAC=${MAC_BEF}${MAC:0:2}:${MAC:2:2}:${MAC:4:2}:${MAC:6}

ifconfig eth0 down
ifconfig eth0 hw ether $MAC
ifconfig eth0 up
