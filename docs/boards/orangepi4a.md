# Orange Pi 4A Board Documentation

## Overview

The Orange Pi 4A is based on the Allwinner T527 octa-core processor with 2-4GB RAM, featuring Gigabit Ethernet, WiFi/BT, NVMe, and eMMC support.

**SoC**: Allwinner T527  
**Board Family**: sun55iw3  
**Kernel Target**: current (5.15)  
**Supported Distributions**: Debian Bookworm, Ubuntu Jammy

## Project File Dependencies

This document describes all project files that the Orange Pi 4A board depends on for building images.

### Board Configuration

#### Primary Board Configuration
- **File**: `external/config/boards/orangepi4a.conf`
- **Description**: Main board configuration file defining board name, family, boot configuration, kernel modules, and supported distributions
- **Key Settings**:
  - `BOARD_NAME="OPI 4A"`
  - `BOARDFAMILY="sun55iw3"`
  - `BOOTCONFIG="sun55iw3p1_t527_defconfig"`
  - `BOOT_FDT_FILE="allwinner/sun55i-t527-orangepi-4a.dtb"`
  - `MODULES="bcmdhd vin_v4l2"`
  - `KERNEL_TARGET="current"`
  - `DISTRIB_TYPE_CURRENT="bookworm jammy"`

### Family Configuration

#### SoC Family Configuration
- **File**: `external/config/sources/families/sun55iw3.conf`
- **Description**: Configuration for the sun55iw3 (T527) family of processors
- **Key Features**:
  - Includes common sunxi64 configuration
  - Kernel branch: `orange-pi-5.15-sun55iw3`
  - U-Boot branch: `v2018.05-t527`
  - Overlay prefix: `sun55i-t527`
  - Custom CPU frequency settings (408MHz-1800MHz)
  - Family-specific tweaks for desktop and system configuration

#### Common Include Files
- **File**: `external/config/sources/families/include/sunxi64_common.inc`
- **Description**: Common configuration for 64-bit Allwinner (sunxi) processors
- **Features**:
  - ARM64 architecture settings
  - ATF (ARM Trusted Firmware) target configuration
  - Common U-Boot and boot script settings
  - Family tweaks for desktop packages

### Kernel Configuration

#### Kernel Config File
- **File**: `external/config/kernel/linux-5.15-sun55iw3-current.config`
- **Description**: Linux 5.15 kernel configuration for sun55iw3 family
- **Purpose**: Defines all kernel features, drivers, and modules enabled for this board

### Boot Configuration

#### U-Boot Configuration
- **Directory**: `external/packages/pack-uboot/sun55iw3/`
- **Contents**:
  - `bin/boot0_sdcard.fex-linux5.15` - Boot0 loader for SD card (Linux 5.15)
  - `bin/boot0_spinor.fex` - Boot0 loader for SPI NOR flash
  - `bin/boot_package.cfg-linux5.15` - Boot package configuration
  - `bin/monitor.fex-linux5.15` - Monitor firmware
  - `bin/scp.fex` - SCP (System Control Processor) firmware
  - `bin/dts/orangepi4a-u-boot-current.dts` - U-Boot device tree source
  - `bin/dts/orangepi4a-u-boot-current.dts-bootargs` - Boot arguments variant
  - `bin/dts/orangepi4a-u-boot-current.dts-no-boot` - No-boot variant
  - `bin/sys_config/sys_config_orangepi4a.fex` - System configuration (FEX format)
  - `tools/` - Various build tools (dragonsecboot, dtc, script, update_* utilities)

#### Boot Scripts and Environment
- **File**: `external/config/bootscripts/boot-sun55iw3.cmd`
- **Description**: U-Boot boot script for sun55iw3 family
- **Purpose**: Defines boot sequence and kernel loading

- **File**: `external/config/bootenv/sun55iw3.txt`
- **Description**: U-Boot environment variables template
- **Purpose**: Sets default boot environment parameters

### BSP (Board Support Package) Files

#### T527 Specific BSP Files
- **Directory**: `external/packages/bsp/t527/`
- **Contents**:
  - `etc/X11/xorg.conf` - X11 display server configuration
  - `etc/udev/rules.d/99-t527-permissions.rules` - udev rules for device permissions

### Build Scripts and Logic

