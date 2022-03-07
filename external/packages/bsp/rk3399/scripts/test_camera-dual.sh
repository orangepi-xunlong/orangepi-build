#!/bin/bash

preview_mode="width=640,height=480,framerate=30/1"
vsnk="autovideosink"

export DISPLAY=:0.0

#----------------------------------------------------------
# selfpath
declare -a PreviewDevs=()
# mainpath
declare -a PictureDevs=()
# camera type
declare -a CameraTypes=()

# isp1
if [ -d /sys/class/video4linux/v4l-subdev2/device/video4linux/video1 -o \
        -d /sys/class/video4linux/v4l-subdev5/device/video4linux/video1 ]; then
        PreviewDevs+=("/dev/video1")
        PictureDevs+=("/dev/video0")
	CameraTypes+=("mipi")
fi

# isp2
if [ -d /sys/class/video4linux/v4l-subdev2/device/video4linux/video6 -o \
        -d /sys/class/video4linux/v4l-subdev5/device/video4linux/video6 ]; then
        PreviewDevs+=("/dev/video6")
        PictureDevs+=("/dev/video5")
	CameraTypes+=("mipi")
fi

# usb camera
if [ -f /sys/class/video4linux/video10/name ]; then
        if [ "$( grep -i "UVC" /sys/class/video4linux/video8/name )" ]; then
		PreviewDevs+=("/dev/video10")
		PictureDevs+=("/dev/video10")
		CameraTypes+=("usb")
        fi
fi

killall gst-launch-1.0 2>&1 > /dev/null
sleep 1

for icam in 0 1
do
	[ -c "${PreviewDevs[$icam]}" ] || break

	echo "Start MIPI CSI Camera Preview ${PreviewDevs[$icam]} ..."

        rkargs="device=${PreviewDevs[$icam]}"
	if [ ${CameraTypes[$icam]} = "mipi" ]; then
        	CMD="gst-launch-1.0 rkisp ${rkargs} io-mode=1 \
                	! video/x-raw,format=NV12,${preview_mode} \
                	! ${vsnk}"
	else
		CMD="gst-launch-1.0 v4l2src ${rkargs} io-mode=4 \
                        ! videoconvert ! video/x-raw,format=NV12,${preview_mode} \
                        ! ${vsnk}"
	fi

        echo "===================================================="
        echo "=== GStreamer 1.1 command:"
        echo "=== $(echo $CMD | sed -e 's/\r//g')"
        echo "===================================================="

	if [ $vsnk = "kmssink" -o "$(id -un)" = "pi" ]; then
                eval "${CMD}"&
        else
                su orangepi -c "${CMD}"&
        fi

        sleep 2
done

