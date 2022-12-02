#!/bin/bash

if [[ -z $1 ]]; then
        user=root
else
        user=$1
fi

sudo sed -i '/autologin-user=/d' /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
sudo echo autologin-user=${user} >> /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
sudo sed -i 's/root/anything/' /etc/pam.d/lightdm-autologin
