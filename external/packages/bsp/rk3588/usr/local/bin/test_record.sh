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

	tinymix -D $card 3 4
	tinymix -D $card 4 2
	tinymix -D $card 14 192
	tinymix -D $card 16 4
	tinymix -D $card 17 4
	if [[ ${BOARD} == orangepi900 ]]; then
	tinymix -D $card 35 1
	tinymix -D $card 36 1
	tinymix -D $card 37 1
	else
	tinymix -D $card 31 1
	tinymix -D $card 32 1
	tinymix -D $card 33 1
	fi

else

	tinymix -D $card 3 2
	tinymix -D $card 4 1
	tinymix -D $card 14 192
	tinymix -D $card 16 4
	tinymix -D $card 17 4
	if [[ ${BOARD} == orangepi900 ]]; then
	tinymix -D $card 35 0
	tinymix -D $card 36 0
	tinymix -D $card 37 0
	else
	tinymix -D $card 31 0
	tinymix -D $card 32 0
	tinymix -D $card 33 0
	fi

fi

echo "Start recording: /tmp/test.wav"
arecord -D hw:${card},0 -d 5 -f cd -t wav /tmp/test.wav

echo "Start playing"
aplay /tmp/test.wav -D hw:${card},0
aplay /tmp/test.wav -D hw:${hdmi0_card},0
