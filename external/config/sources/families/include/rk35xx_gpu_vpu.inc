rk35xx_gpu_vpu_tweaks()
{
	if [[ ${SELECTED_CONFIGURATION} == desktop ]]; then

		debs_dir="$EXTER/cache/sources/rk35xx_packages"

		debs_common_list+=("rga2" "mpp" "rktoolkit")
		debs_list+=("gstreamer" "gst-plugins-base1.0" "gst-plugins-good1.0" "gst-plugins-bad1.0" "gst-plugins-ugly1.0" "gstreamer-rockchip")
		debs_list+=("xserver" "glmark2" "libdrm" "libdrm-cursor")
		debs_list+=("libv4l" "libv4l-rkmpp" "chromium")
		debs_list+=("ffmpeg" "mpv" "cheese" "gst-rkmpp")

		chroot $SDCARD /bin/bash -c "rm -rf /etc/mpv/mpv.conf"

		[[ ${RELEASE} == raspi ]] && chroot $SDCARD /bin/bash -c "apt remove -y chromium-browser rpi-chromium-mods libwidevinecdm0"

		for dir in ${debs_common_list[@]}; do
			dir=$(find "${debs_dir}/common" -type d -name ${dir})
			[[ -n ${dir} ]] && dpkg_install_debs_chroot "${dir}"
		done

		for dir in ${debs_list[@]}; do
			if [[ ${RELEASE} == raspi ]]; then
				[[ ${dir} == "libdrm" ]] && continue
				dir=$(find "${debs_dir}/bullseye" -type d -name ${dir})
			else
				dir=$(find "${debs_dir}/${RELEASE}" -type d -name ${dir})
			fi
			[[ -n ${dir} ]] && dpkg_install_debs_chroot "${dir}"
		done

		if [[ $BOARDFAMILY == "rockchip-rk3588" ]]; then
			dpkg_install_deb_chroot "${debs_dir}/common/libmali/libmali-valhall-g610-g13p0-x11-gbm_1.9-1_arm64.deb"
			dpkg_install_deb_chroot "${debs_dir}/common/rkaiq/camera-engine-rkaiq_rk3588_arm64.deb"
		elif [[ $BOARDFAMILY == "rockchip-rk356x" ]]; then
			dpkg_install_deb_chroot "${debs_dir}/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb"
			dpkg_install_deb_chroot "${debs_dir}/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb"
		fi

		cp ${EXTER}/packages/bsp/rk3588/etc/mpv/mpv.conf $SDCARD/etc/mpv/

		# The chromium using fixed pathes for libv4l2.so
		chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
		chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"
		chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
		chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"

		apply_common_operations() {
			chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"
			chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"
		}

		if [[ ${RELEASE} == focal ]]; then
			apply_common_operations
			cp $debs_dir/$RELEASE/chromium/libjpeg.so.62 $SDCARD/usr/lib/aarch64-linux-gnu/
		fi

		if [[ ${RELEASE} == jammy ]]; then
			apply_common_operations
			cp $debs_dir/$RELEASE/chromium/default $SDCARD/etc/chromium-browser/
		fi

		if [[ ${RELEASE} == bookworm ]]; then
			cp $debs_dir/$RELEASE/chromium/default $SDCARD/etc/chromium-browser/
		fi

		if [[ ${RELEASE} == bullseye ]]; then
			apply_common_operations
		fi

		[[ ${RELEASE} == raspi ]] && chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"
	fi
}
