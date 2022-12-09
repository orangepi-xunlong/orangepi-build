#!/bin/bash

sudo bash -c "echo device > /sys/kernel/debug/usb/fc000000.usb/mode"
sudo systemctl restart usbdevice
