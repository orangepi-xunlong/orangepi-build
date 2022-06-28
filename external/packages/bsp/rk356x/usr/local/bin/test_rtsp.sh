#!/bin/sh

export DISPLAY=:0.0
#export GST_DEBUG=*:5
#export GST_DEBUG_FILE=/tmp/2.txt

# server:
# vlc v4l2:// :v4l2-dev=/dev/video0 :v4l2-width=640 \
# :v4l2-height=480 --sout="#transcode{vcodec=h264,vb=800,scale=1,\
# acodec=mp4a,ab=128,channels=2,samplerate=44100}:rtp{sdp=rtsp://:8554/}" -I dummy

gst-launch-1.0 rtspsrc location=rtsp://192.168.31.163:8554/ ! \
            ! rtph264depay ! h264parse ! mppvideodec ! rkximagesink sync=false
