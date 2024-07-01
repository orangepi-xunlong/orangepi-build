#!/bin/bash

export DISPLAY=:0.0
#export GST_DEBUG=*:5
#export GST_DEBUG_FILE=/tmp/2.txt

echo "Start MIPI CSI Camera Preview!" 

export XDG_RUNTIME_DIR=/run/user/1000

patterns="rkisp0-vir0 rkisp0-vir1 rkisp0-vir2 rkisp1-vir0 rkisp1-vir1 rkisp1-vir2"

devices_info=$(v4l2-ctl --list-devices)
#devices_id=("/dev/video44" "/dev/video53" "/dev/video62" "/dev/video71")

for pattern in $patterns; do
	devices_id+=("$(echo "$devices_info" | awk -v pattern="$pattern" '$0 ~ pattern {getline; print $NF}')")
done

for device_id in "${devices_id[@]}"; do
	if [[ -c "$device_id" ]]; then
		gst-launch-1.0 v4l2src device=$device_id io-mode=4 ! \
			video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1 &
	fi
done

echo "[Ctrl + C] exit"
	while true
	do
	sleep 10
done

trap 'onCtrlC' INT
function onCtrlC () {
	echo 'Ctrl+C is captured'
	killall gst-launch-1.0
	exit 0
}
