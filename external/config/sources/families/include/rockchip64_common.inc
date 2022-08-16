source "${BASH_SOURCE%/*}/rk3399_gpu_vpu.inc"

enable_extension "rkbin-tools"
ARCH=arm64
KERNEL_IMAGE_TYPE=Image
OFFSET=30
BOOTSCRIPT='boot-rockchip64.cmd:boot.cmd'
BOOTENV_FILE='rockchip.txt'
UBOOT_TARGET_MAP=";;idbloader.bin uboot.img trust.bin"
BOOTDELAY=0
OVERLAY_PREFIX='rockchip'
SERIALCON=${SERIALCON:=$([ $BRANCH == "legacy" ] && echo "ttyFIQ0:1500000" || echo "ttyS2:1500000")}
GOVERNOR="ondemand"
BOOTBRANCH="branch:v2020.10-rockchip64"
PACKAGE_LIST_FAMILY="ethtool"
PACKAGE_LIST_DESKTOP_FAMILY="fcitx fonts-wqy-zenhei"
[[ $BRANCH != "next" ]] && PACKAGE_LIST_FAMILY_REMOVE="mpv"

RKBIN_DIR="$EXTER/cache/sources/rkbin-tools"
if [ "$(uname -m)" = "aarch64" ]; then
	case "$(lsb_release -sc)" in
	"bookworm"|"bullseye"|"focal"|"jammy")
		PKG_PREFIX="qemu-x86_64-static -L /usr/x86_64-linux-gnu "
		;;
	*)
		PKG_PREFIX="qemu-x86_64 -L /usr/x86_64-linux-gnu "
		;;
	esac
else
	PKG_PREFIX=""
fi

if [[ $BOOTCONFIG == *3399* ]]; then

	CPUMIN=${CPUMIN:="408000"}
	CPUMAX=${CPUMAX:="1800000"}

else # rk3328

	CPUMIN=${CPUMIN:="408000"}
	CPUMAX=${CPUMAX:="1296000"}

fi

if [[ $BOARD =~ orangepir1plus|orangepir1plus-lts ]]; then

	BOOT_USE_BLOBS=yes
	BOOT_SOC=rk3328
	DDR_BLOB='rk33/rk3328_ddr_333MHz_v1.16.bin'
	MINILOADER_BLOB='rk33/rk322xh_miniloader_v2.50.bin'
	BL31_BLOB='rk33/rk322xh_bl31_v1.44.elf'

else

	ATFPATCHDIR='atf-rk3399'

fi

prepare_boot_configuration()
{

	if [[ $BOOT_USE_MAINLINE_ATF == yes ]]; then

		UBOOT_TARGET_MAP="BL31=bl31.bin idbloader.img u-boot.itb;;idbloader.img u-boot.itb"
		ATFSOURCE='https://github.com/ARM-software/arm-trusted-firmware'
		ATF_COMPILER='aarch64-linux-gnu-'
		ATFDIR='arm-trusted-firmware'
		ATFBRANCH='tag:v2.2'
		ATF_USE_GCC='> 6.3'
		ATF_TARGET_MAP="M0_CROSS_COMPILE=arm-linux-gnueabi- PLAT=$BOOT_SOC bl31;;build/$BOOT_SOC/release/bl31/bl31.elf:bl31.bin"
		ATF_TOOLCHAIN2="arm-linux-gnueabi-:> 5.0"

	elif [[ $BOOT_USE_TPL_SPL_BLOB == yes ]]; then

		UBOOT_TARGET_MAP="BL31=$RKBIN_DIR/$BL31_BLOB idbloader.img u-boot.itb;;idbloader.img u-boot.itb"
		ATFSOURCE=''
		ATF_COMPILE='no'

	elif [[ $BOOT_USE_BLOBS == yes ]]; then

		UBOOT_TARGET_MAP="u-boot-dtb.bin;;idbloader.bin uboot.img trust.bin"
		ATFSOURCE=''
		ATF_COMPILE='no'
	fi

	if [[ $BOOT_SUPPORT_SPI == yes ]]; then

		UBOOT_TARGET_MAP+=" rkspi_loader.img"

	fi
}

