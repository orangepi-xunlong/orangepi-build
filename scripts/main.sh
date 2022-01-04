#!/bin/bash
#
# Copyright (c) 2015 Igor Pecovnik, igor.pecovnik@gma**.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.


# Main program
#

if [[ $(basename "$0") == main.sh ]]; then

	echo "Please use build.sh to start the build process"
	exit 255
fi

# default umask for root is 022 so parent directories won't be group writeable without this
# this is used instead of making the chmod in prepare_host() recursive
umask 002

# destination
DEST="${SRC}"/output
REVISION="2.1.8"

[[ $DOWNLOAD_MIRROR == "china" ]] && NTP_SERVER="cn.pool.ntp.org"

if [[ $BUILD_ALL != "yes" ]]; then
	# override stty size
	[[ -n $COLUMNS ]] && stty cols $COLUMNS
	[[ -n $LINES ]] && stty rows $LINES
	TTY_X=$(($(stty size | awk '{print $2}')-6)) 			# determine terminal width
	TTY_Y=$(($(stty size | awk '{print $1}')-6)) 			# determine terminal height
fi

# We'll use this title on all menus
backtitle="Orange Pi building script, http://www.orangepi.org" 
titlestr="Choose an option"

# if language not set, set to english
[[ -z $LANGUAGE ]] && export LANGUAGE="en_US:en"

# default console if not set
[[ -z $CONSOLE_CHAR ]] && export CONSOLE_CHAR="UTF-8"

[[ -z $FORCE_CHECKOUT ]] && FORCE_CHECKOUT=yes

# Load libraries
# shellcheck source=debootstrap.sh
source "${SRC}"/scripts/debootstrap.sh	# system specific install
# shellcheck source=image-helpers.sh
source "${SRC}"/scripts/image-helpers.sh	# helpers for OS image building
# shellcheck source=distributions.sh
source "${SRC}"/scripts/distributions.sh	# system specific install
# shellcheck source=desktop.sh
source "${SRC}"/scripts/desktop.sh		# desktop specific install
# shellcheck source=compilation.sh
source "${SRC}"/scripts/compilation.sh	# patching and compilation of kernel, uboot, ATF
# shellcheck source=compilation-prepare.sh
#source "${SRC}"/scripts/compilation-prepare.sh	# kernel plugins - 3rd party drivers that are not upstreamed. Like WG, AUFS, various Wifi
# shellcheck source=makeboarddeb.sh
source "${SRC}"/scripts/makeboarddeb.sh	# create board support package
# shellcheck source=general.sh
source "${SRC}"/scripts/general.sh		# general functions
# shellcheck source=chroot-buildpackages.sh
source "${SRC}"/scripts/chroot-buildpackages.sh	# building packages in chroot
# shellcheck source=pack.sh
source "${SRC}"/scripts/pack-uboot.sh

