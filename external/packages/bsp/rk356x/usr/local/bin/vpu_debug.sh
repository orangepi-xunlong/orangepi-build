#!/bin/bash

sudo bash -c "echo 0x100 > /sys/module/rk_vcodec/parameters/mpp_dev_debug"
sudo dmesg -c > /dev/null
dmesg -w
