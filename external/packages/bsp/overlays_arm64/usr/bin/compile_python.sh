#!/bin/bash

sudo apt-get update
sudo apt-get install -y build-essential zlib1g-dev  \
libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libsqlite3-dev  \
libreadline-dev libffi-dev curl libbz2-dev

version=3.9.9
[[ -n $1 ]] && version=$1

# optimize build time with 100% CPU usage
CPUS=$(grep -c 'processor' /proc/cpuinfo)
CTHREADS="-j$((CPUS + CPUS/2))"

wget https://cdn.npmmirror.com/binaries/python/$version/Python-${version}.tgz
tar xvf Python-${version}.tgz
cd Python-${version}
./configure --enable-optimizations
make ${CTHREADS}
sudo make altinstall
