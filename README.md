## Supported boards

Soc | Boards |
|:--|:--|
| Allwinner H6 | Orange Pi 3/3 LTS |
| Allwinner H616 | Orange Pi Zero2/Zero2w/Zero3 | 
| Allwinner T527 | Orange Pi 4A |
| Allwinner A733 | Orange Pi 4Pro | 
| Rockchip RK3399 | Orange Pi 4/4B/4 LTS/800 |
| Rockchip RK3566 | Orange Pi 3B/CM4 |
| Rockchip RK3588S | Orange Pi 5/5B/5Pro/CM5/CM5-tablet |
| Rockchip RK3588 | Orange Pi 5Plus/5MAX/5Ultra |
| Cix P1 | Orange Pi 6Plus |
| Starfive  JH7110 | Orange Pi RV |
| Ky X1 | Orange Pi RV2/R2S |

## Download links

- 中文链接：     http://www.orangepi.cn
- English link：http://www.orangepi.org

## Building an image from this fork

This fork is built manually on a build host; pushing commits does **not** trigger
an image rebuild (there are no GitHub Actions build workflows). The original
Orange Pi 6 Plus image was cross-compiled on an Ubuntu 22.04 VM, with a Debian
root filesystem assembled using `debootstrap`. See
[The Orange Pi 6 Plus — Building the Image](https://taoofmac.com/space/reviews/2026/04/11/1900#building-the-image)
for background on the Trixie, boot-chain, first-boot and GPU/NPU fixes.

### 1. Prepare a build host

Use Ubuntu 22.04 (an x86_64 VM is suitable for cross-compilation), preferably on
a dedicated machine or VM with ample RAM and disk
space for kernel sources, packages, rootfs caches and images. The build scripts
install host dependencies and require root privileges; do not run them on a
machine where you cannot allow package and system configuration changes. Internet
access is required to fetch kernel/vendor sources and distribution packages.

```sh
sudo apt-get update
sudo apt-get install -y git

git clone --branch next https://github.com/rcarmo/orangepi-build.git
cd orangepi-build
```

For an existing checkout, switch to `next` and run `git pull --ff-only` after
saving any local changes. If building over SSH, use `tmux` or a similar persistent
session so a disconnect does not interrupt the build.

### 2. Select a board, kernel branch and distribution

Use a board identifier from `external/config/boards/*.conf`, a kernel branch
supported by that board, and a release listed in its `DISTRIB_TYPE_*` settings.
The Git checkout branch (`next`) and the kernel selection (`BRANCH`) are separate
settings, even when they have the same name.

For example, the server image described in the review uses:

```sh
BOARD=orangepi6plus
BRANCH=next
RELEASE=trixie
```

Change these values for another supported target. Review
`userpatches/customize-image.sh` before building: it contains this fork's image
customizations. Optional `userpatches/<LINUXCONFIG>.config` files override the
shipped kernel configs; `LINUXCONFIG` is selected by the board/family configuration
under `external/config/sources`.

### 3. Build or rebuild the full image

From the repository root, with the variables above set:

```sh
sudo ./build.sh BOARD="$BOARD" BRANCH="$BRANCH" RELEASE="$RELEASE" \
  BUILD_OPT=image BUILD_DESKTOP=no BUILD_MINIMAL=no \
  KERNEL_CONFIGURE=no KERNEL_KEEP_CONFIG=no \
  REPOSITORY_INSTALL= CLEAN_LEVEL=debs COMPRESS_OUTPUTIMAGE=no
```

- `BUILD_OPT=image` builds the kernel packages and a full flashable OS image.
- `BUILD_DESKTOP=no BUILD_MINIMAL=no` selects the standard server image.
- `KERNEL_CONFIGURE=no` avoids the interactive kernel configuration menu.
- `KERNEL_KEEP_CONFIG=no` avoids reusing an exported kernel config from
  `output/config`; a matching config in `userpatches` still takes precedence.
- `REPOSITORY_INSTALL=` selects local compilation rather than repository kernels.
- `CLEAN_LEVEL=debs` deletes cached packages for the selected target, including
  kernel and related Orange Pi packages, so configuration changes are rebuilt.
  It retains downloaded sources and rootfs caches. Back up any packages you need
  to keep before rebuilding.
- `COMPRESS_OUTPUTIMAGE=no` leaves an uncompressed image for flashing.

The Makefile also offers a shorter invocation for ordinary builds:

```sh
make build BOARD=orangepi6plus BRANCH=next RELEASE=trixie
```

`make build-detached` uses `scripts/rebuild.sh` to run in the background and prints
log/PID paths under `output/`. Both Makefile build targets use `sudo -n`, so they
require non-interactive sudo access (passwordless sudo or valid cached credentials).
These
helpers do not forward the cache/config override options in the full command
above, so use that command when rebuilding after kernel configuration changes.

### 4. Locate and verify the result

Final images are placed under `output/images/<image-name>/`; kernel packages are
under `output/debs/`. Inspect build logs under `output/` and, where produced,
`output/debug/`. Cix builds also stage the kernel, DTBs and partition artifacts
under `output/cix/` before assembling the final image.

For an image with a `.sha` checksum file, verify it from its containing directory:

```sh
# Replace image-name with the actual generated filename prefix.
sha256sum --check image-name.img.sha
```

### 5. Flash and validate

Write the resulting `.img` to an SD card using an image writer such as
Balena Etcher. **Flashing erases the destination device**; verify its identity
and do not select the build host's system disk or an existing NVMe installation.
Boot the board from the card and allow the first-boot resize/reboot process to
finish. A serial console is useful for diagnosing boot failures.

On Orange Pi 6 Plus/Trixie, this fork's GRUB configuration defaults to Device Tree
boot. Check the boot menu and console output if boot stops at the EFI/kernel
handoff. Migrating a working installation to NVMe is a separate operation, not a
build step; the review describes the original EFI/root/swap migration and GRUB
PARTUUID changes.

## EROFS kernel support

The shipped EROFS-capable kernel configurations enable built-in
`CONFIG_EROFS_FS=y`, LZ4 compressed images (`CONFIG_EROFS_FS_ZIP=y`), extended
attributes, POSIX ACLs and security labels. Older vendor kernels without an
EROFS configuration entry are unchanged; no driver backport is included.
Custom or previously saved kernel configurations still override these defaults.
Rebuild the kernel to apply the changes; existing prebuilt kernels are unaffected.

This does **not** change the system/root filesystem or image layout. To create
and mount a separate EROFS image, install `erofs-utils` on the machine where you
create it, then use, for example:

```sh
mkfs.erofs -zlz4hc data.erofs /path/to/data
sudo mount -t erofs -o loop,ro data.erofs /mnt
```

Check the shipped configuration defaults with `python3 tests/check-erofs-configs.py`.
After booting the rebuilt image, confirm the running kernel registers EROFS:

```sh
uname -r
grep -w erofs /proc/filesystems
```

A successful configuration check alone does not prove an older running kernel or
cached image has EROFS support.
