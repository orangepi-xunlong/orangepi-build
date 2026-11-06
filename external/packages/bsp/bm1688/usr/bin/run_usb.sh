UDC=39010000.usb
CLASS=acm
VID=0x3346
PID=0x1003
MSC_PID=0x1008
RNDIS_PID=0x1009
UVC_PID=0x100A
UAC_PID=0x100B
ADB_VID=0x18D1
ADB_PID=0x4EE0
ADB_PID_M1=0x4EE2
ADB_PID_M2=0x4EE4
MANUFACTURER="Cvitek"
PRODUCT="USB Com Port"
PRODUCT_RNDIS="RNDIS"
PRODUCT_UVC="UVC"
PRODUCT_UAC="UAC"
PRODUCT_ADB="ADB"
ADBD_PATH=/usr/sbin/
SERIAL="0123456789"
MSC_FILE=$3
CVI_DIR=/tmp/usb
CVI_GADGET=$CVI_DIR/usb_gadget/cvitek
CVI_FUNC=$CVI_GADGET/functions
FUNC_NUM=0
MAX_EP_NUM=4
TMP_NUM=0
INTF_NUM=0
EP_IN=0
EP_OUT=0

case "$2" in
  acm)
	CLASS=acm
	;;
  msc)
	CLASS=mass_storage
	PID=$MSC_PID
	;;
  cvg)
	CLASS=cvg
	;;
  rndis)
	CLASS=rndis
	PID=$RNDIS_PID
	PRODUCT=$PRODUCT_RNDIS
	;;
  uvc)
	CLASS=uvc
	PID=$UVC_PID
	PRODUCT=$PRODUCT_UVC
	;;
  uac1)
	CLASS=uac1
	PID=$UAC_PID
	PRODUCT=$PRODUCT_UAC
	;;
  adb)
	CLASS=ffs.adb
	VID=$ADB_VID
	PID=$ADB_PID
	PRODUCT=$PRODUCT_ADB
	;;
  mtp)
	CLASS=mtp
	;;
  *)
	if [ "$1" = "probe" ] ; then
	  echo "Usage: $0 probe {acm|msc|cvg|rndis|uvc|uac1|adb|mtp}"
	  exit 1
	fi
esac

calc_func() {
  FUNC_NUM=$(ls $CVI_GADGET/functions -l | grep ^d | wc -l)
  echo "$FUNC_NUM file(s)"
}

res_check() {
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "acm*" | wc -l)
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "mass_storage*" | wc -l)
  EP_IN=$(($EP_IN+$TMP_NUM))
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "cvg*" | wc -l)
  EP_IN=$(($EP_IN+$TMP_NUM))
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "rndis*" | wc -l)
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "uvc*" | wc -l)
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "uac1*" | wc -l)
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name ffs.adb | wc -l)
  EP_IN=$(($EP_IN+$TMP_NUM))
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "mtp*" | wc -l)
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  INTF_NUM=$(($INTF_NUM+$TMP_NUM))

  if [ "$CLASS" = "acm" ] ; then
    EP_IN=$(($EP_IN+2))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "mass_storage" ] ; then
    EP_IN=$(($EP_IN+1))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "cvg" ] ; then
    EP_IN=$(($EP_IN+1))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "rndis" ] ; then
    EP_IN=$(($EP_IN+2))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "uvc" ] ; then
    EP_IN=$(($EP_IN+2))
  fi
  if [ "$CLASS" = "uac1" ] ; then
    EP_IN=$(($EP_IN+1))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "ffs.adb" ] ; then
    EP_IN=$(($EP_IN+1))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "mtp" ] ; then
    EP_IN=$(($EP_IN+2))
    EP_OUT=$(($EP_OUT+1))
  fi
  echo "$EP_IN in ep"
  echo "$EP_OUT out ep"
  if [ $EP_IN -gt $MAX_EP_NUM ]; then
    echo "reach maximum resource"
    exit 1
  fi
  if [ $EP_OUT -gt $MAX_EP_NUM ]; then
    echo "reach maximum resource"
    exit 1
  fi
}

