#!/bin/sh

reboot_pi () {
  umount /boot
  mount / -o remount,ro
  sync
  if [ "$NOOBS" = "1" ]; then
    if [ "$NEW_KERNEL" = "1" ]; then
      reboot -f "$BOOT_PART_NUM"
      sleep 5
    else
      echo "$BOOT_PART_NUM" > "/sys/module/${BCM_MODULE}/parameters/reboot_part"
    fi
  fi
  reboot -f
  sleep 5
  exit 0
}

check_commands () {
  if ! command -v whiptail > /dev/null; then
      echo "whiptail not found"
      sleep 5
      return 1
  fi
  for COMMAND in grep cut sed parted fdisk findmnt; do
    if ! command -v $COMMAND > /dev/null; then
      FAIL_REASON="$COMMAND not found"
      return 1
    fi
  done
  return 0
}

check_noobs () {
  if [ "$BOOT_PART_NUM" = "1" ]; then
    NOOBS=0
  else
    NOOBS=1
  fi
}

get_variables () {
  ROOT_PART_DEV=$(findmnt / -o source -n)
  ROOT_PART_NAME=$(echo "$ROOT_PART_DEV" | cut -d "/" -f 3)
  ROOT_DEV_NAME=$(echo /sys/block/*/"${ROOT_PART_NAME}" | cut -d "/" -f 4)
  ROOT_DEV="/dev/${ROOT_DEV_NAME}"
  ROOT_PART_NUM=$(cat "/sys/block/${ROOT_DEV_NAME}/${ROOT_PART_NAME}/partition")

  BOOT_PART_DEV=$(findmnt /boot -o source -n)
  BOOT_PART_NAME=$(echo "$BOOT_PART_DEV" | cut -d "/" -f 3)
  BOOT_DEV_NAME=$(echo /sys/block/*/"${BOOT_PART_NAME}" | cut -d "/" -f 4)
  BOOT_PART_NUM=$(cat "/sys/block/${BOOT_DEV_NAME}/${BOOT_PART_NAME}/partition")

  OLD_DISKID=$(fdisk -l "$ROOT_DEV" | sed -n 's/Disk identifier: 0x\([^ ]*\)/\1/p')

  check_noobs

  ROOT_DEV_SIZE=$(cat "/sys/block/${ROOT_DEV_NAME}/size")
  TARGET_END=$((ROOT_DEV_SIZE - 1))

  PARTITION_TABLE=$(parted -m "$ROOT_DEV" unit s print | tr -d 's')

  LAST_PART_NUM=$(echo "$PARTITION_TABLE" | tail -n 1 | cut -d ":" -f 1)

  ROOT_PART_LINE=$(echo "$PARTITION_TABLE" | grep -e "^${ROOT_PART_NUM}:")
  ROOT_PART_START=$(echo "$ROOT_PART_LINE" | cut -d ":" -f 2)
  ROOT_PART_END=$(echo "$ROOT_PART_LINE" | cut -d ":" -f 3)

  if [ "$NOOBS" = "1" ]; then
    EXT_PART_LINE=$(echo "$PARTITION_TABLE" | grep ":::;" | head -n 1)
    EXT_PART_NUM=$(echo "$EXT_PART_LINE" | cut -d ":" -f 1)
    EXT_PART_START=$(echo "$EXT_PART_LINE" | cut -d ":" -f 2)
    EXT_PART_END=$(echo "$EXT_PART_LINE" | cut -d ":" -f 3)
  fi
}

fix_partuuid() {
  mount -o remount,rw "$ROOT_PART_DEV"
  mount -o remount,rw "$BOOT_PART_DEV"
  DISKID="$(tr -dc 'a-f0-9' < /dev/hwrng | dd bs=1 count=8 2>/dev/null)"
  fdisk "$ROOT_DEV" > /dev/null <<EOF
x
i
0x$DISKID
r
w
EOF
  if [ "$?" -eq 0 ]; then
    sed -i "s/${OLD_DISKID}/${DISKID}/g" /etc/fstab
    sed -i "s/${OLD_DISKID}/${DISKID}/" /boot/cmdline.txt
    sync
  fi

  mount -o remount,ro "$ROOT_PART_DEV"
  mount -o remount,ro "$BOOT_PART_DEV"
}

