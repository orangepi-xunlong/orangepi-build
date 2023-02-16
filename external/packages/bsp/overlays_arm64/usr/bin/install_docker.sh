#!/bin/bash

distributor_id=$(lsb_release -is)
distributor_id=${distributor_id,}

sudo apt-get remove -y docker docker-engine docker-ce docker.io 
sudo apt-get update
sudo apt-get install -y apt-transport-https ca-certificates curl gnupg2 software-properties-common

curl -fsSL https://repo.huaweicloud.com/docker-ce/linux/${distributor_id}/gpg | sudo apt-key add -
echo "deb [arch=$(dpkg --print-architecture)] https://repo.huaweicloud.com/docker-ce/linux/${distributor_id} $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list

sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io

sudo groupadd docker
sudo usermod -aG docker $USER