#### Main Build Script
- **File**: `scripts/main.sh`
- **Relevant Lines**:
  - Line 234: Board menu option definition
    ```bash
    options+=("orangepi4a"		"Allwinner T527 octa core 2-4GB RAM GBE WiFi/BT NVMe eMMC")
    ```
  - Lines 487-491: T527 packages repository fetching
    ```bash
    if [[ ${BOARDFAMILY} == "sun55iw3" && $RELEASE =~ bookworm|jammy ]]; then
        [[ ${BUILD_OPT} == image ]] && fetch_from_repo "https://github.com/orangepi-xunlong/rk-rootfs-build.git" "${EXTER}/cache/sources/t527_packages" "branch:t527_packages"
    fi
    ```

### External Dependencies

#### T527 Desktop Packages
- **Repository**: https://github.com/orangepi-xunlong/rk-rootfs-build.git
- **Branch**: t527_packages
- **Purpose**: Provides pre-built packages for desktop environments (mesa, libcedarc, gst-omx)
- **Cached Location**: `${EXTER}/cache/sources/t527_packages`
- **Required For**: Desktop image builds with Bookworm or Jammy

#### Firmware
- **Source**: `${EXTER}/cache/sources/orangepi-firmware-git`
- **File Used**: `nvram_ap6256.txt-orangepi4a`
- **Purpose**: WiFi/Bluetooth firmware for AP6256 module
- **Install Location**: `/lib/firmware/nvram_ap6256.txt`

### Key Build Process Functions

#### family_tweaks_s() in sun55iw3.conf
This function handles board-specific customizations:
- Desktop environment configuration (GNOME, XFCE)
- WiFi/Bluetooth firmware installation
- ARM64 overlays deployment
- Package installation (mtd-utils, rfkill, bluetooth, etc.)
- Audio configuration (PulseAudio)
- Desktop packages (mesa, libcedarc, gst-omx, vlc, mpv)
- WiringOP library installation

#### uboot_custom_postprocess() in sun55iw3.conf
Custom U-Boot post-processing:
- Device tree compilation
- FEX file processing (system configuration)
- Boot package creation using dragonsecboot
- Optional U-Boot merging for easy deployment

#### write_uboot_platform_mtd() in sun55iw3.conf
MTD (SPI flash) boot loader writing:
- Writes boot0_spinor.fex to /dev/mtd0
- Writes boot_package.fex to /dev/mtd0

## Build Instructions

To build an image for Orange Pi 4A:

```bash
./build.sh
# Select "orangepi4a" from the board menu
# Select desired kernel branch (current recommended)
# Select distribution (bookworm or jammy)
# Select image type (minimal or desktop)
```

## File Summary by Category

### Configuration Files (8 files)
1. `external/config/boards/orangepi4a.conf`
2. `external/config/sources/families/sun55iw3.conf`
3. `external/config/sources/families/include/sunxi64_common.inc`
4. `external/config/kernel/linux-5.15-sun55iw3-current.config`
5. `external/config/bootscripts/boot-sun55iw3.cmd`
6. `external/config/bootenv/sun55iw3.txt`
7. `external/packages/bsp/t527/etc/X11/xorg.conf`
8. `external/packages/bsp/t527/etc/udev/rules.d/99-t527-permissions.rules`

### U-Boot Package Files (1 directory + multiple files)
9. `external/packages/pack-uboot/sun55iw3/` (entire directory with bin/ and tools/ subdirectories)
   - Boot loaders (boot0_sdcard.fex, boot0_spinor.fex)
   - Device tree sources (orangepi4a-u-boot-current.dts*)
   - System config (sys_config_orangepi4a.fex)
   - Build tools (dragonsecboot, dtc, script, update_* utilities)

### Build Scripts (1 file)
10. `scripts/main.sh` (lines 234, 487-491)

### External Repositories
11. orangepi-xunlong/rk-rootfs-build (branch: t527_packages)
12. orangepi-firmware-git (nvram_ap6256.txt)

## Total Dependencies

- **Direct Configuration Files**: 8
- **U-Boot Package Directory**: 1 (with ~20+ files)
- **Build Script References**: 1
- **External Repositories**: 2

**Note**: The actual kernel and U-Boot source code repositories are configured in the family configuration files but are dynamically fetched during the build process based on the branch settings.
