rk3399_gpu_vpu_tweaks()
{
	if [[ "$BOARD" =~ orangepi4|orangepi800|orangepi4-lts && "$BRANCH" == legacy && $BUILD_DESKTOP == yes && ${RELEASE} =~ buster|bionic|focal ]]; then

		# Gstreamer library
		GST_DIR="$EXTER/cache/sources/rk3399_gst_xserver_libs/gstreamer"
		[[ ! -d "${SDCARD}"/usr/libexec ]] && mkdir -p "${SDCARD}"/usr/libexec

		if [[ $RELEASE != buster ]]; then

			cp ${GST_DIR}/${RELEASE}_build/etc/iqfiles             ${SDCARD}/etc/ -rfa
			cp ${GST_DIR}/${RELEASE}_build/usr/bin/*               ${SDCARD}/usr/bin/ -rfa
			cp ${GST_DIR}/${RELEASE}_build/usr/include/*           ${SDCARD}/usr/include/ -rfa
			cp ${GST_DIR}/${RELEASE}_build/usr/lib/*               ${SDCARD}/usr/lib/ -rfa
			cp ${GST_DIR}/${RELEASE}_build/usr/share/*             ${SDCARD}/usr/share/ -rfa
			cp ${GST_DIR}/overlay/etc/udev/rules.d/*               ${SDCARD}/etc/udev/rules.d/ -rfa
			cp ${GST_DIR}/overlay/usr/local/bin/*                  ${SDCARD}/usr/local/bin/ -rfa

			XSERVER_DIR="$EXTER/cache/sources/rk3399_gst_xserver_libs/xserver"
			cp ${XSERVER_DIR}/xserver_for_${RELEASE}/usr/bin/*               ${SDCARD}/usr/bin/ -rfa
			cp ${XSERVER_DIR}/xserver_for_${RELEASE}/usr/include/*           ${SDCARD}/usr/include/ -rfa
			cp ${XSERVER_DIR}/xserver_for_${RELEASE}/usr/lib/*               ${SDCARD}/usr/lib/ -rfa
			cp ${XSERVER_DIR}/xserver_for_${RELEASE}/usr/share/*             ${SDCARD}/usr/share/ -rfa
			cp ${XSERVER_DIR}/xserver_for_${RELEASE}/usr/libexec/*           ${SDCARD}/usr/libexec/ -rfa

		fi

		if [[ $RELEASE == bionic ]]; then

			cp ${GST_DIR}/${RELEASE}_build/usr/lib/gstreamer-1.0/*            ${SDCARD}/usr/lib/aarch64-linux-gnu/gstreamer-1.0/ -rfa

			#Mali GPU library
			install_deb_chroot "$EXTER/cache/debs/rk3399/libmali-rk-midgard-t86x-r14p0-r0p0_1.6-1-1armbian3_arm64.deb"

		elif [[ $RELEASE == xenial ]]; then

			# Mail GPU library
			sed 's/^#\(deb-src\)/\1/' -i ${SDCARD}/etc/apt/sources.list
			chroot $SDCARD /bin/bash -c "apt-get -q update >/dev/null 2>&1"
			chroot $SDCARD /bin/bash -c "apt-get -y build-dep xserver-xorg-core >/dev/null 2>&1"
			chroot $SDCARD /bin/bash -c "apt-get remove -y --purge libegl1-mesa-dev:arm64 libgbm-dev:arm64 >/dev/null 2>&1"
			chroot $SDCARD /bin/bash -c "apt-get install -q -y --no-install-recommends libxcb-xkb-dev libxfont-dev wayland-protocols >/dev/null 2>&1"
			install_deb_chroot "$EXTER/cache/debs/rk3399/libmali-rk-dev_1.6-2_arm64.deb"
			install_deb_chroot "$EXTER/cache/debs/rk3399/libmali-rk-midgard-t86x-r14p0_1.6-2_arm64.deb"
			rm -rf ${SDCARD}/usr/lib/aarch64-linux-gnu/mesa-egl

			# Gstreamer
			cp ${GST_DIR}/others/gstreamer/usr/bin/*               ${SDCARD}/usr/bin/ -rfa
			cp ${GST_DIR}/others/gstreamer/usr/include/*           ${SDCARD}/usr/include/ -rfa
			cp ${GST_DIR}/others/gstreamer/usr/lib/*               ${SDCARD}/usr/lib/ -rfa
			cp ${GST_DIR}/others/gstreamer/usr/libexec/*           ${SDCARD}/usr/libexec/ -rfa
			cp ${GST_DIR}/others/gstreamer/usr/local/*             ${SDCARD}/usr/local/ -rfa
			cp ${GST_DIR}/others/gstreamer/usr/share/*             ${SDCARD}/usr/share/ -rfa

		elif [[ $RELEASE == buster ]]; then

			#Multimedia Framework
			chroot $SDCARD /bin/bash -c "apt-get -q update >/dev/null 2>&1"
			chroot $SDCARD /bin/bash -c "apt-get remove -y --purge xserver-xorg-core >/dev/null 2>&1"
			chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends gstreamer1.0-plugins-ugly gstreamer1.0-plugins-bad \
				qtgstreamer-plugins-qt5	qtmultimedia5-examples mesa-utils fonts-liberation python-bluez python-lightblue python-pil \
				python-simplejson libshairplay0 libaacs0 libcec4 libfstrcmp0 libiso9660-11 liblirc-client0 libmariadb3 libmicrohttpd12 \
				libpcrecpp0v5 libdav1d4 libssh-4 libtag1v5 gstreamer1.0-plugins-good liblua5.2-0 librubberband2 gstreamer1.0-libav \
				gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 \
				gstreamer1.0-pulseaudio libjsoncpp1 libminizip1 libre2-5 libxss1 libxslt1.1 libv4l-0 xserver-common libpciaccess0 \
				libunwind8 libxfont2 libcdio-cdda2 libcdio-paranoia2 libuchardet0 libnspr4 libnss3 >/dev/null 2>&1"

			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libmali/libmali-midgard-t86x-r18p0-x11_1.9-1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga2_2.0.0-1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp1_1.4.0_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-vpu0_1.4.0_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-core-rockchip_1.20.3_arm64.deb"
			#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/gl4es_1.1.5_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/gst/gstreamer1.0-rockchip1_2.1.10_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/gst/gstreamer1.0-rockchip1-extra_3-20180115_arm64.deb"

			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/mpv-legacy_2%3a2020.12.08.df805cfc84_arm64.deb"
			echo "hwdec=rkmpp" > ${SDCARD}/etc/mpv/mpv.conf

			chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends xserver-xorg \
				xserver-xorg-video-fbdev xserver-xorg-input-all xserver-xorg-input-libinput  >/dev/null 2>&1"

			cp $EXTER/packages/bsp/rk3399/02-rockchip-accel.conf ${SDCARD}/etc/X11/xorg.conf.d/

			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rkisp/rkisp-engine_2.2.0_arm64.deb"
			install -m 0755 ${EXTER}/packages/bsp/rk3399/scripts/test_camera-gst.sh    	${SDCARD}/usr/local/bin/
			install -m 0755 ${EXTER}/packages/bsp/rk3399/scripts/test_camera-dual.sh    	${SDCARD}/usr/local/bin/
			install -m 0755 ${EXTER}/packages/bsp/rk3399/scripts/io   			${SDCARD}/usr/local/bin/

			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/glmark2/glmark2-data_2021.02+ds-1_all.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/glmark2/glmark2-es2-x11_2021.02+ds-1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/ir-keytable_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-0_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-dev_1.16.3-3_arm64.deb"

			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l2rds0_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4lconvert0_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-rkmpp_1.3.2-1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/dvb-tools_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-0_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-dev_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/qv4l2_1.16.3-3_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/v4l-utils_1.16.3-3_arm64.deb"

			dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/chromium/chromium-x11_91.0.4472.164_arm64.deb"

			# The chromium using fixed pathes for libv4l2.so
			chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
			chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		fi
	fi

	if [[ ${BOARD} =~ orangepi800 && ${SELECTED_CONFIGURATION} == desktop && ${RELEASE} == bookworm ]]; then

		chroot $SDCARD /bin/bash -c "apt-get remove -y --purge xserver-xorg-core >/dev/null 2>&1"
		chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends libdrm-dev libqt5opengl5 \
					libgdk-pixbuf2.0-0 libminizip1 libre2-9 >/dev/null 2>&1"

		# RK399 GPU and VPU
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga2_2.1.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga-dev_2.1.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libmali/libmali-midgard-t86x-r18p0-x11_1.9-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-common_1.20.11-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-core_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-legacy_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/glmark2/glmark2-data_2021.02+ds-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/glmark2/glmark2-es2-x11_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp1_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp-dev_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-vpu0_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/rockchip-mpp-demos_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/gst-base/gstreamer1.0-x_1.14.4-2+deb10u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/ffmpeg_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavutil56_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavutil-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libaom0_1.0.0.errata1-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libcodec2-0.9_0.9.2-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavcodec58_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavdevice58_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswresample3_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswresample-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavcodec-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavfilter7_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavformat58_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavformat-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libpostproc55_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libpostproc-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswscale5_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswscale-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavfilter-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavdevice-dev_4.4-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libplacebo120_3.120.3-dmo1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libmpv1_0.33.1-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libmpv-dev_0.33.1-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/mpv_0.33.1-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-doc_1.20.0-2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/dvb-tools_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4lconvert0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l2rds0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-rkmpp_1.3.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/v4l-utils_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/chromium/chromium-x11_91.0.4472.164_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libdrm-cursor/libdrm-cursor_1.3.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libdrm-cursor/libdrm-cursor-dev_1.3.0-1_arm64.deb"

		chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends xserver-xorg \
			xserver-xorg-video-fbdev xserver-xorg-input-all xserver-xorg-input-libinput  >/dev/null 2>&1"

		cp $EXTER/packages/bsp/rk3399/02-modesetting.conf ${SDCARD}/etc/X11/xorg.conf.d/
		cp $EXTER/packages/bsp/rk3399/mpv.conf ${SDCARD}/etc/mpv/

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

	fi

	if [[ ${BOARD} =~ orangepi4|orangepi4-lts|orangepi800 && ${SELECTED_CONFIGURATION} == desktop && ${RELEASE} == bullseye ]]; then

		chroot $SDCARD /bin/bash -c "apt-get remove -y --purge xserver-xorg-core >/dev/null 2>&1"
		chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends libchromaprint1 libgme0 \
			libopenmpt0 librabbitmq4 libsrt1.4-gnutls libssh-gcrypt-4 libzmq5 libass9 libbs2b0 \
			libflite1 liblilv-0-0 libmysofa1 libpocketsphinx3 libsphinxbase3 librubberband2 \
			libvidstab1.1 libcdio-cdda2 libcdio-paranoia2 libdc1394-25 libopenal1 libsdl2-2.0-0 \
			libsndio7.0 libxv1 libdvdnav4 liblua5.2-0 libva-wayland2 libxslt1.1 libgles2 libjsoncpp24 \
			libdrm-dev libqt5opengl5 libgdk-pixbuf2.0-0 libminizip1 libre2-9 >/dev/null 2>&1"

		# RK GPU VPU
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga2_2.1.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga-dev_2.1.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libmali/libmali-midgard-t86x-r18p0-x11_1.9-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-common_1.20.11-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-core_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-legacy_1.20.11-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-core-rockchip_1.20.3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/glmark2/glmark2-data_2021.02+ds-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/glmark2/glmark2-es2-x11_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp1_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp-dev_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-vpu0_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/rockchip-mpp-demos_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libaom0_1.0.0.errata1-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libcodec2-0.9_0.9.2-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavutil56_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavutil-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libpostproc55_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libpostproc-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswresample3_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswresample-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswscale5_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswscale-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavcodec58_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavcodec-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavformat58_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavformat-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavresample4_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavresample-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavfilter7_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavfilter-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavdevice58_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavdevice-dev_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/ffmpeg_4.3.2-0+deb11u2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libmpv1_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libmpv-dev_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/mpv_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libdvbv5-doc_1.20.0-2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/dvb-tools_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4lconvert0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l2rds0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/libv4l-rkmpp_1.3.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libv4l/v4l-utils_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/chromium/chromium-x11_91.0.4472.164_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libdrm-cursor/libdrm-cursor_1.3.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libdrm-cursor/libdrm-cursor-dev_1.3.0-1_arm64.deb"

		chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends xserver-xorg \
			xserver-xorg-video-fbdev xserver-xorg-input-all xserver-xorg-input-libinput  >/dev/null 2>&1"

		cp $EXTER/packages/bsp/rk3399/02-modesetting.conf ${SDCARD}/etc/X11/xorg.conf.d/
		cp $EXTER/packages/bsp/rk3399/mpv.conf ${SDCARD}/etc/mpv/

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

	fi

	if [[ ${BOARD} =~ orangepi4|orangepi4-lts|orangepi800 && ${SELECTED_CONFIGURATION} == desktop && ${RELEASE} == focal ]]; then

		#chroot $SDCARD /bin/bash -c "apt-get remove -y --purge xserver-xorg-core >/dev/null 2>&1"
		chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends libdrm-dev \
                        ocl-icd-libopencl1 libgsm1 libshine3 libsnappy1v5 libx264-155 libx265-179 \
                        libxvidcore4 libzvbi0 libbluray2 libchromaprint1 libgme0 libopenmpt0 libssh-gcrypt-4 \
                        libass9 libbs2b0 libflite1 liblilv-0-0 libmysofa1 librubberband2 libvidstab1.1 \
                        libzmq5 libdc1394-22 libopenal1 libsdl2-2.0-0 libsndio7.0 libxv1 libdvdnav4 \
                        liblua5.2-0 libva-wayland2 >/dev/null 2>&1"

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga2_2.1.0-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/rga/librga-dev_2.1.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libmali/libmali-midgard-t86x-r18p0-x11_1.9-1_arm64.deb"
		##dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-common_1.20.13-1ubuntu1~20.04.2_all.deb"
		##dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-core_1.20.13-1ubuntu1~20.04.2_arm64.deb"
		##dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-legacy_1.20.13-1ubuntu1~20.04.2_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/xserver/xserver-xorg-core-rockchip_1.20.3_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp1_1.5.0-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-mpp-dev_1.5.0-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/librockchip-vpu0_1.5.0-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpp/rockchip-mpp-demos_1.5.0-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libaom0_1.0.0.errata1-3_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libcodec2-0.9_0.9.2-4_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavutil56_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavutil-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libpostproc55_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libpostproc-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswresample3_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswresample-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswscale5_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libswscale-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavcodec58_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavcodec-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavformat58_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavformat-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavresample4_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavresample-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavfilter7_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavfilter-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavdevice58_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/libavdevice-dev_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/ffmpeg/ffmpeg_4.2.4-1ubuntu0.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libmpv1_0.32.0-1ubuntu1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/libmpv-dev_0.32.0-1ubuntu1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/mpv/mpv_0.32.0-1ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libdrm-cursor/libdrm-cursor_1.3.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk-rootfs-build-${RELEASE}/libdrm-cursor/libdrm-cursor-dev_1.3.0-1_arm64.deb"

		#chroot $SDCARD /bin/bash -c "apt install -q -y --no-install-recommends xserver-xorg \
		#	xserver-xorg-video-fbdev xserver-xorg-input-all xserver-xorg-input-libinput  >/dev/null 2>&1"
	fi
}
