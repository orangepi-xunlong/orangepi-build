#!/bin/bash

if [[ -n $1 && $1 =~ main|headset ]]; then
	type=$1
else
	echo "usage: test_record.sh main/headset"
	exit
fi

source /etc/orangepi-release

card=$(aplay -l | grep "es8388" | cut -d ':' -f 1 | cut -d ' ' -f 2)
hdmi0_card=$(aplay -l | grep "hdmi0" | cut -d ':' -f 1 | cut -d ' ' -f 2)
if [[ ${BOARD} == orangepi5ultra ]]; then
	hdmi1_card=$(aplay -l | grep "hdmi1" | cut -d ':' -f 1 | cut -d ' ' -f 2)
fi

if [[ $type == "main" ]]; then

	amixer -c $card cset name='ALC Capture Max PGA' 4 >/dev/null
	amixer -c $card cset name='ALC Capture Min PGA' 2 >/dev/null
	amixer -c $card cset name='Capture Digital Volume' 192 >/dev/null
	amixer -c $card cset name='Left Channel Capture Volume' 4 >/dev/null
	amixer -c $card cset name='Right Channel Capture Volume' 4 >/dev/null

	if [[ ${BOARD} == orangepi900 ]]; then
	amixer -c $card cset name='Left Line Mux' 1 >/dev/null
	amixer -c $card cset name='Right Line Mux' 1 >/dev/null
	amixer -c $card cset name='Left Mixer Left Playback Switch' 1 >/dev/null
	else
	amixer -c $card cset name='Left PGA Mux' 1 >/dev/null
	amixer -c $card cset name='Right PGA Mux' 1 >/dev/null
	amixer -c $card cset name='Differential Mux' 1 >/dev/null
	fi

else
	amixer -c $card cset name='ALC Capture Max PGA' 2 >/dev/null
	amixer -c $card cset name='ALC Capture Min PGA' 1 >/dev/null
	amixer -c $card cset name='Capture Digital Volume' 192 >/dev/null
	amixer -c $card cset name='Left Channel Capture Volume' 4 >/dev/null
	amixer -c $card cset name='Right Channel Capture Volume' 4 >/dev/null
	if [[ ${BOARD} == orangepi900 ]]; then
	amixer -c $card cset name='Left Line Mux' 0 >/dev/null
	amixer -c $card cset name='Right Line Mux' 0 >/dev/null
	amixer -c $card cset name='Left Mixer Left Playback Switch' 0 >/dev/null
	else
	amixer -c $card cset name='Left PGA Mux' 0 >/dev/null
	amixer -c $card cset name='Right PGA Mux' 0 >/dev/null
	amixer -c $card cset name='Differential Mux' 0 >/dev/null
	fi

fi

echo "Start recording: /tmp/test.wav"
arecord -D hw:${card},0 -d 5 -f cd -t wav /tmp/test.wav

echo "Start playing"
aplay /tmp/test.wav -D hw:${card},0
if [[ ${BOARD} == orangepi5ultra ]]; then
	aplay /tmp/test.wav -D hw:${hdmi1_card},0
else
	aplay /tmp/test.wav -D hw:${hdmi0_card},0
fi
