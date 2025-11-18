#!/bin/bash

# 通过 sysfs 查找 armcb-00-vid-cap 对应的视频设备
find_cam1_device() {
    for dev in /dev/video*; do
        dev_name=$(basename "$dev")
        if [ -f "/sys/class/video4linux/${dev_name}/name" ]; then
            name=$(cat "/sys/class/video4linux/${dev_name}/name")
            if [ "$name" = "armcb-00-vid-cap" ]; then
                echo "$dev"
                return 0
            fi
        fi
    done
    return 1
}

VIDEO_DEVICE=$(find_cam1_device)

if [ -z "$VIDEO_DEVICE" ]; then
    echo "Error: Cannot find video device for armcb-00-vid-cap"
    exit 1
fi

echo "Using video device: $VIDEO_DEVICE for armcb-00-vid-cap"

gst-launch-1.0 v4l2src device=$VIDEO_DEVICE ! videoconvert ! autovideosink
