#!/bin/bash
#
# Copyright (c) 2017 Igor Pecovnik, igor.pecovnik@gma**.com
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.

# Functions:
# initialize_lte
# function lte
# check_hostapd
# check_advanced_modes
# create_if_config
# reload-nety
# check_port
# check_ht_capab
# check_vht_capab
# check_channels
# netmask_to_cidr
# nm_ip_editor
# systemd_ip_editor
# ip_editor
# wlan_edit_basic
# wlan_edit
# wlan_exceptions
# check_and_warn
# get_wlan_interface
# select_default_interface
# connect_bt_interface



#
# 3G/4G general stuff
#
function initialize_lte ()
{
# This file defines udev rules for some 3G/UMTS/LTE modems.  It
# addresses the issue that the ttyUSB devices are numbered randomly, and
# their numbering can vary between server reboots.  These rules create
# persistent symlinks which can be reliably used in WWAN interface
# startup scripts.

# These rules assume that there is only one WWAN modem in the system. In
# order to address multiple WWAN cards, the rules need to be more
# specific and associate with serial numbers of the modems.

# Copyright (c) 2016 Stanislav Sinyagin <ssinyagin@k-open.com>.

# This content is published under Creative Commons Attribution 4.0
# International (CC BY 4.0) license.
# Source repository: https://github.com/ssinyagin/wwan_udev_rules

if ! check_if_installed ppp then; then
	apt-get -y -qq install ppp >/dev/null 2>&1
fi
cat >/etc/udev/rules.d/99-wwan.rules <<'EOT'


# Source repository: https://github.com/ssinyagin/wwan_udev_rules

# Huawei ME909s-120 LTE modem
SUBSYSTEM=="tty", ATTRS{idVendor}=="12d1", ATTRS{idProduct}=="15c1", SYMLINK+="ttyWWAN%E{ID_USB_INTERFACE_NUM}"
SUBSYSTEM=="net", ATTRS{idVendor}=="12d1", ATTRS{idProduct}=="15c1", NAME="lte0"

# Huawei MU709s-2 UMTS modem
SUBSYSTEM=="tty", ATTRS{idVendor}=="12d1", ATTRS{idProduct}=="1c25", SYMLINK+="ttyWWAN%E{ID_USB_INTERFACE_NUM}"
SUBSYSTEM=="net", ATTRS{idVendor}=="12d1", ATTRS{idProduct}=="1c25", NAME="umts0"

# Qualcomm Gobi2000
SUBSYSTEM=="tty", ATTRS{idVendor}=="03f0", ATTRS{idProduct}=="251d", SYMLINK+="ttyWWAN%E{ID_USB_INTERFACE_NUM}"

# Sierra Wireless MC8775
SUBSYSTEM=="tty", ATTRS{idVendor}=="03f0", ATTRS{idProduct}=="1e1d", SYMLINK+="ttyWWAN%E{.ID_PORT}"

# Novatel Wireless, Inc. Expedite E371
SUBSYSTEM=="tty", ATTRS{idVendor}=="413c", ATTRS{idProduct}=="819b", SYMLINK+="ttyWWAN%E{ID_USB_INTERFACE_NUM}"

# SIMCom SIM7100
SUBSYSTEM=="tty", ATTRS{idVendor}=="1e0e", ATTRS{idProduct}=="9001", SYMLINK+="ttyWWAN%E{ID_USB_INTERFACE_NUM}"
SUBSYSTEM=="net", ATTRS{idVendor}=="1e0e", ATTRS{idProduct}=="9001", NAME="lte0"
EOT

# reload udev rules
udevadm control --reload-rules && udevadm trigger
}




#
# 3G/4G init scripts add + remove
#
function lte ()
{
case $1 in
	"12d1:15c1" )
	# Huawei ME909s-120 LTE modem
	LTE_MODEM_ID=$1
	if [[ $2 == on && $(cat /sys/class/net/lte0/operstate) == "up" ]]; then
		show_box  "Warning" "Huawei ME909s-120 LTE modem is up and running" 7
	elif [[ $2 == on ]]; then
		show_box  "Info" "Huawei ME909s-120 LTE modem will try to connect" 7
