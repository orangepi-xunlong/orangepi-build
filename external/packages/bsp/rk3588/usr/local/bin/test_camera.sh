#!/bin/bash

export DISPLAY=:0.0
#export GST_DEBUG=*:5
#export GST_DEBUG_FILE=/tmp/2.txt

echo "Start MIPI CSI Camera Preview!" 

export XDG_RUNTIME_DIR=/run/user/1000


if [[ -c /dev/video51 ]]; then
	gst-launch-1.0 v4l2src device=/dev/video33 io-mode=4 ! video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1 &
	gst-launch-1.0 v4l2src device=/dev/video42 io-mode=4 ! video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1 &
	gst-launch-1.0 v4l2src device=/dev/video51 io-mode=4 ! video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1
elif [[ -c /dev/video31 ]]; then
	gst-launch-1.0 v4l2src device=/dev/video22 io-mode=4 ! video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1 &
	gst-launch-1.0 v4l2src device=/dev/video31 io-mode=4 ! video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1
elif [[ -c /dev/video11 ]]; then
	gst-launch-1.0 v4l2src device=/dev/video11 io-mode=4 ! video/x-raw,format=NV12,width=720,height=576,framerate=15/1 ! xvimagesink > /dev/null 2>&1
else
	echo "Can not find camera!!!"
fi
