#!/bin/bash

echo 30 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio30/direction
echo 0 > /sys/class/gpio/gpio30/value
sleep 1
echo 1 > /sys/class/gpio/gpio30/value
/usr/bin/brcm_patchram_plus -d --enable_hci --no2bytes --tosleep 200000 --baudrate 115200 --patchram /lib/firmware/BCM4345C5.hcd /dev/ttyS1 &
