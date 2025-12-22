#!/bin/bash
# https://github.com/crackerjacques/orangepi-build

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UBUNTU_DIR="${SCRIPT_DIR}/ubuntu"
IMAGES_DIR="${UBUNTU_DIR}/images"
REQUIREMENTS_DIR="${UBUNTU_DIR}/cix_linux_requirements"
OUTPUT_DIR="${SCRIPT_DIR}/output"
ESP_DIR="${UBUNTU_DIR}/ESP"

BOOTFS_MOUNT="/mnt/bootfs"
ROOTFS_MOUNT="/mnt/rootfs"

UBUNTU_2404_URL="https://cdimage.ubuntu.com/releases/noble/release/ubuntu-24.04.3-preinstalled-desktop-arm64+raspi.img.xz"
UBUNTU_2504_URL="https://cdimage.ubuntu.com/releases/plucky/release/ubuntu-25.04-preinstalled-desktop-arm64+raspi.img.xz"

CIX_DEBS_REPO="https://github.com/cixtech/cix_p1_ubuntu_adaption_debs"
CIX_DEBS_BRANCH="cix_k6.6_25q4_ubuntu_dev"

print_msg() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_important() {
    echo -e "${YELLOW}$1${NC}"
}

check_root() {
    if [[ $EUID -ne 0 ]]; then
        print_error "This script must be run as root (sudo)"
        echo "Usage: sudo $0"
        exit 1
    fi
}

check_dependencies() {
    print_msg "Checking dependencies... (the ritual begins)"

    local missing_deps=()
    local deps=(
        "wget"
        "xz-utils"
        "git"
        "parted"
        "e2fsprogs"
        "cloud-guest-utils"
        "util-linux"
    )

    # add qemu if not on arm
    if [[ "$(uname -m)" != "aarch64" ]]; then
        deps+=("qemu-user-static")
    fi

    for dep in "${deps[@]}"; do
        if ! dpkg -l "$dep" 2>/dev/null | grep -q "^ii"; then
            missing_deps+=("$dep")
        fi
    done

    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        print_warn "Missing packages detected. The abyss hungers for:"
        echo ""
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo ""
        read -p "Install missing packages? (Y/n): " install_choice
        if [[ "${install_choice}" != "n" && "${install_choice}" != "N" ]]; then
            print_msg "Summoning packages from the void..."
            apt update
            apt install -y "${missing_deps[@]}"
            print_msg "Dependencies installed. The ritual is complete."
        else
            print_error "Cannot proceed without dependencies. Your soul remains intact... for now."
            exit 1
        fi
    else
        print_msg "All dependencies satisfied. The stars align."
    fi
}

check_prerequisites() {
    print_msg "Checking prerequisites..."

    if [[ ! -f "${OUTPUT_DIR}/cix/Image" ]]; then
        print_error "Kernel Image not found at ${OUTPUT_DIR}/cix/Image"
        print_error "Please build the kernel first using: ./build.sh"
        exit 1
    fi

    local kernel_debs=0
    for pattern in "linux-headers" "linux-image" "linux-libc"; do
        if ls "${OUTPUT_DIR}/debs/${pattern}"*.deb 1>/dev/null 2>&1; then
            kernel_debs=$((kernel_debs + 1))
        fi
    done

    if [[ $kernel_debs -lt 3 ]]; then
        print_error "Kernel deb packages not found in ${OUTPUT_DIR}/debs/"
        print_error "Expected: linux-headers*.deb, linux-image*.deb, linux-libc*.deb"
        print_error "Please build the kernel packages first using: ./build.sh"
        exit 1
    fi

    print_msg "Prerequisites check a.k.a hell's gate passed!"
}

setup_directories() {
    mkdir -p "${IMAGES_DIR}"
    mkdir -p "${REQUIREMENTS_DIR}"
    mkdir -p "${BOOTFS_MOUNT}"
    mkdir -p "${ROOTFS_MOUNT}"
}

select_ubuntu_version() {
    echo ""
    echo "Select Ubuntu version to install:"
    echo ""
    echo "  1) Ubuntu 24.04 LTS (Noble Numbat) - Unstable as HELL"
    echo "  2) Ubuntu 25.04 (Plucky Puffin) - Unstable as SHEOL"
    echo ""
    read -p "Enter a tragedy choice [1-2]: " choice

    case $choice in
        1)
            UBUNTU_URL="${UBUNTU_2404_URL}"
            UBUNTU_VERSION="24.04"
            UBUNTU_IMG_NAME="ubuntu-24.04.3-preinstalled-desktop-arm64+raspi.img"
            ;;
        2)
            UBUNTU_URL="${UBUNTU_2504_URL}"
            UBUNTU_VERSION="25.04"
            UBUNTU_IMG_NAME="ubuntu-25.04-preinstalled-desktop-arm64+raspi.img"
            ;;
        *)
            print_error "Invalid choice You're a survivor."
            exit 1
            ;;
    esac

    UBUNTU_IMG_XZ="${IMAGES_DIR}/${UBUNTU_IMG_NAME}.xz"
    UBUNTU_IMG="${IMAGES_DIR}/${UBUNTU_IMG_NAME}"

    print_msg "Selected Ubuntu ${UBUNTU_VERSION}"
}

