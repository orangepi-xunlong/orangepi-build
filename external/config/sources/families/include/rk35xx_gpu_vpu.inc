rk35xx_gpu_vpu_tweaks()
{
	local package packages_dir packages_to_install packages_common_to_install

	if [[ $RELEASE =~ raspi|bookworm|bullseye|jammy|focal && ${SELECTED_CONFIGURATION} == desktop ]]; then

		packages_dir="$EXTER/cache/sources/rk35xx_packages"

		if [[ $RELEASE == bookworm ]]; then
			packages_to_install=("rga2" "mpp" "rktoolkit")
			packages_to_install+=("gstreamer" "gst-plugins-base1.0" "gst-plugins-good1.0")
			packages_to_install+=("gst-plugins-bad1.0" "gst-plugins-ugly1.0" "gstreamer-rockchip")
			packages_to_install+=("xserver" "glmark2" "libdrm" "libdrm-cursor" "libv4l" "libv4l-rkmpp")
			packages_to_install+=("chromium" "ffmpeg" "cheese" "mpv" "gst-rkmpp")

		elif [[ $RELEASE == jammy && ${DESKTOP_ENVIRONMENT} == gnome ]]; then

			packages_common_to_install=("rga2" "mpp" "rktoolkit")
			packages_to_install=("gstreamer" "gst-plugins-base1.0" "gst-plugins-good1.0")
			packages_to_install+=("gst-plugins-bad1.0" "gstreamer-rockchip")
			packages_to_install+=("libv4l_gnome" "chromium" "chromium_gnome" "mesa")
			packages_to_install+=("ffmpeg" "mpv_wayland" "kodi" "gst-rkmpp")

			[[ -f $SDCARD/etc/profile.d/x11.sh ]] && rm $SDCARD/etc/profile.d/x11.sh
			echo "PAN_MESA_DEBUG=gofaster" >> $SDCARD/etc/environment

		else
			packages_common_to_install=("rga2" "mpp" "rktoolkit")
			packages_to_install=("gstreamer" "gst-plugins-base1.0" "gst-plugins-good1.0")
			packages_to_install+=("gst-plugins-bad1.0" "gst-plugins-ugly1.0" "gstreamer-rockchip")
			packages_to_install+=("xserver" "glmark2" "libdrm" "libdrm-cursor" "libv4l" "libv4l-rkmpp")
			packages_to_install+=("chromium" "ffmpeg" "cheese" "mpv" "gst-rkmpp")

		fi

		chroot $SDCARD /bin/bash -c "rm -rf /etc/mpv/mpv.conf"
		chroot $SDCARD /bin/bash -c "apt-get update" >> "${DEST}"/${LOG_SUBPATH}/install.log 2>&1

		[[ ${RELEASE} == raspi ]] && chroot $SDCARD /bin/bash -c "apt remove -y chromium-browser rpi-chromium-mods libwidevinecdm0"
		[[ "$BOARDFAMILY" == "rk3399" ]] && chroot $SDCARD /bin/bash -c "apt install -y libqt5quick5" \
			>> "${DEST}"/${LOG_SUBPATH}/install.log 2>&1

		for package in ${packages_common_to_install[@]}; do
			local package_dir="${packages_dir}/common/${package}"
			[[ -d "${package_dir}" ]] && dpkg_install_debs_chroot "${package_dir}"
		done

		for package in ${packages_to_install[@]}; do
			local package_dir
			if [[ ${RELEASE} == raspi ]]; then
				[[ ${package} == "libdrm" ]] && continue
				package_dir="${packages_dir}/bullseye/${package}"
			else
				package_dir="${packages_dir}/${RELEASE}/${package}"
			fi
			[[ -d "${package_dir}" ]] && dpkg_install_debs_chroot "${package_dir}"
		done

		if [[ $BOARDFAMILY == "rockchip-rk3588" ]]; then
			if [[ $BRANCH == "current" ]]; then
				[[ $DESKTOP_ENVIRONMENT != gnome ]] && dpkg_install_deb_chroot \
					"${packages_dir}/common/libmali_linux6.1/libmali-valhall-g610-g13p0-x11-gbm_1.9-1_arm64.deb"
				dpkg_install_deb_chroot "${packages_dir}/bookworm/rkaiq/camera_engine_rkaiq_rk3588_arm64.deb"
			else
				[[ $DESKTOP_ENVIRONMENT != gnome ]] && dpkg_install_deb_chroot \
					"${packages_dir}/common/libmali/libmali-valhall-g610-g13p0-x11-gbm_1.9-1_arm64.deb"
				dpkg_install_deb_chroot "${packages_dir}/common/rkaiq/camera_engine_rkaiq_rk3588_arm64.deb"
			fi
		elif [[ $BOARDFAMILY == "rockchip-rk356x" ]]; then
			dpkg_install_deb_chroot "${packages_dir}/common/libmali/libmali-bifrost-g52-g13p0-x11-gbm_1.9-1_arm64.deb"
			dpkg_install_deb_chroot "${packages_dir}/common/rkaiq/camera_engine_rkaiq_rk3568_arm64.deb"
		elif [[ $BOARDFAMILY == "rk3399" ]]; then
			dpkg_install_deb_chroot "${packages_dir}/common/libmali/libmali-midgard-t86x-r18p0-x11-gbm_1.9-1_arm64.deb"
			dpkg_install_deb_chroot "${packages_dir}/common/rkisp/camera_engine_rkisp_v2.3.0_arm64.deb"
		fi

		cp ${EXTER}/packages/bsp/rk3588/etc/mpv/mpv.conf $SDCARD/etc/mpv/

		if [[ $DESKTOP_ENVIRONMENT != gnome ]]; then
			# The chromium using fixed pathes for libv4l2.so
			chroot $SDCARD /bin/bash -c "ln -rsf /usr/lib/*/libv4l2.so /usr/lib/"
			chroot $SDCARD /bin/bash -c "[ -e /usr/lib/aarch64-linux-gnu/ ] && ln -Tsf lib /usr/lib64"
			chroot $SDCARD /bin/bash -c "cd /usr/lib/aarch64-linux-gnu/dri/;cp kms_swrast_dri.so swrast_dri.so rockchip_dri.so /"
			chroot $SDCARD /bin/bash -c "rm /usr/lib/aarch64-linux-gnu/dri/*.so;mv /*.so /usr/lib/aarch64-linux-gnu/dri/"
		fi

		apply_common_operations() {
			chroot $SDCARD /bin/bash -c "chmod o+x /usr/lib/dbus-1.0/dbus-daemon-launch-helper"
			chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"
		}

		if [[ ${RELEASE} == focal ]]; then
			apply_common_operations
			cp $packages_dir/$RELEASE/chromium/libjpeg.so.62 $SDCARD/usr/lib/aarch64-linux-gnu/
		fi

		if [[ ${RELEASE} == jammy ]]; then
			[[ DESKTOP_ENVIRONMENT != gnome ]] && apply_common_operations
			cp $packages_dir/$RELEASE/chromium/default $SDCARD/etc/chromium-browser/
		fi

		if [[ ${RELEASE} == bullseye ]]; then
			apply_common_operations
		fi

		[[ ${RELEASE} == raspi ]] && chroot $SDCARD /bin/bash -c "sed -i '5aexport LD_PRELOAD=libdrm-cursor.so.1' /usr/bin/X"
	fi
}
