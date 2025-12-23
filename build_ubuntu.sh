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

select_distro_version() {
    echo ""
    echo "Select distribution to install:"
    echo ""
    echo "  1) Ubuntu 24.04 LTS (Noble Numbat) - Partial support, Vulkan works"
    echo "  2) Ubuntu 25.04 (Plucky Puffin) - Better than Noble."
    # echo "  3) DragonOS Pi64 (Raspbian 13/Trixie) - TESTING!! DO NOT INSTALL!!"
    echo ""
    print_warn "All options are EXPERIMENTAL. WebGL does not work yet."
    echo ""
    read -p "Enter your fate [1-3]: " choice

    case $choice in
        1)
            DISTRO_TYPE="ubuntu"
            DISTRO_URL="${UBUNTU_2404_URL}"
            DISTRO_VERSION="24.04"
            DISTRO_IMG_NAME="ubuntu-24.04.3-preinstalled-desktop-arm64+raspi.img"
            DISTRO_CODENAME="noble"
            ;;
        2)
            DISTRO_TYPE="ubuntu"
            DISTRO_URL="${UBUNTU_2504_URL}"
            DISTRO_VERSION="25.04"
            DISTRO_IMG_NAME="ubuntu-25.04-preinstalled-desktop-arm64+raspi.img"
            DISTRO_CODENAME="plucky"
            ;;
        # 3)
        #     DISTRO_TYPE="dragonos"
        #     DISTRO_VERSION="Pi64"
        #     DISTRO_CODENAME="trixie"
        #     select_dragonos_image
        #     ;;
        *)
            print_error "Invalid choice. You escaped the ritual... this time."
            exit 1
            ;;
    esac

    # Set legacy variable names for compatibility
    UBUNTU_VERSION="${DISTRO_VERSION}"

    print_msg "Selected ${DISTRO_TYPE} ${DISTRO_VERSION}"
}

# select_dragonos_image() {
#     echo ""
#     echo "=============================================="
#     echo "  DragonOS Pi64 - Manual Download Required"
#     echo "=============================================="
#     echo ""
#     print_warn "DragonOS Pi64 must be downloaded manually from:"
#     echo ""
#     echo "  https://cemaxecuter.com/"
#     echo ""
#     echo "Download the image and place it in:"
#     echo "  ${IMAGES_DIR}/"
#     echo ""
#     echo "Supported formats:"
#     echo "  - DragonOS_Pi64_*.img"
#     echo "  - DragonOS_Pi64_*.img.gz"
#     echo "  - DragonOS_Pi64_*.img.xz"
#     echo "  - DragonOS_Pi64_*.tar (containing .img)"
#     echo ""
# 
#     # Search for DragonOS images
#     local found_images=()
#     local i=1
# 
#     # Check for various formats
#     for img in "${IMAGES_DIR}"/DragonOS_Pi64_*.img; do
#         [[ -f "$img" ]] && found_images+=("$img")
#     done
#     for img in "${IMAGES_DIR}"/DragonOS_Pi64_*.img.gz; do
#         [[ -f "$img" ]] && found_images+=("$img")
#     done
#     for img in "${IMAGES_DIR}"/DragonOS_Pi64_*.img.xz; do
#         [[ -f "$img" ]] && found_images+=("$img")
#     done
#     for img in "${IMAGES_DIR}"/DragonOS_Pi64_*.tar; do
#         [[ -f "$img" ]] && found_images+=("$img")
#     done
# 
#     if [[ ${#found_images[@]} -eq 0 ]]; then
#         print_error "No DragonOS Pi64 images found in ${IMAGES_DIR}/"
#         print_error "Please download from https://cemaxecuter.com/ and try again."
#         exit 1
#     fi
# 
#     echo "Found DragonOS images:"
#     echo ""
#     for img in "${found_images[@]}"; do
#         echo "  $i) $(basename "$img")"
#         i=$((i + 1))
#     done
#     echo ""
# 
#     read -p "Select image [1-${#found_images[@]}]: " img_choice
# 
#     if [[ "$img_choice" -lt 1 || "$img_choice" -gt ${#found_images[@]} ]]; then
#         print_error "Invalid selection."
#         exit 1
#     fi
# 
#     local selected_img="${found_images[$((img_choice - 1))]}"
#     DISTRO_IMG_NAME="$(basename "$selected_img")"
# 
#     # Handle compressed formats
#     if [[ "$selected_img" == *.gz ]]; then
#         DISTRO_IMG_COMPRESSED="$selected_img"
#         DISTRO_IMG_NAME="${DISTRO_IMG_NAME%.gz}"
#         DISTRO_COMPRESSION="gz"
#     elif [[ "$selected_img" == *.xz ]]; then
#         DISTRO_IMG_COMPRESSED="$selected_img"
#         DISTRO_IMG_NAME="${DISTRO_IMG_NAME%.xz}"
#         DISTRO_COMPRESSION="xz"
#     elif [[ "$selected_img" == *.tar ]]; then
#         DISTRO_IMG_COMPRESSED="$selected_img"
#         DISTRO_IMG_NAME="${DISTRO_IMG_NAME%.tar}"
#         DISTRO_COMPRESSION="tar"
#     else
#         DISTRO_COMPRESSION=""
#     fi
# 
#     print_msg "Selected: ${DISTRO_IMG_NAME}"
# }