# prepare files and devices.
download_ubuntu_image() {
    if [[ -f "${UBUNTU_IMG}" ]]; then
        print_msg "Ubuntu image already exists: ${UBUNTU_IMG}"
        return 0
    fi

    if [[ -f "${UBUNTU_IMG_XZ}" ]]; then
        print_msg "Compressed image exists, extracting..."
    else
        print_msg "Downloading Ubuntu ${UBUNTU_VERSION} image..."
        wget --progress=bar:force -O "${UBUNTU_IMG_XZ}" "${UBUNTU_URL}"
    fi

    print_msg "Extracting image (this may take a while)..."
    xz -dk "${UBUNTU_IMG_XZ}"
    print_msg "Image extracted: ${UBUNTU_IMG}"
}

clone_cix_debs() {
    if [[ -d "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs" ]]; then
        print_msg "CIX debs repository already exists"
        return 0
    fi

    print_msg "Cloning CIX P1 Ubuntu adaptation debs..."
    git clone -b "${CIX_DEBS_BRANCH}" "${CIX_DEBS_REPO}" "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs"
}

select_target_device() {
    echo ""
    print_msg "Available block devices:"
    echo ""

    lsblk -d -o NAME,SIZE,MODEL,TRAN | grep -v "^loop" | grep -v "^NAME"

    echo ""
    print_warn "WARNING: All data on the selected device will be ERASED!"
    echo ""
    read -p "Enter device name (e.g., sda, nvme0n1) or 'q' to quit: " device_name

    if [[ "${device_name}" == "q" ]]; then
        print_msg "Aborted by user"
        exit 0
    fi

    TARGET_DEVICE="/dev/${device_name}"

    if [[ ! -b "${TARGET_DEVICE}" ]]; then
        print_error "Wonderful News!! Device ${TARGET_DEVICE} does not exist! "
        exit 1
    fi

    echo ""
    print_warn "You are about to ERASE ALL DATA on ${TARGET_DEVICE}"
    read -p "Are you sure? (y/N): " confirm

    if [[ "${confirm}" != "y" && "${confirm}" != "Y" ]]; then
        print_msg "Aborted by user, You cherished life and time."
        exit 0
    fi

    if [[ "${device_name}" == nvme* ]]; then
        PART1="${TARGET_DEVICE}p1"
        PART2="${TARGET_DEVICE}p2"
    else
        PART1="${TARGET_DEVICE}1"
        PART2="${TARGET_DEVICE}2"
    fi
}


# burn  image to device
dd_image() {
    print_msg "Writing Ubuntu image to ${TARGET_DEVICE}..."
    print_msg "This will take several minutes, too late for regrets...."

    umount "${PART1}" 2>/dev/null || true
    umount "${PART2}" 2>/dev/null || true

    dd if="${UBUNTU_IMG}" of="${TARGET_DEVICE}" bs=10M status=progress conv=fsync

    print_msg "Image written successfully! Your end draws near..."
    print_msg "Re-reading partition table..."
    sync
    partprobe "${TARGET_DEVICE}"
    sleep 3
}

mount_partitions() {
    print_msg "Mounting partitions..."

    umount "${BOOTFS_MOUNT}" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}" 2>/dev/null || true

    mount "${PART2}" "${ROOTFS_MOUNT}"
    mount "${PART1}" "${BOOTFS_MOUNT}"

    print_msg "Partitions mounted"
}

