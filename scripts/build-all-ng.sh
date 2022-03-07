#!/bin/bash
#
# Copyright (c) 2013-2021 Igor Pecovnik, igor.pecovnik@gma**.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.


# Functions:
# unset_all
# pack_upload
# build_main
# array_contains
# check_hash
# build_all


[[ -z $START ]] && START="0"
[[ -z $BSP_BUILD ]] && BSP_BUILD="no"
[[ -z $MULTITHREAD ]] && MULTITHREAD="0"
[[ -z $STABILITY ]] && STABILITY="stable"
[[ -z $BUILD_OPT ]] && BUILD_OPT="image"
[[ -z $BRANCH_OVER ]] && BRANCH_OVER=""
[[ -z $RELEASE_OVER ]] && RELEASE_OVER=""
[[ -z $CLEAN_LEVEL ]] && CLEAN_LEVEL="oldcache"
[[ -z $KERNEL_CONFIGURE ]] && KERNEL_CONFIGURE="no"

# cleanup
rm -f /run/orangepi/*.pid
mkdir -p /run/orangepi

# read user defined targets if exits
if [[ -f $USERPATCHES_PATH/targets.conf ]]; then

	display_alert "Adding user provided targets configuration"
	BUILD_TARGETS="${USERPATCHES_PATH}/targets.conf"

else

	BUILD_TARGETS="${EXTER}/config/targets.conf"

fi

unset_all ()
{
unset	LINUXFAMILY LINUXCONFIG KERNELDIR KERNELSOURCE KERNELBRANCH BOOTDIR BOOTSOURCE BOOTBRANCH ARCH UBOOT_USE_GCC \
		KERNEL_USE_GCC CPUMIN CPUMAX UBOOT_VER KERNEL_VER GOVERNOR BOOTSIZE BOOTFS_TYPE UBOOT_TOOLCHAIN KERNEL_TOOLCHAIN \
		DEBOOTSTRAP_LIST PACKAGE_LIST_EXCLUDE KERNEL_IMAGE_TYPE write_uboot_platform family_tweaks family_tweaks_bsp \
		setup_write_uboot_platform uboot_custom_postprocess atf_custom_postprocess family_tweaks_s BOOTSCRIPT \
		UBOOT_TARGET_MAP LOCALVERSION UBOOT_COMPILER KERNEL_COMPILER BOOTCONFIG_VAR_NAME INITRD_ARCH BOOTENV_FILE BOOTDELAY \
		ATF_TOOLCHAIN2 MOUNT SDCARD BOOTPATCHDIR KERNELPATCHDIR RELEASE IMAGE_TYPE OVERLAY_PREFIX ASOUND_STATE ATF_COMPILER \
		ATF_USE_GCC ATFSOURCE ATFDIR ATFBRANCH ATFSOURCEDIR PACKAGE_LIST_RM NM_IGNORE_DEVICES DISPLAY_MANAGER \
		family_tweaks_bsp_s CRYPTROOT_ENABLE CRYPTROOT_PASSPHRASE CRYPTROOT_SSH_UNLOCK CRYPTROOT_SSH_UNLOCK_PORT \
		CRYPTROOT_SSH_UNLOCK_KEY_NAME ROOT_MAPPER NETWORK HDMI USB WIRELESS ARMBIANMONITOR FORCE_BOOTSCRIPT_UPDATE \
		UBOOT_TOOLCHAIN2 toolchain2 BUILD_REPOSITORY_URL BUILD_REPOSITORY_COMMIT BUILD_TARGET HOST BUILD_IMAGE \
		DEB_STORAGE REPO_STORAGE REPO_CONFIG REPOSITORY_UPDATE PACKAGE_LIST_RELEASE LOCAL_MIRROR COMPILE_ATF \
		PACKAGE_LIST_BOARD PACKAGE_LIST_FAMILY PACKAGE_LIST_DESKTOP_BOARD PACKAGE_LIST_DESKTOP_FAMILY ATF_COMPILE ATFPATCHDIR OFFSET BOOTSOURCEDIR BOOT_USE_BLOBS \
		BOOT_SOC DDR_BLOB MINILOADER_BLOB BL31_BLOB BOOT_RK3328_USE_AYUFAN_ATF BOOT_USE_BLOBS BOOT_RK3399_LEGACY_HYBRID \
		BOOT_USE_MAINLINE_ATF BOOT_USE_TPL_SPL_BLOB OFFLINE_WORK IMAGE_PARTITION_TABLE BOOT_LOGO
}

pack_upload ()
{
	# pack and upload to server or just pack

	local version="${BOARD^}_${REVISION}_${RELEASE}_${BRANCH}_${VER/-$LINUXFAMILY/}"
	local subdir=""

	cd "${DESTIMG}" || exit

	if [[ -n "${SEND_TO_SERVER}" ]]; then
		ssh "${SEND_TO_SERVER}" "mkdir -p ${SEND_TO_LOCATION}${BOARD}/{archive,nightly}" &
		display_alert "Uploading" "Please wait!" "info"
		nice -n 19 bash -c "rsync -arP --info=progress2 --prune-empty-dirs $DESTIMG/ -e 'ssh -T -c aes128-ctr -o Compression=no -x -p 22' ${SEND_TO_SERVER}:${SEND_TO_LOCATION}${BOARD}/${subdir}; rm -rf ${DESTIMG}/*" &
	fi
}



build_main ()
{

	# build images which we do pack or kernel
	#local upload_image="${BOARD^}_$(cat ${SRC}/VERSION)_${RELEASE}_${BRANCH}_*${VER/-$LINUXFAMILY/}"
	#local upload_subdir="archive"

	[[ $BUILD_DESKTOP == yes ]] && upload_image=${upload_image}_desktop
	[[ $BUILD_MINIMAL == yes ]] && upload_image=${upload_image}_minimal

	touch "/run/orangepi/${BOARD^}_${BRANCH}_${RELEASE}_${BUILD_DESKTOP}_${BUILD_MINIMAL}.pid";

	if [[ $BUILD_OPT == image ]]; then

		#if ssh ${SEND_TO_SERVER} stat ${SEND_TO_LOCATION}${BOARD}/${upload_subdir}/${upload_image}* \> /dev/null 2\>\&1; then
		#	echo "$n exists $upload_image"
		#else
			#shellcheck source=main.sh
			source "${SRC}"/scripts/main.sh
			#[[ $BSP_BUILD != yes ]] && pack_upload
		#fi
	else

		source "${SRC}"/scripts/main.sh
	fi

	echo -e "\n"
	rm "/run/orangepi/${BOARD^}_${BRANCH}_${RELEASE}_${BUILD_DESKTOP}_${BUILD_MINIMAL}.pid"
}




array_contains ()
{

	# utility snippet

	local array="$1[@]"
	local seeking=$2
	local in=1

	for element in "${!array}"; do
		if [[ "${element}" == "${seeking}" ]]; then
			in=0
			break
		fi
	done
	return $in
}




function build_all()
{

	# main routine

	buildall_start=$(date +%s)
	n=0
	ARRAY=()
	buildlist="cat "

	if [[ -f $USERPATCHES_PATH/build_all.config ]]; then

		source $USERPATCHES_PATH/build_all.config

	elif [[ -f $EXTER/config/templates/build_all.config ]]; then

		cp $EXTER/config/templates/build_all.config $USERPATCHES_PATH/
		source $USERPATCHES_PATH/build_all.config
	fi

	# building selected ones
	if [[ -n ${REBUILD_IMAGES} ]]; then

		buildlist="grep -w '"
		filter="'"
		for build in $(tr ',' ' ' <<< "${REBUILD_IMAGES}"); do
				buildlist=$buildlist"$build\|"
				filter=$filter"$build\|"
		done
		buildlist=${buildlist::-2}"'"
		filter=${filter::-2}"'"
	fi

	# find unique boards - we will build debs for all variants
	sorted_unique_ids=($(echo "${ids[@]}" | tr ' ' '\n' | sort -u | tr '\n' ' '))
	unique_boards=$(eval "${buildlist}" ${EXTER}/config/targets.conf | sed '/^#/ d' | awk '{print $1}')
	read -a unique_boards <<< $(echo "${unique_boards[@]}" | tr ' ' '\n' | sort -u | tr '\n' ' ')

	while read -r line; do

		[[ "${line}" =~ ^#.*$ ]] && continue
		[[ -n "${REBUILD_IMAGES}" ]] && [[ -z $(echo "${line}" | eval grep -w "${filter}") ]] && continue
		#[[ $n -lt $START ]] && ((n+=1)) && continue

		unset_all
		# unset also board related variables
		unset BOARDFAMILY DESKTOP_AUTOLOGIN DEFAULT_CONSOLE FULL_DESKTOP MODULES_CURRENT MODULES_LEGACY MODULES_DEV \
		BOOTCONFIG MODULES_BLACKLIST_LEGACY MODULES_BLACKLIST_CURRENT MODULES_BLACKLIST_DEV DEFAULT_OVERLAYS SERIALCON \
		BUILD_MINIMAL RELEASE ATFBRANCH BOOT_FDT_FILE BOOTCONFIG_DEV

		read -r BOARD BRANCH RELEASE BUILD_TARGET BUILD_STABILITY BUILD_IMAGE <<< "${line}"

		# read all possible configurations
		source ${EXTER}"/config/boards/${BOARD}".conf 2> /dev/null

		# override branch to build selected branches if defined
		[[ -n "${BRANCH_OVER}" ]] && [[ ${BRANCH} != ${BRANCH_OVER} ]] && continue
		# override release to build selected release if defined
		[[ -n "${RELEASE_OVER}" ]] && [[ "$RELEASE" != ${RELEASE_OVER} ]] && continue
		# override board to build selected board if defined
		[[ -n "${BOARD_OVER}" ]] && [[ "$BOARD" != ${BOARD_OVER} ]] && continue

		# small optimisation. we only (try to) build needed kernels
		if [[ $BUILD_OPT == kernel ]]; then

			source "${EXTER}/config/sources/families/${BOARDFAMILY}.conf" 2> /dev/null
			array_contains ARRAY "${LINUXFAMILY}${BRANCH}${BUILD_STABILITY}" && continue
			ARRAY+=("${LINUXFAMILY}${BRANCH}${BUILD_STABILITY}")
			BOARDFAMILY=${LINUXFAMILY}

		elif [[ $BUILD_OPT == u-boot ]]; then

			source "${EXTER}/config/sources/families/${BOARDFAMILY}.conf" 2> /dev/null
			array_contains ARRAY "${BOARD}${BRANCH}${BUILD_STABILITY}" && continue
			ARRAY+=("${BOARD}${BRANCH}${BUILD_STABILITY}")

        elif [[ $BUILD_IMAGE == no ]] ; then

            continue
        fi

		BUILD_DESKTOP="no"
		BUILD_MINIMAL="no"

		[[ ${BUILD_TARGET} == "desktop" ]] && BUILD_DESKTOP="yes"
		[[ ${BUILD_TARGET} == "minimal" ]] && BUILD_MINIMAL="yes"

		# create beta or stable
		if [[ "${BUILD_STABILITY}" == "${STABILITY}" ]]; then

			((n+=1))

			if [[ $1 != "dryrun" ]] && [[ $n -ge $START ]]; then

				while :
				do
					[[ $(find /run/orangepi/*.pid 2>/dev/null | wc -l) -le ${MULTITHREAD} || -z ${MULTITHREAD} ]] && break
					sleep 5
				done

				display_alert "Building ${n}."
				(build_main)

			# create BSP for all boards
			elif [[ "${BSP_BUILD}" == yes ]]; then

				for BOARD in "${unique_boards[@]}"
				do
					source ${EXTER}"/config/boards/${BOARD}".conf 2> /dev/null
					IFS=',' read -a RELBRANCH <<< $KERNEL_TARGET
					for BRANCH in "${RELBRANCH[@]}"
					do
					RELTARGETS=(xenial buster bionic focal)
					for RELEASE in "${RELTARGETS[@]}"
					do
						display_alert "BSP for ${BOARD} ${BRANCH} ${RELEASE}."
						build_main
						# unset non board related stuff
						unset_all
					done
					done
				done
				display_alert "Done building all BSP images"
				exit
			else
				# In dryrun it only prints out what will be build
                                printf "%s\t%-32s\t%-8s\t%-14s\t%-6s\t%-6s\t%-6s\n" "${n}." \
                                "$BOARD (${BOARDFAMILY})" "${BRANCH}" "${RELEASE}" "${BUILD_DESKTOP}" "${BUILD_MINIMAL}"
			fi
		fi

	done < "${BUILD_TARGETS}"

}

# display what will be build
echo ""
display_alert "Building all targets" "$STABILITY - $BUILD_OPT"

printf "\n%s\t%-32s\t%-8s\t%-14s\t%-6s\t%-6s\t%-6s\n\n" "" "board" "branch" "release" "XFCE" "minimal"

# display what we will build
build_all "dryrun"

if [[ $BUILD_ALL != demo ]] ; then

	echo ""
	# build
	build_all

fi

# wait until they are not finshed
sleep 5
while :
do
		if [[ $(df | grep -c .tmp) -lt 1 ]]; then
			break
		fi
	sleep 5
done

while :
do
		if [[ -z $(ps -uax | grep 7z | grep OrangePi) ]]; then
			break
		fi
	sleep 5
done

buildall_end=$(date +%s)
buildall_runtime=$(((buildall_end - buildall_start) / 60))
display_alert "Runtime in total" "${buildall_runtime} min" "info"