# prepare files and devices.
download_distro_image() {
    DISTRO_IMG="${IMAGES_DIR}/${DISTRO_IMG_NAME}"

    if [[ -f "${DISTRO_IMG}" ]]; then
        print_msg "Image already exists: ${DISTRO_IMG}"
        return 0
    fi

    if [[ "${DISTRO_TYPE}" == "ubuntu" ]]; then
        local img_xz="${IMAGES_DIR}/${DISTRO_IMG_NAME}.xz"

        if [[ -f "${img_xz}" ]]; then
            print_msg "Compressed image exists, extracting..."
        else
            print_msg "Downloading Ubuntu ${DISTRO_VERSION} image..."
            wget --progress=bar:force -O "${img_xz}" "${DISTRO_URL}"
        fi

        print_msg "Extracting image (this may take a while)..."
        xz -dk "${img_xz}"
        print_msg "Image extracted: ${DISTRO_IMG}"

    # elif [[ "${DISTRO_TYPE}" == "dragonos" ]]; then
    #     if [[ -z "${DISTRO_COMPRESSION}" ]]; then
    #         print_msg "DragonOS image ready: ${DISTRO_IMG}"
    #     elif [[ "${DISTRO_COMPRESSION}" == "gz" ]]; then
    #         print_msg "Extracting DragonOS image (gzip)..."
    #         gunzip -k "${DISTRO_IMG_COMPRESSED}"
    #         print_msg "Image extracted: ${DISTRO_IMG}"
    #     elif [[ "${DISTRO_COMPRESSION}" == *.xz ]]; then
    #         print_msg "Extracting DragonOS image (xz)..."
    #         xz -dk "${DISTRO_IMG_COMPRESSED}"
    #         print_msg "Image extracted: ${DISTRO_IMG}"
    #     elif [[ "${DISTRO_COMPRESSION}" == "tar" ]]; then
    #         print_msg "Extracting DragonOS image (tar)..."
    #         tar -xf "${DISTRO_IMG_COMPRESSED}" -C "${IMAGES_DIR}/"
    #         # Find the extracted .img file
    #         local extracted_img
    #         extracted_img=$(find "${IMAGES_DIR}" -name "*.img" -newer "${DISTRO_IMG_COMPRESSED}" | head -1)
    #         if [[ -n "$extracted_img" ]]; then
    #             DISTRO_IMG="$extracted_img"
    #             DISTRO_IMG_NAME="$(basename "$extracted_img")"
    #             print_msg "Image extracted: ${DISTRO_IMG}"
    #         else
    #             print_error "Failed to find extracted image from tar"
    #             exit 1
    #         fi
    #     fi
    fi
}