uboot_custom_postprocess()
{

	if [[ $BOOT_USE_MAINLINE_ATF == yes || $BOOT_USE_TPL_SPL_BLOB == yes ]]; then

		if [[ $BOOT_SUPPORT_SPI == yes ]]; then
			tools/mkimage -n rk3399 -T rkspi -d tpl/u-boot-tpl.bin:spl/u-boot-spl.bin rkspi_tpl_spl.img
			dd if=/dev/zero of=rkspi_loader.img count=8128 status=none
			dd if=rkspi_tpl_spl.img of=rkspi_loader.img conv=notrunc status=none
			dd if=u-boot.itb of=rkspi_loader.img seek=768 conv=notrunc status=none
		fi

	elif [[ $BOOT_USE_BLOBS == yes ]]; then

		local tempfile=$(mktemp)
		tools/mkimage -n $BOOT_SOC -T rksd -d $RKBIN_DIR/$DDR_BLOB idbloader.bin
		cat $RKBIN_DIR/$MINILOADER_BLOB >> idbloader.bin
		loaderimage --pack --uboot ./u-boot-dtb.bin uboot.img 0x200000
		trust_merger --replace bl31.elf $RKBIN_DIR/$BL31_BLOB trust.ini

	else
		echo "Unsupported u-boot processing configuration!"
		exit 1
	fi

	if [[ ${MERGE_UBOOT} ]]; then

		display_alert "Merge u-boot" "u-boot-${BOARD}-merged.bin" "info"

		dd if=/dev/zero of=u-boot-${BOARD}-merged.bin bs=1M count=32 > /dev/null 2>&1
		dd if=idbloader.bin of=u-boot-${BOARD}-merged.bin seek=64 conv=notrunc status=none
		dd if=uboot.img of=u-boot-${BOARD}-merged.bin seek=16384 conv=notrunc status=none
		dd if=trust.bin of=u-boot-${BOARD}-merged.bin seek=24576 conv=notrunc status=none
		mv u-boot-${BOARD}-merged.bin ${DEB_STORAGE}/u-boot/
	fi
}

write_uboot_platform()
{

	if [[ -f $1/rksd_loader.img ]]; then # legacy rk3399 loader

		dd if=$1/rksd_loader.img of=$2 seek=64 conv=notrunc status=none >/dev/null 2>&1

	elif [[ -f $1/u-boot.itb ]]; then # $BOOT_USE_MAINLINE_ATF == yes || $BOOT_USE_TPL_SPL_BLOB == yes

		dd if=$1/idbloader.img of=$2 seek=64 conv=notrunc status=none >/dev/null 2>&1
		dd if=$1/u-boot.itb of=$2 seek=16384 conv=notrunc status=none >/dev/null 2>&1

	elif [[ -f $1/uboot.img ]]; then # $BOOT_USE_BLOBS == yes

		dd if=$1/idbloader.bin of=$2 seek=64 conv=notrunc status=none >/dev/null 2>&1
		dd if=$1/uboot.img of=$2 seek=16384 conv=notrunc status=none >/dev/null 2>&1
		dd if=$1/trust.bin of=$2 seek=24576 conv=notrunc status=none >/dev/null 2>&1

	else
		echo "Unsupported u-boot processing configuration!"
		exit 1

	fi
}

write_uboot_platform_mtd()
{

	if [[ -f $1/rkspi_loader.img ]]; then

		dd if=$1/rkspi_loader.img of=$2 conv=notrunc status=none >/dev/null 2>&1

	else

		echo "SPI u-boot image not found!"
		exit 1

	fi
}

setup_write_uboot_platform()
{

	if grep -q "ubootpart" /proc/cmdline; then

		local tmp=$(cat /proc/cmdline)
		tmp="${tmp##*ubootpart=}"
		tmp="${tmp%% *}"
		[[ -n $tmp ]] && local part=$(findfs PARTUUID=$tmp 2>/dev/null)
		[[ -n $part ]] && local dev=$(lsblk -n -o PKNAME $part 2>/dev/null)
		[[ -n $dev ]] && DEVICE="/dev/$dev"

	fi
}

atf_custom_postprocess()
{

	# remove bl31.bin which can be 4+GiB in size and thus may fill the tmpfs mount
	rm -f build/rk322xh/debug/bl31.bin
	# ATF
	trust_merger trust.ini
}

