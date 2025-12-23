
# Build Ubuntu a.k.a kernel and bootloader injection 

Automate Cix-Tech's tutorial
https://github.com/cixtech/cix_p1_ubuntu_adaption_debs

## Usage
Host OS are Ubuntu 22.04 is needed for Kernel build.  

The script itself will run even if the host machine is running a different operating system,  
so it is advisable to copy the necessary packages from another machine or VM and run them from the OrangePi6.  

```
# from other machine
# or using samba or sftp.

scp -O -r orangepi_build/output/ orangepi@orangepi6plus.local:~/[your_work_dir]/orangepi_build
```

```
# if you built kernel package or OS image, skip this.

sudo ./build.sh # and choice kernel package build.

# then

sudo ./build_ubuntu.sh

```

# Agreements

Still not perfect to work...