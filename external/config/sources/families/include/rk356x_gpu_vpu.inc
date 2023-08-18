rk356x_gpu_vpu_tweaks()
{

	if [[ ${BOARD} =~ orangepicm4|orangepi3b && ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} == xfce && ${RELEASE} == focal ]]; then

		rk356x_deb+=("rk356x_packages/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga2_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga-dev_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rktoolkit/rktoolkit_1.0.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpp/librockchip-mpp1_1.5.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpp/librockchip-mpp-dev_1.5.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpp/librockchip-vpu0_1.5.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpp/rockchip-mpp-demos_1.5.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gstreamer/libgstreamer1.0-0_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gstreamer/libgstreamer1.0-dev_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gstreamer/gir1.2-gstreamer-1.0_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gstreamer/gstreamer1.0-tools_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/gstreamer1.0-alsa_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/gstreamer1.0-x_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/gstreamer1.0-gl_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-good1.0/gstreamer1.0-gtk3_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-good1.0/gstreamer1.0-qt5_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-good1.0/libgstreamer-plugins-good1.0-0_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/gst-plugins-good1.0/libgstreamer-plugins-good1.0-dev_1.16.3-0ubuntu1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libdvbv5-0_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/dvb-tools_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/ir-keytable_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libdvbv5-dev_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libdvbv5-doc_1.18.0-2build1_all.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libv4l-0_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libv4l2rds0_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libv4lconvert0_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libv4l-dev_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/libv4l-rkmpp_1.4.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/v4l-utils_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libv4l/qv4l2_1.18.0-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/chromium/chromium-x11_91.0.4472.164_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/xserver/xserver-xorg-core_1.20.13-1ubuntu1~20.04.8_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libdrm/libdrm2_2.4.107-8ubuntu1~20.04.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libdrm/libdrm-common_2.4.107-8ubuntu1~20.04.2_all.deb")
		rk356x_deb+=("rk356x_packages/focal/libdrm/libkms1_2.4.107-8ubuntu1~20.04.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavutil56_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavutil-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libpostproc55_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libpostproc-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libswresample3_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libswresample-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libswscale5_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libswscale-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavcodec58_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavcodec-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavformat58_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavformat-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavfilter7_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavfilter-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavdevice58_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavdevice-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavresample4_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/libavresample-dev_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/ffmpeg/ffmpeg_4.2.7-0ubuntu0.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpv/libmpv1_0.32.0-1ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpv/libmpv-dev_0.32.0-1ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/focal/mpv/mpv_0.32.0-1ubuntu1_arm64.deb")

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"
		chroot $SDCARD /bin/bash -c "rm /etc/mpv/mpv.conf"

		for deb in ${rk356x_deb[@]}
		do
			dpkg_install_deb_chroot "$EXTER/cache/sources/${deb}"
		done

		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		cp ${EXTER}/cache/sources/rk356x_packages/focal/chromium/libjpeg.so.62 $SDCARD/usr/lib/aarch64-linux-gnu/
		cp ${EXTER}/packages/bsp/rk356x/etc/mpv/mpv.conf $SDCARD/etc/mpv/

		## The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/focal/zfs/libnvpair3linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/focal/zfs/libuutil3linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/focal/zfs/libzfs4linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/focal/zfs/libzpool5linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/focal/zfs/zfsutils-linux_2.1.6-0york1~20.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/focal/zfs/zfs-dkms_2.1.6-0york1~20.04_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
		fi

	fi

	if [[ ${BOARD} =~ orangepicm4|orangepi3b && ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} == xfce && ${RELEASE} == jammy ]]; then
	
		rk356x_deb+=("rk356x_packages/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga2_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga-dev_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rktoolkit/rktoolkit_1.0.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpp/librockchip-mpp1_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpp/librockchip-mpp-dev_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpp/librockchip-vpu0_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpp/rockchip-mpp-demos_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gstreamer/gir1.2-gstreamer-1.0_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gstreamer/gstreamer1.0-tools_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gstreamer/libgstreamer1.0-0_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gstreamer/libgstreamer1.0-dev_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/gstreamer1.0-alsa_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/gstreamer1.0-gl_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/gstreamer1.0-x_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.20.3-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/gstreamer1.0-opencv_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/gstreamer1.0-wpe_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-good1.0/gstreamer1.0-gtk3_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-good1.0/gstreamer1.0-qt5_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-good1.0/libgstreamer-plugins-good1.0-0_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/gst-plugins-good1.0/libgstreamer-plugins-good1.0-dev_1.20.3-0ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libdvbv5-0_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/dvb-tools_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/ir-keytable_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libdvbv5-dev_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libdvbv5-doc_1.22.1-2build1_all.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libv4l-0_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libv4l2rds0_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libv4lconvert0_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libv4l-dev_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/libv4l-rkmpp_1.5.1-1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/v4l-utils_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libv4l/qv4l2_1.22.1-2build1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/chromium/chromium-x11_91.0.4472.164_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/xserver/xserver-xorg-core_21.1.4-2ubuntu1.7~22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/xserver/xserver-common_21.1.4-2ubuntu1.7~22.04.1_all.deb")
		rk356x_deb+=("rk356x_packages/jammy/xserver/xserver-xorg-dev_21.1.4-2ubuntu1.7~22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/xserver/xserver-xorg-legacy_21.1.4-2ubuntu1.7~22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libdrm/libkms1_2.4.110-1ubuntu1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavutil56_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavutil-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libpostproc55_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libpostproc-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libswresample3_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libswresample-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libswscale5_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libswscale-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavcodec58_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavcodec-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavformat58_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavformat-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavfilter7_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavfilter-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavdevice58_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/libavdevice-dev_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/ffmpeg/ffmpeg_4.4.2-0ubuntu0.22.04.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpv/libmpv1_0.34.1-1ubuntu3_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpv/libmpv-dev_0.34.1-1ubuntu3_arm64.deb")
		rk356x_deb+=("rk356x_packages/jammy/mpv/mpv_0.34.1-1ubuntu3_arm64.deb")

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"
		chroot $SDCARD /bin/bash -c "apt-get install -y libgraphene-1.0-0 libqt5waylandclient5 libgdk-pixbuf2.0-0" > /dev/null
		chroot $SDCARD /bin/bash -c "rm /etc/mpv/mpv.conf"

		for deb in ${rk356x_deb[@]}
		do
			dpkg_install_deb_chroot "$EXTER/cache/sources/${deb}"
		done

		cp ${EXTER}/packages/bsp/rk356x/etc/mpv/mpv.conf $SDCARD/etc/mpv/
		cp ${EXTER}/cache/sources/rk356x_packages/jammy/chromium/libjpeg.so.62 $SDCARD/usr/lib/aarch64-linux-gnu/

		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		## The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/jammy/zfs/libnvpair3linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/jammy/zfs/libuutil3linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/jammy/zfs/libzfs4linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/jammy/zfs/libzpool5linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/jammy/zfs/zfsutils-linux_2.1.6-0york1~22.04_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/jammy/zfs/zfs-dkms_2.1.6-0york1~22.04_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
		fi

	fi


	if [[ ${BOARD} =~ orangepicm4|orangepi3b && ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} == xfce && ${RELEASE} == bullseye ]]; then

		rk356x_deb+=("rk356x_packages/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga2_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga-dev_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rktoolkit/rktoolkit_1.0.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/librockchip-mpp1_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/librockchip-mpp-dev_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/librockchip-vpu0_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/rockchip-mpp-demos_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver/xserver-common_1.20.11-1_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver/xserver-xorg-dev_1.20.11-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver/xserver-xorg-core_1.20.11-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver/xserver-xorg-legacy_1.20.11-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/gir1.2-gstreamer-1.0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/gstreamer1.0-tools_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/libgstreamer1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/libgstreamer1.0-dev_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-alsa_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-gl_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-x_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-opencv_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-wpe_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-gtk3_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-qt5_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-ugly1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-libav1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libdvbv5-0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/dvb-tools_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/ir-keytable_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libdvbv5-dev_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libdvbv5-doc_1.20.0-2_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l-0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l2rds0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4lconvert0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l-dev_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l-rkmpp_1.5.1-1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/v4l-utils_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/qv4l2_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/chromium/chromium-x11_91.0.4472.164_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libdrm/libkms1_2.4.104-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libdrm-cursor/libdrm-cursor_1.4.1-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libdrm-cursor/libdrm-cursor-dev_1.4.1-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-data_2022.06~git.3f1104d-1_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-drm_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-es2-drm_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-es2-wayland_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-es2-x11_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-wayland_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-x11_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavutil56_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavutil-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libpostproc55_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libpostproc-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswresample3_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswresample-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswscale5_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswscale-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavcodec58_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavcodec-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavformat58_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavformat-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavresample4_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavresample-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavfilter7_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavfilter-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavdevice58_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavdevice-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/ffmpeg_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpv/libmpv1_0.32.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpv/libmpv-dev_0.32.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpv/mpv_0.32.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/cheese/cheese_3.38.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/cheese/cheese-common_3.38.0-3_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/cheese/libcheese8_3.38.0-3_arm64.deb")

		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/${RELEASE}/zfs/libnvpair3linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/${RELEASE}/zfs/libuutil3linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/${RELEASE}/zfs/libzfs4linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/${RELEASE}/zfs/libzpool5linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/${RELEASE}/zfs/zfsutils-linux_2.1.11-1~bpo11+1_arm64.deb"
			dpkg_install_deb_chroot "$EXTER/cache/sources/rk356x_packages/${RELEASE}/zfs/zfs-dkms_2.1.11-1~bpo11+1_all.deb"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
		fi

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"

		for deb in ${rk356x_deb[@]}
		do
			dpkg_install_deb_chroot "$EXTER/cache/sources/${deb}"
		done

		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

	fi

	if [[ ${BOARD} =~ orangepicm4|orangepi3b && ${SELECTED_CONFIGURATION} == desktop && ${DESKTOP_ENVIRONMENT} =~ xfce|kde-plasma && ${RELEASE} == bookworm ]]; then


		rk356x_deb+=("rk356x_packages/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga2_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga-dev_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rktoolkit/rktoolkit_1.0.0-1_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/mpp/librockchip-mpp1_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/mpp/librockchip-mpp-dev_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/mpp/librockchip-vpu0_1.5.0-1.2_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/mpp/rockchip-mpp-demos_1.5.0-1.2_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/gstreamer-rockchip/gstreamer1.0-rockchip1_1.14-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gstreamer1.0/gir1.2-gstreamer-1.0_1.22.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gstreamer1.0/gstreamer1.0-tools_1.22.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gstreamer1.0/libgstreamer1.0-0_1.22.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gstreamer1.0/libgstreamer1.0-dev_1.22.0-3_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/gstreamer1.0-alsa_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/gstreamer1.0-gl_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/gstreamer1.0-x_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.22.0-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.22.0-4_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-opencv_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/gstreamer1.0-wpe_1.22.0-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.22.0-5_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-good1.0/gstreamer1.0-gtk3_1.22.0-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.22.0-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.22.0-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-good1.0/gstreamer1.0-qt5_1.22.0-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/gst-plugins-good1.0/gstreamer1.0-qt6_1.22.0-6_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libdvbv5-0_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/dvb-tools_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/ir-keytable_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libdvbv5-dev_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libdvbv5-doc_1.22.1-6_all.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libv4lconvert0_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libv4l-0_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libv4l2rds0_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/libv4l-dev_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/v4l-utils_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/qv4l2_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l/qvidcap_1.22.1-6_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libv4l-rkmpp/libv4l-rkmpp_1.5.1-1.1_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xserver-common_21.1.7-3_all.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xserver-xorg-core_21.1.7-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xserver-xorg-dev_21.1.7-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xserver-xorg-legacy_21.1.7-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xnest_21.1.7-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xserver-xephyr_21.1.7-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/xserver/xvfb_21.1.7-3_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/chromium/chromium-codecs-ffmpeg-extra_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/chromium/chromium-browser_110.0.5481.4-debian11~bullseye~rkmpp20230130+1_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavutil57_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavutil-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libpostproc56_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libpostproc-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libswresample4_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libswresample-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libswscale6_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libswscale-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavcodec59_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavcodec-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavformat59_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavformat-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavfilter8_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavfilter-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavdevice59_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/libavdevice-dev_5.1.3-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/ffmpeg_5.1.3-4_arm64.deb")
		#rk356x_deb+=("rk356x_packages/${RELEASE}/ffmpeg/ffmpeg-doc_5.1.3-4_all.deb")

		rk356x_deb+=("rk356x_packages/${RELEASE}/mpv/libmpv2_0.35.1-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/mpv/libmpv-dev_0.35.1-5_arm64.deb")
		rk356x_deb+=("rk356x_packages/${RELEASE}/mpv/mpv_0.35.1-5_arm64.deb")

		for deb in ${rk356x_deb[@]}
		do
			dpkg_install_deb_chroot "$EXTER/cache/sources/${deb}"
		done

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		cp $EXTER/cache/sources/rk356x_packages/${RELEASE}/chromium/default $SDCARD/etc/chromium-browser/
		if [[ $install_zfs == "yes" ]]; then

			chroot $SDCARD /bin/bash -c "dpkg -i /opt/${CHOSEN_KERNEL/image/headers}_${REVISION}_${ARCH}.deb"
			run_on_sdcard "DEBIAN_FRONTEND=noninteractive apt-get install -yqq zfsutils-linux zfs-dkms"
			chroot $SDCARD /bin/bash -c "apt remove -y ${CHOSEN_KERNEL/image/headers}"
		fi
	fi
}

