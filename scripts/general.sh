#!/bin/bash
#
# Copyright (c) 2015 Igor Pecovnik, igor.pecovnik@gma**.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.


# Functions:
# cleaning
# exit_with_error
# get_package_list_hash
# create_sources_list
# fetch_from_repo
# display_alert
# fingerprint_image
# distro_menu
# addtorepo
# repo-remove-old-packages
# wait_for_package_manager
# prepare_host_basic
# prepare_host
# webseed
# download_and_verify
# run_after_build

# cleaning <target>
#
# target: what to clean
# "make" - "make clean" for selected kernel and u-boot
# "debs" - delete output/debs
# "cache" - delete output/cache
# "oldcache" - remove old output/cache
# "images" - delete output/images
# "sources" - delete output/sources
#
cleaning()
{
	case $1 in
		debs) # delete ${DEB_STORAGE} for current branch and family
		if [[ -d "${DEB_STORAGE}" ]]; then
			display_alert "Cleaning ${DEB_STORAGE} for" "$BOARD $BRANCH" "info"
			# easier than dealing with variable expansion and escaping dashes in file names
			find "${DEB_STORAGE}" -name "${CHOSEN_UBOOT}_*.deb" -delete
			find "${DEB_STORAGE}" \( -name "${CHOSEN_KERNEL}_*.deb" -o \
				-name "orangepi-*.deb" -o \
				-name "${CHOSEN_KERNEL/image/dtb}_*.deb" -o \
				-name "${CHOSEN_KERNEL/image/headers}_*.deb" -o \
				-name "${CHOSEN_KERNEL/image/source}_*.deb" -o \
				-name "${CHOSEN_KERNEL/image/firmware-image}_*.deb" \) -delete
			[[ -n $RELEASE ]] && rm -f "${DEB_STORAGE}/${RELEASE}/${CHOSEN_ROOTFS}"_*.deb
			[[ -n $RELEASE ]] && rm -f ${DEB_STORAGE}/$RELEASE/orangepi-desktop-${RELEASE}_*.deb
		fi
		;;

		extras) # delete ${DEB_STORAGE}/extra/$RELEASE for all architectures
		if [[ -n $RELEASE && -d ${DEB_STORAGE}/extra/$RELEASE ]]; then
			display_alert "Cleaning ${DEB_STORAGE}/extra for" "$RELEASE" "info"
			rm -rf "${DEB_STORAGE}/extra/${RELEASE}"
		fi
		;;

		alldebs) # delete output/debs
		[[ -d "${DEB_STORAGE}" ]] && display_alert "Cleaning" "${DEB_STORAGE}" "info" && rm -rf "${DEB_STORAGE}"/*
		;;

		cache) # delete output/cache
		[[ -d $EXTER/cache/rootfs ]] && display_alert "Cleaning" "rootfs cache (all)" "info" && find $EXTER/cache/rootfs -type f -delete
		;;

		images) # delete output/images
		[[ -d "${DEST}"/images ]] && display_alert "Cleaning" "output/images" "info" && rm -rf "${DEST}"/images/*
		;;

		sources) # delete output/sources and output/buildpkg
		[[ -d $EXTER/cache/sources ]] && display_alert "Cleaning" "sources" "info" && rm -rf $EXTER/cache/sources/* $DEST/buildpkg/*
		;;

		oldcache) # remove old `cache/rootfs` except for the newest 8 files
		if [[ -d $EXTER/cache/rootfs && $(ls -1 $EXTER/cache/rootfs/*.lz4 2> /dev/null | wc -l) -gt ${ROOTFS_CACHE_MAX} ]]; then
			display_alert "Cleaning" "rootfs cache (old)" "info"
			(cd $EXTER/cache/rootfs; ls -t *.lz4 | sed -e "1,${ROOTFS_CACHE_MAX}d" | xargs -d '\n' rm -f)
			# Remove signatures if they are present. We use them for internal purpose
			(cd $EXTER/cache/rootfs; ls -t *.asc | sed -e "1,${ROOTFS_CACHE_MAX}d" | xargs -d '\n' rm -f)
		fi
		;;
	esac
}

# exit_with_error <message> <highlight>
#
# a way to terminate build process
# with verbose error message
#

exit_with_error()
{
	local _file
	local _line=${BASH_LINENO[0]}
	local _function=${FUNCNAME[1]}
	local _description=$1
	local _highlight=$2
	_file=$(basename "${BASH_SOURCE[1]}")

    display_alert "ERROR in function $_function" "$_file:$_line" "err"
    display_alert "$_description" "$_highlight" "err"
    display_alert "Process terminated" "" "info"
    # TODO: execute run_after_build here?
    overlayfs_wrapper "cleanup"
    # unlock loop device access in case of starvation
    exec {FD}>/var/lock/orangepi-debootstrap-losetup
	flock -u "${FD}"

	exit 255
}

# get_package_list_hash
#
# returns md5 hash for current package list and rootfs cache version

get_package_list_hash()
{
	( printf '%s\n' "${PACKAGE_LIST}" | sort -u; printf '%s\n' "${PACKAGE_LIST_EXCLUDE}" | sort -u; echo "${1}" ) \
		| md5sum | cut -d' ' -f 1
}

# create_sources_list <release> <basedir>
#
# <release>: stretch|buster|bullseye|xenial|bionic|eoan|focal
# <basedir>: path to root directory
#
create_sources_list()
{
	local release=$1
	local basedir=$2
	[[ -z $basedir ]] && exit_with_error "No basedir passed to create_sources_list"

	case $release in
	stretch|buster|bullseye)
	cat <<-EOF > "${basedir}"/etc/apt/sources.list
	deb http://${DEBIAN_MIRROR} $release main contrib non-free
	#deb-src http://${DEBIAN_MIRROR} $release main contrib non-free

	deb http://${DEBIAN_MIRROR} ${release}-updates main contrib non-free
	#deb-src http://${DEBIAN_MIRROR} ${release}-updates main contrib non-free

	deb http://${DEBIAN_MIRROR} ${release}-backports main contrib non-free
	#deb-src http://${DEBIAN_MIRROR} ${release}-backports main contrib non-free

	deb http://${DEBIAN_SECURTY} ${release}/updates main contrib non-free
	#deb-src http://${DEBIAN_SECURTY} ${release}/updates main contrib non-free
	EOF
	;;

	xenial|bionic|eoan|focal)
	cat <<-EOF > "${basedir}"/etc/apt/sources.list
	deb http://${UBUNTU_MIRROR} $release main restricted universe multiverse
	#deb-src http://${UBUNTU_MIRROR} $release main restricted universe multiverse

	deb http://${UBUNTU_MIRROR} ${release}-security main restricted universe multiverse
	#deb-src http://${UBUNTU_MIRROR} ${release}-security main restricted universe multiverse

	deb http://${UBUNTU_MIRROR} ${release}-updates main restricted universe multiverse
	#deb-src http://${UBUNTU_MIRROR} ${release}-updates main restricted universe multiverse

	deb http://${UBUNTU_MIRROR} ${release}-backports main restricted universe multiverse
	#deb-src http://${UBUNTU_MIRROR} ${release}-backports main restricted universe multiverse
	EOF
	;;
	esac

	## stage: add armbian repository and install key
	#if [[ $DOWNLOAD_MIRROR == "china" ]]; then
	#	echo "deb http://mirrors.tuna.tsinghua.edu.cn/armbian $RELEASE main ${RELEASE}-utils ${RELEASE}-desktop" > "${SDCARD}"/etc/apt/sources.list.d/armbian.list
	#else
	#	echo "deb http://apt.armbian.com $RELEASE main ${RELEASE}-utils ${RELEASE}-desktop" > "${SDCARD}"/etc/apt/sources.list.d/armbian.list
	#fi

	## add local package server if defined. Suitable for development
	#[[ -n $LOCAL_MIRROR ]] && echo "deb http://$LOCAL_MIRROR $RELEASE main ${RELEASE}-utils ${RELEASE}-desktop" >> "${SDCARD}"/etc/apt/sources.list.d/armbian.list

	#display_alert "Adding Armbian repository and authentication key" "/etc/apt/sources.list.d/armbian.list" "info"
	#cp "${SRC}"/config/armbian.key "${SDCARD}"
	#chroot "${SDCARD}" /bin/bash -c "cat armbian.key | apt-key add - > /dev/null 2>&1"
	#rm "${SDCARD}"/armbian.key
}

# fetch_from_repo <url> <directory> <ref> <ref_subdir>
# <url>: remote repository URL
# <directory>: local directory; subdir for branch/tag will be created
# <ref>:
#	branch:name
#	tag:name
#	head(*)
#	commit:hash
#
# *: Implies ref_subdir=no
#
# <ref_subdir>: "yes" to create subdirectory for tag or branch name
#
fetch_from_repo()
{
	local url=$1
	local dir=$2
	local ref=$3
	local ref_subdir=$4

	# The 'offline' variable must always be set to 'true' or 'false'
	if [ "$OFFLINE_WORK" == "yes" ]; then
		local offline=true
	else
		local offline=false
	fi

	[[ -z $ref || ( $ref != tag:* && $ref != branch:* && $ref != head && $ref != commit:* ) ]] && exit_with_error "Error in configuration"
	local ref_type=${ref%%:*}
	if [[ $ref_type == head ]]; then
		local ref_name=HEAD
	else
		local ref_name=${ref##*:}
	fi

	display_alert "Checking git sources" "$dir $ref_name" "info"

	# get default remote branch name without cloning
	# local ref_name=$(git ls-remote --symref $url HEAD | grep -o 'refs/heads/\S*' | sed 's%refs/heads/%%')
	# for git:// protocol comparing hashes of "git ls-remote -h $url" and "git ls-remote --symref $url HEAD" is needed

	if [[ $ref_subdir == yes ]]; then
		local workdir=$dir/$ref_name
	else
		local workdir=$dir
	fi

	mkdir -p "${workdir}" 2>/dev/null || \
		exit_with_error "No path or no write permission" "${workdir}"

	cd "${workdir}" || exit

	# check if existing remote URL for the repo or branch does not match current one
	# may not be supported by older git versions
	#  Check the folder as a git repository.
	#  Then the target URL matches the local URL.

	if [[ "$(git rev-parse --git-dir 2>/dev/null)" == ".git" && \
		  "$url" != "$(git remote get-url origin 2>/dev/null)" ]]; then
		display_alert "Remote URL does not match, removing existing local copy"
		rm -rf .git ./*
	fi

	if [[ "$(git rev-parse --git-dir 2>/dev/null)" != ".git" ]]; then
		display_alert "Creating local copy"
		git init -q .
		git remote add origin "${url}"
		# Here you need to upload from a new address
		offline=false
	fi

	local changed=false

	# when we work offline we simply return the sources to their original state
	if ! $offline; then
		local local_hash
		local_hash=$(git rev-parse @ 2>/dev/null)

		case $ref_type in
			branch)
			# TODO: grep refs/heads/$name
			local remote_hash
			remote_hash=$(git ls-remote -h "${url}" "$ref_name" | head -1 | cut -f1)
			[[ -z $local_hash || "${local_hash}" != "${remote_hash}" ]] && changed=true
			;;

			tag)
			local remote_hash
			remote_hash=$(git ls-remote -t "${url}" "$ref_name" | cut -f1)
			if [[ -z $local_hash || "${local_hash}" != "${remote_hash}" ]]; then
				remote_hash=$(git ls-remote -t "${url}" "$ref_name^{}" | cut -f1)
				[[ -z $remote_hash || "${local_hash}" != "${remote_hash}" ]] && changed=true
			fi
			;;

			head)
			local remote_hash
			remote_hash=$(git ls-remote "${url}" HEAD | cut -f1)
			[[ -z $local_hash || "${local_hash}" != "${remote_hash}" ]] && changed=true
			;;

			commit)
			[[ -z $local_hash || $local_hash == "@" ]] && changed=true
			;;
		esac

	fi # offline

	if [[ $changed == true ]]; then

		# remote was updated, fetch and check out updates
		display_alert "Fetching updates"
		case $ref_type in
			branch) git fetch --depth 1 origin "${ref_name}" ;;
			tag) git fetch --depth 1 origin tags/"${ref_name}" ;;
			head) git fetch --depth 1 origin HEAD ;;
		esac

		# commit type needs support for older git servers that doesn't support fetching id directly
		if [[ $ref_type == commit ]]; then

			git fetch --depth 1 origin "${ref_name}"

			# cover old type
			if [[ $? -ne 0 ]]; then

				display_alert "Commit checkout not supported on this repository. Doing full clone." "" "wrn"
				git pull
				git checkout -fq "${ref_name}"
				display_alert "Checkout out to" "$(git --no-pager log -2 --pretty=format:"$ad%s [%an]" | head -1)" "info"

			else

				display_alert "Checking out"
				git checkout -f -q FETCH_HEAD
				git clean -qdf

			fi
		else

			display_alert "Checking out"
			git checkout -f -q FETCH_HEAD
			git clean -qdf

		fi
	elif [[ -n $(git status -uno --porcelain --ignore-submodules=all) ]]; then
		# working directory is not clean
		display_alert " Cleaning .... " "$(git status -s | wc -l) files"

		# Return the files that are tracked by git to the initial state.
		git checkout -f -q HEAD

		# Files that are not tracked by git and were added
		# when the patch was applied must be removed.
		git clean -qdf
	else
		# working directory is clean, nothing to do
		display_alert "Up to date"
	fi

	if [[ -f .gitmodules ]]; then
		display_alert "Updating submodules" "" "ext"
		# FML: http://stackoverflow.com/a/17692710
		for i in $(git config -f .gitmodules --get-regexp path | awk '{ print $2 }'); do
			cd "${workdir}" || exit
			local surl sref
			surl=$(git config -f .gitmodules --get "submodule.$i.url")
			sref=$(git config -f .gitmodules --get "submodule.$i.branch")
			if [[ -n $sref ]]; then
				sref="branch:$sref"
			else
				sref="head"
			fi
			fetch_from_repo "$surl" "$workdir/$i" "$sref"
		done
	fi
} #############################################################################

#--------------------------------------------------------------------------------------------------------------------------------
# Let's have unique way of displaying alerts
#--------------------------------------------------------------------------------------------------------------------------------
display_alert()
{
	# log function parameters to install.log
	[[ -n "${DEST}" ]] && echo "Displaying message: $@" >> "${DEST}"/debug/output.log

	local tmp=""
	[[ -n $2 ]] && tmp="[\e[0;33m $2 \x1B[0m]"

	case $3 in
		err)
		echo -e "[\e[0;31m error \x1B[0m] $1 $tmp"
		;;

		wrn)
		echo -e "[\e[0;35m warn \x1B[0m] $1 $tmp"
		;;

		ext)
		echo -e "[\e[0;32m o.k. \x1B[0m] \e[1;32m$1\x1B[0m $tmp"
		;;

		info)
		echo -e "[\e[0;32m o.k. \x1B[0m] $1 $tmp"
		;;

		*)
		echo -e "[\e[0;32m .... \x1B[0m] $1 $tmp"
		;;
	esac
}

#--------------------------------------------------------------------------------------------------------------------------------
# fingerprint_image <out_txt_file> [image_filename]
# Saving build summary to the image
#--------------------------------------------------------------------------------------------------------------------------------
fingerprint_image()
{
	display_alert "Fingerprinting"
	cat <<-EOF > "${1}"
	--------------------------------------------------------------------------------
	Title:			Orange Pi $REVISION ${BOARD^} $DISTRIBUTION $RELEASE $BRANCH
	Kernel:			Linux $VER
	Build date:		$(date +'%d.%m.%Y')
	Maintainer:		$MAINTAINER <$MAINTAINERMAIL>
	Sources: 		https://github.com/orangepi-xunlong/orangepi-build
	Support: 		http://www.orangepi.org/
	EOF

	if [ -n "$2" ]; then
	cat <<-EOF >> "${1}"
	--------------------------------------------------------------------------------
	Partitioning configuration:
	Root partition type: $ROOTFS_TYPE
	Boot partition type: ${BOOTFS_TYPE:-(none)}
	User provided boot partition size: ${BOOTSIZE:-0}
	Offset: $OFFSET
	CPU configuration: $CPUMIN - $CPUMAX with $GOVERNOR
	--------------------------------------------------------------------------------
	Verify GPG signature:
	gpg --verify $2.img.asc

	Verify image file integrity:
	sha256sum --check $2.img.sha

	Prepare micro SD card (four methodes):
	zcat $2.img.gz | pv | dd of=/dev/sdX bs=1M
	dd if=$2.img of=/dev/sdX bs=1M
	balena-etcher $2.img.gz -d /dev/sdX
	balena-etcher $2.img -d /dev/sdX
	EOF
        fi

	cat <<-EOF >> "${1}"
	--------------------------------------------------------------------------------
	$(cat "${SRC}"/LICENSE)
	--------------------------------------------------------------------------------
	EOF
}


#--------------------------------------------------------------------------------------------------------------------------------
# Create kernel boot logo from packages/blobs/splash/logo.png and packages/blobs/splash/spinner.gif (animated)
# and place to the file /lib/firmware/bootsplash
#--------------------------------------------------------------------------------------------------------------------------------
function boot_logo ()
{
display_alert "Building kernel splash logo" "$RELEASE" "info"

	LOGO=${EXTER}/packages/blobs/splash/logo.png
	LOGO_WIDTH=$(identify $LOGO | cut -d " " -f 3 | cut -d x -f 1)
	LOGO_HEIGHT=$(identify $LOGO | cut -d " " -f 3 | cut -d x -f 2)
	THROBBER=${EXTER}/packages/blobs/splash/spinner.gif
	THROBBER_WIDTH=$(identify $THROBBER | head -1 | cut -d " " -f 3 | cut -d x -f 1)
	THROBBER_HEIGHT=$(identify $THROBBER | head -1 | cut -d " " -f 3 | cut -d x -f 2)
	convert -alpha remove -background "#000000"	$LOGO "${SDCARD}"/tmp/logo.rgb
	convert -alpha remove -background "#000000" $THROBBER "${SDCARD}"/tmp/throbber%02d.rgb
	${EXTER}/packages/blobs/splash/bootsplash-packer \
	--bg_red 0x00 \
	--bg_green 0x00 \
	--bg_blue 0x00 \
	--frame_ms 48 \
	--picture \
	--pic_width $LOGO_WIDTH \
	--pic_height $LOGO_HEIGHT \
	--pic_position 0 \
	--blob "${SDCARD}"/tmp/logo.rgb \
	--picture \
	--pic_width $THROBBER_WIDTH \
	--pic_height $THROBBER_HEIGHT \
	--pic_position 0x05 \
	--pic_position_offset 200 \
	--pic_anim_type 1 \
	--pic_anim_loop 0 \
	--blob "${SDCARD}"/tmp/throbber00.rgb \
	--blob "${SDCARD}"/tmp/throbber01.rgb \
	--blob "${SDCARD}"/tmp/throbber02.rgb \
	--blob "${SDCARD}"/tmp/throbber03.rgb \
	--blob "${SDCARD}"/tmp/throbber04.rgb \
	--blob "${SDCARD}"/tmp/throbber05.rgb \
	--blob "${SDCARD}"/tmp/throbber06.rgb \
	--blob "${SDCARD}"/tmp/throbber07.rgb \
	--blob "${SDCARD}"/tmp/throbber08.rgb \
	--blob "${SDCARD}"/tmp/throbber09.rgb \
	--blob "${SDCARD}"/tmp/throbber10.rgb \
	--blob "${SDCARD}"/tmp/throbber11.rgb \
	--blob "${SDCARD}"/tmp/throbber12.rgb \
	--blob "${SDCARD}"/tmp/throbber13.rgb \
	--blob "${SDCARD}"/tmp/throbber14.rgb \
	--blob "${SDCARD}"/tmp/throbber15.rgb \
	--blob "${SDCARD}"/tmp/throbber16.rgb \
	--blob "${SDCARD}"/tmp/throbber17.rgb \
	--blob "${SDCARD}"/tmp/throbber18.rgb \
	--blob "${SDCARD}"/tmp/throbber19.rgb \
	--blob "${SDCARD}"/tmp/throbber20.rgb \
	--blob "${SDCARD}"/tmp/throbber21.rgb \
	--blob "${SDCARD}"/tmp/throbber22.rgb \
	--blob "${SDCARD}"/tmp/throbber23.rgb \
	--blob "${SDCARD}"/tmp/throbber24.rgb \
	--blob "${SDCARD}"/tmp/throbber25.rgb \
	--blob "${SDCARD}"/tmp/throbber26.rgb \
	--blob "${SDCARD}"/tmp/throbber27.rgb \
	--blob "${SDCARD}"/tmp/throbber28.rgb \
	--blob "${SDCARD}"/tmp/throbber29.rgb \
	--blob "${SDCARD}"/tmp/throbber30.rgb \
	--blob "${SDCARD}"/tmp/throbber31.rgb \
	--blob "${SDCARD}"/tmp/throbber32.rgb \
	--blob "${SDCARD}"/tmp/throbber33.rgb \
	--blob "${SDCARD}"/tmp/throbber34.rgb \
	--blob "${SDCARD}"/tmp/throbber35.rgb \
	--blob "${SDCARD}"/tmp/throbber36.rgb \
	--blob "${SDCARD}"/tmp/throbber37.rgb \
	--blob "${SDCARD}"/tmp/throbber38.rgb \
	--blob "${SDCARD}"/tmp/throbber39.rgb \
	--blob "${SDCARD}"/tmp/throbber40.rgb \
	--blob "${SDCARD}"/tmp/throbber41.rgb \
	--blob "${SDCARD}"/tmp/throbber42.rgb \
	--blob "${SDCARD}"/tmp/throbber43.rgb \
	--blob "${SDCARD}"/tmp/throbber44.rgb \
	--blob "${SDCARD}"/tmp/throbber45.rgb \
	--blob "${SDCARD}"/tmp/throbber46.rgb \
	--blob "${SDCARD}"/tmp/throbber47.rgb \
	--blob "${SDCARD}"/tmp/throbber48.rgb \
	--blob "${SDCARD}"/tmp/throbber49.rgb \
	--blob "${SDCARD}"/tmp/throbber50.rgb \
	--blob "${SDCARD}"/tmp/throbber51.rgb \
	--blob "${SDCARD}"/tmp/throbber52.rgb \
	--blob "${SDCARD}"/tmp/throbber53.rgb \
	--blob "${SDCARD}"/tmp/throbber54.rgb \
	--blob "${SDCARD}"/tmp/throbber55.rgb \
	--blob "${SDCARD}"/tmp/throbber56.rgb \
	--blob "${SDCARD}"/tmp/throbber57.rgb \
	--blob "${SDCARD}"/tmp/throbber58.rgb \
	--blob "${SDCARD}"/tmp/throbber59.rgb \
	--blob "${SDCARD}"/tmp/throbber60.rgb \
	--blob "${SDCARD}"/tmp/throbber61.rgb \
	--blob "${SDCARD}"/tmp/throbber62.rgb \
	--blob "${SDCARD}"/tmp/throbber63.rgb \
	--blob "${SDCARD}"/tmp/throbber64.rgb \
	--blob "${SDCARD}"/tmp/throbber65.rgb \
	--blob "${SDCARD}"/tmp/throbber66.rgb \
	--blob "${SDCARD}"/tmp/throbber67.rgb \
	--blob "${SDCARD}"/tmp/throbber68.rgb \
	--blob "${SDCARD}"/tmp/throbber69.rgb \
	--blob "${SDCARD}"/tmp/throbber70.rgb \
	--blob "${SDCARD}"/tmp/throbber71.rgb \
	--blob "${SDCARD}"/tmp/throbber72.rgb \
	--blob "${SDCARD}"/tmp/throbber73.rgb \
	--blob "${SDCARD}"/tmp/throbber74.rgb \
	"${SDCARD}"/lib/firmware/bootsplash.orangepi >/dev/null 2>&1
	if [[ $BOOT_LOGO == yes || $BOOT_LOGO == desktop && $BUILD_DESKTOP == yes ]]; then
		[[ -f "${SDCARD}"/boot/orangepiEnv.txt ]] &&	grep -q '^bootlogo' "${SDCARD}"/boot/orangepiEnv.txt && \
		sed -i 's/^bootlogo.*/bootlogo=true/' "${SDCARD}"/boot/orangepiEnv.txt || echo 'bootlogo=true' >> "${SDCARD}"/boot/orangepiEnv.txt
		[[ -f "${SDCARD}"/boot/boot.ini ]] &&	sed -i 's/^setenv bootlogo.*/setenv bootlogo "true"/' "${SDCARD}"/boot/boot.ini
	fi
	# enable additional services
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable bootsplash-ask-password-console.path >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable bootsplash-hide-when-booted.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable bootsplash-show-on-shutdown.service >/dev/null 2>&1"
}



