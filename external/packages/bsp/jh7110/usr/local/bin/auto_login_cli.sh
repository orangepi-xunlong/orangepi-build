#!/bin/bash

if [[ -z $1 ]]; then
	user=root
else
	user=$1
fi

[[ -d /etc/systemd/system/getty.target.wants/ ]] && rm /etc/systemd/system/getty.target.wants/ -rf

if [[ $1 == "-d" ]]; then
	exit
fi

mkdir -p /etc/systemd/system/getty.target.wants/
cat <<-EOF >  \
/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service
[Service]
ExecStartPre=/bin/sh -c 'exec /bin/sleep 10'
ExecStart=
ExecStart=-/sbin/agetty --noissue --autologin ${user} %I \$TERM
Type=idle
EOF
