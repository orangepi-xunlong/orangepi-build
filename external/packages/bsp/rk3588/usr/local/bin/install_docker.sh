#!/bin/bash

source /etc/orangepi-release

sudo apt-get remove -y docker docker-engine docker-ce docker.io 
sudo apt-get update
sudo apt-get install -y apt-transport-https ca-certificates curl gnupg lsb-release

codename=$(lsb_release -cs)
distributor_id=$(lsb_release -is)
distributor_id=${distributor_id,}

curl -fsSL http://mirrors.aliyun.com/docker-ce/linux/${distributor_id}/gpg | sudo apt-key add  -
echo "deb [arch=$(dpkg --print-architecture) \
signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] \
https://mirrors.aliyun.com/docker-ce/linux/${distributor_id} \
${codename} stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

apt-get update
apt-get install -y docker-ce docker-ce-cli containerd.io

sudo usermod -aG docker $USER