function distro_menu ()
{

	for i in "${!distro_name[@]}"
	do
		if [[ "${i}" == "${1}" ]]; then
			if [[ "${RELEASE_TARGET}" != *$1* ]]; then
				:
			else
				local text=""
				options+=("$i" "${distro_name[$i]} $text")
			fi
			break
		fi
	done
}



# wait_for_package_manager
#
# * installation will break if we try to install when package manager is running
#
wait_for_package_manager()
{
	# exit if package manager is running in the back
	while true; do
		if [[ "$(fuser /var/lib/dpkg/lock 2>/dev/null; echo $?)" != 1 && "$(fuser /var/lib/dpkg/lock-frontend 2>/dev/null; echo $?)" != 1 ]]; then
				display_alert "Package manager is running in the background." "Please wait! Retrying in 30 sec" "wrn"
				sleep 30
			else
				break
		fi
	done
}




# prepare_host_basic
#
# * installs only basic packages
#
prepare_host_basic()
{
	# wait until package manager finishes possible system maintanace
	wait_for_package_manager

	# need lsb_release to decide what to install
	if [[ $(dpkg-query -W -f='${db:Status-Abbrev}\n' lsb-release 2>/dev/null) != *ii* ]]; then
		display_alert "Installing package" "lsb-release"
		apt-get -q update && apt-get install -q -y --no-install-recommends lsb-release
	fi

	# need to install whiptail if person is starting with a interactive mode
	if [[ $(dpkg-query -W -f='${db:Status-Abbrev}\n' whiptail 2>/dev/null) != *ii* ]]; then
		display_alert "Installing package" "whiptail"
		apt-get -q update && apt-get install -q -y --no-install-recommends whiptail
	fi

}




