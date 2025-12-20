## Debian 13 Trixie Build script for OrangePi6Plus

<img width="1024" height="1152" alt="22222" src="https://github.com/user-attachments/assets/4d7441db-b5a7-4fbd-bb72-8657598204c4" />


Custom script for consolidating package contents for Debian Trixie builds.  
It works, but sort of. No guarantees. This is an extremely irresponsible release.  

In other words, **this modded script is merely a stopgap until it is released,**  
**or until Armbian or other third-party images become available.**

## Issues

**You should not install it yet unless you are brave.**

There are others, but the most notable ones are...

- ✅️A fairly turnkey "boot to desktop" system as Official Image.  
- ✅️GPU/Audio and misc drivers are preinstalled.  
- ✅️Discontinuation of packages that do not move or restrict users.  
- ✅️Can be build with custom kernel(but **DO NOT CHANGE SUFFIXES**)  
- ❌️Still need tweak.  
- ❌️Still doesn't work X11,LightDM,SDDM,GDM & Wayland Only.  
- ❌️Incidentally, Console doesn't appear either.
- ❌️Detailed operational testing.  
- ❌️not tested for NPU,GPIO,DSP as HW fuctions.  
- ❔️Should I place the prebuilt image somewhere? 

I intend to devise a clever solution for the drivers when I find the time.  

---

# Usage

It will not run on anything other than Ubuntu 22.04.
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

## Install Driver

The drivers are already installed, so they are not really necessary,  
but the default packages are available for download.  
However, be aware that many of these packages may break dependencies.  

```

# Download directly.
wget https://github.com/orangepi-xunlong/component_cix-next/archive/refs/heads/main.zip
unzip main.zip

# DO NOT INSTALL FOLLOW DRIVER PACKAGES!!
# cix-npu-onnxruntime_1.1.0_arm64.deb
# cix-noe-umd

cd component_cix-next-main/debs
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

## After installation
Verify the partition size using "df -h". If necessary, 
**use "sudo apt install cloud-guest-utils" to then "growpart" and "resize2fs" to increase the free space.**    
So, If you installed to NVME SSD.  

```
sudo fdisk -l #check your system drive
sudo apt install cloud-guest-utils
ls /dev/nvme* # or /dev/sd* # find your disk.

#example: Extend the 2nd partition of /dev/nvme0n1 to maximum size.
sudo growpart /dev/nvme0n1 2

#Then, fill free space to NVME Partition 2. 
sudo resize2fs /dev/nvme0n1p2

#Please proceed according to your environment.
```

## Appendix: Build kernel and install boot image.
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
