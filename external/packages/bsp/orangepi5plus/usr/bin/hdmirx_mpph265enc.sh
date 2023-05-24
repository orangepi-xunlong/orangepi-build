#!/bin/bash

trap 'onCtrlC' INT
function onCtrlC () {
	echo 'Ctrl+C is captured'
	killall gst-launch-1.0
	exit 0
}

device_id=$(v4l2-ctl --list-devices | grep -A1 hdmirx | grep -v hdmirx | awk -F ' ' '{print $NF}')
v4l2-ctl -d $device_id --set-dv-bt-timings query 2>&1 > /dev/null
width=$(v4l2-ctl -d $device_id --get-dv-timings | grep "Active width" |awk -F ' ' '{print $NF}')
heigh=$(v4l2-ctl -d $device_id --get-dv-timings | grep "Active heigh" |awk -F ' ' '{print $NF}')

es8388_card=$(aplay -l | grep "es8388" | cut -d ':' -f 1 | cut -d ' ' -f 2)
hdmi0_card=$(aplay -l | grep "hdmi0" | cut -d ':' -f 1 | cut -d ' ' -f 2)
hdmi1_card=$(aplay -l | grep "hdmi1" | cut -d ':' -f 1 | cut -d ' ' -f 2)
hdmiin_card=$(arecord -l | grep "hdmiin" | cut -d ":" -f 1 | cut -d ' ' -f 2)

gst-launch-1.0 -e v4l2src device=$device_id ! videoconvert ! video/x-raw,format=NV12,width=${width},height=${heigh} ! mpph265enc ! h265parse ! queue ! mpegtsmux ! filesink location="/home/orangepi/hdmiin_video_$(date +"%Y%m%d_%H%M%S").ts"

while true
do
	sleep 10
done