# compress and remove old logs
mkdir -p "${DEST}"/debug
(cd "${DEST}"/debug && tar -czf logs-"$(<timestamp)".tgz ./*.log) > /dev/null 2>&1
rm -f "${DEST}"/debug/*.log > /dev/null 2>&1
date +"%d_%m_%Y-%H_%M_%S" > "${DEST}"/debug/timestamp
# delete compressed logs older than 7 days
(cd "${DEST}"/debug && find . -name '*.tgz' -mtime +7 -delete) > /dev/null

if [[ $PROGRESS_DISPLAY == none ]]; then

	OUTPUT_VERYSILENT=yes

elif [[ $PROGRESS_DISPLAY == dialog ]]; then

	OUTPUT_DIALOG=yes

fi

if [[ $PROGRESS_LOG_TO_FILE != yes ]]; then unset PROGRESS_LOG_TO_FILE; fi

SHOW_WARNING=yes

if [[ $USE_CCACHE != no ]]; then

	CCACHE=ccache
	export PATH="/usr/lib/ccache:$PATH"
	# private ccache directory to avoid permission issues when using build script with "sudo"
	# see https://ccache.samba.org/manual.html#_sharing_a_cache for alternative solution
	[[ $PRIVATE_CCACHE == yes ]] && export CCACHE_DIR=$DEST/cache/ccache

else

	CCACHE=""

fi

if [ "$OFFLINE_WORK" == "yes" ]; then
	echo -e "\n"
	display_alert "* " "You are working offline."
	display_alert "* " "Sources, time and host will not be checked"
	echo -e "\n"
	sleep 3s
else
	# we need dialog to display the menu in case not installed. Other stuff gets installed later
	prepare_host_basic
fi

# if BUILD_OPT, KERNEL_CONFIGURE, BOARD, BRANCH or RELEASE are not set, display selection menu

if [[ -z $BUILD_OPT ]]; then

	options+=("u-boot"	 "U-boot package")
	options+=("kernel"	 "Kernel package")
	options+=("rootfs"	 "Rootfs and all deb packages")
	options+=("image"	 "Full OS image for flashing")

	menustr="Compile image | rootfs | kernel | u-boot"
	BUILD_OPT=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" --notags \
			  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
			  --cancel-button Exit --ok-button Select "${options[@]}" \
			  3>&1 1>&2 2>&3)

	unset options
	[[ -z $BUILD_OPT ]] && exit_with_error "No option selected"
fi

if [[ -z $BOARD ]]; then

	options+=("orangepir1"			"Allwinner H2+ quad core 256MB RAM WiFi SPI 2xETH")
	options+=("orangepizero"		"Allwinner H2+ quad core 256MB/512MB RAM WiFi SPI")
	options+=("orangepipc"			"Allwinner H3 quad core 1GB RAM")
	options+=("orangepipcplus"		"Allwinner H3 quad core 1GB RAM WiFi eMMC")
	options+=("orangepione"			"Allwinner H3 quad core 512MB/1GB RAM")
	options+=("orangepilite"		"Allwinner H3 quad core 512MB/1GB RAM WiFi")
	options+=("orangepiplus"		"Allwinner H3 quad core 1GB/2GB RAM WiFi GBE eMMC")
	options+=("orangepiplus2e"		"Allwinner H3 quad core 2GB RAM WiFi GBE eMMC")
	options+=("orangepizeroplus2h3" 	"Allwinner H3 quad core 512MB RAM WiFi/BT eMMC")
	options+=("orangepipch5"		"Allwinner H5 quad core 1GB RAM")
	options+=("orangepipc2"			"Allwinner H5 quad core 1GB RAM GBE SPI")
	options+=("orangepioneh5"		"Allwinner H5 quad core 512MB/1GB RAM")
	options+=("orangepiprime"		"Allwinner H5 quad core 2GB RAM GBE WiFi/BT")
	options+=("orangepizeroplus"		"Allwinner H5 quad core 512MB RAM GBE WiFi SPI")
	options+=("orangepizeroplus2h5"		"Allwinner H5 quad core 512MB RAM WiFi/BT eMMC")
	options+=("orangepi3"                   "Allwinner H6 quad core 1GB/2GB RAM GBE WiFi/BT-AP6256 eMMC USB3")
	options+=("orangepi3-lts"               "Allwinner H6 quad core 2GB RAM GBE WiFi/BT-AW859A eMMC USB3")
	options+=("orangepilite2"		"Allwinner H6 quad core 1GB RAM WiFi/BT USB3")
	options+=("orangepioneplus"		"Allwinner H6 quad core 1GB RAM GBE")
	options+=("orangepizero2"		"Allwinner H616 quad core 512MB/1GB RAM WiFi/BT GBE SPI")
	#options+=("orangepizero2-b"		"Allwinner H616 quad core 1GB RAM WiFi/BT GBE SPI")
	#options+=("orangepizero2-lts"		"Allwinner H616 quad core 1.5GB RAM WiFi/BT GBE SPI")
	options+=("orangepi4"                   "Rockchip  RK3399 hexa core 4GB RAM GBE eMMc USB3 USB-C WiFi/BT")
	options+=("orangepir1plus"              "Rockchip  RK3328 quad core 1GB RAM 2xGBE 8211E USB2 SPI")
	options+=("orangepir1plus-lts"          "Rockchip  RK3328 quad core 1GB RAM 2xGBE YT8531C USB2 SPI")

	menustr="Please choose a Board."
	BOARD=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" \
			  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
			  --cancel-button Exit --ok-button Select "${options[@]}" \
			  3>&1 1>&2 2>&3)

	unset options
	[[ -z $BOARD ]] && exit_with_error "No option selected"
fi

BOARD_TYPE="conf"
# shellcheck source=/dev/null
source "${EXTER}/config/boards/${BOARD}.${BOARD_TYPE}"
LINUXFAMILY="${BOARDFAMILY}"

[[ -z $KERNEL_TARGET ]] && exit_with_error "Board configuration does not define valid kernel config"

if [[ -z $BRANCH ]]; then

    options=()
    [[ $KERNEL_TARGET == *current* ]] && options+=("current" "Mainline")
    [[ $KERNEL_TARGET == *legacy* ]] && options+=("legacy" "Old stable")
    [[ $KERNEL_TARGET == *dev* && $EXPERT = yes ]] && options+=("dev" "\Z1Development version (@kernel.org)\Zn")

    menustr="Select the target kernel branch\nExact kernel versions depend on selected board"
    # do not display selection dialog if only one kernel branch is available
    if [[ "${#options[@]}" == 2 ]]; then
        BRANCH="${options[0]}"
    else
		BRANCH=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" \
				  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
				  --cancel-button Exit --ok-button Select "${options[@]}" \
				  3>&1 1>&2 2>&3)
    fi

    unset options
    [[ -z $BRANCH ]] && exit_with_error "No kernel branch selected"

else

    [[ $KERNEL_TARGET != *$BRANCH* ]] && exit_with_error "Kernel branch not defined for this board" "$BRANCH"

fi

if [[ ${BUILD_OPT} == image || ${BUILD_OPT} == kernel ]]; then

	if [[ -z $KERNEL_CONFIGURE ]]; then

		options+=("no" "Do not change the kernel configuration")
		options+=("yes" "Show a kernel configuration menu before compilation")

		menustr="Select the kernel configuration."
		KERNEL_CONFIGURE=$(whiptail --title "${titlestr}" --backtitle "$backtitle" --notags \
						 --menu "${menustr}" $TTY_Y $TTY_X $((TTY_Y - 8)) \
						 --cancel-button Exit --ok-button Select "${options[@]}" \
						 3>&1 1>&2 2>&3)

		unset options
		[[ -z $KERNEL_CONFIGURE ]] && exit_with_error "No option selected"
	fi
fi

# define distribution support status
declare -A distro_name
distro_name['stretch']="Debian 9 Stretch"
distro_name['buster']="Debian 10 Buster"
distro_name['bullseye']="Debian 11 Bullseye"
distro_name['xenial']="Ubuntu Xenial 16.04 LTS"
distro_name['bionic']="Ubuntu Bionic 18.04 LTS"
distro_name['focal']="Ubuntu Focal 20.04 LTS"
distro_name['eoan']="Ubuntu Eoan 19.10"

if [[ ${BUILD_OPT} == image || ${BUILD_OPT} == rootfs ]]; then

	RELEASE_TARGET="stretch buster bullseye xenial bionic eoan focal"

	if [[ -z $RELEASE ]]; then

		if [[ $BRANCH == legacy ]]; then
		
			if [[ $LINUXFAMILY == sun50iw9 || $LINUXFAMILY == sun50iw6 ]]; then
		
				RELEASE_TARGET="buster bionic focal"
			elif [[ $LINUXFAMILY == rk3399 ]]; then

				RELEASE_TARGET="xenial bionic buster"
			else
	       	 		RELEASE_TARGET="xenial"
			fi

		elif [[ $BRANCH == current ]]; then

	        	RELEASE_TARGET="buster bionic focal"
			[[ $LINUXFAMILY == sun50iw6 ]] && RELEASE_TARGET="buster focal"
		else

			[[ -z $BRANCH ]] && exit_with_error "No kernel branch selected"
		fi

                distro_menu "stretch"
                distro_menu "buster"
                distro_menu "bullseye"
                distro_menu "xenial"
                distro_menu "bionic"
                distro_menu "eoan"
                distro_menu "focal"

		menustr="Select the target OS release package base"
		RELEASE=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" \
				  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
				  --cancel-button Exit --ok-button Select "${options[@]}" \
				  3>&1 1>&2 2>&3)

		unset options
		[[ -z $RELEASE ]] && exit_with_error "No option selected"
	fi

	# don't show desktop option if we choose minimal build
	[[ $BUILD_MINIMAL == yes ]] && BUILD_DESKTOP="no"

	if [[ -z $BUILD_DESKTOP ]]; then

		options+=("no"		"Image with console interface (server)")
		options+=("yes"		"Image with desktop environment")

		menustr="Select the target image type."
		BUILD_DESKTOP=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" --notags \
				  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
				  --cancel-button Exit --ok-button Select "${options[@]}" \
				  3>&1 1>&2 2>&3)

		unset options
		[[ -z $BUILD_DESKTOP ]] && exit_with_error "No option selected"
		[[ $BUILD_DESKTOP == yes ]] && BUILD_MINIMAL="no"
	fi

	if [[ $BUILD_DESKTOP == "no" && -z $BUILD_MINIMAL ]]; then
	
	    options+=("no" "Standard image with console interface")
	    options+=("yes" "Minimal image with console interface")
		
		menustr="Select the target image type."
	    BUILD_MINIMAL=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" --notags \
				  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
				  --cancel-button Exit --ok-button Select "${options[@]}" \
				  3>&1 1>&2 2>&3)

	    unset options
	    [[ -z $BUILD_MINIMAL ]] && exit_with_error "No option selected"
	fi
fi

#prevent conflicting setup
[[ $BUILD_DESKTOP == yes ]] && BUILD_MINIMAL=no
[[ $BUILD_DESKTOP == yes ]] && IMAGETYPE="desktop"
[[ $BUILD_DESKTOP == no ]] && IMAGETYPE="server"
[[ $BUILD_MINIMAL == yes ]] && EXTERNAL_NEW=no

CONTAINER_COMPAT="no"
[[ -z $COMPRESS_OUTPUTIMAGE ]] && COMPRESS_OUTPUTIMAGE="yes"

#shellcheck source=configuration.sh
source "${SRC}"/scripts/configuration.sh

# optimize build time with 100% CPU usage
CPUS=$(grep -c 'processor' /proc/cpuinfo)
if [[ $USEALLCORES != no ]]; then

	CTHREADS="-j$((CPUS + CPUS/2))"

else

	CTHREADS="-j1"

fi

if [[ $BUILD_ALL == yes && -n $GPG_PASS ]]; then
    IMAGE_TYPE=stable
else
    IMAGE_TYPE=user-built
fi

branch2dir() {
	[[ "${1}" == "head" ]] && echo "HEAD" || echo "${1##*:}"
}

BOOTSOURCEDIR="${BOOTDIR}/$(branch2dir "${BOOTBRANCH}")"
LINUXSOURCEDIR="${KERNELDIR}/$(branch2dir "${KERNELBRANCH}")"
[[ -n $ATFSOURCE ]] && ATFSOURCEDIR="${ATFDIR}/$(branch2dir "${ATFBRANCH}")"

# The version of the Linux kernel
#VER=$(grab_version "$LINUXSOURCEDIR")
#KERNEL_NAME="linux${VER}"

# define package names
DEB_BRANCH=${BRANCH//default}
# if not empty, append hyphen
DEB_BRANCH=${DEB_BRANCH:+${DEB_BRANCH}-}
CHOSEN_UBOOT=linux-u-boot-${DEB_BRANCH}${BOARD}
CHOSEN_KERNEL=linux-image-${DEB_BRANCH}${LINUXFAMILY}
CHOSEN_ROOTFS=linux-${RELEASE}-root-${DEB_BRANCH}${BOARD}
CHOSEN_DESKTOP=orangepi-${RELEASE}-desktop
CHOSEN_KSRC=linux-source-${BRANCH}-${LINUXFAMILY}

do_default() {

start=$(date +%s)

# Check and install dependencies, directory structure and settings
# The OFFLINE_WORK variable inside the function
prepare_host

[[ $CLEAN_LEVEL == *sources* ]] && cleaning "sources"

# fetch_from_repo <url> <dir> <ref> <subdir_flag>

# ignore updates help on building all images - for internal purposes
if [[ $IGNORE_UPDATES != yes ]]; then
display_alert "Downloading sources" "" "info"

	fetch_from_repo "$BOOTSOURCE" "$BOOTDIR" "$BOOTBRANCH" "yes"
	fetch_from_repo "$KERNELSOURCE" "$KERNELDIR" "$KERNELBRANCH" "yes"
	if [[ -n $ATFSOURCE ]]; then
		fetch_from_repo "$ATFSOURCE" "${EXTER}/cache/sources/$ATFDIR" "$ATFBRANCH" "yes"
	fi
	
	fetch_from_repo "https://github.com/linux-sunxi/sunxi-tools" "${EXTER}/cache/sources/sunxi-tools" "branch:master"
	fetch_from_repo "https://github.com/armbian/rkbin" "${EXTER}/cache/sources/rkbin-tools" "branch:master"

	if [[ $BOARD == orangepi4 ]]; then
		fetch_from_repo "https://github.com/orangepi-xunlong/rk3399_gst_xserver_libs.git" "${EXTER}/cache/sources/rk3399_gst_xserver_libs" "branch:main"
	fi
fi

compile_sunxi_tools
install_rkbin_tools

for option in $(tr ',' ' ' <<< "$CLEAN_LEVEL"); do
	[[ $option != sources ]] && cleaning "$option"
done

# Compile u-boot if packed .deb does not exist or use the one from Orange Pi
if [[ $BUILD_OPT == u-boot || $BUILD_OPT == image ]]; then

	if [[ ! -f "${DEB_STORAGE}"/u-boot/${CHOSEN_UBOOT}_${REVISION}_${ARCH}.deb ]]; then

		[[ -n "${ATFSOURCE}" && "${REPOSITORY_INSTALL}" != *u-boot* ]] && compile_atf
		
		[[ ${REPOSITORY_INSTALL} != *u-boot* ]] && compile_uboot
	fi

	if [[ $BUILD_OPT == "u-boot" ]]; then
		display_alert "U-boot build done" "@host" "info"
		display_alert "Target directory" "${DEB_STORAGE}/u-boot" "info"
		display_alert "File name" "${CHOSEN_UBOOT}_${REVISION}_${ARCH}.deb" "info"
	fi
fi

# Compile kernel if packed .deb does not exist or use the one from Orange Pi
if [[ $BUILD_OPT == kernel || $BUILD_OPT == image ]]; then

	if [[ ! -f ${DEB_STORAGE}/${CHOSEN_KERNEL}_${REVISION}_${ARCH}.deb ]]; then 

		[[ "${REPOSITORY_INSTALL}" != *kernel* ]] && compile_kernel
	fi

	if [[ $BUILD_OPT == "kernel" ]]; then
		display_alert "Kernel build done" "@host" "info"
		display_alert "Target directory" "${DEB_STORAGE}/" "info"
		display_alert "File name" "${CHOSEN_KERNEL}_${REVISION}_${ARCH}.deb" "info"
	fi
fi

if [[ $BUILD_OPT == rootfs || $BUILD_OPT == image ]]; then

	# Compile orangepi-config if packed .deb does not exist or use the one from Orange Pi
	if [[ ! -f ${DEB_STORAGE}/orangepi-config_${REVISION}_all.deb ]]; then
	
		[[ "${REPOSITORY_INSTALL}" != *orangepi-config* ]] && compile_orangepi-config
	fi 
	
	if [[ ! -f ${DEB_STORAGE}/orangepi-firmware_${REVISION}_all.deb ]]; then 
	
		[[ "${REPOSITORY_INSTALL}" != *orangepi-firmware* ]] && compile_firmware
	fi
	
	overlayfs_wrapper "cleanup"
	
	# create board support package
	if [[ ! -f ${DEB_STORAGE}/$RELEASE/${CHOSEN_ROOTFS}_${REVISION}_${ARCH}.deb ]]; then 
	
		[[ "${REPOSITORY_INSTALL}" != *bsp* ]] && create_board_package
	fi
	
	# create desktop package
	if [[ ! -f ${DEB_STORAGE}/$RELEASE/${CHOSEN_DESKTOP}_${REVISION}_all.deb ]]; then
	
		[[ "${REPOSITORY_INSTALL}" != *orangepi-desktop* ]] && create_desktop_package
	fi
	
	# build additional packages
	[[ $EXTERNAL_NEW == compile ]] && chroot_build_packages
	
	[[ $BSP_BUILD != yes ]] && debootstrap_ng

fi

# hook for function to run after build, i.e. to change owner of $SRC
# NOTE: this will run only if there were no errors during build process
[[ $(type -t run_after_build) == function ]] && run_after_build || true

end=$(date +%s)
runtime=$(((end-start)/60))
display_alert "Runtime" "$runtime min" "info"

# Make it easy to repeat build by displaying build options used
[ "$(systemd-detect-virt)" == 'docker' ] && BUILD_CONFIG='docker'

display_alert "Repeat Build Options" "sudo ./build.sh ${BUILD_CONFIG} BOARD=${BOARD} BRANCH=${BRANCH} \
$([[ -n $BUILD_OPT ]] && echo "BUILD_OPT=${BUILD_OPT} ")\
$([[ -n $RELEASE ]] && echo "RELEASE=${RELEASE} ")\
$([[ -n $BUILD_MINIMAL ]] && echo "BUILD_MINIMAL=${BUILD_MINIMAL} ")\
$([[ -n $BUILD_DESKTOP ]] && echo "BUILD_DESKTOP=${BUILD_DESKTOP} ")\
$([[ -n $KERNEL_CONFIGURE ]] && echo "KERNEL_CONFIGURE=${KERNEL_CONFIGURE}")\
" "info"

} # end of do_default()

if [[ -z $1 ]]; then
	do_default
else
	eval "$@"
fi
