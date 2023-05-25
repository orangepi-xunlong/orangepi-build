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

if [[ $XDG_SESSION_TYPE == wayland ]]; then
	DISPLAY=:0.0 gst-launch-1.0 v4l2src device=${device_id} ! videoconvert \
		! videoscale ! video/x-raw,width=1280,height=720 \
		! waylandsink sync=false 2>&1 > /dev/null &
else
	DISPLAY=:0.0 gst-launch-1.0 v4l2src device=${device_id} io-mode=4 ! videoconvert \
		! video/x-raw,format=NV12,width=${width},height=${heigh} \
		! videoscale ! video/x-raw,width=1280,height=720 \
		! autovideosink sync=false 2>&1 > /dev/null &

fi

gst-launch-1.0 alsasrc device=hw:${hdmiin_card},0 ! audioconvert ! audioresample ! queue \
	! tee name=t ! queue ! alsasink device="hw:${hdmi0_card},0" \
	t. ! queue ! alsasink device="hw:${hdmi1_card},0" \
	t. ! queue ! alsasink device="hw:${es8388_card},0" &

while true
do
	sleep 10
done
