#!/bin/bash

export DISPLAY=:0.0
#export GST_DEBUG=*:5
#export GST_DEBUG_FILE=/tmp/2.txt

echo "Start MIPI CSI Camera Preview!" 

export XDG_RUNTIME_DIR=/run/user/1000

gst-launch-1.0 v4l2src device=/dev/video11 io-mode=4 ! queue ! video/x-raw,format=NV12,width=2112,height=1568,framerate=30/1 ! glimagesink
