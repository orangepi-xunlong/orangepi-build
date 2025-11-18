#!/bin/bash

echo usb_device > /sys/devices/platform/soc@3000000/10.usbc0/otg_role

disable_udc="/etc/.disable_udc"
udc_config=/sys/kernel/config/usb_gadget/g1/UDC

function enable_udc(){
    while [ 1 ];do
        udc=`ls /sys/class/udc 2>/dev/null`
        isudc=`cat $udc_config 2>/dev/null`
        if [ "x$isudc" = "x" ] && [ -f $udc_config ]; then
            echo $udc > $udc_config
        fi
        sleep 1
        if [ -f $disable_udc ];then
            rm $disable_udc
            break
        fi
    done
}

function start_adb(){
    serialnumber=$1
    if [ "x$serialnumber" = "x" ];then
        serialnumber="0402101560"
    fi
    printf "Starting adb "
    # for adbd compatibilities
    mkdir -p /system/
    mkdir -p /system/bin
    if [ ! -f /system/bin/sh ];then
        ln -s /bin/sh /system/bin/sh
    fi

    # config ptmx
    mkdir -p /dev/pts
    mount -t devpts none /dev/pts

    # config adb function
    mount -t configfs none /sys/kernel/config > /dev/null 2>&1
    mkdir -p /sys/kernel/config/usb_gadget/g1
    echo "0x18d1" > /sys/kernel/config/usb_gadget/g1/idVendor
    echo "0x0002" > /sys/kernel/config/usb_gadget/g1/idProduct
    mkdir -p /sys/kernel/config/usb_gadget/g1/strings/0x409
    echo "$serialnumber" > /sys/kernel/config/usb_gadget/g1/strings/0x409/serialnumber
    echo "Google.Inc" > /sys/kernel/config/usb_gadget/g1/strings/0x409/manufacturer
    echo "Configfs ffs gadget" > /sys/kernel/config/usb_gadget/g1/strings/0x409/product
    mkdir -p /sys/kernel/config/usb_gadget/g1/functions/ffs.adb
    mkdir -p /sys/kernel/config/usb_gadget/g1/configs/c.1
    mkdir -p /sys/kernel/config/usb_gadget/g1/configs/c.1/strings/0x409
    echo 0xc0 > /sys/kernel/config/usb_gadget/g1/configs/c.1/bmAttributes
    echo 500 > /sys/kernel/config/usb_gadget/g1/configs/c.1/MaxPower
    ln -s /sys/kernel/config/usb_gadget/g1/functions/ffs.adb/ /sys/kernel/config/usb_gadget/g1/configs/c.1/ffs.adb > /dev/null 2>&1
    mkdir -p /dev/usb-ffs
    mkdir -p /dev/usb-ffs/adb
    if [ "x`ls -A /dev/usb-ffs/adb`" = "x" ];then
        mount -o uid=2000,gid=2000 -t functionfs adb /dev/usb-ffs/adb/
    fi

    # start adbd daemon
    adbd &
    sleep 1

    # enable udc
    cat /sys/bus/platform/drivers/otg\ manager/10.usbc0/usb_device

    enable_udc &
}

case "$1" in
    start|"")
        otg_role_file="/sys/bus/platform/drivers/otg manager/10.usbc0/otg_role"
        [ -f "$otg_role_file" ] && otg_role=`cat "$otg_role_file"`
        if /sbin/ifconfig > /dev/null 2>&1 && [ "x$otg_role" != "xusb_host" ];then
			autotest=/etc/.autotest
            [ -f $autotest ] && serialnumber=`cat $autotest`
            start_adb $serialnumber
        fi
        ;;
    stop)
        printf "Stopping adbd "
        touch $disable_udc
		sleep 2
        killall adbd &
        [ $? -eq 0 ] && echo "OK" || "FAIL"
        ;;
    restart|reload)
        "$0" stop
        "$0" start
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac

