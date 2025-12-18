# TOTALLY WIP.

## Debian 13 Trixie Build script for OrangePi6Plus

Custom script for consolidating package contents for Debian Trixie builds.  
It works, but sort of. No guarantees. This is an extremely irresponsible release.  

In other words, **this modded script is merely a stopgap until it is released,**  
**or until Armbian or other third-party images become available.**

## Issues

**You should not install it yet unless you are brave.**

There are others, but the most notable ones are...

- ✅️A fairly turnkey "boot to desktop" as Official Image.  
- ✅️GPU/Audio and misc drivers are gradually being supported.  
- ✅️Discontinuation of packages that do not move or restrict users.  
- ✅️Can be build with custom kernel(but **DO NOT change suffix**)  
- ❌️Still WIP.  
- ❌️Detailed operational testing.  
- ❌️not tested for NPU,GPIO,DSP as HW fuctions.  
- ❔️Should I place the prebuilt image somewhere? 

I intend to devise a clever solution for the drivers when I find the time.  

---

# Usage

```
git clone -b trixie-test https://github.com/crackerjacques/orangepi-build.git opi6_build_trixie
cd opi6_build_trixie
sudo ./build.sh GITEE_SERVER=yes

# It is essentially exclusive to the OrangePi6Plus. Other single-board computers cannot be selected.
# !!DO NOT EDIT KERNEL SUFFIXES, It cannot boot unless the kernel name "6.6.8.9-cix"!!
```

## Docker
```
#it's not tested at 20251215
# in the opi6_build dir
chmod +x docker-run.sh
./run-docker.sh
```

# Network Setup

When Trixie launched,often network was dead.
```

sudo ip link # get your nic name
sudo ip link set [Your_NIC] up

# new setup resolv.conf
sudo rm /etc/resolv.conf
echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf # or your provider's
sudo systemctl enable --now NetworkManager
sudo reboot

# test
ping -c 3 google.com
sudo apt install nano
sudo apt update

# if apt update caught error with update
# set clock to now
sudo date -s "year-month-day hour:min:sec" # exam, "2025-12-18 16:30:00"
sudo timedatectl set-ntp true #if available

# or 
sudo apt install -y chrony
sudo systemctl enable --now chrony

```

## Install Driver

The debs directory contains a fair number of items that break dependencies.  
Note that in version 20251219,  
these have been fixed and are built with the driver installed from the outset.  

```
# transfer files from opi6_build/external/cache/sources/component_cix_next/debs
# to OrangePi6Plus with scp,smb,sftp or physical media

scp -O -r external/cache/sources/component_cix_next/debs orangepi6plus.local:~/

# or plan B in OrangePi6plus, download directly.
wget https://github.com/orangepi-xunlong/${comp_name}/archive/refs/heads/main.zip
unzip main.zip

# In OrangePi6Plus

# DO NOT INSTALL!!
# cix-npu-onnxruntime_1.1.0_arm64.deb
# cix-noe-umd

cd ~/debs
sudo apt update

# Drivers

sudo dpkg -i cix-audio-dsp_1.0.0_arm64.deb cix-isp-umd_1.0.0_arm64_orangepi.deb \
cix-common-misc_1.0.0_arm64.deb cix-libdrm_1.0.0_arm64.deb cix-cpipe_1.0.0_arm64.deb \
cix-libglvnd_1.7.0_arm64.deb cix-debian-misc_1.0.0_arm64.deb cix-env_1.0.0_arm64.deb \
cix-mesa_24.0.4_arm64.deb cix-firmware_1.0.0_arm64.deb cix-mnn_1.2.1_arm64.deb \
cix-gpu-dkms_1.0.0_arm64.deb cix-gpu-test_1.0.0_arm64.deb cix-gpu-umd_2.0.0_arm64.deb \
cix-optee_1.0.0_arm64.deb cix-grubcfg_1.0.0_arm64.deb cix-tools_1.0.0_arm64.deb \
cix-gstreamer_1.22.1_arm64.deb cix-vpu-test_1.0.0_arm64.deb

sudo apt --fix-broken install
reboot
```

# Appendix: Build kernel and install boot image.
A bit tricky.  
Please read:  
https://github.com/crackerjacques/orangepi-build/blob/orangepi6plus/bootloader.md


## Donation

For those like me who love splashing out on AliExpress

[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-ffdd00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/jaxxworkshv) 



# Original Article 
---

## Supported boards

Soc | Boards |
|:--|:--|
| Allwinner H6 | Orange Pi 3/3 LTS |
| Allwinner H616 | Orange Pi Zero2/Zero2w/Zero3 | 
| Rockchip RK3399 | Orange Pi 4/4B/4 LTS/800 |
| Rockchip RK3566 | Orange Pi 3B/CM4 |
| Rockchip RK3588S | Orange Pi 5/5B |
| Rockchip RK3588 | Orange Pi 5Plus |

## Download links

- 中文链接：     http://www.orangepi.cn
- English link：http://www.orangepi.org
