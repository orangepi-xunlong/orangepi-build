function ffmepg_kodi_install()
{

	#chroot $SDCARD /bin/bash -c "apt-get clean"
	#chroot $SDCARD /bin/bash -c "mv /var/lib/apt/lists /tmp"
	#chroot $SDCARD /bin/bash -c "mkdir -p /var/lib/apt/lists/partial"
	#chroot $SDCARD /bin/bash -c "apt-get clean"

	chroot $SDCARD /bin/bash -c "apt-get -q update"
	chroot $SDCARD /bin/bash -c "apt-get install -q -y ffmpeg kodi libgles2 libjs-bootstrap lockfile-progs"
	#chroot $SDCARD /bin/bash -c "apt-get -q update >/dev/null 2>&1"
	#chroot $SDCARD /bin/bash -c "apt-get install -q -y ffmpeg kodi libgles2 libjs-bootstrap >/dev/null 2>&1"

	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libavcodec58_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libavdevice58_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libavfilter7_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libavformat58_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libavresample4_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libavutil56_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libpostproc55_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libswresample3_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/libswscale5_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/ffmpeg_4.3.3-0+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/ffmpeg/ffmpeg-doc_4.3.3-0+deb11u1_all.deb"

	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/kodi/kodi_19.1+dfsg2-2+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/kodi/kodi-bin_19.1+dfsg2-2+deb11u1_arm64.deb"
	dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_debian11/kodi/kodi-data_19.1+dfsg2-2+deb11u1_all.deb"

	install_deb_chroot "$EXTER/cache/debs/arm64/usbmount_0.0.24_all.deb"

	cp $EXTER/packages/bsp/sunxi/appliance.xml ${SDCARD}/usr/share/kodi/system/settings/appliance.xml

	rm $SDCARD/root/*.deb >/dev/null 2>&1

}
