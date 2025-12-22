#!/bin/bash
#
# Copyright (c) 2018 Igor Pecovnik, igor.pecovnik@gma**.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.

# Functions:
# submenu_settings
# submenu_networking
# submenu_personal
# submenu_software



#
# system settings
#
function submenu_settings ()
{
unset main
while true; do

	LIST=()
	DIALOG_CANCEL=1
	DIALOG_ESC=255

	# detect desktop
	check_desktop

	# check update status of BSP packages
	local mark=$(apt-mark showhold | egrep "linux|orangepi")

	# check if eMMC/SD is on the system
	if [[ $(sed -n 's/^DEVNAME=//p' /sys/dev/block/$(mountpoint -d /)/uevent 2> /dev/null) == mmcblk* \
	&& -f /usr/sbin/nand-sata-install ]]; then
		LIST+=( "Install" "Install to/update boot loader" )
	fi

	# orangepi specific function
	if [[ -f /etc/orangepi-release ]]; then
		if [[ -n "${mark}" ]]; then
				:
				#LIST+=( "Defreeze" "Enable Orange Pi upgrades " )
			else
				:
				#LIST+=( "Freeze" "Disable Orange Pi upgrades " )
		fi
	fi

	# orangepi specific function, when upgrades are enabled
	if [[ -z ${mark} ]]; then
		[[ -f /etc/apt/sources.list.d/orangepi.list ]] && [[ -n $(grep -w apt /etc/apt/sources.list.d/orangepi.list) ]] \
		&& LIST+=( "Nightly" "Switch to nightly automated builds" )
		[[ -f /etc/apt/sources.list.d/orangepi.list ]] && [[ -n $(grep -w beta /etc/apt/sources.list.d/orangepi.list) ]] \
		&& LIST+=( "Stable" "Switch to stable builds" )
	fi

	[[ -n $(grep -w "#kernel.printk" /etc/sysctl.conf ) ]] && LIST+=( "Lowlevel" "Stop low-level messages on console" )

	[[ -f /boot/orangepiEnv.txt ]] && LIST+=( "Bootenv" "Edit boot environment" )
	[[ -f /boot/boot.ini ]] && LIST+=( "Bootscript" "Edit boot script" )

	if [[ -f /etc/default/cpufrequtils ]]; then
		LIST+=( "CPU" "Set CPU speed and governor" )
	fi

	AVAHIDSTATUS=$(service avahi-daemon status 2> /dev/null | grep -w active | grep -w running)
	if [[ -n "$AVAHIDSTATUS" ]]; then
		LIST+=( "Avahi" "Disable system announce in the network" )
		else
		LIST+=( "Avahi" "Announce system in the network" )
	fi

	[[ -d ${OVERLAYDIR} ]] && \
	LIST+=( "Hardware" "Toggle hardware configuration: UART, I2C, etc." )
	[[ "$LINUXFAMILY" = cubox && "$BRANCH" = "next" ]] && LIST+=( "DTB" "Switch board .dtb configuration" )
	# this is avaliable only in kernel higher than 4.10
	testvercomp "$(uname -r | sed 's/-.*//')" "4.10.0" ">"
	[[ "$LINUXFAMILY" = odroidxu4 && $? == 0 ]] && LIST+=( "DTB" "Select optimised board configuration" )
	[[ -f /usr/bin/bin2fex && "$LINUXFAMILY" = sun*i && "$BRANCH" = "default" ]] && LIST+=( "Fexedit" "Board (fex) settings editor" )

	[[ -z ${mark} ]] && [[ -f /etc/apt/sources.list.d/orangepi.list ]] && LIST+=( "Other" "Switch to other kernels" )

	LIST+=( "SSH" "Reconfigure SSH daemon" )
	LIST+=( "Firmware" "Run apt update & apt upgrade" )

	if [[ "$SHELL" != "/bin/bash" ]]; then
		LIST+=( "BASH" "Revert to stock BASH shell" )
		else
		LIST+=( "ZSH" "Install ZSH with plugins and tmux" )
	fi


	[[ "$LINUXFAMILY" = sun*i && "$BRANCH" = "default" && \
	-n $(bin2fex </boot/script.bin 2>/dev/null | grep -w "hdmi_used = 1") ]] && LIST+=( "Display" "Set the display resolution" )
	[[ "$LINUXFAMILY" = odroidc* || "$LINUXFAMILY" = odroidn2 && "$BRANCH" = "default" ]] && LIST+=( "Display" "Set the display resolution" )
	# desktop
	if [[ -n $DISPLAY_MANAGER ]]; then
		LIST+=( "Desktop" "Disable desktop or change login type" )
	else
		if [[ -n $DESKTOP_INSTALLED ]]; then
			LIST+=( "Desktop" "Enable desktop" )
		else
			:
			#LIST+=( "Default" "Install desktop with browser and extras" )
		fi
	fi

	# overlayroot
	if [[ "$DISTRO" =~ "Ubuntu"|"Debian" && "$(modinfo overlay > /dev/null 2>&1; echo $?)" == "0" ]]; then
		#if [ -n "$(mount | grep -w tmpfs-root)" ]; then
		#	:
		#	#LIST+=( "Overlayroot" "Disable virtual read-only root filesystem" )
		#else
		#	:
		#	#LIST+=( "Overlayroot" "Enable virtual read-only root filesystem" )
		#fi
		LIST+=( "Overlayroot" "Enable or disable a overlay filesystem." )
	fi

	# count number of menu items to adjust window size
	LISTLENGTH="$((6+${#LIST[@]}/2))"
	BOXLENGTH=${#LIST[@]}
	temp_rc=$(mktemp)
	if [[ -n ${mark} || -n $(grep -w beta /etc/apt/sources.list.d/orangepi.list 2> /dev/null) ]]; then
			cat <<-'EOF' > $temp_rc
			dialog_color = (RED,WHITE,OFF)
			screen_color = (WHITE,RED,ON)
			tag_color = (RED,WHITE,ON)
			item_selected_color = (WHITE,RED,ON)
			tag_selected_color = (WHITE,RED,ON)
			tag_key_selected_color = (WHITE,RED,ON)
			EOF
			[[ -n ${mark} ]] && local sys_title=" Warning - firmware packages frozen"
			[[ -n $(grep -w beta /etc/apt/sources.list.d/orangepi.list 2> /dev/null) ]] && \
			local sys_title=" Warning - attached to beta repository"
			[[ -n ${mark} && -n $(grep -w beta /etc/apt/sources.list.d/orangepi.list 2> /dev/null) ]] && \
			sys_title=" Warning - frozen & attached to beta repository"
		else
			local sys_title=" System settings "
			echo > $temp_rc
	fi

if [[ -z $selection ]]; then
	exec 3>&1
	selection=$(DIALOGRC=$temp_rc dialog --colors --backtitle "$BACKTITLE" --title " $sys_title " --clear \
	--cancel-label "Back" --menu "$disclaimer" $LISTLENGTH 0 $BOXLENGTH \
	"${LIST[@]}" 2>&1 1>&3)
	exit_status=$?
	exec 3>&-
	[[ $exit_status == $DIALOG_CANCEL || $exit_status == $DIALOG_ESC ]] && clear && break
fi

	# run main function
	jobs "$selection"
done
}




#
# menu for networking
#
function submenu_networking ()
{

# select default interface if there is more than one connected
#select_interface "default"
unset main
while true; do

	LIST=()
	DIALOG_CANCEL=1
	DIALOG_ESC=255

	# check if we have some LTE modems
	for i in $(lsusb | awk '{print $6}'); do lte "$i"; done;

	# edit ip
	LIST+=( "IP" "Select dynamic or edit static IP address" )

	# hostapd
	HOSTAPDBRIDGE=$(cat /etc/hostapd.conf 2> /dev/null | grep -w "^bridge=br0")
	HOSTAPDSTATUS=$(service hostapd status 2> /dev/null | grep -w active | grep -w running)
	if [[ -n "$HOSTAPDSTATUS" ]]; then
			HOSTAPDINFO=$(hostapd_cli get_config 2> /dev/null | grep ^ssid | sed 's/ssid=//g')
			HOSTAPDCLIENTS=$(hostapd_cli all_sta 2> /dev/null | grep connected_time | wc -l)
			LIST+=( "Hotspot" "Manage active wireless access point" )
		else
			:
			#[[ -n $(LC_ALL=C nmcli device status 2> /dev/null | grep wifi | grep -w disconnected) ]] && \
			#LIST+=( "Hotspot" "Create WiFi access point" )

	fi

	# toggle IPv6
	if [[ $(cat /proc/sys/net/ipv6/conf/all/disable_ipv6) == 1 ]]; then
			LIST+=( "IPV6" "Enable IPv6 for APT and system" )
		else
			LIST+=( "IPV6" "Disable IPv6 for APT and system" )
	fi

	# network throughput test
	if check_if_installed iperf3; then
		if pgrep -x "iperf3" > /dev/null
		then
			LIST+=( "Iperf3" "Disable network throughput tests daemon" )
			else
			LIST+=( "Iperf3" "Enable network throughput tests daemon" )
		fi
	fi

	if [[ -n $(LC_ALL=C nmcli device status 2> /dev/null | grep wifi | grep -v unavailable | grep -v unmanaged) ]]; then
		LIST+=( "WiFi" "Manage wireless networking" )
	else
		LIST+=( "Clear" "Clear possible blocked interfaces" )
	fi

	if [[ -n $LTE_MODEM ]]; then
		# apply udev rules
		initialize_lte
		LIST+=( "LTE" "$LTE_MODEM" )
	fi

	check_if_installed lirc && LIST+=( "Remove IR" "Remove IR support" ) || LIST+=( "IR" "Install IR support" )
	if check_if_installed bluetooth then ; then
			LIST+=( "BT remove" "Remove Bluetooth support" )
			if [[ -n $(service bluetooth status | grep -w active | grep -w running) ]]; then
				[[ $(hcitool dev | sed '1d') != "" ]] && LIST+=( "BT discover" "Discover and connect Bluetooth devices" )
			fi
		else
			LIST+=( "BT install" "Install Bluetooth support" )
	fi



	[[ -d /usr/local/vpnclient ]] && LIST+=( "VPN" "Manage Softether VPN client" ) && VPNSERVERIP=$(/usr/local/vpnclient/vpncmd /client localhost /cmd accountlist | grep "VPN Server" |grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}' | head -1)

	LIST+=( "Advanced" "Edit /etc/network/interfaces" )
	[[ $(ls -1 /etc/NetworkManager/system-connections 2> /dev/null) ]] && \
	LIST+=( "Forget" "Disconnect and forget all wireless connections" )

	# count number of menu items to adjust window size
	LISTLENGTH="$((12+${#LIST[@]}/2))"
	BOXLENGTH=${#LIST[@]}
	WIFICONNECTED=$(LC_ALL=C nmcli -f NAME,TYPE connection show --active 2> /dev/null | grep wireless | awk 'NF{NF-=1};1')

	local disclaimer=""

	local ipadd=$(ip -4 addr show dev $DEFAULT_ADAPTER | awk '/inet/ {print $2}' | cut -d'/' -f1)


	if [[ -n $(LC_ALL=C nmcli device status 2> /dev/null | grep $DEFAULT_ADAPTER | grep connected) ]]; then
		local ifup="\nIP ($DEFAULT_ADAPTER) via Network Manager: \Z1${ipadd}\n\Z0 "
	else
		if [[ -n $(service systemd-networkd status | grep -w active | grep -w running) ]]; then
			local ifup="\nIP ($DEFAULT_ADAPTER) via systemd-networkd: \Z1${ipadd}\n\Z0 "
		else
			local ifup="\nIP ($DEFAULT_ADAPTER) via IFUPDOWN: \Z1${ipadd}\n\Z0 "
		fi
	fi

	disclaimer="$ifup"

	if [[ -n $WIFICONNECTED ]]; then
		LISTLENGTH=$((LISTLENGTH+2))
		local connected="\n\Z0Connected to SSID: \Z1${WIFICONNECTED}\n\Z0 "
		disclaimer=$disclaimer"$connected"
	fi

	if [[ -n $VPNSERVERIP ]]; then
		local vpnserverip="\n\Z0Connected to VPN server: \Z1${VPNSERVERIP}\n\Z0 "
		disclaimer=$disclaimer"$vpnserverip"
		LISTLENGTH=$((LISTLENGTH+2))
	fi

	if [[ -n $HOSTAPDINFO && -n $HOSTAPDSTATUS ]]; then
		LISTLENGTH=$((LISTLENGTH+2))
		chpid=$(dmesg | grep $(grep ^interface /etc/hostapd.conf | sed 's/interface=//g') | head -1 | sed 's/\[.*\]//g' | awk '{print $1}')
		disclaimer=$disclaimer$"\n\Z0Hotspot SSID: \Z1$HOSTAPDINFO\Zn Band:";
		if [[ `grep ^hw_mode=a /etc/hostapd.conf` ]]; then local band="5Ghz"; else band="2.4Ghz"; fi
		if [[ `grep ^ieee80211n /etc/hostapd.conf` ]]; then local type="N"; fi
		if [[ `grep ^ieee80211ac /etc/hostapd.conf` ]]; then local type="AC"; fi
		disclaimer=$disclaimer$" \Z1${band} ${type}\Z0"
		[[ ! "$chpid" =~ .*IPv6.* ]] && disclaimer=$disclaimer$"\n\nChip: \Z1${chpid}\Z0";
		if [ "$HOSTAPDCLIENTS" -gt "0" ]; then disclaimer=$disclaimer$" Connected clients: \Z1$HOSTAPDCLIENTS\Zn"; fi
		if [[ ! "$chpid" =~ .*IPv6.* ]]; then LISTLENGTH=$((LISTLENGTH+2)); fi
		disclaimer=$disclaimer$"\n";
	fi
	disclaimer=$disclaimer"\n\Z1Note\Zn: This tool can be successful only when drivers are configured properly. If auto-detection fails, you are on your own.\n "

if [[ -z $selection ]]; then
	exec 3>&1
	selection=$(dialog --backtitle "$BACKTITLE" --colors --title " Wired, Wireless, Bluetooth, Hotspot " --clear \
	--cancel-label "Back" --menu "${disclaimer}" $LISTLENGTH 70 $BOXLENGTH \
	"${LIST[@]}" 2>&1 1>&3)
	exit_status=$?
	exec 3>&-
	[[ $exit_status == $DIALOG_CANCEL || $exit_status == $DIALOG_ESC ]] && clear && break
fi
	# run main function
	jobs "$selection"

done
}




#
# personal
#
function submenu_personal ()
{
unset main
while true; do

	LIST=()
	LIST+=( "Timezone" "Change timezone \Z5($(LC_ALL=C timedatectl | grep zone | awk '{$1=$1;print}' | sed "s/Time zone: //"))\Z0" )
	LIST+=( "Locales" "Reconfigure language \Z5($(locale | grep LANGUAGE | cut -d= -f2 | cut -d_ -f1))\Z0 and character set" )
	LIST+=( "Keyboard" "Change console keyboard layout (\Z5$(cat /etc/default/keyboard | grep XKBLAYOUT | grep -o '".*"' | sed 's/"//g')\Z0)")
	LIST+=( "Hostname" "Change your hostname \Z5($(cat /etc/hostname))\Z0" )
	LIST+=( "Welcome" "Toggle welcome screen items" )

	# count number of menu items to adjust window sizee
	LISTLENGTH="$((6+${#LIST[@]}/2))"
	BOXLENGTH=${#LIST[@]}

	exec 3>&1
	selection=$(dialog --colors --backtitle "$BACKTITLE" --title "Personal settings" --clear \
	--cancel-label "Back" --menu "$disclaimer" $LISTLENGTH 70 $BOXLENGTH \
	"${LIST[@]}" 2>&1 1>&3)
	exit_status=$?
	exec 3>&-
	[[ $exit_status == $DIALOG_CANCEL || $exit_status == $DIALOG_ESC ]] && clear && break

	# run main function
	jobs "$selection"

done
}




#
# software
#
function submenu_software ()
{
unset main
while true; do

	# detect desktop
	check_desktop

	LIST=()
	[[ -f /usr/bin/softy || -f softy ]] && LIST+=( "Softy" "3rd party applications installer" )
	[[ -f /usr/bin/h3consumption && "$LINUXFAMILY" = "sun8i" && "$BRANCH" = "default" ]] && \
	LIST+=( "Consumption" "Control board consumption" )
	[[ -f /usr/bin/orangepimonitor ]] && LIST+=( "Monitor" "Simple CLI board monitoring" )
	LIST+=( "Benchmarking" "sbc-bench board benchmarking" )
	[[ -f /usr/bin/orangepimonitor ]] && LIST+=( "Diagnostics" "Send diagnostics" )

	#if [[ -n $(dpkg -l | grep linux-headers) ]]; then LIST+=( "Headers_remove" "kernel headers" ); else \
	#LIST+=( "Headers_install" "kernel headers" ); fi

	SOURCE_PKG_LIST=$(apt-cache --names-only search ^linux-source-* | awk '{ print $1 }' | grep -w ${BRANCH}-${LINUXFAMILY} | tail -n 5)
	if [[ -n $SOURCE_PKG_LIST ]]; then
		:
		#LIST+=( "Source_install" "kernel source" )
	fi
	SOURCE_PKG_LIST_INSTALLED=$(find /usr/src -maxdepth 1 -name "linux-source*")
	if [[ -n $SOURCE_PKG_LIST_INSTALLED ]]; then
		:
		#LIST+=( "Source_remove" "kernel source" );
	fi

	if [[ -f /etc/orangepi.txt ]]; then
		if [[ -n $(dpkg -l | grep -w "orangepi-firmware-full ") ]]; then
			:
			#LIST+=( "Mini" "Install mini firmware package" );
		else
			:
			#LIST+=( "Full" "Install full firmware package" );
		fi
	fi

	if [[ -n $DISPLAY_MANAGER ]]; then
		if [[ $(service xrdp status 2> /dev/null | grep -w active) ]]; then
			LIST+=( "RDP" "Disable remote desktop access from Windows" )
		else
			LIST+=( "RDP" "Enable remote desktop access from Windows" )
		fi
		if ! check_if_installed thunderbird ; then
			LIST+=( "Thunderbird" "Install full featured email client" )
		else
			LIST+=( "Thunderbird" "Remove full featured email client" )
		fi
		if ! check_if_installed gimp ; then
			LIST+=( "Gimp" "Install photo manipulation tool" )
        else
			LIST+=( "Gimp" "Remove photo manipulation tool" )
		fi
		if ! check_if_installed libreoffice-writer ; then
			LIST+=( "Writer" "Libreoffice: Writer only" )
			LIST+=( "Suite" "Libreoffice: Full suite" )
		else
			if ! check_if_installed libreoffice-base ; then
				LIST+=( "Suite" "Libreoffice: Full suite" )
				LIST+=( "Libre" "Remove Libre office packages" )
			else
				LIST+=( "Libre" "Remove Libre office packages" )
			fi
		fi
	fi

	# count number of menu items to adjust window sizee
	LISTLENGTH="$((6+${#LIST[@]}/2))"
	BOXLENGTH=${#LIST[@]}

	if [[ -z $selection ]]; then
		exec 3>&1
		selection=$(dialog --backtitle "$BACKTITLE" --title "System and 3rd party software" --clear \
		--cancel-label "Back" --menu "$disclaimer" $LISTLENGTH 70 $BOXLENGTH \
		"${LIST[@]}" 2>&1 1>&3)
		exit_status=$?
		exec 3>&-
		[[ $exit_status == $DIALOG_CANCEL || $exit_status == $DIALOG_ESC ]] && clear && break
	fi

	# run main function
	jobs "$selection"

done
}
