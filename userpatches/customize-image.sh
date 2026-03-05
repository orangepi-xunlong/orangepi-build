#!/bin/bash

# arguments: $RELEASE $LINUXFAMILY $BOARD $BUILD_DESKTOP
#
# This is the image customization script

# NOTE: It is copied to /tmp directory inside the image
# and executed there inside chroot environment
# so don't reference any files that are not already installed

# NOTE: If you want to transfer files between chroot and host
# userpatches/overlay directory on host is bind-mounted to /tmp/overlay in chroot
# The sd card's root path is accessible via $SDCARD variable.

RELEASE=$1
LINUXFAMILY=$2
BOARD=$3
BUILD_DESKTOP=$4

Main() {
	case $RELEASE in
		xenial)
			# your code here
			;;
		stretch)
			# your code here
			# InstallOpenMediaVault # uncomment to get an OMV 4 image
			;;
		buster)
			# your code here
			;;
		bullseye)
			# your code here
			;;
		trixie)
			cat > /etc/apt/sources.list <<- 'EOF'
			deb http://deb.debian.org/debian trixie main contrib non-free non-free-firmware
			deb http://deb.debian.org/debian trixie-updates main contrib non-free non-free-firmware
			deb http://security.debian.org/debian-security trixie-security main contrib non-free non-free-firmware
			EOF

			# ── GPU: Mali CSF firmware symlink for panthor ──
			if [ -f /lib/firmware/arm/mali/arch10.8/mali_csffw.bin ]; then
				ln -sf /lib/firmware/arm/mali/arch10.8/mali_csffw.bin /lib/firmware/mali_csffw.bin
			fi

			# ── Display: ensure display modules load early for framebuffer console ──
			if [ -d /etc/modules-load.d ]; then
				printf "linlon_dp\ntrilin_dpsub\n" > /etc/modules-load.d/opi-display.conf
			fi

			# ── Console: enable local console gettys ──
			if command -v systemctl >/dev/null 2>&1; then
				systemctl --no-reload enable getty@tty1.service >/dev/null 2>&1 || true
				systemctl --no-reload enable serial-getty@ttyAMA2.service >/dev/null 2>&1 || true
			fi

			# ── Resize: ensure resize tooling and service are present for first-boot expansion ──
			if [ ! -x /usr/sbin/resize2fs ]; then
				export DEBIAN_FRONTEND=noninteractive
				apt-get update
				apt-get -y install e2fsprogs
			fi
			if command -v systemctl >/dev/null 2>&1; then
				systemctl --no-reload enable orangepi-resize-filesystem.service >/dev/null 2>&1 || true
			fi

			# ── Resize: add console hints and auto-reboot when resize needs it ──
			if [ -f /usr/lib/orangepi/orangepi-resize-filesystem ]; then
				if ! grep -q "Resize complete; rebooting" /usr/lib/orangepi/orangepi-resize-filesystem; then
					awk -f - /usr/lib/orangepi/orangepi-resize-filesystem > /tmp/orangepi-resize-filesystem <<'AWK'
/^[[:space:]]*start\)/ {
    print;
    print "\t\tif [ -e /dev/console ]; then";
    print "\t\t\techo \"[orangepi] Resizing root filesystem; system will reboot if required...\" > /dev/console";
    print "\t\tfi";
    next
}
/^[[:space:]]*# disable itself/ {
    print "\t\tif [[ -f /var/run/resize2fs-reboot ]]; then";
    print "\t\t\tif [ -e /dev/console ]; then";
    print "\t\t\t\techo \"[orangepi] Resize complete; rebooting to finish...\" > /dev/console";
    print "\t\t\tfi";
    print "\t\t\tsystemctl reboot || reboot";
    print "\t\t\texit 0";
    print "\t\tfi";
    print;
    next
}
{ print }
AWK
					mv /tmp/orangepi-resize-filesystem /usr/lib/orangepi/orangepi-resize-filesystem
					chmod 755 /usr/lib/orangepi/orangepi-resize-filesystem
				fi
			fi

			# ── Swap: create a 4 GB swapfile for first boot ──
			if [ ! -f /swapfile ]; then
				dd if=/dev/zero of=/swapfile bs=1M count=4096 2>/dev/null
				chmod 600 /swapfile
				mkswap /swapfile >/dev/null 2>&1
				echo "/swapfile none swap sw 0 0" >> /etc/fstab
			fi

			# ── Packages & NPU: install dev tooling + NPU userspace for CIX server images ──
			if [ "${LINUXFAMILY}" = "cix" ] && [ "${BUILD_DESKTOP}" != "yes" ]; then
				export DEBIAN_FRONTEND=noninteractive
				DEV_PACKAGES="build-essential pkg-config cmake git python3-dev python3-venv python3-pip python3-setuptools python3-wheel cython3 gfortran \
					libopenblas-dev libblas-dev liblapack-dev \
					libjpeg62-turbo-dev zlib1g-dev libpng-dev libtiff5-dev libwebp-dev libopenjp2-7-dev libfreetype6-dev liblcms2-dev \
					libffi-dev libssl-dev libgeos-dev \
					libglib2.0-0 libgl1 ffmpeg \
					protobuf-compiler libprotobuf-dev \
					clang llvm lld ninja-build \
					libopencv-dev python3-opencv \
					ocl-icd-libopencl1 ocl-icd-opencl-dev opencl-headers clinfo pocl-opencl-icd mesa-opencl-icd \
					docker.io docker-compose \
					zsh btop vim neovim ripgrep jq avahi-daemon lm-sensors libsensors-config"

				apt-get update
				apt-get -y install wget ${DEV_PACKAGES}

				# ── NPU userspace: use local 2.0.4 debs from overlay (has Python 3.13 support) ──
				NPU_DEB_DIR="/root/npu-debs"
				mkdir -p "${NPU_DEB_DIR}"

				# cix-npu-onnxruntime: download if not cached
				if [ ! -f "${NPU_DEB_DIR}/cix-npu-onnxruntime_1.1.0_arm64.deb" ]; then
					wget -q -O "${NPU_DEB_DIR}/cix-npu-onnxruntime_1.1.0_arm64.deb" \
						https://github.com/orangepi-xunlong/component_cix-next/releases/download/v1.1.0/cix-npu-onnxruntime_1.1.0_arm64.deb
				fi

				# cix-noe-umd 2.0.4: copy from overlay (has cpython-313 .so in bundled wheels)
				if [ -f /tmp/overlay/npu-debs/cix-noe-umd_2.0.4_arm64.deb ]; then
					cp /tmp/overlay/npu-debs/cix-noe-umd_2.0.4_arm64.deb "${NPU_DEB_DIR}/"
				fi

				apt-get -y --allow-downgrades install "${NPU_DEB_DIR}"/*.deb

				# ── Docker: add orangepi user to docker group ──
				if getent group docker >/dev/null 2>&1; then
					usermod -aG docker orangepi 2>/dev/null || true
				fi
			fi
			;;
		bionic)
			# your code here
			;;
		focal)
			# your code here
			;;
	esac
} # Main

Main "$@"