probe() {
  if [ ! -d $CVI_DIR ]; then
    mkdir $CVI_DIR
  fi
  if [ ! -d $CVI_DIR/usb_gadget ]; then
    # Enale USB ConfigFS
    mount none $CVI_DIR -t configfs
    # Create gadget dev
    mkdir $CVI_GADGET
    # Set the VID and PID
    echo $VID >$CVI_GADGET/idVendor
    echo $PID >$CVI_GADGET/idProduct
    # Set the product information string
    mkdir $CVI_GADGET/strings/0x409
    echo $MANUFACTURER>$CVI_GADGET/strings/0x409/manufacturer
    echo $PRODUCT>$CVI_GADGET/strings/0x409/product
    echo $SERIAL>$CVI_GADGET/strings/0x409/serialnumber
    # Set the USB configuration
    mkdir $CVI_GADGET/configs/c.1
    mkdir $CVI_GADGET/configs/c.1/strings/0x409
    echo "config1">$CVI_GADGET/configs/c.1/strings/0x409/configuration
    # Set the MaxPower of USB descriptor
    echo 120 >$CVI_GADGET/configs/c.1/MaxPower
  fi
  # get current functions number
  calc_func
  # assign the class code for composite device
  if [ ! $FUNC_NUM -eq 0 ]; then
    echo 0xEF >$CVI_GADGET/bDeviceClass
    echo 0x02 >$CVI_GADGET/bDeviceSubClass
    echo 0x01 >$CVI_GADGET/bDeviceProtocol
  fi
  # resource check
  res_check
  # create the desired function
  if [ "$CLASS" = "ffs.adb" ] ; then
    # adb shall be the last function to probe. Override the pid/vid
    echo $VID >$CVI_GADGET/idVendor
    echo $PID >$CVI_GADGET/idProduct
    # choose pid for different function number
    if [ $INTF_NUM -eq 1 ]; then
      echo $ADB_PID_M1 >$CVI_GADGET/idProduct
    fi
    if [ $INTF_NUM -eq 2 ]; then
      echo $ADB_PID_M2 >$CVI_GADGET/idProduct
    fi
    mkdir $CVI_GADGET/functions/$CLASS
  else
    mkdir $CVI_GADGET/functions/$CLASS.usb$FUNC_NUM
  fi
  if [ "$CLASS" = "mass_storage" ] ; then
    echo $MSC_FILE >$CVI_GADGET/functions/$CLASS.usb$FUNC_NUM/lun.0/file
  fi
  if [ "$CLASS" = "rndis" ] ; then
    #OS STRING
    echo 1 >$CVI_GADGET/os_desc/use
    echo 0xcd >$CVI_GADGET/os_desc/b_vendor_code
    echo MSFT100 >$CVI_GADGET/os_desc/qw_sign
    #COMPATIBLE ID
    echo RNDIS >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/compatible_id
    #MAKE c.1 THE ONE ASSOCIATED WITH OS DESCRIPTORS
    ln -s $CVI_GADGET/configs/c.1 $CVI_GADGET/os_desc
    #MAKE "Icons" EXTENDED PROPERTY
    mkdir $CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Icons
    echo 2 >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Icons/type
    echo "%SystemRoot%\\system32\\shell32.dll,-233" >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Icons/data
    #MAKE "Label" EXTENDED PROPERTY
    mkdir $CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Label
    echo 1 >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Label/type
    echo "XYZ Device" >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Label/data
  fi
}

start() {
  # link this function to the configuration
  calc_func
  if [ $FUNC_NUM -eq 0 ]; then
    echo "Functions Empty!"
    exit 1
  fi
  if [ -d $CVI_GADGET/functions/ffs.adb ]; then
    FUNC_NUM=$(($FUNC_NUM-1))
  fi
  for i in `seq 0 $(($FUNC_NUM-1))`;
  do
    find $CVI_GADGET/functions/ -name "*.usb$i" | xargs -I % ln -s % $CVI_GADGET/configs/c.1
  done
  if [ -d $CVI_GADGET/functions/mtp.usb0 ]; then
    $ADBD_PATH/mtp-server &
  fi
  if [ -d $CVI_GADGET/functions/ffs.adb ]; then
    ln -s $CVI_GADGET/functions/ffs.adb $CVI_GADGET/configs/c.1
    mkdir /dev/usb-ffs/adb -p
    mount -t functionfs adb /dev/usb-ffs/adb
    if [ -f $ADBD_PATH/adbd ]; then
	echo high-speed > $CVI_GADGET/max_speed
	$ADBD_PATH/adbd &
    fi
  else
    # Start the gadget driver
    echo ${UDC} >$CVI_GADGET/UDC
  fi
}

stop() {
  if [ -d $CVI_GADGET/configs/c.1/ffs.adb ]; then
    pkill adbd
    rm $CVI_GADGET/configs/c.1/ffs.adb
  else
    echo "" > $CVI_GADGET/UDC
  fi
  if [ -d $CVI_GADGET/functions/mtp.usb0 ]; then
    pkill mtp-server
  fi
  find $CVI_GADGET/configs/ -name "*.usb*" | xargs rm -f
  rmdir $CVI_GADGET/configs/c.*/strings/0x409/
  tmp_dirs=$(find $CVI_GADGET/os_desc/c.* -type d)
  if [ -n tmp_dirs ]; then
    echo "remove os_desc!"
    rm -rf $CVI_GADGET/os_desc/c.*/
    find $CVI_GADGET/functions/ -name Icons | xargs rmdir
    find $CVI_GADGET/functions/ -name Label | xargs rmdir
  fi
  rmdir $CVI_GADGET/configs/c.*/
  rmdir $CVI_GADGET/functions/*
  rmdir $CVI_GADGET/strings/0x409/
  rmdir $CVI_GADGET
  umount $CVI_DIR
  rmdir $CVI_DIR
}

case "$1" in
  start)
	if [ $# -eq 2 ] && [ $2 -eq "1" ]; then
		UDC=39110000.usb
	fi
	start
	;;
  stop)
	stop
	;;
  probe)
	probe
	;;
  UDC)
	echo ${UDC} > $CVI_GADGET/UDC
	;;
  *)
	echo "Usage: $0 probe {acm|msc|cvg|uvc|uac1|mtp} {file (msc)}"
	echo "Usage: $0 start"
	echo "Usage: $0 stop"
	exit 1
esac
exit $?