clone_cix_debs() {
    if [[ -d "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs" ]]; then
        print_msg "CIX debs repository already exists"
        return 0
    fi

    print_msg "Cloning CIX P1 Ubuntu adaptation debs..."
    git clone -b "${CIX_DEBS_BRANCH}" "${CIX_DEBS_REPO}" "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs"
    patch_cix_go_install
}

patch_cix_go_install() {
    local CIX_GO_TARBALL="${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs/cix-go-2025q3.tar.gz"
    local CIX_GO_DIR="${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs/cix-go"
    local INSTALL_SH="${CIX_GO_DIR}/install.sh"

    if [[ -f "${CIX_GO_TARBALL}" && ! -d "${CIX_GO_DIR}" ]]; then
        print_msg "Extracting cix-go tarball for patching..."
        tar -xzf "${CIX_GO_TARBALL}" -C "${REQUIREMENTS_DIR}/cix_p1_ubuntu_adaption_debs/debs/"
    fi

    if [[ -f "${INSTALL_SH}" ]]; then
        print_msg "Patching cix-go install.sh... (fixing the cursed script)"

        cat > "${INSTALL_SH}" << 'INSTALL_SCRIPT'
#!/bin/bash
export DEBIAN_FRONTEND=noninteractive

BOLD_RED='\033[1;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

# DKMS enabled by default
dkms_enable=true

case $1 in
    ("--help")
        echo "Usage:"
        echo "       --dkms: enable dkms build (default)"
        echo "       --no-dkms: disable dkms build"
        exit
        ;;
    ("--no-dkms")
        dkms_enable=false
        ;;
esac

if [[ $(dpkg -l | grep cix-gpu-umd | awk '{print $2}') == "cix-gpu-umd" ]]; then
    echo -e "${BOLD_RED}Pre-installed OS image detected, please run uninstall.sh first!${RESET}"
else
    echo -e "${GREEN}Install cix-go:${RESET}"
    echo ""
    sudo dpkg -i --force-overwrite cix-gpu-umd*.deb
    sudo dpkg -i cix-libglvnd*.deb
    sudo dpkg -i cix-mesa*.deb

    if [[ "${dkms_enable}" == "true" ]]; then
        echo -e "${GREEN}Installing DKMS kernel module...${RESET}"
        sudo dpkg -i cix-gpu-dkms*.deb
        sudo apt install -y dkms
        sudo dkms add -m cix-gpu-kmd -v 1.0.0 || true
        sudo dkms build -m cix-gpu-kmd -v 1.0.0
        sudo dkms install -m cix-gpu-kmd -v 1.0.0 --force
    fi

    codename=$(lsb_release -c | awk '{print $2}')
    if [[ "$codename" == "bookworm" ]] || [[ "$codename" == "jammy" ]]; then
        sudo dpkg -i xwayland_22.1.9-1+cix_arm64.deb
    elif [[ "$codename" == "trixie" ]] || [[ "$codename" == "noble" ]] || [[ "$codename" == "oracular" ]] || [[ "$codename" == "plucky" ]]; then
        sudo dpkg -i xwayland_24.1.1-1+cix_arm64.deb
    fi

    echo -e "${GREEN}Installation complete. May the GPU gods have mercy on your soul.${RESET}"
fi
INSTALL_SCRIPT

        chmod +x "${INSTALL_SH}"
        print_msg "cix-go install.sh patched successfully"
    else
        print_warn "cix-go install.sh not found, skipping patch"
    fi
}

