#!/bin/bash

while true; do

	gpu_load=$(cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/load | cut -d "@" -f 1)
	echo $(date "+%H:%M:%S") : GPU load is : ${gpu_load}%
	sleep 1

done
