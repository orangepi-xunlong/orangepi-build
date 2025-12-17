# Build script for OrangePi6Plus

The official build script is...
- Fails to boot when kernel options are altered
- Includes numerous pieces of software that are utterly useless

The official image was built using an outdated kernel configuration, with features such as hidraw disabled,   
resulting in issues like drawing tablets failing to function.  
Therefore, I have provisionally enabled kernel option configuration and removed superfluous software.  

Once distribution of EDK2 for OrangePi6Plus commences, the boot-related issues will be resolved.   
However, at present, the bootloader only supports the naming convention 6.6.89-cix,  
and it has been rewritten to prevent this from being altered.  

In other words, **this modded script is merely a stopgap until it is released,**  
**or until Armbian or other third-party images become available.**

# Usage

```
git clone -b orangepi6plus https://github.com/crackerjacques/orangepi-build.git opi6_build
cd opi6_build
sudo ./build.sh GITEE_SERVER=yes

# It is essentially exclusive to the OrangePi6Plus. Other single-board computers cannot be selected.
```

## Docker
```
#it's not tested at 20251215
# in the opi6_build dir
chmod +x docker-run.sh
./run-docker.sh

```

# After Install

```
# Add debian-security to sources.list
cd ~/Desktop
./patch_missing.sh

```

## Install Driver

```
# transfer files from opi6_build/external/cache/sources/component_cix_next/debs
# to OrangePi6Plus with scp,smb,sftp or physical media

scp -O -r external/cache/sources/component_cix_next/debs orangepi6plus.local:~/

# or plan B in OrangePi6plus, download directly.
wget https://github.com/orangepi-xunlong/${comp_name}/archive/refs/heads/main.zip
unzip main

# In OrangePi6Plus

cd ~/debs
sudo apt update
sudo dpkg -i *.deb
sudo apt install --fix-broken
reboot

```
## Donation

For those like me who love splashing out on AliExpress

https://cdn.buymeacoffee.com/uploads/profile_pictures/2025/12/aMyc5KgFzFw5NbVz.png@300w_0e.webp
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