# copy kernel Image to bootfs and tweak grub.conf
setup_bootloader() {
    print_msg "Setting up bootloader..."

    mkdir -p "${ESP_DIR}"
    cp -f "${OUTPUT_DIR}/cix/Image" "${ESP_DIR}/"

    rm -rf "${BOOTFS_MOUNT:?}"/*
    cp -rf "${ESP_DIR}"/* "${BOOTFS_MOUNT}/"

    PART2_PARTUUID=$(blkid -s PARTUUID -o value "${PART2}")
    PART2_DEVNAME=$(basename "${PART2}")

    print_msg "Partition UUID: ${PART2_PARTUUID}"
    print_msg "Device name: ${PART2_DEVNAME}"

    GRUB_CFG="${BOOTFS_MOUNT}/GRUB/GRUB.CFG"
    if [[ -f "${GRUB_CFG}" ]]; then
        sed -i "s|resume=PARTUUID=[^ ]* noresume root=/dev/[^ ]*|resume=PARTUUID=${PART2_PARTUUID} noresume root=/dev/${PART2_DEVNAME}|g" "${GRUB_CFG}"
        print_msg "GRUB config updated"
    else
        print_warn "GRUB.CFG not found at ${GRUB_CFG}"
    fi
}

# copy some packages
setup_rootfs() {
    print_msg "Setting up rootfs..."

    KERNEL_PKG_DIR="${ROOTFS_MOUNT}/opt/kernel_packages"
    mkdir -p "${KERNEL_PKG_DIR}"

    cp -f "${OUTPUT_DIR}"/debs/linux-headers*.deb "${KERNEL_PKG_DIR}/"
    cp -f "${OUTPUT_DIR}"/debs/linux-image*.deb "${KERNEL_PKG_DIR}/"
    cp -f "${OUTPUT_DIR}"/debs/linux-libc*.deb "${KERNEL_PKG_DIR}/"
    print_msg "Kernel packages copied to ${KERNEL_PKG_DIR}"

    CIX_GO_TARBALL="${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs/cix-go-2025q3.tar.gz"
    if [[ -f "${CIX_GO_TARBALL}" ]]; then
        print_msg "Extracting cix-go packages..."
        tar -xzf "${CIX_GO_TARBALL}" -C "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs/"
    fi

    if [[ -d "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs" ]]; then
        cp -rf "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs" "${ROOTFS_MOUNT}/opt/"
        print_msg "CIX debs copied to /opt/debs"
    fi

    # Setup resolv.conf
    if [[ -L "${ROOTFS_MOUNT}/etc/resolv.conf" ]]; then
        ORIG_RESOLV_LINK=$(readlink "${ROOTFS_MOUNT}/etc/resolv.conf")
        rm -f "${ROOTFS_MOUNT}/etc/resolv.conf"
    elif [[ -f "${ROOTFS_MOUNT}/etc/resolv.conf" ]]; then
        ORIG_RESOLV_LINK=""
        rm -f "${ROOTFS_MOUNT}/etc/resolv.conf"
    else
        ORIG_RESOLV_LINK=""
    fi
    cp /etc/resolv.conf "${ROOTFS_MOUNT}/etc/resolv.conf"
}

# x86
setup_qemu() {
    if [[ "$(uname -m)" != "aarch64" ]]; then
        print_msg "Setting up QEMU for ARM64 chroot..."
        if [[ -f /usr/bin/qemu-aarch64-static ]]; then
            cp /usr/bin/qemu-aarch64-static "${ROOTFS_MOUNT}/usr/bin/"
        else
            print_warn "qemu-aarch64-static not found. Install qemu-user-static package."
            print_warn "On Ubuntu: sudo apt install qemu-user-static"
        fi
    fi
}

# chroot->install dpkg->setup services and users

run_chroot_setup() {
    print_msg "Entering chroot environment..."

    mount --bind /dev "${ROOTFS_MOUNT}/dev"
    mount --bind /dev/pts "${ROOTFS_MOUNT}/dev/pts"
    mount --bind /proc "${ROOTFS_MOUNT}/proc"
    mount --bind /sys "${ROOTFS_MOUNT}/sys"

    cat > "${ROOTFS_MOUNT}/tmp/setup.sh" << 'CHROOT_SCRIPT'
#!/bin/bash
set -e

echo "[INFO] Updating package lists..."
apt update

echo "[INFO] Installing kernel packages..."
cd /opt/kernel_packages
apt install -y ./*.deb || true

echo "[INFO] Installing CIX firmware and environment..."
cd /opt/debs
dpkg -i cix-firmware_*_arm64.deb || true
dpkg -i --force-overwrite cix-env_*_arm64.deb || true

echo "[INFO] Configuring system services..."
systemctl disable oem-config.service || true
systemctl disable unattended-upgrades || true
systemctl disable cloud-init-local cloud-init cloud-config cloud-final || true
systemctl set-default graphical.target

echo "[INFO] Chroot setup complete!"
CHROOT_SCRIPT

    chmod +x "${ROOTFS_MOUNT}/tmp/setup.sh"

    chroot "${ROOTFS_MOUNT}" /tmp/setup.sh

    echo ""
    read -p "Enter username to create: " NEW_USER
    if [[ -n "${NEW_USER}" ]]; then
        chroot "${ROOTFS_MOUNT}" useradd -m -G sudo,video,render,audio -s /bin/bash "${NEW_USER}"
        echo "Set password for ${NEW_USER}:"
        chroot "${ROOTFS_MOUNT}" passwd "${NEW_USER}"
        print_msg "User ${NEW_USER} created"
    fi

    umount "${ROOTFS_MOUNT}/sys"
    umount "${ROOTFS_MOUNT}/proc"
    umount "${ROOTFS_MOUNT}/dev/pts"
    umount "${ROOTFS_MOUNT}/dev"

    rm -f "${ROOTFS_MOUNT}/tmp/setup.sh"

    rm -f "${ROOTFS_MOUNT}/etc/resolv.conf"
    if [[ -n "${ORIG_RESOLV_LINK}" ]]; then
        ln -sf "${ORIG_RESOLV_LINK}" "${ROOTFS_MOUNT}/etc/resolv.conf"
        print_msg "Restored resolv.conf symlink to ${ORIG_RESOLV_LINK}"
    else
        ln -sf ../run/systemd/resolve/stub-resolv.conf "${ROOTFS_MOUNT}/etc/resolv.conf"
        print_msg "Created default resolv.conf symlink for systemd-resolved"
    fi

    # Create Drivers_and_note.txt on user's Desktop
    if [[ -n "${NEW_USER}" ]]; then
        USER_DESKTOP="${ROOTFS_MOUNT}/home/${NEW_USER}/Desktop"
        mkdir -p "${USER_DESKTOP}"
        cat > "${USER_DESKTOP}/Drivers_and_note.txt" << 'EOF'
================================================================================
                    CIX P1 / Orange Pi 6 Driver Information
================================================================================

Driver packages are located in: /opt/debs/

WARNING:
  Some driver packages may cause system instability or prevent the desktop
  environment from starting properly. This is currently under investigation.

RECOMMENDED:
  - The kernel and essential firmware (cix-firmware, cix-env) have already
    been installed during the image creation process.
  - Additional drivers in /opt/debs/ should be installed with caution.
  - Test each driver individually and reboot to verify stability.

TO INSTALL DRIVERS (at your own risk):
  cd /opt/debs
  sudo dpkg -i <package_name>.deb

KNOWN ISSUES (under investigation):
  - Some GPU/graphics drivers may cause display issues
  - Certain driver combinations may prevent GNOME/desktop from loading
  - If desktop fails to start, boot to recovery mode and remove problematic packages

For support and updates, check the project repository.
================================================================================
EOF
        chown -R 1000:1000 "${USER_DESKTOP}"
        print_msg "Created Drivers_and_note.txt on ${NEW_USER}'s Desktop"
    fi
}

resize_partition() {
    print_msg "Unmounting partitions..."
    umount "${BOOTFS_MOUNT}" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}" 2>/dev/null || true

    print_msg "Resizing root partition..."

    if [[ "${TARGET_DEVICE}" == *nvme* ]]; then
        PART_NUM=2
    else
        PART_NUM=2
    fi

    growpart "${TARGET_DEVICE}" ${PART_NUM} || true
    e2fsck -f "${PART2}" || true
    resize2fs "${PART2}"

    print_msg "Partition resized successfully! You're over!"
}

print_final_message() {
    echo ""
    echo "=============================================="
    print_msg "Ubuntu ${UBUNTU_VERSION} installation complete!"
    echo "=============================================="
    echo ""
    print_important "IMPORTANT: After first boot, install additional drivers:"
    echo ""
    echo "...It was written that way, but when I installed it and rebooted," 
    echo "the display splendidly showed nothing but the **ultimate abyss**"
    echo "I am currently investigating various matters. Proceed with caution."
    echo ""
    echo "  cd /opt/debs"
    echo "  sudo dpkg -i *.deb"
    echo ""
    echo "Or install specific packages as needed."
    echo ""
    print_msg "You can now eject media or reboot and Rapidly press the ESC key."
    echo ""
}

cleanup() {
    print_msg "Cleaning up..."
    umount "${BOOTFS_MOUNT}" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}/sys" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}/proc" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}/dev/pts" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}/dev" 2>/dev/null || true
    umount "${ROOTFS_MOUNT}" 2>/dev/null || true
}

trap cleanup EXIT

# Main
main() {
    echo ""
    echo "=============================================="
    echo "  Ubuntu Bootloader Injector for Orange Pi6"
    echo "=============================================="
    echo ""
    echo "CAUTION: This release is VERY UNSTABLE."
    echo "Use at your own risk. Data loss may occur."
    echo "Best used for testing and development purposes only."
    echo ""
    echo "best regards."
    echo ""
    
    check_root
    check_dependencies
    check_prerequisites
    setup_directories
    select_ubuntu_version
    download_ubuntu_image
    clone_cix_debs
    select_target_device
    dd_image
    mount_partitions
    setup_bootloader
    setup_rootfs
    setup_qemu
    run_chroot_setup
    resize_partition
    print_final_message
}

main "$@"
