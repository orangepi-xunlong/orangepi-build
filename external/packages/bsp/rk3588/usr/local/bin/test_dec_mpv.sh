#!/bin/bash

export DISPLAY=:0.0
#export GST_DEBUG=*:5
#export GST_DEBUG_FILE=/tmp/2.txt

# xv vo
while true
do
	mpv --hwdec=rkmpp --vd-lavc-software-fallback=no --vo=xv /usr/local/test.mp4
done
