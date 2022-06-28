#!/bin/bash

export DISPLAY=:0.0

if [[ -z $1 ]]; then
	video=/usr/local/test.mp4
else
	video=$1
fi

while true
do
	gst-play-1.0 $video --videosink=xvimagesink
done
