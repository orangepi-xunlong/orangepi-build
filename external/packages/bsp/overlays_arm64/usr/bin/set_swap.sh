#!/bin/bash

sudo swapoff -a
sudo dd if=/dev/zero of=/swapfile bs=1G count=8
sudo chmod 0600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
sudo sed -i '/swapfile/d' /etc/fstab
sudo bash -c 'echo "/swapfile swap swap sw 0 0" >> /etc/fstab'
