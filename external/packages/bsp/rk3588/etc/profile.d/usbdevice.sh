#!/bin/sh

# The env variables below can be overridden

# option: adb acm hid mtp ntb rndis uac1 uac2 ums uvc
export USB_FUNCS="adb"

export UMS_FILE=/userdata/ums_shared.img
export UMS_SIZE=256M
export UMS_FSTYPE=vfat
export UMS_MOUNT=0
export UMS_MOUNTPOINT=/mnt/ums
export UMS_RO=0