# User settings
collect_user_settings() {
    echo ""
    echo "=============================================="
    echo "  User Configuration (before the abyss)"
    echo "=============================================="
    echo ""

    while [[ -z "${NEW_USER}" ]]; do
        read -p "Enter username to create: " NEW_USER
        if [[ -z "${NEW_USER}" ]]; then
            print_warn "Username cannot be empty. Try again, mortal."
        fi
    done

    while true; do
        read -s -p "Enter password for ${NEW_USER}: " NEW_PASSWORD
        echo ""
        read -s -p "Confirm password: " NEW_PASSWORD_CONFIRM
        echo ""
        if [[ "${NEW_PASSWORD}" == "${NEW_PASSWORD_CONFIRM}" ]]; then
            if [[ -z "${NEW_PASSWORD}" ]]; then
                print_warn "Password cannot be empty. Even chaos has rules."
            else
                break
            fi
        else
            print_warn "Passwords do not match. The void rejects your offering."
        fi
    done

    # DKMS option
    echo ""
    echo "GPU Driver Installation Options:"
    echo "  DKMS builds kernel modules for GPU support."
    echo "  This is recommended but may cause instability."
    echo ""
    read -p "Enable DKMS GPU driver build? (Y/n): " dkms_choice
    if [[ "${dkms_choice}" == "n" || "${dkms_choice}" == "N" ]]; then
        INSTALL_DKMS="false"
        print_msg "DKMS disabled. Your GPU may weep in silence."
    else
        INSTALL_DKMS="true"
        print_msg "DKMS enabled. Preparing the sacrifice..."
    fi

    echo ""
    print_msg "User settings collected. Proceeding to device selection..."
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
    print_msg "Writing ${DISTRO_TYPE} image to ${TARGET_DEVICE}..."
    print_msg "This will take several minutes, too late for regrets...."

    umount "${PART1}" 2>/dev/null || true
    umount "${PART2}" 2>/dev/null || true

    dd if="${DISTRO_IMG}" of="${TARGET_DEVICE}" bs=10M status=progress conv=fsync

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

    # Create setup script with variables embedded
    cat > "${ROOTFS_MOUNT}/tmp/setup.sh" << CHROOT_SCRIPT
#!/bin/bash
set -e
export DEBIAN_FRONTEND=noninteractive

NEW_USER="${NEW_USER}"
NEW_PASSWORD="${NEW_PASSWORD}"
INSTALL_DKMS="${INSTALL_DKMS}"

echo "[INFO] Updating package lists..."
apt update

echo "[INFO] Installing kernel packages..."
cd /opt/kernel_packages
apt install -y ./*.deb || true

echo "[INFO] Installing CIX firmware and environment..."
cd /opt/debs
dpkg -i cix-firmware_*_arm64.deb || true
dpkg -i --force-overwrite cix-env_*_arm64.deb || true

echo "[INFO] Installing GPU drivers (cix-go)..."
cd /opt/debs/cix-go
if [[ "\${INSTALL_DKMS}" == "true" ]]; then
    echo "[INFO] DKMS enabled, running full GPU installation..."
    ./install.sh --dkms || true
else
    echo "[INFO] DKMS disabled, running minimal GPU installation..."
    ./install.sh --no-dkms || true
fi

echo "[INFO] Configuring system services..."
systemctl disable oem-config.service || true
systemctl disable unattended-upgrades || true
systemctl disable cloud-init-local cloud-init cloud-config cloud-final || true
systemctl set-default graphical.target

echo "[INFO] Creating user: \${NEW_USER}"
useradd -m -G sudo,video,render,audio -s /bin/bash "\${NEW_USER}"
echo "\${NEW_USER}:\${NEW_PASSWORD}" | chpasswd
echo "[INFO] User \${NEW_USER} created successfully"

echo "[INFO] Configuring GDM3 autologin..."
mkdir -p /etc/gdm3
cat >> /etc/gdm3/custom.conf << EOF
[daemon]
AutomaticLoginEnable=true
AutomaticLogin=\${NEW_USER}
EOF

echo "[INFO] Installing DKMS to avoid prompts..."
apt install -y dkms || true

echo "[INFO] Chroot setup complete! The ritual is done."
CHROOT_SCRIPT

    chmod +x "${ROOTFS_MOUNT}/tmp/setup.sh"

    chroot "${ROOTFS_MOUNT}" /tmp/setup.sh

    print_msg "User ${NEW_USER} created with specified password"

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

CURRENT STATUS (under active development):
  - Vulkan:  WORKING
  - WebGL:   NOT WORKING (under investigation)
  - X11/SDDM/LightDM: May fail to start desktop environment

DISTRO-SPECIFIC NOTES:
  - Ubuntu 24.04 (Noble): Partial support, Vulkan works.
  - Ubuntu 25.04 (Plucky): Better than Noble by dependecy problem.

WARNING:
  Some driver packages may cause system instability or prevent the desktop
  environment from starting properly. This is currently under investigation.

RECOMMENDED:
  - The kernel and essential firmware (cix-firmware, cix-env) have already
    been installed during the image creation process.
  - GPU drivers (cix-go) have been installed with DKMS support.
  - Additional drivers in /opt/debs/ should be installed with caution.
  - Test each driver individually and reboot to verify stability.

TO INSTALL ADDITIONAL DRIVERS (at your own risk):
  cd /opt/debs
  sudo dpkg -i <package_name>.deb

KNOWN ISSUES (under investigation):
  - WebGL does not work in browsers
  - Some GPU/graphics drivers may cause display issues
  - Certain driver combinations may prevent GNOME/desktop from loading
  - If desktop fails to start, boot to recovery mode and remove problematic packages

For support and updates, check the project repository:
https://github.com/crackerjacques/orangepi-build
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

setup_dualboot() {
    echo ""
    echo "=============================================="
    echo "  Dual Boot Configuration (Optional)"
    echo "=============================================="
    echo ""
    echo "You can now configure dual boot to add other OS entries"
    echo "to the boot menu or set a default boot OS."
    echo ""
    read -p "Do you want to configure dual boot? (y/N): " dualboot_choice

    if [[ "${dualboot_choice}" == "y" || "${dualboot_choice}" == "Y" ]]; then
        if [[ -f "${SCRIPT_DIR}/dualboot_enabler.sh" ]]; then
            print_msg "Launching dual boot configuration..."
            bash "${SCRIPT_DIR}/dualboot_enabler.sh"
            print_msg "Dual boot configuration complete!"
        else
            print_error "dualboot_enabler.sh not found in ${SCRIPT_DIR}/"
            print_error "Skipping dual boot setup."
        fi
    else
        print_msg "Skipping dual boot configuration."
    fi
}

print_final_message() {
    echo ""
    echo "=============================================="
    print_msg "${DISTRO_TYPE} ${DISTRO_VERSION} installation complete!"
    echo "=============================================="
    echo ""
    echo "DRIVER STATUS (as of now):"
    echo "  - Vulkan: WORKING"
    echo "  - WebGL:  NOT WORKING (under investigation)"
    echo "  - X11/SDDM/LightDM: May fail to start DE"
    echo ""
    print_important "IMPORTANT: After first boot, additional drivers are in /opt/debs/"
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
    echo "  Ubuntu Bootloader Injector for Orange Pi6 V0.2"
    echo "=============================================="
    echo ""
    echo "UPDATE: GPU support is now!"
    # echo "UPDATE: DragonOS Pi64 support."
    echo ""
    print_important "Don't worry about all the errors popping up during the chroot operation."
    echo ""
    echo "CAUTION: This script will ERASE ALL DATA on the selected target device."
    echo "Use at your own risk. Data loss may occur."
    echo ""
    echo "And This release not fully operational yet."
    echo "SDDM, LightDM, X11 may fail to start the desktop environment."
    echo "WebGL coud not work properly."
    echo ""
    echo "Best Regards."
    echo ""
    
    check_root
    check_dependencies
    check_prerequisites
    setup_directories
    select_distro_version
    download_distro_image
    clone_cix_debs
    collect_user_settings
    select_target_device
    dd_image
    mount_partitions
    setup_bootloader
    setup_rootfs
    setup_qemu
    run_chroot_setup
    resize_partition
    setup_dualboot
    print_final_message

    echo ""
    print_msg "Done!"
    echo ""
}

main "$@"
