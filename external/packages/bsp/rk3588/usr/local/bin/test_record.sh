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

if [[ $type == "main" ]]; then

	tinymix -D $card 13 4
	tinymix -D $card 14 2
	tinymix -D $card 24 192
	tinymix -D $card 26 4
	tinymix -D $card 27 4
	if [[ ${BOARD} == orangepi900 ]]; then
	tinymix -D $card 45 1
	tinymix -D $card 46 1
	tinymix -D $card 47 1
	else
	tinymix -D $card 41 1
	tinymix -D $card 42 1
	tinymix -D $card 43 1
	fi

else

	tinymix -D $card 13 2
	tinymix -D $card 14 1
	tinymix -D $card 24 192
	tinymix -D $card 26 4
	tinymix -D $card 27 4
	if [[ ${BOARD} == orangepi900 ]]; then
	tinymix -D $card 45 0
	tinymix -D $card 46 0
	tinymix -D $card 47 0
	else
	tinymix -D $card 41 0
	tinymix -D $card 42 0
	tinymix -D $card 43 0
	fi

fi

echo "Start recording: /tmp/test.wav"
arecord -D hw:${card},0 -d 5 -f cd -t wav /tmp/test.wav

echo "Start playing"
aplay /tmp/test.wav -D hw:${card},0
aplay /tmp/test.wav -D hw:${hdmi0_card},0
