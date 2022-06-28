#!/bin/bash
#
# Copyright (c) Authors: http://www.armbian.com/authors, info@armbian.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.

# Functions:
# main
# check_desktop
# exceptions
# check_if_installed
# is_package_manager_running
# display_qr_code
# beta_disclaimer
# show_box
# description
# generic_select
# reload_bsp
# other_kernel_version
# aval_dtbs
# get_a20modes
# get_h3modes
# add_choose_user
# google_token_allusers
# configure_desktop


#
# gather info about the board and start with loading menu
#
function main(){

	DIALOG_CANCEL=1
	DIALOG_ESC=255

	# we have our own lsb_release which does not use Python. Others shell install it here
	if [[ ! -f /usr/bin/lsb_release ]]; then
		if is_package_manager_running; then
			sleep 3
		fi
		debconf-apt-progress -- apt-get update
		debconf-apt-progress -- apt -y -qq --allow-downgrades --no-install-recommends install lsb-release
	fi

	[[ -f /etc/orangepi-release ]] && source /etc/orangepi-release && ORANGEPI="Orange Pi $VERSION $IMAGE_TYPE";
	DISTRO=$(lsb_release -is)
	DISTROID=$(lsb_release -sc)
	KERNELID=$(uname -r)
	[[ -z "${ORANGEPI// }" ]] && ORANGEPI="$DISTRO $DISTROID"
	DEFAULT_ADAPTER=$(ip -4 route ls | grep default | tail -1 | grep -Po '(?<=dev )(\S+)')
	LOCALIPADD=$(ip -4 addr show dev $DEFAULT_ADAPTER | awk '/inet/ {print $2}' | cut -d'/' -f1)
	BACKTITLE="Configuration utility, $ORANGEPI"
	[[ -n "$LOCALIPADD" ]] && BACKTITLE=$BACKTITLE", "$LOCALIPADD
	TITLE="$BOARD_NAME "
	[[ -z "${DEFAULT_ADAPTER// }" ]] && DEFAULT_ADAPTER="lo"
	OVERLAYDIR="/boot/dtb/overlay";
	[[ "$LINUXFAMILY" == "sunxi64" ]] && OVERLAYDIR="/boot/dtb/allwinner/overlay";
	[[ "$LINUXFAMILY" == "meson64" ]] && OVERLAYDIR="/boot/dtb/amlogic/overlay";
	[[ "$LINUXFAMILY" == "rockchip64" ]] && OVERLAYDIR="/boot/dtb/rockchip/overlay";
	[[ "$LINUXFAMILY" == "rockchip-rk3588" ]] && OVERLAYDIR="/boot/dtb/rockchip/overlay";
	[[ "$LINUXFAMILY" == "sun50iw9" && "$BRANCH" == "current" ]] && OVERLAYDIR="/boot/dtb/sunxi/overlay";
	[[ "$LINUXFAMILY" == "sun50iw9" && "$BRANCH" == "next" ]] && OVERLAYDIR="/boot/dtb/allwinner/overlay";
	[[ "$LINUXFAMILY" == "rockchip-rk356x" ]] && OVERLAYDIR="/boot/dtb/rockchip/overlay";
	# detect desktop
	check_desktop
	dialog --backtitle "$BACKTITLE" --title "Please wait" --infobox "\nLoading Orange Pi configuration utility ... " 5 45
	sleep 1

}




