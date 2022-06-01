enable_extension "sunxi-tools"
ARCH=arm64
ATF_TARGET_MAP="PLAT=$ATF_PLAT DEBUG=1 bl31;;build/$ATF_PLAT/debug/bl31.bin"
BOOTDELAY=1
BOOTPATCHDIR='u-boot-sunxi'
BOOTENV_FILE='sunxi.txt'
UBOOT_TARGET_MAP=';;u-boot-sunxi-with-spl.bin'
BOOTSCRIPT='boot-sun50i-next.cmd:boot.cmd'
LINUXFAMILY=sunxi64
PACKAGE_LIST_DESKTOP_FAMILY="fcitx fonts-wqy-zenhei"

#PACKAGE_LIST_FAMILY_REMOVE="mpv"

case $BRANCH in

	current)

		KERNELBRANCH="branch:orange-pi-5.4"
		KERNELPATCHDIR='sunxi-'$BRANCH

	;;

esac

family_tweaks()
{
	# execute specific tweaks function if present
	[[ $(type -t family_tweaks_s) == function ]] && family_tweaks_s
	#cp $SRC/packages/blobs/splash/orangepi-u-boot-24.bmp $SDCARD/boot/boot.bmp

	#if [[ $BOARD == orangepizero2 ]]; then

	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/xfce4-zorinmenulite-plugin_1.1.2_arm64.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx-common_1.0~beta+git20210222-1~ppa1_all.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx-dockx_1.0~beta+git20210222-1~ppa1_all.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx_1.0~beta+git20210222-1~ppa1_all.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/xfce4-dockbarx-plugin_0.6+git20210221-1~ppa2~20.10_arm64.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx-theme-zorin_1.1.0_all.deb"
	#	#install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/scratch-desktop_3.26.0_arm64.deb"
	#fi

	if [[ $BOARD =~ orangepi3-lts|orangepizero2|orangepizero2-b|orangepizero2-lts ]]; then

		[[ $RELEASE == bookworm ]] && dpkg_install_deb_chroot "$EXTER/cache/debs/extra/${RELEASE}-utils/wpasupplicant_2.10-9_arm64.deb"
		[[ $RELEASE == jammy ]] && dpkg_install_deb_chroot "$EXTER/cache/debs/extra/${RELEASE}-utils/wpasupplicant_2.10-6ubuntu1_arm64.deb"

	fi

	if [[ $RELEASE == jammy && $BUILD_DESKTOP == yes && $install_chromium == yes ]]; then

		chromium_name="chromium-browser_101.0.4951.15-0ubuntu1~ppa1~22.04.1_arm64.deb"
		chromium_extra_name="chromium-codecs-ffmpeg-extra_101.0.4951.15-0ubuntu1~ppa1~22.04.1_arm64.deb"
		chromium_url="http://ppa.launchpadcontent.net/saiarcot895/chromium-beta/ubuntu/pool/main/c/chromium-browser"
		chromium=${EXTER}/cache/debs/arm64/${chromium_name}
		chromium_extra=${EXTER}/cache/debs/arm64/${chromium_extra_name}

		if [[ ! -f $chromium_extra ]]; then
			wget -P ${EXTER}/cache/debs/arm64 ${chromium_url}/${chromium_name}
			wget -P ${EXTER}/cache/debs/arm64 ${chromium_url}/${chromium_extra_name}
		fi

		install_deb_chroot $chromium_extra
		install_deb_chroot $chromium

	fi

	rm $SDCARD/root/*.deb >/dev/null 2>&1
}

write_uboot_platform()
{
	if [[ -f $1/boot0_sdcard.fex ]]; then # legacy h616/h6 loader
		dd if=$1/boot0_sdcard.fex of=$2 bs=8k seek=1 conv=fsync > /dev/null 2>&1
		dd if=$1/boot_package.fex of=$2 bs=8k seek=2050 conv=fsync > /dev/null 2>&1 || true
	elif [[ -f $1/u-boot-with-dtb.bin ]]; then # legacy a64 loader
		dd if=/dev/zero of=$2 bs=1k count=1023 seek=1 status=noxfer > /dev/null 2>&1
		dd if=$1/u-boot-with-dtb.bin of=$2 bs=1k seek=8 conv=fsync > /dev/null 2>&1 || true
	elif [[ -f $1/u-boot-sunxi-with-spl.bin ]]; then
		dd if=/dev/zero of=$2 bs=1k count=1023 seek=1 status=noxfer > /dev/null 2>&1
		dd if=$1/u-boot-sunxi-with-spl.bin of=$2 bs=1k seek=8 conv=fsync > /dev/null 2>&1 || true
	else
		[[ -f $1/sunxi-spl.bin ]] && dd if=$1/sunxi-spl.bin of=$2 bs=8k seek=1 conv=fsync > /dev/null 2>&1
		[[ -f $1/u-boot.itb ]] && dd if=$1/u-boot.itb of=$2 bs=8k seek=5 conv=fsync > /dev/null 2>&1 || true
	fi
}

setup_write_uboot_platform()
{
	if grep -q "ubootpart" /proc/cmdline; then
		# mainline with new boot script
		local tmp=$(cat /proc/cmdline)
		tmp="${tmp##*ubootpart=}"
		tmp="${tmp%% *}"
		[[ -n $tmp ]] && local part=$(findfs PARTUUID=$tmp 2>/dev/null)
		[[ -n $part ]] && local dev=$(lsblk -n -o PKNAME $part 2>/dev/null)
		[[ -n $dev ]] && DEVICE="/dev/$dev"
	else
		# legacy or old boot script
		local tmp=$(cat /proc/cmdline)
		tmp="${tmp##*root=}"
		tmp="${tmp%% *}"
		[[ -n $tmp ]] && local part=$(findfs $tmp 2>/dev/null)
		[[ -n $part ]] && local dev=$(lsblk -n -o PKNAME $part 2>/dev/null)
		# do not try to write u-boot to USB devices
		[[ -n $dev && $dev == mmcblk* ]] && DEVICE="/dev/$dev"
	fi
}
