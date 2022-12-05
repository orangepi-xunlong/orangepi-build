#!/bin/bash

mmc_dev=$(ls -d -1 /dev/mmcblk* | grep -w 'mmcblk[0-9]' | cut -d '/' -f3)
sed -i "s/^rootdev=.*/rootdev=\/dev\/${mmc_dev}p2/" /boot/orangepiEnv.txt
sed -i '/boot/d' /etc/fstab
echo "/dev/${mmc_dev}p1 /boot vfat defaults 0 2" >> /etc/fstab

echo "Done"