#
# compare two strings in dot separated version format
#
vercomp () {

	if [[ $1 == $2 ]]
	then
		return 0
	fi
	local IFS=.
	local i ver1=($1) ver2=($2)
	# fill empty fields in ver1 with zeros
	for ((i=${#ver1[@]}; i<${#ver2[@]}; i++))
	do
		ver1[i]=0
	done
	for ((i=0; i<${#ver1[@]}; i++))
	do
		if [[ -z ${ver2[i]} ]]
		then
			# fill empty fields in ver2 with zeros
			ver2[i]=0
		fi
		if ((10#${ver1[i]} > 10#${ver2[i]}))
		then
			return 1
		fi
		if ((10#${ver1[i]} < 10#${ver2[i]}))
		then
			return 2
		fi
	done
	return 0

}




#
# test compare two strings $1="3.4.12" $2="5.4.2" $3="<" returns 0 if relation is correct
#
testvercomp () {

	vercomp $1 $2
	case $? in
		0) op='=';;
		1) op='>';;
		2) op='<';;
	esac
	if [[ $op != $3 ]]
	then
		return 1
	else
		return 0
	fi

}




#
# read desktop parameters
#
function check_desktop()
{

	DISPLAY_MANAGER=""; DESKTOP_INSTALLED=""
	check_if_installed nodm && DESKTOP_INSTALLED="nodm";
	check_if_installed lightdm && DESKTOP_INSTALLED="lightdm";
	check_if_installed gdm && DESKTOP_INSTALLED="gdm";
	[[ -n $(service lightdm status 2> /dev/null | grep -w active) ]] && DISPLAY_MANAGER="lightdm"
	[[ -n $(service nodm status 2> /dev/null | grep -w active) ]] && DISPLAY_MANAGER="nodm"
	[[ -n $(service gdm status 2> /dev/null | grep -w active) ]] && DISPLAY_MANAGER="gdm"

}




#
# naming exceptions for packages
#
function exceptions ()
{

	TARGET_FAMILY=$LINUXFAMILY
	UBOOT_BRANCH=$TARGET_BRANCH # uboot naming is different

	if [[ $TARGET_BRANCH == "default" ]]; then TARGET_BRANCH=""; else TARGET_BRANCH="-"$TARGET_BRANCH; fi
	# pine64
	if [[ $TARGET_FAMILY == pine64 ]]; then
		TARGET_FAMILY="sunxi64"
	fi
	# allwinner legacy kernels
	if [[ $TARGET_FAMILY == sun*i ]]; then
		TARGET_FAMILY="sunxi"
		if [[ $UBOOT_BRANCH == "default" ]]; then
			TARGET_FAMILY=$(cat /proc/cpuinfo | grep "Hardware" | sed 's/^.*Allwinner //' | awk '{print $1;}')
		fi
	fi

}




#
# check dpkg status of $1 -- currently only 'not installed at all' case caught
#
check_if_installed (){

	local DPKG_Status="$(dpkg -s "$1" 2>/dev/null | awk -F": " '/^Status/ {print $2}')"
	if [[ "X${DPKG_Status}" = "X" || "${DPKG_Status}" = *deinstall* ]]; then
		return 1
	else
		return 0
	fi

}




#
# check if package manager is doing something
#
function is_package_manager_running() {

	if ps -C apt-get,apt,dpkg >/dev/null ; then
		[[ -z $scripted ]] && dialog --colors --title " \Z1Error\Z0 " --backtitle "$BACKTITLE" --no-collapse --msgbox \
		"\n\Z0Package manager is running in the background. \n\nCan't install dependencies. Try again later." 9 53
		return 0
	else
		return 1
	fi

}




#
# wget with dialog progress bar $1=URL $2=parameters
#
function fancy_wget()
{

	LANG=C wget $2 --progress=bar:force:noscroll $1 2>&1 | stdbuf -i0 -o0 -e0 tr '>' '\n' | \
	stdbuf -i0 -o0 -e0 sed -rn 's/^.*\<([0-9]+)%\[.*$/\1/p' | dialog --backtitle "$BACKTITLE" --title " Downloading " \
	--gauge "Please wait" 7 70 0

}




#
# display qr code for authentication method
#
function display_qr_code()
{

	clear
	SECRET=$(head -1 /root/.google_authenticator)
	qrencode -m 2 -d 9 -8 -t ANSI256 "otpauth://totp/test?secret=$SECRET"
	echo -e "\nHow to setup your one type password generator?\n"
	echo -e "\nInstall a one-time password authenticator on your mobile device (e.g. FreeOTP) from the Android \
market or F-Droid.	\n\nIn the application menu, click the corresponding button to create a new account and either \
scan the QR code or enter the secret key manually:\\n\n$SECRET \n\nYou should now see a new passcode token being \
generated every 60 seconds on your phone.\n"  | fold -sw 60
	read -n 1 -s -r -p "Press any key to continue"

}




#
# show disclaimer
#
function beta_disclaimer ()
{

	exec 3>&1
	ACKNOWLEDGEMENT=$(dialog --colors --nocancel --backtitle "$BACKTITLE" --no-collapse --title " Warning " \
	--clear \--radiolist "\n$1\n \n" 0 56 5 "Yes, I understand" "" off	 2>&1 1>&3)
	exec 3>&-

}




#
# show box
#
function show_box ()
{

	dialog --colors --backtitle "$BACKTITLE" --no-collapse --title " $1 " --clear --msgbox "\n$2\n \n" $3 56

}




#
# show description for MOTD files
#
function description
{
	case $1 in
		*header*)
			echo "Big board logo and kernel info"
		;;
		*sysinfo*)
			echo "Sysinfo - load, ip, memory, uptime, ..."
		;;
		*tips*)
			echo "Shows tip of the day"
		;;
		*updates*)
			echo "Display number of available updates"
		;;
		*orangepi-config*)
			echo "Show command for system configuration"
		;;
		*autoreboot-warn*)
			echo "Show warning when reboot is needed"
		;;
		*uk.armbian.com*)
			echo "United Kingdom"
		;;
		*mirrors.tuna.tsinghua.edu.cn/armbian/*)
			echo "China"
		;;
		*mirrors.netix.net/armbian/apt/*)
			echo "Bulgarija"
		;;
		*mirrors.dotsrc.org/armbian-apt/*)
			echo "Denmark"
		;;
		*.armbian.com*)
			echo "Estonia"
		;;
		*)
			echo ""
		;;
	esac
}


#
# Generic select box
#
function generic_select()
{
	IFS=$' '
	PARAMETER=($1)
	local LIST=()
	for i in "${PARAMETER[@]}"
	do
		if [[ -n $3 ]]; then
			[[ ${i[0]} -ge $3 ]] && \
			LIST+=( "${i[0]//[[:blank:]]/}" "" )
		else
			LIST+=( "${i[0]//[[:blank:]]/}" "" )
		fi
	done
	LIST_LENGTH=$((${#LIST[@]}/2));
	if [ "$LIST_LENGTH" -eq 1 ]; then
		PARAMETER=${PARAMETER[0]}
	else
		exec 3>&1
		PARAMETER=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
		--title "$2" --clear --menu "" $((6+${LIST_LENGTH})) 0 $((1+${LIST_LENGTH})) "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
	fi
}




#
# reload kernel, bsp and orangepi-config
#
function reload_bsp(){

	# switch to defined branch
	[[ -n "$1" ]] && BRANCH=$1

	# deal with exceptions
	if [[ $BRANCH == current || $BRANCH == dev ]]; then
		[[ ${LINUXFAMILY} == rk3399 ]] && LINUXFAMILY=rockchip64
	fi

	clear
	debconf-apt-progress -- apt-get update

	# must exits
	PACKAGE_INSTALL="linux-image-${BRANCH}-${LINUXFAMILY} linux-dtb-${BRANCH}-${LINUXFAMILY}"
	PACKAGE_PURGE="linux-image* linux-dtb*"

	# create install and remove logic
	command=$(LC_ALL=C apt-get --download-only --simulate --allow-downgrades -y --no-install-recommends install linux-${DISTROID}-root-${BRANCH}-${BOARD} 2>/dev/null)
	if [[ $? -eq 0 && -z $(echo $command | grep "not possible") ]]; then
		PACKAGE_INSTALL+=" linux-${DISTROID}-root-${BRANCH}-${BOARD}"
		PACKAGE_PURGE+=" linux-${DISTROID}-root*"
	fi

	command=$(LC_ALL=C apt-get --download-only --simulate --reinstall --allow-downgrades -y --no-install-recommends install linux-u-boot-${BOARD}-${BRANCH} 2>/dev/null)
	if [[ $? -eq 0 && -z $(echo $command | grep "not possible") ]]; then
		PACKAGE_INSTALL+=" linux-u-boot-${BOARD}-${BRANCH}"
		PACKAGE_PURGE+=" linux-u-boot-${BOARD}-*"
	fi

	if check_if_installed orangepi-${DISTROID}-desktop ; then
		PACKAGE_INSTALL+=" orangepi-${DISTROID}-desktop"
		PACKAGE_PURGE+=" orangepi-${DISTROID}-desktop*"
	fi

	if check_if_installed linux-headers-${BRANCH}-${FAMILY} ; then
		PACKAGE_INSTALL+=" linux-headers-${BRANCH}-${FAMILY}"
		PACKAGE_PURGE+=" linux-headers*"
	fi

	debconf-apt-progress -- apt-get -o Dpkg::Options::="--force-confold" --force-yes --download-only --allow-downgrades -y --no-install-recommends install $PACKAGE_INSTALL
	# if download is ok, remove old packages
	if [[ $? = 0 ]]; then

		debconf-apt-progress -- apt-get -y -qq purge $PACKAGE_PURGE
		find "/boot/" -name "System.map*" -type f -delete
		find "/boot/" -name "config*" -type f -delete
		find "/boot/" -name "vmlinuz*" -type f -delete
		find "/boot/" -name "*nitrd*" -type f -delete
		find "/boot/" -name "*Image" -type l -delete

		# install packages
		echo $PACKAGE_INSTALL >> /var/log/upgrade.log
		debconf-apt-progress -- apt-get -y -qq --allow-downgrades --no-install-recommends --reinstall \
		-o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" install $PACKAGE_INSTALL

		if [[ $? -eq 1 ]]; then
			echo "Something went wrong ... check logs."; exit;
		else
			apt clean
		fi

	fi

}




function other_kernel_version ()
{

	IFS=$'\r\n'
	GLOBIGNORE='*'
	local HIDDEN='hidden'

	# get current kernel information
	CURRENT_VERSION_TEMP=$(dpkg -l | grep '^ii' | grep linux-image)
	CURRENT_VERSION=$(echo $CURRENT_VERSION_TEMP | awk '{print $2}')"="$(echo $CURRENT_VERSION_TEMP | awk '{print $3}')

	# Merge families and handle exceptions
	[[ ${LINUXFAMILY} == sun*i ]] && LINUXFAMILY=sunxi
	[[ ${LINUXFAMILY} == pine64 ]] && LINUXFAMILY=sunxi64
	[[ ${LINUXFAMILY} == sun*iw* ]] && LINUXFAMILY=sunxi64
	[[ ${LINUXFAMILY} == cubox || ${LINUXFAMILY} == udoo ]] && LINUXFAMILY=imx6
	[[ ${BOARD} == odroidc2 || ${BOARD} == kvim1 || ${BOARD} == lafrite || ${BOARD} == lepotato || ${BOARD} == nanopik2-s905 ]] && HIDDEN="legacy"
	[[ ${LINUXFAMILY} == odroidn2 ]] && LINUXFAMILY=meson64

	# check what is available from the repository
	debconf-apt-progress -- apt-get update
	LIST=($(apt-cache show linux-image*${LINUXFAMILY} | grep -E  "Package:|Version:|version:" \
	| grep -v "Config-Version" | sed -n -e 's/^.*: //p' | sed 's/\.$//g'))
	new_list=()

	# create a human readable menu
	for ((n=0;n<$((${#LIST[@]}));n++));
	do
		m=$(( $n + 1 ))
		prvi=$((3*$m - 3))
		drugi=$((3*$m - 2))
		tretji=$((3*$m - 1))
		[[ -z ${LIST[$prvi]} ]] && break
		if [[ $CURRENT_VERSION != "${LIST[$prvi]}=${LIST[$drugi]}" && ${LIST[$prvi]} != *${HIDDEN}* ]]; then
			new_list+=( "${LIST[$prvi]}=${LIST[$drugi]}" )
			new_list+=( ${LIST[$tretji]} )
		fi
		local tmp="${LIST[$prvi]}=${LIST[$drugi]}${LIST[$tretji]}"
		[[ ${#tmp} -gt $chrlen ]] && local chrlen=${#tmp}
	done

	# copy back to main array
	LIST=("${new_list[@]}")
	LIST_LENGTH=$((${#LIST[@]}/2));

	if [ "$LIST_LENGTH" -eq 0 ]; then
		dialog --backtitle "$BACKTITLE" --title " Warning " --msgbox  "\nNo other kernels available!" 7 32
	else
		beta_disclaimer "Switching between kernels might change functionality of your board or it might fail to boot. \
		\n\n\Z1In case of troubles expect no help!\Z0"
		if [[ -n $ACKNOWLEDGEMENT ]]; then
			exec 3>&1
			TARGET_VERSION=$(dialog --ok-label "Reboot" --cancel-label "Cancel" --backtitle "$BACKTITLE" --no-collapse \
			--title "Select kernel and reboot" --clear --menu "" $((6+${LIST_LENGTH})) $((7+${chrlen})) 25 "${LIST[@]}" 2>&1 1>&3)
			exitstatus=$?;
			exec 3>&-
			if [[ $exitstatus = 0 ]]; then
				IFS=" "

				# determine upgrade packages
				UPGRADE_ROOT="default"
				UPGRADE_UBOOT=""
				[[ $TARGET_VERSION == *legacy* ]] && UPGRADE_ROOT="-legacy" && UPGRADE_UBOOT="legacy"
				[[ $TARGET_VERSION == *next* ]] && UPGRADE_ROOT="-next"  && UPGRADE_UBOOT="next"
				[[ $TARGET_VERSION == *current* ]] && UPGRADE_ROOT="-current" && UPGRADE_UBOOT="current"
				[[ $TARGET_VERSION == *dev* ]] && UPGRADE_ROOT="-dev" && UPGRADE_UBOOT="dev"

				# install packages
				PACKAGE_LIST="$TARGET_VERSION"
				TARGET_VERSION_DTB=${TARGET_VERSION/image/dtb}
				TARGET_VERSION_PRE=$(echo $TARGET_VERSION_DTB | cut -f1 -d"=")
				TARGET_VERSION_SUB=$(echo $TARGET_VERSION_DTB | cut -f2 -d"=")
				[[ -n $(apt-cache madison "$TARGET_VERSION_PRE" | grep $TARGET_VERSION_SUB ) ]] && \
				# installs u-boot only if exits
				apt-cache show linux-u-boot-${BOARD}-${UPGRADE_UBOOT} 2> /dev/null
				[[ $? -eq 0 ]] && PACKAGE_LIST=$PACKAGE_LIST" linux-u-boot-${BOARD}-${UPGRADE_UBOOT}"

				PACKAGE_LIST=$PACKAGE_LIST" $TARGET_VERSION_DTB"
				echo $PACKAGE_LIST > /tmp/switch_kernel.log 2>&1
				debconf-apt-progress -- apt -o Dpkg::Options::="--force-confold" --force-yes --download-only --allow-downgrades -y --no-install-recommends install $PACKAGE_LIST

				if [[ $? = 0 ]]; then
					dialog --backtitle "$BACKTITLE" --title "Please wait" --infobox "\nRemoving current kernel ..." 5 36

					# remove old kernel
					debconf-apt-progress -- apt -y -qq purge linux-u-boot* linux-image* linux-dtb* linux-headers* linux-${DISTROID}-root*

					# cleanup
					find "/boot/" -name "System.map*" -type f -delete;	find "/boot/" -name "config*" -type f -delete
					find "/boot/" -name "vmlinuz*" -type f -delete;	find "/boot/" -name "*nitrd*" -type f -delete
					apt clean
					# BSP must be installed separate otherwise it won't work
					debconf-apt-progress -- apt -o Dpkg::Options::="--force-confold" --force-yes -y -qq --allow-downgrades --no-install-recommends install linux-${DISTROID}-root${UPGRADE_ROOT}-${BOARD}
					debconf-apt-progress -- apt -o Dpkg::Options::="--force-confold" --force-yes -y -qq --allow-downgrades --no-install-recommends install $PACKAGE_LIST
					if [[ $? = 0 ]]; then
						# update boot loader
						[[ -f /usr/lib/u-boot/platform_install.sh ]] && source /usr/lib/u-boot/platform_install.sh
						#recognize_root
						root_uuid=$(sed -e 's/^.*root=//' -e 's/ .*$//' < /proc/cmdline)
						root_partition=$(blkid | tr -d '":' | grep "${root_uuid}" | awk '{print $1}')
						root_partition_device="${root_partition::-2}"
						write_uboot_platform "$DIR" "${root_partition_device}"
						reboot
					fi
				else
					dialog --backtitle "$BACKTITLE" --title "Warning" --msgbox \
					"\nTest install failed. Can't change firmware \n\nCheck /tmp/switch_kernel.log" 9 48
				fi
			fi
		fi
	fi
}




#
# check if board has alternative kernels
#
function aval_dtbs ()
{

	if [[ $LINUXFAMILY == cubox ]]; then
		local width=80
		LIST=("imx6dl-hummingboard.dtb" "HB Solo/DualLite" "imx6dl-hummingboard-emmc-som-v15.dtb" "HB Solo/DualLite v1.5 with eMMC" "imx6dl-hummingboard-som-v15.dtb" "HB Solo/DualLite v1.5" \
		"imx6dl-hummingboard2.dtb" "HB2 Solo/DualLite" "imx6dl-hummingboard2-emmc-som-v15.dtb" "HB2 Solo/DualLite v1.5 with eMMC" "imx6dl-hummingboard2-som-v15.dtb" "HB2 Solo/DualLite v1.5" \
		"imx6q-hummingboard.dtb" "HB Dual/Quad" "imx6q-hummingboard-emmc-som-v15.dtb" "HB Dual/Quad v1.5 with eMMC" "imx6q-hummingboard-som-v15.dtb" "HB Dual/Quad v1.5" \
		"imx6q-hummingboard2.dtb" "HB2 Dual/Quad" "imx6q-hummingboard2-emmc-som-v15.dtb" "HB2 Dual/Quad v1.5 with eMMC" "imx6q-hummingboard2-som-v15.dtb" "HB2 Dual/Quad v1.5" \
		"imx6dl-cubox-i.dtb" "Cubox-i Solo/DualLite" "imx6dl-cubox-i-emmc-som-v15.dtb" "Cubox-i Solo/DualLite v1.5 with eMMC" "imx6dl-cubox-i-som-v15.dtb" "Cubox-i Solo/DualLite v1.5" \
		"imx6q-cubox-i.dtb" "Cubox-i Dual/Quad" "imx6q-cubox-i-emmc-som-v15.dtb" "Cubox-i Dual/Quad v1.5 with eMMC" "imx6q-cubox-i-som-v15.dtb" "Cubox-i Dual/Quad v1.5")
	else
		local width=52
		LIST=("xu4" "Odroid XU4" "xu3" "Odroid XU3" "xu3l" "Odroid XU3 Lite" "hc1" "Odroid HC1/HC2")
	fi

	LIST_LENGTH=$((${#LIST[@]}/2));
	if [ "$LIST_LENGTH" -eq 1 ]; then
		TARGET_BOARD=${AVAL_KERNEL[0]}
	else
		exec 3>&1
		TARGET_BOARD=$(dialog --cancel-label "Cancel" --backtitle "$BACKTITLE" --no-collapse \
		--title "Select optimised board configuration" --clear --menu \
		"" $((6+${LIST_LENGTH})) ${width} 25 "${LIST[@]}" 2>&1 1>&3)
		exitstatus=$?;
		exec 3>&-
	fi

}




#
# select video modes for a10 and a20
#
function get_a20modes ()
{

	IFS=$'\r'
	GLOBIGNORE='*'
	SCREEN_RESOLUTION=("1920x1080p60" "1280x720p60" "1920x1080p50" "1280x1024p60" "1024x768p60" "800x600p60" "640x480p60" "1360x768p60" "1440x900p60" "1680x1050p60")
	local LIST=()
	for i in "${SCREEN_RESOLUTION[@]}"
	do
		LIST+=( "${i[0]//[[:blank:]]/}" "" )
	done
	LIST_LENGTH=$((${#LIST[@]}/2));
	#echo $LIST_LENGTH
	#exit
	if [ "$LIST_LENGTH" -eq 1 ]; then
		SCREEN_RESOLUTION=${SCREEN_RESOLUTION[0]}
	else
		exec 3>&1
		SCREEN_RESOLUTION=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
		--title "Select video mode" --clear --menu \
		"" $((6+${LIST_LENGTH})) 25 $((1+${LIST_LENGTH})) "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
	fi

}




#
# select video modes for odroid c1/c2
#
function get_odroidmodes ()
{

	IFS=$'\r\n'
	GLOBIGNORE='*'
	SCREEN_RESOLUTION=($(cat /boot/boot.ini | grep -w "# setenv" | grep "hz" | cut -d'"' -f 2))
	SCREEN_RESOLUTION=($(cat /boot/boot.ini | grep "Progressive" | grep -v "setenv" | cut -d'"' -f 2))
	local LIST=()
	for i in "${SCREEN_RESOLUTION[@]}"
	do
		LIST+=( "${i[0]//[[:blank:]]/}" "" )
	done
	LIST_LENGTH=$((${#LIST[@]}/2));
	#echo $LIST_LENGTH
	#exit
	if [ "$LIST_LENGTH" -eq 1 ]; then
		SCREEN_RESOLUTION=${SCREEN_RESOLUTION[0]}
	else
		exec 3>&1
		SCREEN_RESOLUTION=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
		--title "Select video mode" --clear --menu \
		"" $((6+${LIST_LENGTH})) 25 $((1+${LIST_LENGTH})) "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
	fi

}




#
# select video modes for h3
#
function get_h3modes ()
{

	IFS=$'\r\n'
	GLOBIGNORE='*'
	SCREEN_RESOLUTION=($(h3disp -i clean))
	local LIST=()
	for i in "${SCREEN_RESOLUTION[@]}"
	do
		LIST+=( "${i[0]//[[:blank:]]/}" "" )
	done
	LIST_LENGTH=$((${#LIST[@]}/2));
	#echo $LIST_LENGTH
	#exit
	if [ "$LIST_LENGTH" -eq 1 ]; then
		SCREEN_RESOLUTION=${SCREEN_RESOLUTION[0]}
	else
		exec 3>&1
		SCREEN_RESOLUTION=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
		--title "Select video mode" --clear --menu \
		"" $((6+${LIST_LENGTH})) 25 $((1+${LIST_LENGTH})) "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
	fi

}




#
# create or pick unprivileged user
#
function add_choose_user ()
{

	IFS=$'\r\n'
	GLOBIGNORE='*'

	local USERS=($(awk -F'[/:]' '{if ($3 >= 1000 && $3 != 65534) print $1}' /etc/passwd))
	local LIST=()
	for i in "${USERS[@]}"
	do
		LIST+=( "${i[0]//[[:blank:]]/}" "" )
	done
	LIST_LENGTH=$((${#LIST[@]}/2));

	if [ "$LIST_LENGTH" -eq 0 ]; then
		dialog --backtitle "$BACKTITLE" --title " Notice " --msgbox \
		"\nWe didn't find any unprivileged user with sudo rights which is required to run this service.\
		\n\nPress enter to create one!" 10 48
		read -t 0 temp
		echo -e "\nPlease provide a username (eg. your forename) or leave blank for canceling user creation: \c"
		read -e username
		CHOSEN_USER="$(echo "$username" | tr '[:upper:]' '[:lower:]' | tr -d -c '[:alnum:]')"
		[ -z "$CHOSEN_USER" ] && return
		echo "Trying to add user $CHOSEN_USER"
		adduser $CHOSEN_USER || return
	elif [ "$LIST_LENGTH" -eq 1 ]; then
		CHOSEN_USER=${USERS[0]}
	else
		exec 3>&1
		CHOSEN_USER=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
		--title "Select unprivileged user" --clear --menu "" $((6+${LIST_LENGTH})) 40 15 "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
	fi

}




#
# Copy Google token to all local users.
#
function google_token_allusers ()
{

	if [[ -f /root/.google_authenticator ]]; then
		local USERS=($(awk -F'[/:]' '{if ($3 >= 1000 && $3 != 65534) print $1}' /etc/passwd))
		for i in "${USERS[@]}"
		do
			USER=${i[0]//[[:blank:]]/}
			if [[ -d /home/$USER ]]; then
				cp /root/.google_authenticator /home/$USER/
				chown ${USER}:${USER} /home/${USER}/.google_authenticator
			fi
		done
	fi

}




#
# configure orangepi desktop
#
function configure_desktop ()
{

	add_choose_user

	if [ -n "$CHOSEN_USER" ]; then

		# update packages
		debconf-apt-progress -- apt-get update

		# install new package if exists
		unset PACKAGE_SUFIX
		[[ -n $(apt-cache search --names-only "^orangepi-${DISTROID}-desktop-xfce$") ]] && PACKAGE_SUFIX="-xfce"

		# remove desktop package to secure proper install
		if check_if_installed orangepi-${DISTROID}-desktop ; then
			debconf-apt-progress -- apt-get -y \
			remove orangepi-${DISTROID}-desktop${PACKAGE_SUFIX} lightdm lightdm-gtk-greeter
		fi

		# install desktop package
		debconf-apt-progress -- apt-get --reinstall -o Dpkg::Options::="--force-confdef" \
		-o Dpkg::Options::="--force-confold" -y \
		install $1 orangepi-${DISTROID}-desktop${PACKAGE_SUFIX} lightdm lightdm-gtk-greeter

		# in case previous install was interrupted
		[[ $? -eq 130 ]] && dpkg --configure -a

		# clean apt cache
		apt clean

		# add user to groups
		for additionalgroup in sudo netdev audio video dialout plugdev input bluetooth systemd-journal ssh; do
				usermod -aG ${additionalgroup} ${CHOSEN_USER} 2>/dev/null
		done

		# Prevent loading paralel printer port drivers which we don't need here.
		# suppress boot error if kernel modules are absent
		if [[ -f /etc/modules-load.d/cups-filters.conf ]]; then
			sed "s/^lp/#lp/" -i /etc/modules-load.d/cups-filters.conf
			sed "s/^ppdev/#ppdev/" -i /etc/modules-load.d/cups-filters.conf
			sed "s/^parport_pc/#parport_pc/" -i /etc/modules-load.d/cups-filters.conf
		fi

		# enable show windows content on stronger boards
		cpu_cores=$(grep -c '^processor' /proc/cpuinfo | sed 's/^0$/1/')
		if [[ ${cpu_cores} -gt 2 && -f /etc/skel/.config/xfce4/xfconf/xfce-perchannel-xml/xfwm4.xml ]]; then
			sed -i 's/<property name="box_move" type="bool" value=".*/<property name="box_move" type="bool" value="false"\/>/g' \
			/etc/skel/.config/xfce4/xfconf/xfce-perchannel-xml/xfwm4.xml
		fi

		# fix for gksu in Xenial
		touch /home/${CHOSEN_USER}/.Xauthority
		cp -R /etc/skel/. /home/${CHOSEN_USER}

		# set up profile sync daemon on desktop systems
		which psd >/dev/null 2>&1
		if [[ $? -eq 0 && -z $(grep overlay-helper /etc/sudoers) ]]; then
			echo "${CHOSEN_USER} ALL=(ALL) NOPASSWD: /usr/bin/psd-overlay-helper" >> /etc/sudoers
			touch /home/${CHOSEN_USER}/.activate_psd
		fi

		mkdir -p /etc/lightdm/lightdm.conf.d
		echo "[Seat:*]" > /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
		echo "autologin-user=$CHOSEN_USER" >> /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
		echo "autologin-user-timeout=0" >> /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
		echo "user-session=xfce" >> /etc/lightdm/lightdm.conf.d/22-orangepi-autologin.conf
		ln -s /lib/systemd/system/lightdm.service /etc/systemd/system/display-manager.service >/dev/null 2>&1
		# fix permissions
		chown -R ${CHOSEN_USER}:${CHOSEN_USER} /home/${CHOSEN_USER}/.
		service lightdm start >/dev/null 2>&1
	fi

}