cat >/etc/chatscripts/sunrise.HUAWEI <<'EOT'
ABORT BUSY
ABORT 'NO CARRIER'
ABORT ERROR
TIMEOUT 10
'' ATZ
OK 'AT+CFUN=1'
OK 'AT+CMEE=1'
OK 'AT\^NDISDUP=1,1,"internet"'
OK
EOT
cat >/etc/chatscripts/gsm_off.HUAWEI <<'EOT'
ABORT ERROR
TIMEOUT 5
'' AT+CFUN=0 OK
EOT

cat >/etc/network/interfaces.d/lte0 <<'EOT'
allow-hotplug lte0
iface lte0 inet dhcp
    pre-up /usr/sbin/chat -v -f /etc/chatscripts/sunrise.HUAWEI >/dev/ttyWWAN02 </dev/ttyWWAN02
    post-down /usr/sbin/chat -v -f /etc/chatscripts/gsm_off.HUAWEI >/dev/ttyWWAN02 </dev/ttyWWAN02
EOT

		# disable interface
		ifup lte0 2> /dev/null

	elif [[ $2 == off ]]; then
		show_box  "Warning" "Huawei ME909s-120 LTE will kill connection" 7
		# enable interface
		ifdown lte0 2> /dev/null
		rm /etc/chatscripts/sunrise.HUAWEI
		rm /etc/chatscripts/gsm_off.HUAWEI
		rm /etc/network/interfaces.d/lte0
	elif [[ $(cat /sys/class/net/lte0/operstate) == "up" ]]; then
		LTE_MODEM="Huawei ME909s-120 LTE modem is online"
	else
		LTE_MODEM="Huawei ME909s-120 LTE modem is offline"
	fi
	;;
esac
}




