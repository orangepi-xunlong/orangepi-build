#!/bin/bash
set -e

source /etc/orangepi-release

FLAG_FILE=/var/lib/overlay_done

if [ -f $FLAG_FILE ]; then
    exit 0
fi

ROOT_PART=$(findmnt -n -o SOURCE /)
echo "ROOT_PART=$ROOT_PART"

DISK=$(lsblk -no pkname "$ROOT_PART")
DISK="/dev/$DISK"
echo "DISK=$DISK"

PART="${DISK}p2"
echo "WRITE PART=$PART"

parted -s "$DISK" resizepart 2 100%
partprobe "$DISK"
sleep 1

echo "Expand EXT4..."
e2fsck -f "$PART" -y || true
sleep 1
resize2fs "$PART"

UUID=$(blkid -s UUID -o value "$PART")

#sed -i "s/^overlayroot=.*/overlayroot=\"\"/" /etc/overlayroot.conf
sed -i "s|^overlayroot=.*|overlayroot=\"device:dev=UUID=$UUID\"|" /etc/overlayroot.conf

touch $FLAG_FILE

echo "[OK].."

reboot