# prepare_host
#
# * checks and installs necessary packages
# * creates directory structure
# * changes system settings
#
prepare_host()
{
	display_alert "Preparing" "host" "info"

	# The 'offline' variable must always be set to 'true' or 'false'
	if [ "$OFFLINE_WORK" == "yes" ]; then
		local offline=true
	else
		local offline=false
	fi

	if [[ $(dpkg --print-architecture) != amd64 ]]; then
		display_alert "Please read documentation to set up proper compilation environment"
		display_alert "http://www.orangepi.org"
		exit_with_error "Running this tool on non x86-x64 build host in not supported"
	fi

	# wait until package manager finishes possible system maintanace
	wait_for_package_manager

	# temporally fix for Locales settings
	export LC_ALL="en_US.UTF-8"

	# packages list for host
	# NOTE: please sync any changes here with the Dockerfile and Vagrantfile
	local hostdeps="wget ca-certificates device-tree-compiler pv bc lzop zip binfmt-support build-essential ccache debootstrap ntpdate \
	gawk gcc-arm-linux-gnueabihf qemu-user-static u-boot-tools uuid-dev zlib1g-dev unzip libusb-1.0-0-dev fakeroot \
	parted pkg-config libncurses5-dev whiptail debian-keyring debian-archive-keyring f2fs-tools libfile-fcntllock-perl rsync libssl-dev \
	nfs-kernel-server btrfs-progs ncurses-term p7zip-full kmod dosfstools libc6-dev-armhf-cross imagemagick \
	curl patchutils liblz4-tool libpython2.7-dev linux-base swig acl python3-dev python3-distutils libfdt-dev \
	locales ncurses-base pixz dialog systemd-container udev lib32stdc++6 libc6-i386 lib32ncurses5 lib32tinfo5 \
	bison libbison-dev flex libfl-dev cryptsetup gpg gnupg1 gpgv1 gpgv2 cpio aria2 pigz dirmngr python3-distutils distcc git"

	local codename=$(lsb_release -sc)

	# Getting ready for Ubuntu 20.04
	if [[ $codename == focal || $codename == ulyana ]]; then
		hostdeps+=" python2 python3"
		ln -fs /usr/bin/python2.7 /usr/bin/python2
		ln -fs /usr/bin/python2.7 /usr/bin/python
	else
		hostdeps+=" python libpython-dev"
	fi

	display_alert "Build host OS release" "${codename:-(unknown)}" "info"

	# Ubuntu Focal x86_64 is the only fully supported host OS release
	# Ubuntu Bionic x86_64 support is legacy until it breaks
	# Using Docker/VirtualBox/Vagrant is the only supported way to run the build script on other Linux distributions
	#
	# NO_HOST_RELEASE_CHECK overrides the check for a supported host system
	# Disable host OS check at your own risk, any issues reported with unsupported releases will be closed without a discussion
	if [[ -z $codename || "bionic buster eoan focal debbie tricia ulyana" != *"$codename"* ]]; then
		if [[ $NO_HOST_RELEASE_CHECK == yes ]]; then
			display_alert "You are running on an unsupported system" "${codename:-(unknown)}" "wrn"
			display_alert "Do not report any errors, warnings or other issues encountered beyond this point" "" "wrn"
		else
			exit_with_error "It seems you ignore documentation and run an unsupported build system: ${codename:-(unknown)}"
		fi
	fi

	if grep -qE "(Microsoft|WSL)" /proc/version; then
		exit_with_error "Windows subsystem for Linux is not a supported build environment"
	fi

	if [[ -z $codename || "focal" == "$codename" || "eoan" == "$codename"  || "debbie" == "$codename"  || "buster" == "$codename" || "ulyana" == "$codename" ]]; then
	    hostdeps="${hostdeps/lib32ncurses5 lib32tinfo5/lib32ncurses6 lib32tinfo6}"
	fi

	grep -q i386 <(dpkg --print-foreign-architectures) || dpkg --add-architecture i386
	if systemd-detect-virt -q -c; then
		display_alert "Running in container" "$(systemd-detect-virt)" "info"
		# disable apt-cacher unless NO_APT_CACHER=no is not specified explicitly
		if [[ $NO_APT_CACHER != no ]]; then
			display_alert "apt-cacher is disabled in containers, set NO_APT_CACHER=no to override" "" "wrn"
			NO_APT_CACHER=yes
		fi
		CONTAINER_COMPAT=yes
		# trying to use nested containers is not a good idea, so don't permit EXTERNAL_NEW=compile
		if [[ $EXTERNAL_NEW == compile ]]; then
			display_alert "EXTERNAL_NEW=compile is not available when running in container, setting to prebuilt" "" "wrn"
			EXTERNAL_NEW=prebuilt
		fi
		SYNC_CLOCK=no
	fi

	# Skip verification if you are working offline
	if ! $offline; then

	# warning: apt-cacher-ng will fail if installed and used both on host and in container/chroot environment with shared network
	# set NO_APT_CACHER=yes to prevent installation errors in such case
	if [[ $NO_APT_CACHER != yes ]]; then hostdeps="$hostdeps apt-cacher-ng"; fi

	local deps=()
	local installed=$(dpkg-query -W -f '${db:Status-Abbrev}|${binary:Package}\n' '*' 2>/dev/null | grep '^ii' | awk -F '|' '{print $2}' | cut -d ':' -f 1)

	for packet in $hostdeps; do
		if ! grep -q -x -e "$packet" <<< "$installed"; then deps+=("$packet"); fi
	done

	# distribution packages are buggy, download from author
	if [[ ! -f /etc/apt/sources.list.d/aptly.list ]]; then
		display_alert "Updating from external repository" "aptly" "info"
		if [ x"" != x"${http_proxy}" ]; then
			apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --keyserver-options http-proxy="${http_proxy}" --recv-keys ED75B5A4483DA07C >/dev/null 2>&1
			apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --keyserver-options http-proxy="${http_proxy}" --recv-keys ED75B5A4483DA07C >/dev/null 2>&1
		else
			apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys ED75B5A4483DA07C >/dev/null 2>&1
			apt-key adv --keyserver pool.sks-keyservers.net --recv-keys ED75B5A4483DA07C >/dev/null 2>&1
		fi
		echo "deb http://repo.aptly.info/ nightly main" > /etc/apt/sources.list.d/aptly.list
	else
		sed "s/squeeze/nightly/" -i /etc/apt/sources.list.d/aptly.list
	fi

	local aptly_ver=$(dpkg -s aptly | grep '^Version:' | cut -d " " -f 2)
	if [[ $aptly_ver != "1.4.0+44+g24a0271" ]]; then 
		apt-get -q -y purge aptly 2>&1 >/dev/null
		dpkg -i ${EXTER}/cache/debs/aptly/aptly_1.4.0+44+g24a0271_amd64.deb 2>&1 >/dev/null
	fi

	if [[ ${#deps[@]} -gt 0 ]]; then
		display_alert "Installing build dependencies"
		apt-get -q update
		apt-get -y upgrade
		apt-get -q -y --no-install-recommends install -o Dpkg::Options::='--force-confold' "${deps[@]}" | tee -a "${DEST}"/debug/hostdeps.log
		update-ccache-symlinks
	fi

	# sync clock
	if [[ $SYNC_CLOCK != no ]]; then
		display_alert "Syncing clock" "${NTP_SERVER:-pool.ntp.org}" "info"
		ntpdate -s "${NTP_SERVER:-pool.ntp.org}"
	fi

	if [[ $(dpkg-query -W -f='${db:Status-Abbrev}\n' 'zlib1g:i386' 2>/dev/null) != *ii* ]]; then
		apt-get install -qq -y --no-install-recommends zlib1g:i386 >/dev/null 2>&1
	fi

	# create directory structure
	mkdir -p $SRC/output $EXTER/cache $USERPATCHES_PATH
	if [[ -n $SUDO_USER ]]; then
		chgrp --quiet sudo cache output "${USERPATCHES_PATH}"
		# SGID bit on cache/sources breaks kernel dpkg packaging
		chmod --quiet g+w,g+s output "${USERPATCHES_PATH}"
		# fix existing permissions
		find "${SRC}"/output "${USERPATCHES_PATH}" -type d ! -group sudo -exec chgrp --quiet sudo {} \;
		find "${SRC}"/output "${USERPATCHES_PATH}" -type d ! -perm -g+w,g+s -exec chmod --quiet g+w,g+s {} \;
	fi
	mkdir -p $DEST/debs/{extra,u-boot}  $DEST/{config,debug,patch,images} $USERPATCHES_PATH/overlay $EXTER/cache/{debs,sources,hash} $SRC/toolchains  $SRC/.tmp

	display_alert "Checking for external GCC compilers" "" "info"
	# download external Linaro compiler and missing special dependencies since they are needed for certain sources

	local toolchains=(
		"gcc-linaro-aarch64-none-elf-4.8-2013.11_linux.tar.xz"
		"gcc-linaro-arm-none-eabi-4.8-2014.04_linux.tar.xz"
		"gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux.tar.xz"
		"gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi.tar.xz"
		"gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu.tar.xz"
		"gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf.tar.xz"
		"gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabi.tar.xz"
		"gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu.tar.xz"
		"gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf.tar.xz"
		"gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu.tar.xz"
		)

	USE_TORRENT_STATUS=${USE_TORRENT}
	USE_TORRENT="no"
	for toolchain in ${toolchains[@]}; do
		download_and_verify "_toolchain" "${toolchain##*/}"
	done
	USE_TORRENT=${USE_TORRENT_STATUS}

	rm -rf $SRC/toolchains/*.tar.xz*
	local existing_dirs=( $(ls -1 $SRC/toolchains) )
	for dir in ${existing_dirs[@]}; do
		local found=no
		for toolchain in ${toolchains[@]}; do
			local filename=${toolchain##*/}
			local dirname=${filename//.tar.xz}
			[[ $dir == $dirname ]] && found=yes
		done
		if [[ $found == no ]]; then
			display_alert "Removing obsolete toolchain" "$dir"
			rm -rf $SRC/toolchains/$dir
		fi
	done

	fi # check offline

	# enable arm binary format so that the cross-architecture chroot environment will work
	if [[ $BUILD_OPT == "image" || $BUILD_OPT == "rootfs" ]]; then
		modprobe -q binfmt_misc
		mountpoint -q /proc/sys/fs/binfmt_misc/ || mount binfmt_misc -t binfmt_misc /proc/sys/fs/binfmt_misc
		if [[ ! -f /proc/sys/fs/binfmt_misc/qemu-arm ]]; then
			apt-get -q -y purge qemu-user-static
			apt-get -q -y install qemu-user-static
		fi
		test -e /proc/sys/fs/binfmt_misc/qemu-arm || update-binfmts --enable qemu-arm
		test -e /proc/sys/fs/binfmt_misc/qemu-aarch64 || update-binfmts --enable qemu-aarch64
	fi

	[[ ! -f $USERPATCHES_PATH/customize-image.sh ]] && cp $EXTER/config/templates/customize-image.sh.template $USERPATCHES_PATH/customize-image.sh

	if [[ ! -f $USERPATCHES_PATH/README ]]; then
		rm -f $USERPATCHES_PATH/readme.txt
		echo 'Please read documentation about customizing build configuration' > $USERPATCHES_PATH/README
		echo 'http://www.orangepi.org' >> $USERPATCHES_PATH/README

		# create patches directory structure under USERPATCHES_PATH
		find $EXTER/patch -maxdepth 2 -type d ! -name . | sed "s%/.*patch%/$USERPATCHES_PATH%" | xargs mkdir -p
	fi

	# check free space (basic)
	local freespace=$(findmnt --target "${SRC}" -n -o AVAIL -b 2>/dev/null) # in bytes
	if [[ -n $freespace && $(( $freespace / 1073741824 )) -lt 10 ]]; then
		display_alert "Low free space left" "$(( $freespace / 1073741824 )) GiB" "wrn"
		# pause here since dialog-based menu will hide this message otherwise
		echo -e "Press \e[0;33m<Ctrl-C>\x1B[0m to abort compilation, \e[0;33m<Enter>\x1B[0m to ignore and continue"
		read
	fi
}




function webseed ()
{
# list of mirrors that host our files
unset text
WEBSEED=(
	"https://dl.armbian.com/"
	"https://imola.armbian.com/dl/"
	"https://mirrors.netix.net/armbian/dl/"
	"https://mirrors.dotsrc.org/armbian-dl/"
	"https://us.mirrors.fossho.st/armbian/dl/"
	"https://uk.mirrors.fossho.st/armbian/dl/"
	"https://armbian.systemonachip.net/dl/"
	)
	if [[ -z $DOWNLOAD_MIRROR ]]; then
		WEBSEED=(
                "https://dl.armbian.com/"
                )
	fi
	# aria2 simply split chunks based on sources count not depending on download speed
	# when selecting china mirrors, use only China mirror, others are very slow there
	if [[ $DOWNLOAD_MIRROR == china ]]; then
		WEBSEED=(
		"https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/"
		)
	fi
	for toolchain in ${WEBSEED[@]}; do
		# use only live
		if [[ $(wget -S --spider "${toolchain}${1}" 2>&1 >/dev/null | grep 'HTTP/1.1 200 OK') ]]; then
			text="${text} ${toolchain}${1}"
		fi
	done
	text="${text:1}"
	echo "${text}"
}




download_and_verify()
{

	local remotedir=$1
	local filename=$2
	local localdir=$SRC/toolchains
	local dirname=${filename//.tar.xz}

        if [[ $DOWNLOAD_MIRROR == china ]]; then
		local server="https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/"
			else
		local server="https://dl.armbian.com/"
        fi

	if [[ -f ${localdir}/${dirname}/.download-complete ]]; then
		return
	fi

	cd "${localdir}" || exit

	# use local control file
	if [[ -f $EXTER/config/torrents/${filename}.asc ]]; then
		local torrent=$EXTER/config/torrents/${filename}.torrent
		ln -s $EXTER/config/torrents/${filename}.asc ${localdir}/${filename}.asc
	elif [[ ! $(wget -S --spider "${server}${remotedir}/${filename}.asc" 2>&1 >/dev/null | grep 'HTTP/1.1 200 OK') ]]; then
		return
	else
		# download control file
		local torrent=${server}$remotedir/${filename}.torrent
		aria2c --download-result=hide --disable-ipv6=true --summary-interval=0 --console-log-level=error --auto-file-renaming=false \
		--continue=false --allow-overwrite=true --dir="${localdir}" "$(webseed "$remotedir/${filename}.asc")" -o "${filename}.asc"
		[[ $? -ne 0 ]] && display_alert "Failed to download control file" "" "wrn"
	fi

	# download torrent first
	if [[ ${USE_TORRENT} == "yes" ]]; then

		display_alert "downloading using torrent network" "$filename"
		local ariatorrent="--summary-interval=0 --auto-save-interval=0 --seed-time=0 --bt-stop-timeout=15 --console-log-level=error \
		--allow-overwrite=true --download-result=hide --rpc-save-upload-metadata=false --auto-file-renaming=false \
		--file-allocation=trunc --continue=true ${torrent} \
		--dht-file-path=$EXTER/cache/.aria2/dht.dat --disable-ipv6=true --stderr --follow-torrent=mem --dir=${localdir}"

		# exception. It throws error if dht.dat file does not exists. Error suppress needed only at first download.
		if [[ -f $EXTER/cache/.aria2/dht.dat ]]; then
			# shellcheck disable=SC2086
			aria2c ${ariatorrent}
		else
			# shellcheck disable=SC2035
			aria2c ${ariatorrent} &> "${DEST}"/debug/torrent.log
		fi
		# mark complete
		[[ $? -eq 0 ]] && touch "${localdir}/${filename}.complete"

	fi


	# direct download if torrent fails
	if [[ ! -f "${localdir}/${filename}.complete" ]]; then
		if [[ $(wget -S --spider "${server}${remotedir}/${filename}" 2>&1 >/dev/null \
			| grep 'HTTP/1.1 200 OK') ]]; then
			display_alert "downloading using http(s) network" "$filename"
			aria2c --download-result=hide --rpc-save-upload-metadata=false --console-log-level=error \
			--dht-file-path="${SRC}"/cache/.aria2/dht.dat --disable-ipv6=true --summary-interval=0 --auto-file-renaming=false --dir="${localdir}" "$(webseed "${remotedir}/${filename}")" -o "${filename}"
			# mark complete
			[[ $? -eq 0 ]] && touch "${localdir}/${filename}.complete" && echo ""

		fi
	fi

	if [[ -f ${localdir}/${filename}.asc ]]; then

		if grep -q 'BEGIN PGP SIGNATURE' "${localdir}/${filename}.asc"; then

			if [[ ! -d $EXTER/cache/.gpg ]]; then
				mkdir -p $EXTER/cache/.gpg
				chmod 700 $EXTER/cache/.gpg
				touch $EXTER/cache/.gpg/gpg.conf
				chmod 600 $EXTER/cache/.gpg/gpg.conf
			fi

			# Verify archives with Linaro and Armbian GPG keys

			if [ x"" != x"${http_proxy}" ]; then
				(gpg --homedir $EXTER/cache/.gpg --no-permission-warning --list-keys 8F427EAF >> $DEST/debug/output.log 2>&1\
				 || gpg --homedir $EXTER/cache/.gpg --no-permission-warning \
				--keyserver hkp://keyserver.ubuntu.com:80 --keyserver-options http-proxy="${http_proxy}" \
				--recv-keys 8F427EAF >> $EXTER/debug/output.log 2>&1)

				(gpg --homedir $EXTER/cache/.gpg --no-permission-warning --list-keys 9F0E78D5 >> $DEST/debug/output.log 2>&1\
				|| gpg --homedir $EXTER/cache/.gpg --no-permission-warning \
				--keyserver hkp://keyserver.ubuntu.com:80 --keyserver-options http-proxy="${http_proxy}" \
				--recv-keys 9F0E78D5 >> $DEST/debug/output.log 2>&1)
			else
				(gpg --homedir $EXTER/cache/.gpg --no-permission-warning --list-keys 8F427EAF >> $DEST/debug/output.log 2>&1\
				 || gpg --homedir $EXTER/cache/.gpg --no-permission-warning \
				--keyserver hkp://keyserver.ubuntu.com:80 \
				--recv-keys 8F427EAF >> "${DEST}"/debug/output.log 2>&1)

				(gpg --homedir $EXTER/cache/.gpg --no-permission-warning --list-keys 9F0E78D5 >> $DEST/debug/output.log 2>&1\
				|| gpg --homedir $EXTER/cache/.gpg --no-permission-warning \
				--keyserver hkp://keyserver.ubuntu.com:80 \
				--recv-keys 9F0E78D5 >> "${DEST}"/debug/output.log 2>&1)
			fi

			gpg --homedir $EXTER/cache/.gpg --no-permission-warning --verify \
			--trust-model always -q "${localdir}/${filename}.asc" >> "${DEST}"/debug/output.log 2>&1
			[[ ${PIPESTATUS[0]} -eq 0 ]] && verified=true && display_alert "Verified" "PGP" "info"

		else

			md5sum -c --status "${localdir}/${filename}.asc" && verified=true && display_alert "Verified" "MD5" "info"

		fi

		if [[ $verified == true ]]; then
			if [[ "${filename:(-6)}" == "tar.xz" ]]; then

				display_alert "decompressing"
				pv -p -b -r -c -N "[ .... ] ${filename}" "${filename}" | xz -dc | tar xp --xattrs --no-same-owner --overwrite
				[[ $? -eq 0 ]] && touch "${localdir}/${dirname}/.download-complete"
			fi
		else
			exit_with_error "verification failed"
		fi

	fi
}




function run_after_build()
{
	chown -R $(logname).$(logname) $BOOTSOURCEDIR
	chown -R $(logname).$(logname) $LINUXSOURCEDIR
	chown -R $(logname).$(logname) $USERPATCHES_PATH
	chown -R $(logname).$(logname) $DEST/{config,debs,debug,images,patch}
}
