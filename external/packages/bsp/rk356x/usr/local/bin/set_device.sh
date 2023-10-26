#!/bin/bash

sudo bash -c "echo peripheral > /sys/devices/platform/fe8a0000.usb2-phy/otg_mode"
sudo systemctl restart usbdevice
