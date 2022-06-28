#!/bin/bash

export DISPLAY=:0.0

device_id0=$(v4l2-ctl --list-devices | grep -A1 rkisp-vir0 |grep -v rkisp-vir0 |awk -F ' ' '{print $NF}')
device_id1=$(v4l2-ctl --list-devices | grep -A1 rkisp-vir1 |grep -v rkisp-vir1 |awk -F ' ' '{print $NF}')

echo "Start MIPI CSI Camera Preview!" 

if [[ ! -z $device_id0 ]]; then
	su root -c "gst-launch-1.0 v4l2src device=${device_id0} io-mode=4 ! videoconvert \
		! video/x-raw,format=NV12,width=1280,height=720 \
		! autovideosink 2>&1 > /dev/null &"
fi

if [[ ! -z $device_id1 ]]; then
	su root -c "gst-launch-1.0 v4l2src device=${device_id1} io-mode=4 ! videoconvert \
		! video/x-raw,format=NV12,width=640,height=480 \
		! autovideosink 2>&1 > /dev/null &"
fi


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
