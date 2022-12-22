#!/bin/bash

if [[ -n $1 && $1 =~ main|headset ]]; then
	type=$1
else
	echo "usage: test_record.sh main/headset"
	exit
fi

card=$(aplay -l | grep "es8388" | cut -d ':' -f 1 | cut -d ' ' -f 2)

if [[ $type == "main" ]]; then

	tinymix -D 2 3 4
	tinymix -D 2 4 2
	tinymix -D 2 14 192
	tinymix -D 2 16 0
	tinymix -D 2 17 0
	tinymix -D 2 31 1
	tinymix -D 2 32 1
	tinymix -D 2 33 1

else

	tinymix -D 2 3 2
	tinymix -D 2 4 1
	tinymix -D 2 14 192
	tinymix -D 2 16 0
	tinymix -D 2 17 0
	tinymix -D 2 31 0
	tinymix -D 2 32 0
	tinymix -D 2 33 0

fi


echo "Start recording: /tmp/test.wav"
arecord -D hw:${card},0 -d 5 -f cd -t wav /tmp/test.wav

echo "Start playing"
aplay /tmp/test.wav -D hw:1,0
aplay /tmp/test.wav -D hw:2,0
#aplay /tmp/test.wav -D hw:0,0
