#!/bin/sh
# Copy splash file to initrd
#
mkdir -p "${DESTDIR}"/lib/firmware
splashfile=/lib/firmware/bootsplash.orangepi

if [ -f "${splashfile}" ]; then
	cp "${splashfile}" "${DESTDIR}"/lib/firmware
fi

exit 0
