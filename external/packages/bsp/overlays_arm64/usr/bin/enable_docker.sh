#!/bin/bash

if [[ $(uname -r) == 5.4.* ]]; then
	echo 1 | sudo update-alternatives --config iptables > /dev/null
fi

sudo systemctl enable docker.service
sudo systemctl start docker.service
