#!/bin/bash

if [[ -n $1 && $1 =~ left|right|inverted|none ]]; then
        type=$1
else
        echo "usage: set_lcd_rotate.sh [left|right|inverted|none]"
        exit
fi

user=$(cat /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf | grep "autologin-user=" | cut -d "=" -f 2)
#echo $user

if [[ $user == "root" ]]; then

	lcd_num=$(sudo xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | wc -l)
	[[ $lcd_num == 1 ]] && id=$(sudo xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | cut -d '=' -f2 | cut -d " " -f 1 | cut -d "[" -f 1)

	if [[ $lcd_num == 2 ]]; then

		id1=$(sudo xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | head -1 | cut -d "=" -f2 | cut -d "[" -f1)
		id2=$(sudo xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | tail -1 | cut -d "=" -f2 | cut -d "[" -f1)
		#echo "id1 = $id1"
		#echo "id2 = $id2"

	fi

fi

if [[ $user != "root" ]]; then

	lcd_num=$(xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | wc -l)
	[[ $lcd_num == 1 ]] && id=$(xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | cut -d '=' -f2 | cut -d " " -f 1 | cut -d "[" -f 1)

	if [[ $lcd_num == 2 ]]; then

		id1=$(xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | head -1 | cut -d "=" -f2 | cut -d "[" -f1)
		id2=$(xinput | grep "Goodix Capacitive TouchScreen" | grep "pointer" | tail -1 | cut -d "=" -f2 | cut -d "[" -f1)
		#echo "id1 = $id1"
		#echo "id2 = $id2"

	fi

fi

#echo $lcd_num
#echo $id

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

[[ -f /etc/profile.d/lcd.sh ]] && sudo rm /etc/profile.d/lcd.sh
sudo bash -c "echo '#!/bin/bash' >> /etc/profile.d/lcd.sh"
sudo bash -c "echo ' ' >> /etc/profile.d/lcd.sh"
sudo bash -c "echo '[[ \$XDG_SESSION_TYPE == x11 ]] && \' >> /etc/profile.d/lcd.sh"
if [[ $lcd_num == 2 ]]; then
sudo bash -c "echo xinput set-prop $id1 \'Coordinate Transformation Matrix\' ${matrix} >> /etc/profile.d/lcd.sh"
sudo bash -c "echo '[[ \$XDG_SESSION_TYPE == x11 ]] && \' >> /etc/profile.d/lcd.sh"
sudo bash -c "echo xinput set-prop $id2 \'Coordinate Transformation Matrix\' ${matrix} >> /etc/profile.d/lcd.sh"
else
sudo bash -c "echo xinput set-prop $id \'Coordinate Transformation Matrix\' ${matrix} >> /etc/profile.d/lcd.sh"
fi

#xrandr -o $1
#sed -i '/xrandr -o/d' ~/.bashrc
#echo "xrandr -o $1" >> ~/.bashrc

sudo sed -i '/extraargs=fbcon=rotate/d' /boot/orangepiEnv.txt
sudo bash -c "echo extraargs=fbcon=rotate:${rotate} >> /boot/orangepiEnv.txt"
sudo sync

sudo reboot
