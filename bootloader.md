# Kernel update and install boot image

As EDK2 is not currently provided for the OrangePi6, the bootloader cannot boot any image other than “6.6.89-cix”.  
Therefore, it becomes a somewhat tricky approach.  

Fortunately, the OrangePi6's processor is quite fast, so you won't have to wait long for kernel builds in the native environment. Unless you enable a large number of options, it should finish during your lunch break.  


## Build
```
# Download source code, copy setting and modify setting.

git clone -b orange-pi-6.6-cix https://gitee.com/orangepi-xunlong/orange-pi-6.6-cix.git linux-source

cd linux-source

zcat /proc/config.gz > .config  

make menuconfig # mod and save .config  

# disable suffix for kernel and make as same name to original  
 
touch .scmversion  

make LOCALVERSION="-cix" -j$(nproc) Image modules  

# Tea Time...

```

## Install
```

# Install  

sudo make modules_install   

# backup kernel image  

sudo mkdir -p /media/$USER/ESP  

sudo mount /dev/nvme0n1p1 /media/$USER/ESP  

sudo mv /media/$USER/ESP/IMAGE /media/$USER/ESP/IMAGE.bak  

sudo cp arch/arm64/boot/Image /media/$USER/ESP/IMAGE  

# abracadabra (maybe isn't necessary )

sudo sync  

# The moment of destiny, Alea jacta est.(；´Д｀)人
sudo reboot  
```

To the OrangePi 6 team,  
We would appreciate it if you could release EDK2 as soon as possible.