#
# check hostapd configuration. Return error or empty if o.k.
#
check_hostapd ()
{

	systemctl daemon-reload
	hostapd_error=""
	[[ -n $1 && -n $2 ]] && dialog --title " $1 " --backtitle "$BACKTITLE" --no-collapse --colors --infobox "$2" 5 $((${#2}-3))
	service hostapd stop
	rm -f /var/run/hostapd/*
	sleep 1
	service hostapd start
	sleep 6
	if [[ "$(hostapd_cli ping 2> /dev/null| grep PONG)" == "PONG" ]]; then
			hostapd_error=""
		else
			hostapd_error=$(hostapd /etc/hostapd.conf)
			sleep 6
			[[ -n $(echo $hostapd_error | grep "channel") ]] && hostapd_error="channel_restriction"
			[[ -n $(echo $hostapd_error | grep "does not support" | grep "hw_mode") ]] && hostapd_error="hw_mode"
			[[ -n $(echo $hostapd_error | grep "not found from the channel list" | grep "802.11g") ]] && hostapd_error="wrong_channel"
			[[ -n $(echo $hostapd_error | grep "VHT") ]] && hostapd_error="unsupported_vht"
			[[ -n $(echo $hostapd_error | grep " HT capability") ]] && hostapd_error="unsupported_ht"
	fi

}




#
# check all possible wireless modes
#
function check_advanced_modes ()
{

	sed '/### IEEE 802.11n/,/^### IEEE 802.11n/ s/^# *//' -i /etc/hostapd.conf
	check_hostapd "Probing" "\n802.11n \Z1(150-300Mbps)\Z0"
	# check HT capability
	check_ht_capab
	if [[ -z "$hostapd_error" ]]; then
		sed '/### IEEE 802.11a\>/,/^### IEEE 802.11a\>/ s/^# *//' -i /etc/hostapd.conf
		sed -i "s/^channel=.*/channel=40/" /etc/hostapd.conf
		check_hostapd "Probing" "\n802.11a \Z1(5Ghz)\Z0"
		if [[ "$hostapd_error" == "channel_restriction" ]]; then check_channels; fi
		if [[ "$hostapd_error" == "channel_restriction" ]]; then
			# revering configuration
			sed -i "s/^channel=.*/channel=5/" /etc/hostapd.conf
			sed '/## IEEE 802.11a\>/,/^## IEEE 802.11a\>/ s/.*/#&/' -i /etc/hostapd.conf
			check_hostapd "Reverting" "\nWireless \Z1802.11a (5Ghz)\Z0 is not supported"
		else
			sed '/### IEEE 802.11ac\>/,/^### IEEE 802.11ac\>/ s/^# *//' -i /etc/hostapd.conf
			# check VHT capability
			check_vht_capab
			if [[ "$hostapd_error" == "unsupported_vht" || "$hostapd_error" == "channel_restriction" ]]; then
				# revering configuration
				sed '/## IEEE 802.11ac\>/,/^## IEEE 802.11ac\>/ s/.*/#&/' -i /etc/hostapd.conf
				check_hostapd "Reverting" "\nWireless 802.11ac \Z1(433Mbps x n @ 5Ghz)\Z0 is not supported"
				if [[ "$hostapd_error" == "channel_restriction" ]]; then check_channels; fi
			fi
		fi
	else
		sed '/## IEEE 802.11n/,/^## IEEE 802.11n/ s/.*/#&/' -i /etc/hostapd.conf
	fi

}




#
# create interface configuration section
#
function create_if_config() {

	address=$(ip -4 addr show dev $1 | awk '/inet/ {print $2}' | cut -d'/' -f1)
	netmask=$(ip -4 addr show dev $1 | awk '/inet/ {print $2}' | cut -d'/' -f2)
	gateway=$(route -n | grep 'UG[ \t]' | awk '{print $2}' | sed -n '1p')
	echo -e "# orangepi-config created"
	echo -e "source /etc/network/interfaces.d/*\n"
	if [[ "$3" == "fixed" ]]; then
		echo -e "# Local loopback\nauto lo\niface lo init loopback\n"
		echo -e "# Interface $2\nauto $2\nallow-hotplug $2"
		echo -e "iface $2 inet static\n\taddress $address\n\tnetmask $netmask\n\tgateway $gateway\n\tdns-nameservers 8.8.8.8"
	fi

}




#
# reload network related services
#
function reload-nety() {

	systemctl daemon-reload
	if [[ "$1" == "reload" ]]; then WHATODO="Reloading services"; else WHATODO="Stopping services"; fi
	(service network-manager stop >/dev/null 2>&1; service NetworkManager stop >/dev/null 2>&1; echo 10; sleep 1; service hostapd stop; echo 20; sleep 1; service dnsmasq stop; echo 30; sleep 1;\
	[[ "$1" == "reload" ]] && service dnsmasq start && echo 60 && sleep 1 && service hostapd start && echo 80 && sleep 1;\
	service network-manager start >/dev/null 2>&1; service NetworkManager start >/dev/null 2>&1; echo 90; sleep 5;) | dialog --backtitle "$BACKTITLE" --title " $WHATODO " --gauge "" 6 70 0
	systemctl restart systemd-resolved.service

}




#
# Check if something is running on port $1 and display info
#
function check_port ()
{
	[[ -n $(netstat -lnt | awk '$6 == "LISTEN" && $4 ~ ".'$1'"') ]] && dialog --backtitle "$BACKTITLE" --title "Checking service" \
	--msgbox "\nIt looks good.\n\nThere is $2 service on port $1" 9 52
}




#
# check wifi high throughput
#
function check_ht_capab ()
{
	declare -a arr=("[HT40+][LDPC][SHORT-GI-20][SHORT-GI-40][TX-STBC][RX-STBC1][DSSS_CCK-40][SMPS-STATIC]" \
	"[HT40-][SHORT-GI-40][SHORT-GI-40][DSSS_CCK-40]" "[SHORT-GI-20][SHORT-GI-40][HT40+]" "[DSSS_CK-40][HT20+]" "")
	local j=0
	for i in "${arr[@]}"
	do
		j=$((j+(100/${#arr[@]})))
		echo $j | dialog --title " Probing HT " --colors --gauge "\nSeeking for optimal \Z1high throughput\Z0 settings." 8 50 0
		sed -i "s/^ht_capab=.*/ht_capab=$i/" /etc/hostapd.conf
		check_hostapd
		if [[ "$hostapd_error" == "channel_restriction" ]]; then check_channels; fi
		if [[ "$hostapd_error" == "" ]]; then break; fi
	done
}




#
# check wifi high throughput
#
function check_vht_capab ()
{

	declare -a arr=("[MAX-MPDU-11454][SHORT-GI-80][TX-STBC-2BY1][RX-STBC-1][MAX-A-MPDU-LEN-EXP3]" "[MAX-MPDU-11454][SHORT-GI-80][RX-STBC-1][MAX-A-MPDU-LEN-EXP3]" "")
	local j=0
	for i in "${arr[@]}"
	do
		j=$((j+(100/${#arr[@]})))
		echo $j | dialog --title " Probing VHT " --colors --gauge "\nSeeking for optimal \Z1very high throughput\Z0 settings." 8 54 0
		sed -i "s/^vht_capab=.*/vht_capab=$i/" /etc/hostapd.conf
		check_hostapd
		if [[ "$hostapd_error" == "channel_restriction" ]]; then check_channels; fi
		if [[ "$hostapd_error" == "" ]]; then break; fi
	done

}




#
# check 5Ghz channels
#
function check_channels ()
{

	declare -a arr=("36" "40")
	for i in "${arr[@]}"
	do
		sed -i "s/^channel=.*/channel=$i/" /etc/hostapd.conf
		check_hostapd "Probing" "\nChannel:\Z1 ${i}\Z0"
		if [[ "$hostapd_error" != "channel_restriction" ]]; then break; fi
	done

}


#
# convert netmask to CIDR
#
function netmask_to_cidr ()
{
	IFS=' '
	local bits=0
	for octet in $(echo $1| sed 's/\./ /g'); do
		binbits=$(echo "obase=2; ibase=10; ${octet}"| bc | sed 's/0//g')
		let bits+=${#binbits}
	done
	echo "${bits}"
}


#
# edit ip address within network manager
#
function nm_ip_editor ()
{

exec 3>&1
	dialog --title " Static IP configuration" --backtitle "$BACKTITLE" --form "\nAdapter: $1
	\n " 12 38 0 \
	"Address:"				1 1 "$address"				1 15 15 0 \
	"Netmask:"			2 1 "$netmask"	2 15 15 0 \
	"Gateway:"			3 1 "$gateway"			3 15 15 0 \
	2>&1 1>&3 | {
		read -r address;read -r netmask;read -r gateway
		if [[ $? = 0 ]]; then
			localuuid=$(LC_ALL=C nmcli -f UUID,DEVICE connection show | grep $1 | awk '{print $1}')
			# convert netmask value to CIDR if required
			if [[ $netmask =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
				CIDR=$(netmask_to_cidr ${netmask})
			else
				CIDR=${netmask}
			fi
			if [[ -n "$localuuid" ]]; then
				# adjust existing
				nmcli con mod $localuuid ipv4.method manual ipv4.addresses "$address/$CIDR" >/dev/null 2>&1
				nmcli con mod $localuuid ipv4.method manual ipv4.gateway  "$gateway" >/dev/null 2>&1
				nmcli con mod $localuuid ipv4.dns "8.8.8.8,$gateway" >/dev/null 2>&1
				nmcli con down $localuuid >/dev/null 2>&1
				sleep 2
				nmcli con up $localuuid >/dev/null 2>&1
			else
				# create new
				nmcli con add con-name "orangepi" ifname "$1" type 802-3-ethernet ip4 "$address/$CIDR" gw4 "$gateway" >/dev/null 2>&1
				nmcli con mod "orangepi" ipv4.dns "8.8.8.8,$gateway" >/dev/null 2>&1
				nmcli con up "orangepi" >/dev/null 2>&1
			fi
		fi
		}
}




#
# edit ip address
#
function systemd_ip_editor ()
{

	local filename="/etc/systemd/network/10-$1.network"
	if [[ -f $filename ]]; then
	sed -i '/Network/,$d' $filename
	exec 3>&1
		dialog --title " Static IP configuration" --backtitle "$BACKTITLE" --form "\nAdapter: $1
		\n " 12 38 0 \
		"Address:"				1 1 "$address"				1 15 15 0 \
		"Netmask:"			2 1 "$netmask"	2 15 15 0 \
		"Gateway:"			3 1 "$gateway"			3 15 15 0 \
		2>&1 1>&3 | {
			read -r address;read -r netmask;read -r gateway
			if [[ $? = 0 ]]; then
				echo -e "[Network]" >>$filename
				echo -e "Address=$address" >> $filename
				echo -e "Gateway=$gateway" >> $filename
				echo -e "DNS=8.8.8.8" >> $filename
			fi
			}
	fi

}




#
# edit ip address
#
function ip_editor ()
{

	exec 3>&1
	dialog --title " Static IP configuration" --backtitle "$BACKTITLE" --form "\nAdapter: $1
	\n " 12 38 0 \
	"Address:"				1 1 "$address"				1 15 15 0 \
	"Netmask:"			2 1 "$netmask"	2 15 15 0 \
	"Gateway:"			3 1 "$gateway"			3 15 15 0 \
	2>&1 1>&3 | {
		read -r address;read -r netmask;read -r gateway
		if [[ $? = 0 ]]; then
			echo -e "# orangepi-config created\nsource /etc/network/interfaces.d/*\n" >$3
			echo -e "# Local loopback\nauto lo\niface lo inet loopback\n" >> $3
			echo -e "# Interface $2\nauto $2\nallow-hotplug $2\niface $2 inet static\
			\n\taddress $address\n\tnetmask $netmask\n\tgateway $gateway\n\tdns-nameservers 8.8.8.8" >> $3
		fi
		}

}




#
# edit hostapd parameters
#
function wlan_edit_basic ()
{
	source /etc/hostapd.conf
	exec 3>&1
	dialog --title "AP configuration" --backtitle "$BACKTITLE" --form "\nWPA2 enabled, \
	advanced config: edit /etc/hostapd.conf\n " 12 58 0 \
	"SSID:"				1 1 "$ssid"				1 31 22 0 \
	"Password:"			2 1 "$wpa_passphrase"	2 31 22 0 \
	"Channel:"			3 1 "$channel"			3 31 3 0 \
	2>&1 1>&3 | {
		read -r ssid;read -r wpa_passphrase;read -r channel
		if [[ $? = 0 ]]; then
			sed -i "s/^ssid=.*/ssid=$ssid/" /etc/hostapd.conf
			sed -i "s/^wpa_passphrase=.*/wpa_passphrase=$wpa_passphrase/" /etc/hostapd.conf
			sed -i "s/^channel=.*/channel=$channel/" /etc/hostapd.conf
			wpa_psk=$(wpa_passphrase $ssid $wpa_passphrase | grep '^[[:blank:]]*[^[:blank:]#;]' | grep psk | cut -d= -f2-)
			sed -i "s/^wpa_psk=.*/wpa_psk=$wpa_psk/" /etc/hostapd.conf
		fi
		}
}




#
# edit hostapd parameters
#
function wlan_edit ()
{

	# select default interfaces if there is more than one
	select_interface "default"
	dialog --title " Configuration edit " --colors --backtitle "$BACKTITLE" --help-button --help-label "Cancel" --yes-label "Basic" \
	--no-label "Advanced" --yesno "\n\Z1Basic:\Z0    Change SSID, password and channel\n\n\Z1Advanced:\Z0 Edit /etc/hostapd.conf file" 9 70
	if [[ $? = 0 ]]; then
		wlan_edit_basic
	elif [[ $? = 1 ]]; then
		dialog --backtitle "$BACKTITLE" --title " Edit hostapd configuration /etc/hostapd.conf" --no-collapse \
		--ok-label "Save" --editbox /etc/hostapd.conf 30 0 2> /etc/hostapd.conf.out
		[[ $? = 0 ]] && mv /etc/hostapd.conf.out /etc/hostapd.conf && service hostapd restart
	fi

}




#
# here we add wifi exceptions
#
function wlan_exceptions ()
{

	reboot_module=false
	[[ -n "$(lsmod | grep -w dhd)" && $1 = "on" ]] && \
	echo 'options dhd op_mode=2' >/etc/modprobe.d/ap6212.conf && rmmod dhd && modprobe dhd
	[[ -n "$(lsmod | grep -w dhd)" && $1 = "off" ]] && \
	rm /etc/modprobe.d/ap6212.conf && rmmod dhd && modprobe dhd
	# Cubietruck
	[[ -n "$(lsmod | grep -w ap6210)" && $1 = "on" ]] && \
	echo 'options ap6210 op_mode=2' >/etc/modprobe.d/ap6210.conf && reboot_module=true
	[[ -n "$(lsmod | grep -w ap6210)" && $1 = "off" ]] && \
	rm /etc/modprobe.d/ap6210.conf && reboot_module=true

}




#
# here add shaddy wifi adaptors
#
function check_and_warn ()
{

	local shaddy=false
	# blacklist
	[[ "$LINUXFAMILY" == "sun8i" && $BOARD == "orangepizero" ]] && shaddy=true
	[[ -n "$(lsmod | grep mt7601u)" ]] && shaddy=true
	[[ -n "$(lsmod | grep r8188eu)" ]] && shaddy=true
	# blacklist
	if [[ "$shaddy" == "true" ]]; then
	dialog --title " Warning " --ok-label " Accept and proceed " --msgbox '\nOne of your wireless drivers are on a black list due to poor quality.\n\nAP mode might not be possible!' 9 73
	fi

}




#
# search for wlan interfaces and provide a selection menu if there are more than one
#
function get_wlan_interface ()
{

	IFS=$'\r\n'
	GLOBIGNORE='*'

	WLAN_INTERFACES=($(LC_ALL=C nmcli --wait 10 dev status | grep wifi | grep disconnected |awk '{print $1}'))
	local LIST=()
	for i in "${WLAN_INTERFACES[@]}"
	do
		LIST+=( "${i[0]//[[:blank:]]/}" "" )
	done
	LIST_LENGTH=$((${#LIST[@]}/2));

	if [ "$LIST_LENGTH" -eq 1 ]; then
		WIRELESS_ADAPTER=${WLAN_INTERFACES[0]}
	else
		exec 3>&1
		WIRELESS_ADAPTER=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
		--title "Select wlan interface" --clear --menu "" $((6+${LIST_LENGTH})) 40 15 "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
	fi

}




function select_interface ()
{
	IFS=$'\r\n'
	GLOBIGNORE='*'
	local ADAPTER=($(nmcli device status | grep ethernet | awk '{ print $1 }' | grep -v lo))
	local LIST=()
	for i in "${ADAPTER[@]}"
	do
		local IPADDR=$(LC_ALL=C ip -4 addr show dev ${i[0]} | awk '/inet/ {print $2}' | cut -d'/' -f1)
		ADD_SPEED=""
		[[ $i == eth* || $i == en* ]] && ADD_SPEED=$(ethtool $i | grep Speed)
		[[ $i == wl* ]] && ADD_SPEED=$(LC_ALL=C nmcli -f RATE,DEVICE,ACTIVE dev wifi list | grep $i | grep yes | awk 'NF==4{print $1""$2}NF==1' | sed -e 's/^/  Speed: /' | tail -1)
		LIST+=( "${i[0]//[[:blank:]]/}" "${IPADDR} ${ADD_SPEED}" )
	done
	LIST_LENGTH=$((${#LIST[@]}/2));
	if [ "$LIST_LENGTH" -eq 0 ]; then
		SELECTED_ADAPTER="lo"
	elif [ "$LIST_LENGTH" -eq 1 ]; then
		SELECTED_ADAPTER=${ADAPTER[0]}
	else
	exec 3>&1
	SELECTED_ADAPTER=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse --title "Select $1 interface" --clear \
	--menu "" $((6+${LIST_LENGTH})) 74 14 "${LIST[@]}" 2>&1 1>&3)
	exec 3>&-
	fi

}




#
# select interface if there is more than one and adjust metric
#
# $1 = default | all
#
function select_default_interface ()
{

	ALREADY_DEFINED=$(cat /etc/iptables.ipv4.nat 2> /dev/null | grep "POSTROUTING -o" | tail -1 | awk '{ print $4 }')
	if [[ -n "${ALREADY_DEFINED}" ]]; then
		DEFAULT_ADAPTER=${ALREADY_DEFINED}
	else
		IFS=$'\r\n'
		GLOBIGNORE='*'
		if [[ $1 == "default" ]]; then
			local ADAPTER=($(nmcli -t -f DEVICE connection show --active))
		else
			local ADAPTER=($(nmcli device status | tail -n +2 | awk '{ print $1 }' | grep -v lo))
		fi
		local LIST=()
		for i in "${ADAPTER[@]}"
		do
			local IPADDR=$(LC_ALL=C ip -4 addr show dev ${i[0]} | awk '/inet/ {print $2}' | cut -d'/' -f1)
			ADD_SPEED=""
			[[ $i == eth* || $i == en* ]] && ADD_SPEED=$(ethtool $i | grep Speed)
			[[ $i == wl* ]] && ADD_SPEED=$(LC_ALL=C nmcli -f RATE,DEVICE,ACTIVE dev wifi list | grep $i | grep yes | awk 'NF==4{print $1""$2}NF==1' | sed -e 's/^/  Speed: /' | tail -1)
			if [[ $1 == "default" ]]; then
				[[ $IPADDR != "172.24.1.1" && -n $IPADDR ]] && LIST+=( "${i[0]//[[:blank:]]/}" "${IPADDR} ${ADD_SPEED}" )
			else
				[[ $IPADDR != "172.24.1.1" ]] && LIST+=( "${i[0]//[[:blank:]]/}" "${IPADDR} ${ADD_SPEED}" )
			fi
		done
		LIST_LENGTH=$((${#LIST[@]}/2));
		if [ "$LIST_LENGTH" -eq 0 ]; then
			DEFAULT_ADAPTER="lo"
		elif [ "$LIST_LENGTH" -eq 1 ]; then
			DEFAULT_ADAPTER=${ADAPTER[0]}
		else
			exec 3>&1
			DEFAULT_ADAPTER=$(dialog --nocancel --backtitle "$BACKTITLE" --no-collapse \
			--title "Select default interface" --clear --menu "" $((6+${LIST_LENGTH})) 74 14 "${LIST[@]}" 2>&1 1>&3)
			exec 3>&-
		fi
	fi

	# set highest metric to default adaptor
	HIGHEST_METRIC=$(nmcli -t -f UUID,TYPE,DEVICE connection show --active | grep $DEFAULT_ADAPTER | sed 's/:.*$//')

	# set metric to 50
	nmcli connection modify $HIGHEST_METRIC ipv4.route-metric 50 2> /dev/null

	METRIC=77
	# set others wired
	REMAINING=( `nmcli -t -f UUID,TYPE,DEVICE connection show --active | grep ethernet | grep -v $DEFAULT_ADAPTER | sed 's/:.*$//'` )
	if [[ ${#REMAINING[@]} -ge 1 ]]; then
		for i in "${REMAINING[@]}"
		do
			METRIC=$(( $METRIC + 1 ))
			nmcli connection modify ${i} ipv4.route-metric $METRIC
		done
	fi

	# set other wireless
	METRIC=88
	REMAINING=( `nmcli -t -f UUID,TYPE,DEVICE connection show --active | grep wireless | grep -v $DEFAULT_ADAPTER | sed 's/:.*$//'` )
	if [[ ${#REMAINING[@]} -ge 1 ]]; then
		for i in "${REMAINING[@]}"
		do
			METRIC=$(( $METRIC + 1 ))
			nmcli connection modify ${i} ipv4.route-metric $METRIC
		done
	fi

	# create default metrics file
	cat <<-EOF > /etc/NetworkManager/conf.d/orangepi-default-metric.conf
	[connection-ethernet-gateway]
	match-device=interface-name:$DEFAULT_ADAPTER
	ipv4.route-metric=50

	[connection-wifi-other]
	match-device=type:wifi
	ipv4.route-metric=88

	[connection-ethernet-other]
	match-device=type:ethernet
	ipv4.route-metric=77
EOF
}




#
# search and connect to Bluetooth devices
#
function connect_bt_interface ()
{

	IFS=$'\r\n'
	GLOBIGNORE='*'
	dialog --backtitle "$BACKTITLE" --title "Please wait" --infobox "\nDiscovering Bluetooth devices ... " 5 37
	BT_INTERFACES=($(hcitool scan | sed '1d'))

	local LIST=()
	for i in "${BT_INTERFACES[@]}"
	do
		local a=$(echo ${i[0]//[[:blank:]]/} | sed -e 's/^\(.\{17\}\).*/\1/')
		local b=${i[0]//$a/}
		local b=$(echo $b | sed -e 's/^[ \t]*//')
		LIST+=( "$a" "$b")
	done

	LIST_LENGTH=$((${#LIST[@]}/2));
	if [ "$LIST_LENGTH" -eq 0 ]; then
		BT_ADAPTER=${WLAN_INTERFACES[0]}
		dialog --backtitle "$BACKTITLE" --title "Bluetooth" --msgbox "\nNo nearby Bluetooth devices were found!" 7 43
	else
		exec 3>&1
		BT_ADAPTER=$(dialog --backtitle "$BACKTITLE" --no-collapse --title "Select interface" \
		--clear --menu "" $((6+${LIST_LENGTH})) 50 15 "${LIST[@]}" 2>&1 1>&3)
		exec 3>&-
		if [[ $BT_ADAPTER != "" ]]; then
			dialog --backtitle "$BACKTITLE" --title "Please wait" --infobox "\nConnecting to $BT_ADAPTER " 5 35
			BT_EXEC=$(
			expect -c 'set prompt "#";set address '$BT_ADAPTER';spawn bluetoothctl;expect -re $prompt;send "disconnect $address\r";
			sleep 1;send "remove $address\r";sleep 1;expect -re $prompt;send "scan on\r";sleep 8;send "scan off\r";
			expect "Controller";send "trust $address\r";sleep 2;send "pair $address\r";sleep 2;send "connect $address\r";
			send_user "\nShould be paired now.\r";sleep 2;send "quit\r";expect eof')
			echo "$BT_EXEC" > /tmp/bt-connect-debug.log
				if [[ $(echo "$BT_EXEC" | grep "Connection successful" ) != "" ]]; then
					dialog --backtitle "$BACKTITLE" --title "Bluetooth" --msgbox "\nYour device is ready to use!" 7 32
				else
					dialog --backtitle "$BACKTITLE" --title "Bluetooth" --msgbox "\nError connecting. Try again!" 7 32
				fi
		fi
	fi

}
