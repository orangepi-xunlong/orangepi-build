#!/bin/bash -e

if [ "$RELEASE" != "raspi" ]; then
	echo "WARNING: RELEASE does not match the intended option for this branch."
	echo "         Please check the relevant README.md section."
fi

#if [ ! -d "${ROOTFS_DIR}" ] || [ "${USE_QCOW2}" = "1" ]; then
#	bootstrap ${RELEASE} "${ROOTFS_DIR}" https://mirrors.ustc.edu.cn/debian/
#	#bootstrap ${RELEASE} "${ROOTFS_DIR}" http://deb.debian.org/debian/
#fi
