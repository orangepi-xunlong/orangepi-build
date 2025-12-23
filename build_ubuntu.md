
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

## Dualboot

launch dualboot_enbaler.sh, or hand writing with template.  

```
# mount current system disk partion 1

sudo mkdir -p /mnt/ESP

sudo mount /dev/xyz1 /mnt/ESP

#check UUID for Ubuntu Disk.
sudo blkid

# then edit 

sudo nano /mnt/ESP/GRUB/GRUB.CONF

```
like this
```
menuentry '0 Your Current OS (ACPI)' {
    linux /Image \
    .....
}

menuentry '1 Ubuntu (ACPI)' {
    linux /Image \
        console=ttyAMA2,115200 \
        efi=noruntime \
        earlycon=pl011,0x040d0000 \
        arm-smmu-v3.disable_bypass=0 \
        cma=640M \
        acpi=force splash \
        loglevel=4 \
        pcie_aspm=off \
        resume=PARTUUID=[DISK UUID] noresume root=/dev/[diskname] rootwait rw
}

```

The Template is stored here

https://github.com/crackerjacques/orangepi-build/blob/trixie-test/ubuntu/ESP/GRUB/GRUB.CFG

# Agreements

Still not perfect to work...