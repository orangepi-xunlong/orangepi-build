rk3588_gpu_vpu_tweaks()
{
	if [[ ${BOARD} =~ orangepi5 && ${SELECTED_CONFIGURATION} == desktop && ${RELEASE} == bullseye ]]; then

		chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"

		#chroot $SDCARD /bin/bash -c "apt-get update"
		#chroot $SDCARD /bin/bash -c "apt-get upgrade -y"

		# RK GPU VPU
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/rkaiq/camera-engine-rkaiq_rk3588_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/rga2/librga2_2.2.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/rga2/librga-dev_2.2.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpp/librockchip-mpp1_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpp/librockchip-mpp-dev_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpp/librockchip-vpu0_1.5.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpp/rockchip-mpp-demos_1.5.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-rkmpp/gstreamer1.0-rockchip1_1.14-4_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gstreamer/gir1.2-gstreamer-1.0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gstreamer/gstreamer1.0-tools_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gstreamer/libgstreamer1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gstreamer/libgstreamer1.0-dev_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/gir1.2-gst-plugins-base-1.0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/gstreamer1.0-alsa_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/gstreamer1.0-gl_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/gstreamer1.0-plugins-base_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/gstreamer1.0-plugins-base-apps_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/gstreamer1.0-x_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/libgstreamer-gl1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/libgstreamer-plugins-base1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-base1.0/libgstreamer-plugins-base1.0-dev_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/gir1.2-gst-plugins-bad-1.0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/libgstreamer-opencv1.0-0_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/gstreamer1.0-opencv_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/gstreamer1.0-plugins-bad_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/gstreamer1.0-plugins-bad-apps_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/gstreamer1.0-wpe_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-bad1.0/libgstreamer-plugins-bad1.0-dev_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-good1.0/gstreamer1.0-gtk3_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-good1.0/gstreamer1.0-plugins-good_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-good1.0/gstreamer1.0-pulseaudio_1.18.5-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-good1.0/gstreamer1.0-qt5_1.18.5-1_arm64.deb"
	
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-plugins-ugly1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb"
	
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/gst-libav1.0/gstreamer1.0-plugins-ugly_1.18.5-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libdvbv5-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/dvb-tools_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/ir-keytable_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libdvbv5-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libdvbv5-doc_1.20.0-2_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libv4l-0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libv4l2rds0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libv4lconvert0_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libv4l-dev_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/libv4l-rkmpp_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/v4l-utils_1.20.0-2_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libv4l/qv4l2_1.20.0-2_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/xserver/xserver-xorg-core_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/xserver/xserver-common_1.20.11-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/xserver/xserver-xorg-dev_1.20.11-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/xserver/xserver-xorg-legacy_1.20.11-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/chromium/chromium-x11_91.0.4472.164_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libdrm/libkms1_2.4.104-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libdrm-cursor/libdrm-cursor_1.4.0-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libdrm-cursor/libdrm-cursor-dev_1.4.0-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-data_2021.02+ds-1_all.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-drm_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-es2-drm_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-es2-wayland_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-es2-x11_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-wayland_2021.02+ds-1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/glmark2/glmark2-x11_2021.02+ds-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/rktoolkit/rktoolkit_1.0.0-1_arm64.deb"

		install_deb_chroot "$EXTER/cache/sources/rk3588_packages/libmali/libmali-valhall-g610-g6p0-x11_1.9-1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavutil56_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavutil-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libpostproc55_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libpostproc-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libswresample3_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libswresample-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libswscale5_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libswscale-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavcodec58_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavcodec-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavformat58_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavformat-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavresample4_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavresample-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavfilter7_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavfilter-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavdevice58_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/libavdevice-dev_4.3.5-0+deb11u1_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/ffmpeg/ffmpeg_4.3.5-0+deb11u1_arm64.deb"

		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpv/libmpv1_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpv/libmpv-dev_0.32.0-3_arm64.deb"
		dpkg_install_deb_chroot "$EXTER/cache/sources/rk3588_packages/mpv/mpv_0.32.0-3_arm64.deb"

		chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"

		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

	fi
}
