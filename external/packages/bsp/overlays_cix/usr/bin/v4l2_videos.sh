#!/bin/bash

echo "Video Device and Name Correspondence:"
echo "====================================="
for dev in /dev/video*; do
    dev_name=$(basename $dev)
    if [ -f "/sys/class/video4linux/${dev_name}/name" ]; then
        name=$(cat "/sys/class/video4linux/${dev_name}/name")
        echo "$dev : $name"
    fi
done
