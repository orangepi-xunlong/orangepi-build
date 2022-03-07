#!/bin/bash

sleep 0.35
if [[ $(xset q | grep "Caps Lock" | cut -d " " -f10) == off ]];
then
        #notify-send -i keyboard "Caps Lock" "Off";
	echo 0 > /sys/class/leds/caps_led/brightness
else
        #notify-send -i keyboard "Caps Lock" "On";
	echo 1 > /sys/class/leds/caps_led/brightness
fi