family_tweaks()
{

	# execute specific tweaks function if present
	[[ $(type -t family_tweaks_s) == function ]] && family_tweaks_s

	[[ "$BRANCH" =~ legacy|current ]] && rk3399_gpu_vpu_tweaks

	#if [[ ${BOARD} == orangepi800 && ${SELECTED_CONFIGURATION} == desktop && ${RELEASE} =~ bullseye|bookworm ]]; then

	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/xfce4-zorinmenulite-plugin_1.1.2_arm64.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx-common_1.0~beta+git20210222-1~ppa1_all.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx-dockx_1.0~beta+git20210222-1~ppa1_all.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx_1.0~beta+git20210222-1~ppa1_all.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/xfce4-dockbarx-plugin_0.6+git20210221-1~ppa2~20.10_arm64.deb"
	#	install_deb_chroot "$EXTER/cache/debs/extra/bullseye-desktop/dockbarx-theme-zorin_1.1.0_all.deb"

	#	## Orange Pi Desktop layout switcher
	#	install_deb_chroot "$EXTER/cache/debs/arm64/layout_switcher/layoutswitcher_0.2-0_all.deb"
	#	#install_deb_chroot "$EXTER/cache/debs/arm64/layout_switcher/switcher_1.0-1_arm64.deb"
	#fi

	if [[ $BOARD =~ orangepir1plus|orangepir1plus-lts ]]; then

		# rename USB based network to lan0
		mkdir -p $SDCARD/etc/udev/rules.d/
		echo 'SUBSYSTEM=="net", ACTION=="add", DRIVERS=="r8152", KERNEL=="eth1", NAME="lan0"' > $SDCARD/etc/udev/rules.d/70-rename-lan.rules

		# workaround - create old school initial network config since network manager have issues
		#echo "auto eth0" >> $SDCARD/etc/network/interfaces
		#echo "	iface eth0 inet dhcp" >> $SDCARD/etc/network/interfaces
		#echo "auto lan0" >> $SDCARD/etc/network/interfaces
		#echo "	iface lan0 inet dhcp" >> $SDCARD/etc/network/interfaces
	fi

	if [[ -f $SDCARD/lib/systemd/system/rk3399-bluetooth.service ]]; then

		# install and enable Bluetooth
		chroot $SDCARD /bin/bash -c "apt-get -y -qq install rfkill bluetooth bluez bluez-tools >/dev/null 2>&1"
		chroot $SDCARD /bin/bash -c "systemctl --no-reload enable rk3399-bluetooth.service >/dev/null 2>&1"
	fi

	if [[ $BOARD =~ orangepi800|orangepi4-lts ]]; then

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

	if [[ $RELEASE == jammy && $BUILD_DESKTOP == yes && $install_firefox == yes ]]; then

		firefox_name="firefox-esr_102.1.0esr+build1-0ubuntu0.22.04.1_arm64.deb"
		firefox_url="https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu/pool/main/f/firefox-esr/"
		firefox=${EXTER}/cache/debs/arm64/${firefox_name}

		if [[ ! -f $firefox ]]; then
			wget -P ${EXTER}/cache/debs/arm64 ${firefox_url}/${firefox_name}
		fi

		install_deb_chroot $firefox

	fi

	rm $SDCARD/root/*.deb >/dev/null 2>&1
}


family_tweaks_bsp()
{

	if [[ $BOOTCONFIG == *3328* ]] && [[ $BRANCH != legacy ]]; then	
		mkdir -p "$destination"/etc/X11/xorg.conf.d
		cat <<-EOF > "$destination"/etc/X11/xorg.conf.d/02-driver.conf
		# set fbdev as default driver.
		Section "Device"
	         Identifier "NOGPU"
	         Driver "fbdev"
		EndSection
		EOF
	fi

	# Graphics and media
	mkdir -p $destination/etc/udev/rules.d
	cp $EXTER/packages/bsp/rk3399/50-mali.rules $destination/etc/udev/rules.d/
	cp $EXTER/packages/bsp/rk3399/50-rk3399-vpu.rules $destination/etc/udev/rules.d/
	mkdir -p $destination/etc/sysfs.d
	cp $EXTER/packages/bsp/rk3399/20-gpu-governor.conf $destination/etc/sysfs.d/
}