rk356x_gpu_vpu_tweaks_for_raspios()
{
	if [[ ${BOARD} =~ orangepicm4|orangepi3b && ${SELECTED_CONFIGURATION} == desktop && ${RELEASE} == raspi ]]; then

		rk356x_deb+=("rk356x_packages/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga2_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rga2/librga-dev_2.2.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb")
		rk356x_deb+=("rk356x_packages/common/rktoolkit/rktoolkit_1.0.0-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/librockchip-mpp1_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/librockchip-mpp-dev_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/librockchip-vpu0_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpp/rockchip-mpp-demos_1.5.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver-raspios/xserver-common_1.20.11-1+rpt3+deb11u6_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver-raspios/xserver-xorg-dev_1.20.11-1+rpt3+deb11u6_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver-raspios/xserver-xorg-core_1.20.11-1+rpt3+deb11u6_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver-raspios/xserver-xorg-legacy_1.20.11-1+rpt3+deb11u6_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/xserver-raspios/xwayland_1.20.11-1+rpt3+deb11u6_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/gir1.2-gstreamer-1.0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/gstreamer1.0-tools_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/libgstreamer1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gstreamer/libgstreamer1.0-dev_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-alsa_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-gl_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/gstreamer1.0-x_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-opencv_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/gstreamer1.0-wpe_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-gtk3_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-plugins-ugly1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/gst-libav1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libdvbv5-0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/dvb-tools_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/ir-keytable_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libdvbv5-dev_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libdvbv5-doc_1.20.0-2_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l-0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l2rds0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4lconvert0_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l-dev_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/libv4l-rkmpp_1.5.1-1.1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libv4l/v4l-utils_1.20.0-2_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/chromium/chromium-x11_91.0.4472.164_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libdrm-cursor/libdrm-cursor_1.4.1-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/libdrm-cursor/libdrm-cursor-dev_1.4.1-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-data_2022.06~git.3f1104d-1_all.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-es2-drm_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/glmark2/glmark2-es2-x11_2022.06~git.3f1104d-1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavutil56_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavutil-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libpostproc55_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libpostproc-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswresample3_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswresample-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswscale5_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libswscale-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavcodec58_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavcodec-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavformat58_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavformat-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavresample4_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavresample-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavfilter7_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavfilter-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavdevice58_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/libavdevice-dev_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/ffmpeg/ffmpeg_4.3.5-0+deb11u1_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpv/libmpv1_0.32.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpv/libmpv-dev_0.32.0-3_arm64.deb")
		rk356x_deb+=("rk356x_packages/bullseye/mpv/mpv_0.32.0-3_arm64.deb")

		chroot $SDCARD /bin/bash -c "apt remove -y chromium-browser rpi-chromium-mods libwidevinecdm0"

		for deb in ${rk356x_deb[@]}
		do
			dpkg_install_deb_chroot "$EXTER/cache/sources/${deb}"
		done


		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"
	fi
}
