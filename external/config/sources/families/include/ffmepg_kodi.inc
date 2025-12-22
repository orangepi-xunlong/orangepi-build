function ffmepg_kodi_install()
{

	chroot $SDCARD /bin/bash -c "apt-get -q update >/dev/null 2>&1"
	#chroot $SDCARD /bin/bash -c "apt-get install -q -y ffmpeg"
	#chroot $SDCARD /bin/bash -c "apt-get install -q -y kodi"
	#chroot $SDCARD /bin/bash -c "apt-get install -q -y libgles2 libjs-bootstrap lockfile-progs"
	chroot $SDCARD /bin/bash -c "apt-get -q update >/dev/null 2>&1"
	chroot $SDCARD /bin/bash -c "apt-get install -q -y ffmpeg kodi libgles2 libjs-bootstrap >/dev/null 2>&1"

	if [[ $RELEASE == bullseye ]]; then

		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavcodec58_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavdevice58_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavfilter7_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavformat58_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavresample4_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavutil56_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libpostproc55_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libswresample3_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libswscale5_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/ffmpeg_4.3.4-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/ffmpeg-doc_4.3.4-0+deb11u1_all.deb"

		kodiversion=19.1+dfsg2-2+deb11u1

	fi

	if [[ $RELEASE == jammy ]]; then

		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavcodec58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavdevice58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavfilter7_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavformat58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libavutil56_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libpostproc55_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libswresample3_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/libswscale5_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/ffmpeg_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/ffmpeg_kodi_$RELEASE/ffmpeg/ffmpeg-doc_4.4.2-0ubuntu0.22.04.1_all.deb"

		kodiversion=19.4+dfsg1-2

	fi

	dpkg_install_deb_chroot $EXTER/cache/sources/ffmpeg_kodi_${RELEASE}/kodi/kodi_${kodiversion}_arm64.deb
	dpkg_install_deb_chroot $EXTER/cache/sources/ffmpeg_kodi_${RELEASE}/kodi/kodi-bin_${kodiversion}_arm64.deb
	dpkg_install_deb_chroot $EXTER/cache/sources/ffmpeg_kodi_${RELEASE}/kodi/kodi-data_${kodiversion}_all.deb

	install_deb_chroot "$EXTER/cache/debs/arm64/usbmount_0.0.24_all.deb"

	cp $EXTER/packages/bsp/kodi/appliance.xml ${SDCARD}/usr/share/kodi/system/settings/appliance.xml

	rm $SDCARD/root/*.deb >/dev/null 2>&1

}
