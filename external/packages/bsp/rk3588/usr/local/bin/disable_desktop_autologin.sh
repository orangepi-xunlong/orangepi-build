#!/bin/bash

sudo sed -i  \
"s/autologin-user=orangepi/#autologin-user=orangepi/"  \
/etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
