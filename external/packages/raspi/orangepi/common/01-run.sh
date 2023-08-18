#!/bin/bash -e

#sed "s/# SKEL=.*/SKEL=\/etc\/skel/" -i "${ROOTFS_DIR}/etc/default/useradd"
#
#install -d				"${ROOTFS_DIR}/etc/systemd/system/rc-local.service.d"
#install -m 644 files/ttyoutput.conf	"${ROOTFS_DIR}/etc/systemd/system/rc-local.service.d/"
#
#on_chroot << EOF
#systemctl enable regenerate_ssh_host_keys
#EOF

install -m 644 files/hciattach_opi	"${ROOTFS_DIR}/usr/bin/"
install -m 644 files/brcm_patchram_plus	"${ROOTFS_DIR}/usr/bin/"
