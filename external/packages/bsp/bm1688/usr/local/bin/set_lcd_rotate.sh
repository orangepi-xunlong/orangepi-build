#!/bin/bash

if [[ -n $1 && $1 =~ left|right|inverted|none ]]; then
        type=$1
else
        echo "usage: set_lcd_rotate.sh [left|right|inverted|none]"
        exit
fi

case $1 in
left)
	rotate=3
	matrix="0 -1 1 1 0 0 0 0 1"
	;;
right)
	rotate=1
	matrix="0 1 0 -1 0 1 0 0 1"
	;;
inverted)
	rotate=2
	matrix="-1 0 1 0 -1 1 0 0 1"
	;;
none)
	rotate=0
	matrix="1 0 0 0 1 0 0 0 1"
	;;
esac

sudo sed -i '/TransformationMatrix/d' /usr/share/X11/xorg.conf.d/40-libinput.conf
sudo sed -i "/libinput touchscreen catchall/a Option \"TransformationMatrix\" \"${matrix}\"" /usr/share/X11/xorg.conf.d/40-libinput.conf

sudo sed -i '/extraargs=fbcon=rotate/d' /boot/orangepiEnv.txt
sudo bash -c "echo extraargs=fbcon=rotate:${rotate} >> /boot/orangepiEnv.txt"
sudo sed -i 's/bootlogo=true/bootlogo=false/g' /boot/orangepiEnv.txt
sudo sync

sudo reboot
