#!/bin/sh
# Copy firmware file to initrd
#

mkdir -p "${DESTDIR}"/lib/firmware
cp -rf /lib/firmware/rgx.* "${DESTDIR}"/lib/firmware

exit 0
