rk3588_gpu_vpu_tweaks()
{
	if [[ ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} =~ xfce|kde-plasma && ${RELEASE} == bullseye ]]; then

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"

		#chroot $SDCARD /bin/bash -c "apt-get update"
		#chroot $SDCARD /bin/bash -c "apt-get upgrade -y"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/camera-engine-rkaiq_rk3588_arm64.deb"
		cp "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/rkaiq_3A_server" $SDCARD/usr/bin/ -f

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rga2/librga2_2.2.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rga2/librga-dev_2.2.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/librockchip-mpp1_1.5.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/librockchip-mpp-dev_1.5.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/librockchip-vpu0_1.5.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/rockchip-mpp-demos_1.5.0-2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer/gir1.2-gstreamer-1.0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer/gstreamer1.0-tools_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer/libgstreamer1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer/libgstreamer1.0-dev_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-alsa_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-gl_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-x_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-opencv_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-wpe_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-gtk3_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-qt5_1.18.5-1_arm64.deb"
	
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-ugly1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb"
	
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-libav1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libdvbv5-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/dvb-tools_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/ir-keytable_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libdvbv5-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libdvbv5-doc_1.20.0-2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l2rds0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4lconvert0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l-rkmpp_1.5.1-1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/v4l-utils_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/qv4l2_1.20.0-2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xorg-core_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-common_1.20.11-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xorg-dev_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xorg-legacy_1.20.11-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium/chromium-codecs-ffmpeg-extra_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium/chromium-browser_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb"
		cp $EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium/default $SDCARD/etc/chromium-browser/

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libdrm/libkms1_2.4.104-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-data_2021.02+ds-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-drm_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-es2-drm_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-es2-wayland_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-es2-x11_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-wayland_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/glmark2/glmark2-x11_2021.02+ds-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rktoolkit/rktoolkit_1.0.0-1_arm64.deb"

		install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libmali/libmali-valhall-g610-g6p0-x11_1.9-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavutil56_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavutil-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libpostproc55_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libpostproc-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswresample3_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswresample-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswscale5_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswscale-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavcodec58_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavcodec-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavformat58_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavformat-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavresample4_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavresample-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavfilter7_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavfilter-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavdevice58_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavdevice-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/ffmpeg_4.3.5-0+deb11u1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpv/libmpv1_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpv/libmpv-dev_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpv/mpv_0.32.0-3_arm64.deb"

		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/zfs/libnvpair3linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/zfs/libuutil3linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/zfs/libzfs4linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/zfs/libzpool5linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/zfs/zfsutils-linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/zfs/zfs-dkms_2.1.11-1~bpo11+1_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
			chroot $SDCARD /bin/bash -c "rm -r /usr/src/linux-headers-5.10.110-rockchip-rk3588"
		fi
	fi

	if [[ ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} =~ xfce|kde-plasma && ${RELEASE} == bookworm ]]; then


		#chroot $SDCARD /bin/bash -c "apt-get update"
		#chroot $SDCARD /bin/bash -c "apt-get install -y"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/camera-engine-rkaiq_rk3588_arm64.deb"
		cp "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/rkaiq_3A_server" $SDCARD/usr/bin/ -f

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rga2/librga2_2.1.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rga2/librga-dev_2.1.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/librockchip-mpp1_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/librockchip-mpp-dev_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/librockchip-vpu0_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpp/rockchip-mpp-demos_1.5.0-1.2_arm64.deb"

		install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libmali/libmali-valhall-g610-g6p0-x11_1.9-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer-rockchip/gstreamer1.0-rockchip1_1.14-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer1.0/gir1.2-gstreamer-1.0_1.22.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer1.0/gstreamer1.0-tools_1.22.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer1.0/libgstreamer1.0-0_1.22.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gstreamer1.0/libgstreamer1.0-dev_1.22.0-3_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-alsa_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-gl_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/gstreamer1.0-x_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.22.0-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.22.0-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-opencv_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-wpe_1.22.0-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.22.0-5_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-gtk3_1.22.0-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.22.0-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.22.0-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-qt5_1.22.0-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/gst-plugins-good1.0/gstreamer1.0-qt6_1.22.0-6_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libdvbv5-0_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/dvb-tools_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/ir-keytable_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libdvbv5-dev_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libdvbv5-doc_1.22.1-6_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4lconvert0_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l-0_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l2rds0_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/libv4l-dev_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/v4l-utils_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/qv4l2_1.22.1-6_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l/qvidcap_1.22.1-6_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libv4l-rkmpp/libv4l-rkmpp_1.5.1-1.1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-common_21.1.7-4_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xorg-core_21.1.7-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xorg-dev_21.1.7-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xorg-legacy_21.1.7-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xnest_21.1.7-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xserver-xephyr_21.1.7-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/xserver/xvfb_21.1.7-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium/chromium-codecs-ffmpeg-extra_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium/chromium-browser_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb"
		cp $EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium/default $SDCARD/etc/chromium-browser/

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rktoolkit/rktoolkit_1.0.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavutil57_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavutil-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libpostproc56_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libpostproc-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswresample4_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswresample-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswscale6_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libswscale-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavcodec59_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavcodec-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavformat59_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavformat-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavfilter8_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavfilter-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavdevice59_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/libavdevice-dev_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/ffmpeg_5.1.3-4_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/ffmpeg/ffmpeg-doc_5.1.3-4_all.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpv/libmpv2_0.35.1-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpv/libmpv-dev_0.35.1-5_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/mpv/mpv_0.35.1-5_arm64.deb"

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			run_on_sdcard "DEBIAN_FRONTEND=noninteractive apt-get install -yqq zfsutils-linux zfs-dkms"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
			chroot $SDCARD /bin/bash -c "rm -r /usr/src/linux-headers-5.10.110-rockchip-rk3588"
		fi
	fi

	if [[ ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} =~ xfce|kde-plasma && ${RELEASE} == jammy ]]; then

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"

		#chroot $SDCARD /bin/bash -c "apt-get update"
		#chroot $SDCARD /bin/bash -c "apt-get upgrade -y" > /dev/null
		chroot $SDCARD /bin/bash -c "apt-get install -y libgraphene-1.0-0 libqt5waylandclient5 libgdk-pixbuf2.0-0 libjsoncpp-dev" > /dev/null

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rkaiq/camera-engine-rkaiq_rk3588_arm64.deb"
		cp "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/rkaiq_3A_server" $SDCARD/usr/bin/ -f

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rga2/librga2_2.2.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rga2/librga-dev_2.2.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/librockchip-mpp1_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/librockchip-mpp-dev_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/librockchip-vpu0_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/rockchip-mpp-demos_1.5.0-1.2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/gir1.2-gstreamer-1.0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/gstreamer1.0-tools_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/libgstreamer1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/libgstreamer1.0-dev_1.20.3-0ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-alsa_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-gl_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-x_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.20.3-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-opencv_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-wpe_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.20.3-0ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-gtk3_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-qt5_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/libgstreamer-plugins-good1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/libgstreamer-plugins-good1.0-dev_1.20.3-0ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libdvbv5-0_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/dvb-tools_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/ir-keytable_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libdvbv5-dev_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libdvbv5-doc_1.22.1-2build1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l-0_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l2rds0_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4lconvert0_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l-dev_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l-rkmpp_1.5.1-1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/v4l-utils_1.22.1-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/qv4l2_1.22.1-2build1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-xorg-core_21.1.3-2ubuntu2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-common_21.1.3-2ubuntu2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-xorg-dev_21.1.3-2ubuntu2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-xorg-legacy_21.1.3-2ubuntu2_arm64.deb"

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/chromium/chromium-x11_91.0.4472.164_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libdrm/libkms1_2.4.110-1ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rktoolkit/rktoolkit_1.0.0-1_arm64.deb"

		install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libmali/libmali-valhall-g610-g6p0-x11_1.9-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavutil56_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavutil-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libpostproc55_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libpostproc-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswresample3_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswresample-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswscale5_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswscale-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavcodec58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavcodec-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavformat58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavformat-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavfilter7_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavfilter-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavdevice58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavdevice-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/ffmpeg_4.4.2-0ubuntu0.22.04.1_arm64.deb"

		chroot $SDCARD /bin/bash -c "rm /etc/mpv/mpv.conf"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpv/libmpv1_0.34.1-1ubuntu3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpv/libmpv-dev_0.34.1-1ubuntu3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpv/mpv_0.34.1-1ubuntu3_arm64.deb"
		cp ${EXTER}/packages/bsp/rk3588/etc/mpv/mpv.conf $SDCARD/etc/mpv/

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/chromium/chromium-x11_91.0.4472.164_arm64.deb"
		#cp ${EXTER}/cache/sources/rk3588_packages_jammy/chromium/libjpeg.so.62 $SDCARD/usr/lib/aarch64-linux-gnu/

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium-x11/chromium-codecs-ffmpeg-extra_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium-x11/chromium-browser_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb"
		cp $EXTER/cache/sources/rk3588_packages_${RELEASE}/chromium-x11/default $SDCARD/etc/chromium-browser/

		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		## The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libnvpair3linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libuutil3linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libzfs4linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libzpool5linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/zfsutils-linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/zfs-dkms_2.1.6-0york1~22.04_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
			chroot $SDCARD /bin/bash -c "rm -r /usr/src/linux-headers-5.10.110-rockchip-rk3588"
		fi
	fi

	if [[ ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} == xfce && ${RELEASE} == focal ]]; then

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"

		#chroot $SDCARD /bin/bash -c "apt-get update"
		#chroot $SDCARD /bin/bash -c "apt-get upgrade -y" > /dev/null
		#chroot $SDCARD /bin/bash -c "apt-get install -y " > /dev/null

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/rkaiq/camera-engine-rkaiq_arm64.deb"
		cp "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/rkaiq_3A_server" $SDCARD/usr/bin/ -f

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/rga/librga2_2.2.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/rga/librga-dev_2.2.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpp/librockchip-mpp1_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpp/librockchip-mpp-dev_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpp/librockchip-vpu0_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpp/rockchip-mpp-demos_1.5.0-1_arm64.deb"

		install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libmali/libmali-valhall-g610-g6p0-x11_1.9-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gstreamer/libgstreamer1.0-0_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gstreamer/libgstreamer1.0-dev_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gstreamer/gir1.2-gstreamer-1.0_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gstreamer/gstreamer1.0-tools_1.16.3-0ubuntu1.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gstreamer/gstreamer1.0-doc_1.16.3-0ubuntu1.1_all.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gstreamer1.0-alsa_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gstreamer1.0-x_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gstreamer1.0-gl_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.16.3-0ubuntu1.1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-base1.0/gstreamer1.0-plugins-base-doc_1.16.3-0ubuntu1.1_all.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-good1.0/gstreamer1.0-gtk3_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-good1.0/gstreamer1.0-qt5_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-good1.0/libgstreamer-plugins-good1.0-0_1.16.3-0ubuntu1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-good1.0/libgstreamer-plugins-good1.0-dev_1.16.3-0ubuntu1.1_arm64.deb"

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.18.5-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libdvbv5-0_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/dvb-tools_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/ir-keytable_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libdvbv5-dev_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libdvbv5-doc_1.18.0-2build1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libv4l-0_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libv4l2rds0_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libv4lconvert0_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libv4l-dev_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/libv4l-rkmpp_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/v4l-utils_1.18.0-2build1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libv4l/qv4l2_1.18.0-2build1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/xserver/xserver-xorg-core_1.20.13-1ubuntu1~20.04.2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/chromium/chromium-x11_91.0.4472.164_arm64.deb"
		cp ${EXTER}/cache/sources/rk3588_packages_focal/chromium/libjpeg.so.62 $SDCARD/usr/lib/aarch64-linux-gnu/

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libdrm/libdrm2_2.4.107-8ubuntu1~20.04.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libdrm/libdrm-common_2.4.107-8ubuntu1~20.04.2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libdrm/libkms1_2.4.107-8ubuntu1~20.04.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libdrm/libdrm-dev_2.4.107-8ubuntu1~20.04.2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb"
		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/rktoolkit/rktoolkit_1.0.0-1_arm64.deb"

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/gl4es/gl4es_1.1.5-0armbian1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavutil56_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavutil-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libpostproc55_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libpostproc-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libswresample3_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libswresample-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libswscale5_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libswscale-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavcodec58_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavcodec-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavformat58_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavformat-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavfilter7_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavfilter-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavdevice58_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavdevice-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavresample4_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/libavresample-dev_4.2.7-0ubuntu0.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/ffmpeg/ffmpeg_4.2.7-0ubuntu0.1_arm64.deb"

		chroot $SDCARD /bin/bash -c "rm /etc/mpv/mpv.conf"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpv/libmpv1_0.32.0-1ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpv/libmpv-dev_0.32.0-1ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/mpv/mpv_0.32.0-1ubuntu1_arm64.deb"
		cp ${EXTER}/packages/bsp/rk3588/etc/mpv/mpv.conf $SDCARD/etc/mpv/

		## The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/zfs/libnvpair3linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/zfs/libuutil3linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/zfs/libzfs4linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/zfs/libzpool5linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/zfs/zfsutils-linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_focal/zfs/zfs-dkms_2.1.6-0york1~20.04_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
			chroot $SDCARD /bin/bash -c "rm -r /usr/src/linux-headers-5.10.110-rockchip-rk3588"
		fi

	fi


	if [[ ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} =~ gnome && ${RELEASE} == jammy ]]; then

		[[ -f $SDCARD/etc/profile.d/x11.sh ]] && rm $SDCARD/etc/profile.d/x11.sh

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rkaiq/camera-engine-rkaiq_rk3588_arm64.deb"
		cp "$EXTER/cache/sources/rk3588_packages_${RELEASE}/rkaiq/rkaiq_3A_server" $SDCARD/usr/bin/ -f

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rga2/librga2_2.2.0-1+git220920.c1d5c71~jammy_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rga2/librga-dev_2.2.0-1+git220920.c1d5c71~jammy_arm64.deb"


		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/librockchip-mpp1_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/librockchip-mpp-dev_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/librockchip-vpu0_1.5.0-1.2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpp/rockchip-mpp-demos_1.5.0-1.2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/gir1.2-gstreamer-1.0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/gstreamer1.0-tools_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/libgstreamer1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gstreamer/libgstreamer1.0-dev_1.20.3-0ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-alsa_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-gl_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/gstreamer1.0-x_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.20.3-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.20.3-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-opencv_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/gstreamer1.0-wpe_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.20.3-0ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-gtk3_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/gstreamer1.0-qt5_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/libgstreamer-plugins-good1.0-0_1.20.3-0ubuntu1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/gst-plugins-good1.0/libgstreamer-plugins-good1.0-dev_1.20.3-0ubuntu1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libdvbv5-0_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/dvb-tools_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/ir-keytable_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libdvbv5-dev_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libdvbv5-doc_1.22.1-2build1+rkmpp20221209_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4lconvert0_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l-0_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l2rds0_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l-dev_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/v4l-utils_1.22.1-2build1+rkmpp20221209_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/qv4l2_1.22.1-2build1+rkmpp20221209_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/libv4l-rkmpp_1.5.1-1.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libv4l/rockchip-multimedia-config_1.0.0-2+jammy_all.deb"

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-xorg-core_21.1.3-2ubuntu2_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-common_21.1.3-2ubuntu2_all.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-xorg-dev_21.1.3-2ubuntu2_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/xserver/xserver-xorg-legacy_21.1.3-2ubuntu2_arm64.deb"

		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libdrm/libkms1_2.4.110-1ubuntu1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/chromium/chromium-codecs-ffmpeg-extra_110.0.5481.4-0ubuntu1~jammy~rkmpp20230130_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/chromium/chromium-browser_110.0.5481.4-0ubuntu1~jammy~rkmpp20230130_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/chromium/chromium-browser-l10n_110.0.5481.4-0ubuntu1~jammy~rkmpp20230130_all.deb"
		#dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/chromium/chromium-chromedriver_110.0.5481.4-0ubuntu1~jammy~rkmpp20230130_arm64.deb"
		cp $EXTER/cache/sources/rk3588_packages_jammy/chromium/default $SDCARD/etc/chromium-browser/

		## for panfork
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/mali-g610-firmware_1.0.0-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/libgbm1_23.0~panfork~csf~git221210.120202c6757~j+3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/libgbm-dev_23.0~panfork~csf~git221210.120202c6757~j+3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/libglapi-mesa_23.0~panfork~csf~git221210.120202c6757~j+3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/libgl1-mesa-dri_23.0~panfork~csf~git221210.120202c6757~j+3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/libegl-mesa0_23.0~panfork~csf~git221210.120202c6757~j+3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mesa/libglx-mesa0_23.0~panfork~csf~git221210.120202c6757~j+3_arm64.deb"
		# Improve mesa performance
		echo "PAN_MESA_DEBUG=gofaster" >> $SDCARD/etc/environment

		## for ffmpeg
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavutil56_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavutil-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libpostproc55_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libpostproc-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswresample3_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswresample-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswscale5_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libswscale-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavcodec58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavcodec-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavformat58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavformat-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavfilter7_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavfilter-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavdevice58_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/libavdevice-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/ffmpeg/ffmpeg_4.4.2-0ubuntu0.22.04.1_arm64.deb"

		chroot $SDCARD /bin/bash -c "rm /etc/mpv/mpv.conf"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpv/libmpv1_0.34.1-1ubuntu3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpv/libmpv-dev_0.34.1-1ubuntu3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/mpv/wayland/mpv_0.34.1-1ubuntu3_arm64.deb"
		cp ${EXTER}/cache/sources/rk3588_packages_jammy/mpv/mpv.conf $SDCARD/etc/mpv/

		## for kodi
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/kodi/kodi-bin_19.4+dfsg1-2+gles2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/kodi/kodi-data_19.4+dfsg1-2+gles2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/kodi/kodi_19.4+dfsg1-2+gles2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/rktoolkit/rktoolkit_1.0.0-1_arm64.deb"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libnvpair3linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libuutil3linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libzfs4linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/libzpool5linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/zfsutils-linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages_jammy/zfs/zfs-dkms_2.1.6-0york1~22.04_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
			chroot $SDCARD /bin/bash -c "rm -r /usr/src/linux-headers-5.10.110-rockchip-rk3588"
		fi

	fi
}
