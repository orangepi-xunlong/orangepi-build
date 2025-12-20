#!/bin/bash
#
# Copyright (c) 2013-2021 Igor Pecovnik, igor.pecovnik@gma**.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# Main program
#


cleanup_list() {
	local varname="${1}"
	local list_to_clean="${!varname}"
	list_to_clean="${list_to_clean#"${list_to_clean%%[![:space:]]*}"}"
	list_to_clean="${list_to_clean%"${list_to_clean##*[![:space:]]}"}"
	echo ${list_to_clean}
}




if [[ $(basename "$0") == main.sh ]]; then

	echo "Please use build.sh to start the build process"
	exit 255

fi




# default umask for root is 022 so parent directories won't be group writeable without this
# this is used instead of making the chmod in prepare_host() recursive
umask 002

# destination
if [ -d "$CONFIG_PATH/output" ]; then
	DEST="${CONFIG_PATH}"/output
else
	DEST="${SRC}"/output
fi

[[ -z $REVISION ]] && REVISION="3.0.8"

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

# Warnings mitigation
[[ -z $LANGUAGE ]] && export LANGUAGE="en_US:en"            # set to english if not set
[[ -z $CONSOLE_CHAR ]] && export CONSOLE_CHAR="UTF-8"       # set console to UTF-8 if not set

# Libraries include

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
#source "${SRC}"/scripts/compilation-prepare.sh	# drivers that are not upstreamed
# shellcheck source=makeboarddeb.sh
source "${SRC}"/scripts/makeboarddeb.sh		# board support package
# shellcheck source=general.sh
source "${SRC}"/scripts/general.sh		# general functions
# shellcheck source=chroot-buildpackages.sh
source "${SRC}"/scripts/chroot-buildpackages.sh	# chroot packages building
# shellcheck source=pack.sh
source "${SRC}"/scripts/pack-uboot.sh
# shellcheck source=build-cix-image.sh
source "${SRC}"/scripts/build-cix-image.sh


# set log path
LOG_SUBPATH=${LOG_SUBPATH:=debug}

# compress and remove old logs
mkdir -p "${DEST}"/${LOG_SUBPATH}
(cd "${DEST}"/${LOG_SUBPATH} && tar -czf logs-"$(<timestamp)".tgz ./*.log) > /dev/null 2>&1
rm -f "${DEST}"/${LOG_SUBPATH}/*.log > /dev/null 2>&1
date +"%d_%m_%Y-%H_%M_%S" > "${DEST}"/${LOG_SUBPATH}/timestamp

# delete compressed logs older than 7 days
(cd "${DEST}"/${LOG_SUBPATH} && find . -name '*.tgz' -mtime +7 -delete) > /dev/null

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
	[[ $PRIVATE_CCACHE == yes ]] && export CCACHE_DIR=$EXTER/cache/ccache

else

	CCACHE=""

fi




if [[ -n $REPOSITORY_UPDATE ]]; then

		# select stable/beta configuration
		if [[ $BETA == yes ]]; then
				DEB_STORAGE=$DEST/debs-beta
				REPO_STORAGE=$DEST/repository-beta
				REPO_CONFIG="aptly-beta.conf"
		else
				DEB_STORAGE=$DEST/debs
				REPO_STORAGE=$DEST/repository
				REPO_CONFIG="aptly.conf"
		fi

		# For user override
		if [[ -f "${USERPATCHES_PATH}"/lib.config ]]; then
				display_alert "Using user configuration override" "userpatches/lib.config" "info"
			source "${USERPATCHES_PATH}"/lib.config
		fi

		repo-manipulate "$REPOSITORY_UPDATE"
		exit

fi




if [[ -z $BOARD ]]; then
	BOARD="orangepi6plus"
fi

BOARD_TYPE="conf"
# shellcheck source=/dev/null
source "${EXTER}/config/boards/${BOARD}.${BOARD_TYPE}"
LINUXFAMILY="${BOARDFAMILY}"

[[ -z $KERNEL_TARGET ]] && exit_with_error "Board configuration does not define valid kernel config"

# if BUILD_OPT, KERNEL_CONFIGURE, BOARD, BRANCH or RELEASE are not set, display selection menu
if [[ -z $BUILD_OPT ]]; then

	if [[ $BOARDFAMILY != "cix" ]]; then
		options+=("u-boot"	 "U-boot package")
	fi
	options+=("kernel"	 "Kernel package")
	options+=("rootfs"	 "Rootfs and all deb packages")
	options+=("image"	 "Full OS image for flashing")

	if [[ $BOARDFAMILY != "cix" ]]; then
		menustr="Compile image | rootfs | kernel | u-boot"
	else
		menustr="Compile image | rootfs | kernel"
	fi
	BUILD_OPT=$(whiptail --title "${titlestr}" --backtitle "${backtitle}" --notags \
			  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
			  --cancel-button Exit --ok-button Select "${options[@]}" \
			  3>&1 1>&2 2>&3)

	unset options
	[[ -z $BUILD_OPT ]] && exit_with_error "No option selected"
	[[ $BUILD_OPT == rootfs ]] && ROOT_FS_CREATE_ONLY="yes"
fi




if [[ ${BUILD_OPT} =~ kernel|image ]]; then

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



# 
if [[ -z $BRANCH ]]; then
	BRANCH="next"
	[[ $BRANCH == dev && $SHOW_WARNING == yes ]] && show_developer_warning
fi

if [[ $BUILD_OPT =~ rootfs|image && -z $RELEASE ]]; then

	options=()

	distros_options

	menustr="Select the target OS release package base"
	RELEASE=$(whiptail --title "Choose a release package base" --backtitle "${backtitle}" \
			  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
			  --cancel-button Exit --ok-button Select "${options[@]}" \
			  3>&1 1>&2 2>&3)
	#echo "options : ${options}"
	[[ -z $RELEASE ]] && exit_with_error "No release selected"

	unset options
fi

# don't show desktop option if we choose minimal build
[[ $BUILD_MINIMAL == yes ]] && BUILD_DESKTOP=no

if [[ $BUILD_OPT =~ rootfs|image && -z $BUILD_DESKTOP ]]; then

	# read distribution support status which is written to the orangepi-release file
	set_distribution_status

	options=()
	options+=("no" "Image with console interface (server)")
	options+=("yes" "Image with desktop environment")

	menustr="Select the target image type"
	BUILD_DESKTOP=$(whiptail --title "Choose image type" --backtitle "${backtitle}" \
			  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
			  --cancel-button Exit --ok-button Select "${options[@]}" \
			  3>&1 1>&2 2>&3)
	unset options
	[[ -z $BUILD_DESKTOP ]] && exit_with_error "No option selected"
	if [[ ${BUILD_DESKTOP} == "yes" ]]; then
		BUILD_MINIMAL=no
		SELECTED_CONFIGURATION="desktop"
	fi

fi

if [[ $BUILD_OPT =~ rootfs|image && $BUILD_DESKTOP == no && -z $BUILD_MINIMAL ]]; then

	options=()
	options+=("no" "Standard image with console interface")
	options+=("yes" "Minimal image with console interface")
	menustr="Select the target image type"
	BUILD_MINIMAL=$(whiptail --title "Choose image type" --backtitle "${backtitle}" \
			  --menu "${menustr}" "${TTY_Y}" "${TTY_X}" $((TTY_Y - 8))  \
			  --cancel-button Exit --ok-button Select "${options[@]}" \
			  3>&1 1>&2 2>&3)
	unset options
	[[ -z $BUILD_MINIMAL ]] && exit_with_error "No option selected"
	if [[ $BUILD_MINIMAL == "yes" ]]; then
		SELECTED_CONFIGURATION="cli_minimal"
	else
		SELECTED_CONFIGURATION="cli_standard"
	fi

fi

#prevent conflicting setup
if [[ $BUILD_DESKTOP == "yes" ]]; then
	BUILD_MINIMAL=no
	SELECTED_CONFIGURATION="desktop"
elif [[ $BUILD_MINIMAL != "yes" || -z "${BUILD_MINIMAL}" ]]; then
	BUILD_MINIMAL=no # Just in case BUILD_MINIMAL is not defined
	BUILD_DESKTOP=no
	SELECTED_CONFIGURATION="cli_standard"
elif [[ $BUILD_MINIMAL == "yes" ]]; then
	BUILD_DESKTOP=no
	SELECTED_CONFIGURATION="cli_minimal"
fi

#[[ ${KERNEL_CONFIGURE} == prebuilt ]] && [[ -z ${REPOSITORY_INSTALL} ]] && \
#REPOSITORY_INSTALL="u-boot,kernel,bsp,orangepi-zsh,orangepi-config,orangepi-firmware${BUILD_DESKTOP:+,orangepi-desktop}"


#shellcheck source=configuration.sh
source "${SRC}"/scripts/configuration.sh

# optimize build time with 100% CPU usage
CPUS=$(grep -c 'processor' /proc/cpuinfo)
if [[ $USEALLCORES != no ]]; then

	CTHREADS="-j$((CPUS + CPUS/2))"

else

	CTHREADS="-j1"

fi

call_extension_method "post_determine_cthreads" "config_post_determine_cthreads" << 'POST_DETERMINE_CTHREADS'
*give config a chance modify CTHREADS programatically. A build server may work better with hyperthreads-1 for example.*
Called early, before any compilation work starts.
POST_DETERMINE_CTHREADS

if [[ $BETA == yes ]]; then
	IMAGE_TYPE=nightly
elif [[ $BETA != "yes" && $BUILD_ALL == yes && -n $GPG_PASS ]]; then
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

BSP_CLI_PACKAGE_NAME="orangepi-bsp-cli-${BOARD}"
BSP_CLI_PACKAGE_FULLNAME="${BSP_CLI_PACKAGE_NAME}_${REVISION}_${ARCH}"
BSP_DESKTOP_PACKAGE_NAME="orangepi-bsp-desktop-${BOARD}"
BSP_DESKTOP_PACKAGE_FULLNAME="${BSP_DESKTOP_PACKAGE_NAME}_${REVISION}_${ARCH}"

CHOSEN_UBOOT=linux-u-boot-${BRANCH}-${BOARD}
CHOSEN_KERNEL=linux-image-${BRANCH}-${LINUXFAMILY}
CHOSEN_ROOTFS=${BSP_CLI_PACKAGE_NAME}
CHOSEN_DESKTOP=orangepi-${RELEASE}-desktop-${DESKTOP_ENVIRONMENT}
CHOSEN_KSRC=linux-source-${BRANCH}-${LINUXFAMILY}

do_default() {

start=$(date +%s)

# Check and install dependencies, directory structure and settings
# The OFFLINE_WORK variable inside the function
prepare_host

[[ "${JUST_INIT}" == "yes" ]] && exit 0

[[ $CLEAN_LEVEL == *sources* ]] && cleaning "sources"

# fetch_from_repo <url> <dir> <ref> <subdir_flag>

# ignore updates help on building all images - for internal purposes
if [[ ${IGNORE_UPDATES} != yes ]]; then

	display_alert "Downloading sources" "" "info"

	if [[ $BOARDFAMILY != "cix" ]]; then

		[[ $BUILD_OPT =~ u-boot|image ]] && fetch_from_repo "$BOOTSOURCE" "$BOOTDIR" "$BOOTBRANCH" "yes"

	fi

	if [[ $BOARDFAMILY == "cix" ]]; then
		local comp_name="component_cix-$BRANCH"
		local comp_dest="${EXTER}/cache/sources/${comp_name}"
		
		local zip_url="https://github.com/orangepi-xunlong/${comp_name}/archive/refs/heads/main.zip"
		local local_zip="${SRC}/${comp_name}-main.zip"
		local extracted_dir="${SRC}/${comp_name}-main"

		if [[ ! -d "${comp_dest}" ]]; then
			display_alert "CIX component missing" "Attemping download..." "info"

			if [[ ! -f "${local_zip}" && ! -d "${extracted_dir}" ]]; then
				display_alert "Downloading ZIP" "${zip_url}" "info"
				wget -q -O "${local_zip}" "${zip_url}" || rm -f "${local_zip}"
			fi

			if [[ -f "${local_zip}" ]]; then
				display_alert "Extracting ZIP" "${local_zip}" "info"
				unzip -q -o "${local_zip}" -d "${SRC}"
				
				if [[ -d "${extracted_dir}" ]]; then
					display_alert "Setting up source" "${comp_name}" "info"
					mkdir -p "$(dirname "${comp_dest}")"
					mv "${extracted_dir}" "${comp_dest}"
				else
					display_alert "Extraction failed" "Directory not found" "wrn"
				fi
			fi
		fi

		if [[ ! -d "${comp_dest}" ]]; then
			display_alert "Fallback to Gitee" "Cloning ${comp_name}..." "wrn"
			fetch_from_repo "https://gitee.com/orangepi-xunlong/${comp_name}.git" "${comp_dest}" "branch:main"
		fi
		
		# Copy selected CIX deb packages to overlay

		# echo "DEBUG: comp_dest is '${comp_dest}'"
		# echo "DEBUG: Checking source dir: '${comp_dest}/debs'"

		CIX_DEBS_SOURCE="${comp_dest}/debs"
		CIX_DEBS_TARGET="${SRC}/userpatches/overlay/opt/cix_debs"
		mkdir -p "${CIX_DEBS_TARGET}"

		CIX_PACKAGES=(
			"cix-audio-dsp_1.0.0_arm64.deb"
			"cix-isp-umd_1.0.0_arm64_orangepi.deb"
			"cix-common-misc_1.0.0_arm64.deb"
			"cix-libdrm_1.0.0_arm64.deb"
			"cix-cpipe_1.0.0_arm64.deb"
			"cix-libglvnd_1.7.0_arm64.deb"
			"cix-debian-misc_1.0.0_arm64.deb"
			"cix-env_1.0.0_arm64.deb"
			"cix-mesa_24.0.4_arm64.deb"
			"cix-firmware_1.0.0_arm64.deb"
			"cix-mnn_1.2.1_arm64.deb"
			"cix-gpu-dkms_1.0.0_arm64.deb"
			"cix-gpu-test_1.0.0_arm64.deb"
			"cix-gpu-umd_2.0.0_arm64.deb"
			"cix-optee_1.0.0_arm64.deb"
			"cix-grubcfg_1.0.0_arm64.deb"
			"cix-tools_1.0.0_arm64.deb"
			"cix-gstreamer_1.22.1_arm64.deb"
			"cix-vpu-test_1.0.0_arm64.deb"
		)

		if [ -d "$CIX_DEBS_SOURCE" ]; then
			display_alert "Custom Patch" "Copying selected CIX debs to overlay..." "info"
			
			count=0
			
			for deb in "${CIX_PACKAGES[@]}"; do
				if [ -f "$CIX_DEBS_SOURCE/$deb" ]; then
					cp -f "$CIX_DEBS_SOURCE/$deb" "$CIX_DEBS_TARGET/"
					((count++))
				else
					echo "WARN: Package not found: $deb"
				fi
			done
			
			echo "DEBUG: Copied $count files to $CIX_DEBS_TARGET"
			ls -l "$CIX_DEBS_TARGET"
		else
			display_alert "ERROR" "Source directory not found: $CIX_DEBS_SOURCE" "err"
			ls -ld "${comp_dest}"
		fi
	fi

	[[ $BUILD_OPT =~ kernel|image ]] && fetch_from_repo "$KERNELSOURCE" "$KERNELDIR" "$KERNELBRANCH" "yes"

	if [[ -n ${ATFSOURCE} ]]; then

		[[ ${BUILD_OPT} =~ u-boot|image ]] && fetch_from_repo "$ATFSOURCE" "${EXTER}/cache/sources/$ATFDIR" "$ATFBRANCH" "yes"

	fi

	if [[ ${BOARD} =~ orangepi4|orangepi4-lts|orangepi800 && $BRANCH == legacy ]]; then

		[[ $BUILD_OPT =~ image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk3399_gst_xserver_libs.git" "${EXTER}/cache/sources/rk3399_gst_xserver_libs" "branch:main"

	fi

	if [[ ${BOARD} =~ orangepiaimax ]]; then

		[[ $BUILD_OPT =~ image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/ascend-driver.git" "${EXTER}/cache/sources/ascend-driver" "branch:main"

	fi

	if [[ "${BOARD}x" =~ orangepi4x|orangepi4-ltsx|orangepi800x && $RELEASE =~ focal|buster|bullseye|bookworm ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/rk-rootfs-build-${RELEASE}" "branch:rk-rootfs-build-${RELEASE}"
		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/rk35xx_packages" "branch:rk35xx_packages"

	fi

	if [[ ${BOARDFAMILY} == "rockchip-rk3588" && $RELEASE =~ bullseye|bookworm|focal|jammy|raspi ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/rk35xx_packages" "branch:rk35xx_packages"

	fi

	if [[ ${BOARDFAMILY} == "rockchip-rk356x" && $RELEASE =~ bullseye|focal|jammy|raspi ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/rk35xx_packages" "branch:rk35xx_packages"

	fi

	if [[ ${BOARDFAMILY} == "ky" && $RELEASE =~ noble ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/ky_packages" "branch:ky_packages"

	fi

	if [[ ${BOARDFAMILY} == "sun55iw3" && $RELEASE =~ bookworm|jammy ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/t527_packages" "branch:t527_packages"

	fi

	if [[ ${BOARDFAMILY} == "sun60iw2" && $RELEASE =~ bullseye ]]; then
		if [[ ${GITEE_SERVER} == yes ]]; then
		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://gitee.com/orangepi-xunlong/sun60iw2_packages.git" "${EXTER}/cache/sources/sun60iw2_packages" "branch:sun60iw2_packages"
		else
		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/sun60iw2_packages" "branch:sun60iw2_packages"
		fi
	fi

	if [[ ${BOARD} =~ orangepi3|orangepi3-lts && $RELEASE =~ bullseye && $BRANCH == current ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/ffmpeg_kodi_${RELEASE}" "branch:ffmpeg_kodi_${RELEASE}"

	fi

	if [[ "${BOARD}x" =~ orangepi4x|orangepi4-ltsx|orangepi800x && $RELEASE =~ jammy && $BRANCH == next ]]; then

		[[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/ffmpeg_kodi_${RELEASE}" "branch:ffmpeg_kodi_${RELEASE}"

	fi

	call_extension_method "fetch_sources_tools"  <<- 'FETCH_SOURCES_TOOLS'
	*fetch host-side sources needed for tools and build*
	Run early to fetch_from_repo or otherwise obtain sources for needed tools.
	FETCH_SOURCES_TOOLS

	call_extension_method "build_host_tools"  <<- 'BUILD_HOST_TOOLS'
	*build needed tools for the build, host-side*
	After sources are fetched, build host-side tools needed for the build.
	BUILD_HOST_TOOLS
fi

for option in $(tr ',' ' ' <<< "$CLEAN_LEVEL"); do
	[[ $option != sources ]] && cleaning "$option"
done

# Compile u-boot if packed .deb does not exist or use the one from Orange Pi
if [[ $BUILD_OPT == u-boot || $BUILD_OPT == image ]]; then

	if [[ $BOARDFAMILY != "cix" ]]; then
		if [[ ! -f "${DEB_STORAGE}"/u-boot/${CHOSEN_UBOOT}_${REVISION}_${ARCH}.deb ]]; then

			[[ -n "${ATFSOURCE}" && "${REPOSITORY_INSTALL}" != *u-boot* ]] && compile_atf

			[[ ${REPOSITORY_INSTALL} != *u-boot* ]] && compile_uboot
		fi

		if [[ $BUILD_OPT == "u-boot" ]]; then
			unset BUILD_MINIMAL BUILD_DESKTOP COMPRESS_OUTPUTIMAGE
			display_alert "U-boot build done" "@host" "info"
			display_alert "Target directory" "${DEB_STORAGE}/u-boot" "info"
			display_alert "File name" "${CHOSEN_UBOOT}_${REVISION}_${ARCH}.deb" "info"
		fi
	fi
fi

# Compile kernel if packed .deb does not exist or use the one from Orange Pi
if [[ $BUILD_OPT == kernel || $BUILD_OPT == image ]]; then

	if [[ ! -f ${DEB_STORAGE}/${CHOSEN_KERNEL}_${REVISION}_${ARCH}.deb ]]; then 

		KDEB_CHANGELOG_DIST=$RELEASE
		[[ "${REPOSITORY_INSTALL}" != *kernel* ]] && compile_kernel
	fi

	if [[ $BUILD_OPT == "kernel" ]]; then
		unset BUILD_MINIMAL BUILD_DESKTOP COMPRESS_OUTPUTIMAGE
		display_alert "Kernel build done" "@host" "info"
		display_alert "Target directory" "${DEB_STORAGE}/" "info"
		display_alert "File name" "${CHOSEN_KERNEL}_${REVISION}_${ARCH}.deb" "info"
	fi
fi

if [[ $BUILD_OPT == rootfs || $BUILD_OPT == image ]]; then

	# # Compile orangepi-config if packed .deb does not exist or use the one from Orange Pi
	# if [[ ! -f ${DEB_STORAGE}/orangepi-config_${REVISION}_all.deb ]]; then
	
	# 	[[ "${REPOSITORY_INSTALL}" != *orangepi-config* ]] && compile_orangepi-config
	# fi 

	# # Compile orangepi-zsh if packed .deb does not exist or use the one from repository
	# if [[ ! -f ${DEB_STORAGE}/orangepi-zsh_${REVISION}_all.deb ]]; then

	#         [[ "${REPOSITORY_INSTALL}" != *orangepi-zsh* ]] && compile_orangepi-zsh
	# fi

	# # Compile plymouth-theme-orangepi if packed .deb does not exist or use the one from repository
	# if [[ ! -f ${DEB_STORAGE}/plymouth-theme-orangepi_${REVISION}_all.deb && $PLYMOUTH == yes ]]; then

	# 	[[ "${REPOSITORY_INSTALL}" != *plymouth-theme-orangepi* ]] && compile_plymouth-theme-orangepi
	# fi

	# Compile orangepi-firmware if packed .deb does not exist or use the one from repository
	if [[ "${REPOSITORY_INSTALL}" != *orangepi-firmware* ]]; then

		if ! ls "${DEB_STORAGE}/orangepi-firmware_${REVISION}_all.deb" 1> /dev/null 2>&1; then

			FULL=""
			REPLACE="-full"
			compile_firmware

		fi

		#if ! ls "${DEB_STORAGE}/orangepi-firmware-full_${REVISION}_all.deb" 1> /dev/null 2>&1; then

			#FULL="-full"
			#REPLACE=""
			#compile_firmware

		#fi

	fi

	overlayfs_wrapper "cleanup"
	
	# create board support package
	[[ -n $RELEASE && ! -f ${DEB_STORAGE}/$RELEASE/${BSP_CLI_PACKAGE_FULLNAME}.deb ]] && create_board_package

	# create desktop package
	#[[ -n $RELEASE && $DESKTOP_ENVIRONMENT && ! -f ${DEB_STORAGE}/$RELEASE/${CHOSEN_DESKTOP}_${REVISION}_all.deb ]] && create_desktop_package
	#[[ -n $RELEASE && $DESKTOP_ENVIRONMENT && ! -f ${DEB_STORAGE}/${RELEASE}/${BSP_DESKTOP_PACKAGE_FULLNAME}.deb ]] && create_bsp_desktop_package
	[[ -n $RELEASE && $DESKTOP_ENVIRONMENT ]] && create_desktop_package
	[[ -n $RELEASE && $DESKTOP_ENVIRONMENT ]] && create_bsp_desktop_package
	
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
$([[ -n $KERNEL_CONFIGURE ]] && echo "KERNEL_CONFIGURE=${KERNEL_CONFIGURE} ")\
$([[ -n $DESKTOP_ENVIRONMENT ]] && echo "DESKTOP_ENVIRONMENT=${DESKTOP_ENVIRONMENT} ")\
$([[ -n $DESKTOP_ENVIRONMENT_CONFIG_NAME  ]] && echo "DESKTOP_ENVIRONMENT_CONFIG_NAME=${DESKTOP_ENVIRONMENT_CONFIG_NAME} ")\
$([[ -n $DESKTOP_APPGROUPS_SELECTED ]] && echo "DESKTOP_APPGROUPS_SELECTED=\"${DESKTOP_APPGROUPS_SELECTED}\" ")\
$([[ -n $DESKTOP_APT_FLAGS_SELECTED ]] && echo "DESKTOP_APT_FLAGS_SELECTED=\"${DESKTOP_APT_FLAGS_SELECTED}\" ")\
$([[ -n $COMPRESS_OUTPUTIMAGE ]] && echo "COMPRESS_OUTPUTIMAGE=${COMPRESS_OUTPUTIMAGE} ")\
" "ext"

} # end of do_default()

if [[ -z $1 ]]; then
	do_default
else
	eval "$@"
fi
