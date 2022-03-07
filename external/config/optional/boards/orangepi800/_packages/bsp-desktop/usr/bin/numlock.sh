#!/bin/bash

sleep 0.35
if [[ $(xset q | grep "Num Lock" | cut -d ":" -f5) == *off* ]];
then
        #notify-send -i keyboard "Num Lock" "Off";
	echo 0 > /sys/class/leds/num_led/brightness
else
        #notify-send -i keyboard "Num Lock" "On";
	echo 1 > /sys/class/leds/num_led/brightness
fi