fix_wpa() {
  if [ -e /boot/firstrun.sh ] \
     && ! grep -q 'imager_custom set_wlan' /boot/firstrun.sh \
     && grep -q wpa_supplicant.conf /boot/firstrun.sh; then
    mount -o remount,rw "$ROOT_PART_DEV"
    modprobe rfkill
    REGDOMAIN=$(sed -n 's/^\s*country=\(..\)$/\1/p' /boot/firstrun.sh)
    [ -n "$REGDOMAIN" ] && raspi-config nonint do_wifi_country "$REGDOMAIN"
    if systemctl -q is-enabled NetworkManager; then
      systemctl disable NetworkManager
    fi
    mount -o remount,ro "$ROOT_PART_DEV"
  fi
}

check_variables () {
  if [ "$NOOBS" = "1" ]; then
    if [ "$EXT_PART_NUM" -gt 4 ] || \
       [ "$EXT_PART_START" -gt "$ROOT_PART_START" ] || \
       [ "$EXT_PART_END" -lt "$ROOT_PART_END" ]; then
      FAIL_REASON="Unsupported extended partition"
      return 1
    fi
  fi

  if [ "$BOOT_DEV_NAME" != "$ROOT_DEV_NAME" ]; then
      FAIL_REASON="Boot and root partitions are on different devices"
      return 1
  fi

  if [ "$ROOT_PART_NUM" -ne "$LAST_PART_NUM" ]; then
    FAIL_REASON="Root partition should be last partition"
    return 1
  fi

  if [ "$ROOT_PART_END" -gt "$TARGET_END" ]; then
    FAIL_REASON="Root partition runs past the end of device"
    return 1
  fi

  if [ ! -b "$ROOT_DEV" ] || [ ! -b "$ROOT_PART_DEV" ] || [ ! -b "$BOOT_PART_DEV" ] ; then
    FAIL_REASON="Could not determine partitions"
    return 1
  fi
}

check_kernel () {
  MAJOR="$(uname -r | cut -f1 -d.)"
  MINOR="$(uname -r | cut -f2 -d.)"
  if [ "$MAJOR" -eq "4" ] && [ "$MINOR" -lt "9" ]; then
    return 0
  fi
  if [ "$MAJOR" -lt "4" ]; then
    return 0
  fi
  NEW_KERNEL=1
}

main () {
  get_variables

  if ! check_variables; then
    return 1
  fi

  # Switch to dhcpcd here if Imager < v1.7.3 was used to generate firstrun.sh
  fix_wpa > /dev/null 2>&1

  check_kernel

  if [ "$NOOBS" = "1" ] && [ "$NEW_KERNEL" != "1" ]; then
    BCM_MODULE=$(grep -e "^Hardware" /proc/cpuinfo | cut -d ":" -f 2 | tr -d " " | tr '[:upper:]' '[:lower:]')
    if ! modprobe "$BCM_MODULE"; then
      FAIL_REASON="Couldn't load BCM module $BCM_MODULE"
      return 1
    fi
  fi

  if [ "$ROOT_PART_END" -eq "$TARGET_END" ]; then
    reboot_pi
  fi

  if [ "$NOOBS" = "1" ]; then
    if ! printf "resizepart %s\nyes\n%ss\n" "$EXT_PART_NUM" "$TARGET_END" | parted "$ROOT_DEV" ---pretend-input-tty; then
      FAIL_REASON="Extended partition resize failed"
      return 1
    fi
  fi

  if ! parted -m "$ROOT_DEV" u s resizepart "$ROOT_PART_NUM" "$TARGET_END"; then
    FAIL_REASON="Root partition resize failed"
    return 1
  fi

  fix_partuuid

  return 0
}

mount -t proc proc /proc
mount -t sysfs sys /sys
mount -t tmpfs tmp /run
mkdir -p /run/systemd

mount /boot
mount / -o remount,ro

sed -i 's| init=/usr/lib/raspi-config/init_resize\.sh||' /boot/cmdline.txt
sed -i 's| sdhci\.debug_quirks2=4||' /boot/cmdline.txt

if ! grep -q splash /boot/cmdline.txt; then
  sed -i "s/ quiet//g" /boot/cmdline.txt
fi
mount /boot -o remount,ro
sync

if ! check_commands; then
  reboot_pi
fi

if main; then
  whiptail --infobox "Resized root filesystem. Rebooting in 5 seconds..." 20 60
  sleep 5
else
  whiptail --msgbox "Could not expand filesystem, please try raspi-config or rc_gui.\n${FAIL_REASON}" 20 60
  sleep 5
fi

reboot_pi
