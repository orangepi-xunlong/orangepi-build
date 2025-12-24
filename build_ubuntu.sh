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

    local need_kernel=false
    local DEFAULT_OUTPUT_DIR="${UBUNTU_DIR}/output_default"

    if [[ ! -f "${OUTPUT_DIR}/cix/Image" ]]; then
        need_kernel=true
    fi

    if [[ "${need_kernel}" == "false" ]]; then
        local kernel_debs=0
        for pattern in "linux-headers" "linux-image" "linux-libc"; do
            if ls "${OUTPUT_DIR}/debs/${pattern}"*.deb 1>/dev/null 2>&1; then
                kernel_debs=$((kernel_debs + 1))
            fi
        done
        if [[ $kernel_debs -lt 3 ]]; then
            need_kernel=true
        fi
    fi

    # if kernel not found, offer to use default output
    if [[ "${need_kernel}" == "true" ]]; then
        echo ""
        print_warn "Kernel packages not found in ${OUTPUT_DIR}/"
        echo ""

        if [[ -f "${DEFAULT_OUTPUT_DIR}/cix/Image" ]]; then
            echo "=============================================="
            echo "  Default Kernel Available"
            echo "=============================================="
            echo ""
            echo "A pre-built kernel (6.6.8.9-cix) is available in:"
            echo "  ${DEFAULT_OUTPUT_DIR}/"
            echo ""
            echo "This kernel will be copied to ${OUTPUT_DIR}/"
            echo ""
            read -p "Use default kernel? (Y/n): " use_default

            if [[ "${use_default}" != "n" && "${use_default}" != "N" ]]; then
                print_msg "Copying default kernel to output directory..."
                mkdir -p "${OUTPUT_DIR}"
                cp -rf "${DEFAULT_OUTPUT_DIR}/cix" "${OUTPUT_DIR}/"
                cp -rf "${DEFAULT_OUTPUT_DIR}/debs" "${OUTPUT_DIR}/"
                print_msg "Default kernel copied successfully."
            else
                print_error "Kernel packages required but not found."
                print_error "Please build the kernel first using: ./build.sh"
                exit 1
            fi
        else
            print_error "Kernel Image not found at ${OUTPUT_DIR}/cix/Image"
            print_error "Default kernel also not found at ${DEFAULT_OUTPUT_DIR}/"
            print_error "Please build the kernel first using: ./build.sh"
            exit 1
        fi
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
    echo "  3) Custom image from ubuntu/images/ folder"
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
        3)
            select_custom_image
            ;;
        *)
            print_error "Invalid choice. You escaped the ritual... this time."
            exit 1
            ;;
    esac

    UBUNTU_VERSION="${DISTRO_VERSION}"

    print_msg "Selected ${DISTRO_TYPE} ${DISTRO_VERSION}"
}

select_custom_image() {
    echo ""
    echo "=============================================="
    echo "  Custom Image Selection"
    echo "=============================================="
    echo ""
    echo "Scanning ${IMAGES_DIR}/ for images..."
    echo ""

    local found_images=()
    local i=1

    while IFS= read -r -d '' img; do
        found_images+=("$img")
    done < <(find "${IMAGES_DIR}" -maxdepth 1 -type f -name "*.img" -print0 2>/dev/null | sort -z)

    while IFS= read -r -d '' img; do
        found_images+=("$img")
    done < <(find "${IMAGES_DIR}" -maxdepth 1 -type f \( -name "*.img.xz" -o -name "*.img.gz" -o -name "*.img.zst" \) -print0 2>/dev/null | sort -z)

    while IFS= read -r -d '' img; do
        found_images+=("$img")
    done < <(find "${IMAGES_DIR}" -maxdepth 1 -type f \( -name "*.tar" -o -name "*.tar.gz" -o -name "*.tar.xz" -o -name "*.tgz" \) -print0 2>/dev/null | sort -z)

    while IFS= read -r -d '' img; do
        found_images+=("$img")
    done < <(find "${IMAGES_DIR}" -maxdepth 1 -type f -name "*.zip" -print0 2>/dev/null | sort -z)

    if [[ ${#found_images[@]} -eq 0 ]]; then
        print_error "No images found in ${IMAGES_DIR}/"
        echo ""
        echo "Supported formats:"
        echo "  - Raw images: *.img"
        echo "  - Compressed: *.img.xz, *.img.gz, *.img.zst"
        echo "  - Archives:   *.tar, *.tar.gz, *.tar.xz, *.tgz, *.zip"
        echo ""
        echo "Place your image file in: ${IMAGES_DIR}/"
        exit 1
    fi

    echo "Found images:"
    echo ""
    for img in "${found_images[@]}"; do
        local basename_img
        basename_img=$(basename "$img")
        local size
        size=$(du -h "$img" 2>/dev/null | cut -f1)
        printf "  %2d) %s (%s)\n" "$i" "$basename_img" "$size"
        i=$((i + 1))
    done
    echo ""

    read -p "Select image [1-${#found_images[@]}]: " img_choice

    if ! [[ "$img_choice" =~ ^[0-9]+$ ]] || [[ "$img_choice" -lt 1 ]] || [[ "$img_choice" -gt ${#found_images[@]} ]]; then
        print_error "Invalid selection. The void claims another soul."
        exit 1
    fi

    local selected_img="${found_images[$((img_choice - 1))]}"
    local selected_basename
    selected_basename=$(basename "$selected_img")

    echo ""
    print_warn "=============================================="
    print_warn "  WARNING: Custom Image Selected"
    print_warn "=============================================="
    echo ""
    print_warn "You selected: ${selected_basename}"
    echo ""
    print_warn "I don't know if it'll work. Honestly."
    print_warn "Compatibility is NOT guaranteed."
    print_warn "You may encounter boot failures, driver issues,"
    print_warn "or a beautiful void of nothingness."
    echo ""
    read -p "Proceed anyway? (y/N): " confirm_custom

    if [[ "${confirm_custom}" != "y" && "${confirm_custom}" != "Y" ]]; then
        print_msg "Wise choice. You live to fight another day."
        exit 0
    fi

    # for custom image unzip and place
    CUSTOM_IMG_ORIGINAL="$selected_img"
    CUSTOM_IMG_NEEDS_EXTRACT="false"

    if [[ "$selected_basename" == *.img ]]; then
        DISTRO_IMG_NAME="$selected_basename"
    elif [[ "$selected_basename" == *.img.xz ]]; then
        DISTRO_IMG_NAME="${selected_basename%.xz}"
        CUSTOM_IMG_NEEDS_EXTRACT="true"
        CUSTOM_IMG_COMPRESSION="xz"
    elif [[ "$selected_basename" == *.img.gz ]]; then
        DISTRO_IMG_NAME="${selected_basename%.gz}"
        CUSTOM_IMG_NEEDS_EXTRACT="true"
        CUSTOM_IMG_COMPRESSION="gz"
    elif [[ "$selected_basename" == *.img.zst ]]; then
        DISTRO_IMG_NAME="${selected_basename%.zst}"
        CUSTOM_IMG_NEEDS_EXTRACT="true"
        CUSTOM_IMG_COMPRESSION="zst"
    elif [[ "$selected_basename" == *.tar ]] || [[ "$selected_basename" == *.tar.gz ]] || [[ "$selected_basename" == *.tar.xz ]] || [[ "$selected_basename" == *.tgz ]]; then
        CUSTOM_IMG_NEEDS_EXTRACT="true"
        CUSTOM_IMG_COMPRESSION="tar"
    elif [[ "$selected_basename" == *.zip ]]; then
        CUSTOM_IMG_NEEDS_EXTRACT="true"
        CUSTOM_IMG_COMPRESSION="zip"
    fi

    DISTRO_TYPE="custom"
    DISTRO_VERSION="custom"
    DISTRO_CODENAME="unknown"
    DISTRO_URL=""

    # GRUB entry name
    echo ""
    echo "=============================================="
    echo "  GRUB Boot Entry Name"
    echo "=============================================="
    echo ""
    echo "Specify a name for the boot menu entry."
    echo "This will appear in the GRUB boot menu."
    echo ""
    echo "Examples: 'Debian Trixie', 'Manjaro ARM', 'DragonOS'"
    echo ""
    read -p "Boot entry name [Custom Linux]: " custom_grub_entry

    if [[ -z "${custom_grub_entry}" ]]; then
        CUSTOM_GRUB_ENTRY="Custom Linux"
    else
        CUSTOM_GRUB_ENTRY="${custom_grub_entry}"
    fi

    print_msg "Boot entry will be: '0 ${CUSTOM_GRUB_ENTRY} (ACPI)'"
    print_msg "Selected: ${selected_basename}"
}

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

    elif [[ "${DISTRO_TYPE}" == "custom" ]]; then
        if [[ "${CUSTOM_IMG_NEEDS_EXTRACT}" == "true" ]]; then
            print_msg "Extracting custom image..."

            case "${CUSTOM_IMG_COMPRESSION}" in
                xz)
                    print_msg "Decompressing xz archive..."
                    xz -dk "${CUSTOM_IMG_ORIGINAL}"
                    ;;
                gz)
                    print_msg "Decompressing gzip archive..."
                    gunzip -k "${CUSTOM_IMG_ORIGINAL}"
                    ;;
                zst)
                    print_msg "Decompressing zstd archive..."
                    if command -v zstd &>/dev/null; then
                        zstd -dk "${CUSTOM_IMG_ORIGINAL}"
                    else
                        print_error "zstd not installed. Please install: apt install zstd"
                        exit 1
                    fi
                    ;;
                tar)
                    print_msg "Extracting tar archive..."
                    tar -xf "${CUSTOM_IMG_ORIGINAL}" -C "${IMAGES_DIR}/"
                    local extracted_img
                    extracted_img=$(find "${IMAGES_DIR}" -maxdepth 1 -name "*.img" -newer "${CUSTOM_IMG_ORIGINAL}" | head -1)
                    if [[ -n "$extracted_img" ]]; then
                        DISTRO_IMG="$extracted_img"
                        DISTRO_IMG_NAME="$(basename "$extracted_img")"
                        print_msg "Image extracted: ${DISTRO_IMG}"
                    else
                        print_error "Failed to find extracted .img file from tar archive"
                        exit 1
                    fi
                    ;;
                zip)
                    print_msg "Extracting zip archive..."
                    unzip -o "${CUSTOM_IMG_ORIGINAL}" -d "${IMAGES_DIR}/"
                    local extracted_img
                    extracted_img=$(find "${IMAGES_DIR}" -maxdepth 1 -name "*.img" -newer "${CUSTOM_IMG_ORIGINAL}" | head -1)
                    if [[ -n "$extracted_img" ]]; then
                        DISTRO_IMG="$extracted_img"
                        DISTRO_IMG_NAME="$(basename "$extracted_img")"
                        print_msg "Image extracted: ${DISTRO_IMG}"
                    else
                        print_error "Failed to find extracted .img file from zip archive"
                        exit 1
                    fi
                    ;;
                *)
                    print_error "Unknown compression type: ${CUSTOM_IMG_COMPRESSION}"
                    exit 1
                    ;;
            esac

            if [[ "${CUSTOM_IMG_COMPRESSION}" != "tar" && "${CUSTOM_IMG_COMPRESSION}" != "zip" ]]; then
                DISTRO_IMG="${IMAGES_DIR}/${DISTRO_IMG_NAME}"
            fi

            if [[ ! -f "${DISTRO_IMG}" ]]; then
                print_error "Extracted image not found: ${DISTRO_IMG}"
                exit 1
            fi

            print_msg "Custom image ready: ${DISTRO_IMG}"
        else
            DISTRO_IMG="${CUSTOM_IMG_ORIGINAL}"
            print_msg "Custom image ready: ${DISTRO_IMG}"
        fi
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

# various user settings
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

    # DKMS option and auto-login
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
    echo "NPU Driver Installation Options:"
    echo "  NPU (Neural Processing Unit) enables AI/ML acceleration."
    echo "  Requires DKMS for kernel module build."
    echo ""
    read -p "Install NPU driver? (Y/n): " npu_choice
    if [[ "${npu_choice}" == "n" || "${npu_choice}" == "N" ]]; then
        INSTALL_NPU="false"
        print_msg "NPU driver disabled."
    else
        INSTALL_NPU="true"
        print_msg "NPU driver enabled. The silicon awakens..."
    fi

    echo ""
    echo "VPU Driver Installation Options:"
    echo "  VPU (Video Processing Unit) enables hardware video encoding/decoding."
    echo "  Requires DKMS for kernel module build."
    echo ""
    read -p "Install VPU driver? (Y/n): " vpu_choice
    if [[ "${vpu_choice}" == "n" || "${vpu_choice}" == "N" ]]; then
        INSTALL_VPU="false"
        print_msg "VPU driver disabled."
    else
        INSTALL_VPU="true"
        print_msg "VPU driver enabled. Video streams shall flow..."
    fi

    echo ""
    echo "Display Manager Auto-login:"
    echo "  Enable automatic login for ${NEW_USER}?"
    echo "  This is convenient but less secure."
    echo ""
    read -p "Enable GDM3 auto-login? (Y/n): " autologin_choice
    if [[ "${autologin_choice}" == "n" || "${autologin_choice}" == "N" ]]; then
        ENABLE_AUTOLOGIN="false"
        print_msg "Auto-login disabled. You'll need to login manually."
    else
        ENABLE_AUTOLOGIN="true"
        print_msg "Auto-login enabled for ${NEW_USER}."
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
        # Update partition info
        sed -i "s|resume=PARTUUID=[^ ]* noresume root=/dev/[^ ]*|resume=PARTUUID=${PART2_PARTUUID} noresume root=/dev/${PART2_DEVNAME}|g" "${GRUB_CFG}"

        # Update boot entry name for custom images
        if [[ "${DISTRO_TYPE}" == "custom" && -n "${CUSTOM_GRUB_ENTRY}" ]]; then
            sed -i "s|menuentry '0 Ubuntu (ACPI)'|menuentry '0 ${CUSTOM_GRUB_ENTRY} (ACPI)'|g" "${GRUB_CFG}"
            print_msg "GRUB entry name set to: 0 ${CUSTOM_GRUB_ENTRY} (ACPI)"
        fi

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

    # Setup resolv.conf for apt
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

    # chroot worker task set
    cat > "${ROOTFS_MOUNT}/tmp/setup.sh" << CHROOT_SCRIPT
#!/bin/bash
set -e
export DEBIAN_FRONTEND=noninteractive

NEW_USER="${NEW_USER}"
NEW_PASSWORD="${NEW_PASSWORD}"
INSTALL_DKMS="${INSTALL_DKMS}"
INSTALL_NPU="${INSTALL_NPU}"
INSTALL_VPU="${INSTALL_VPU}"
ENABLE_AUTOLOGIN="${ENABLE_AUTOLOGIN}"

echo "[INFO] Updating package lists..."
apt update

echo "[INFO] Installing kernel packages..."
cd /opt/kernel_packages
apt install -y ./*.deb || true

echo "[INFO] Installing CIX firmware and environment..."
cd /opt/debs
dpkg -i cix-firmware*.deb || true
dpkg -i --force-overwrite cix-env*.deb || true

echo "[INFO] Installing GPU drivers (cix-go)..."
cd /opt/debs/cix-go
if [[ "\${INSTALL_DKMS}" == "true" ]]; then
    echo "[INFO] DKMS enabled, running full GPU installation..."
    ./install.sh --dkms || true
else
    echo "[INFO] DKMS disabled, running minimal GPU installation..."
    ./install.sh --no-dkms || true
fi

# NPU Driver Installation
if [[ "\${INSTALL_NPU}" == "true" ]]; then
    echo "[INFO] Installing NPU drivers..."
    cd /opt/debs
    apt install -y python3-pip || true
    dpkg -i cix-npu-driver*.deb || true
    dpkg -i cix-noe-umd*.deb || true

    echo "[INFO] Building NPU DKMS module..."
    apt install -y dkms || true
    dkms add -m aipu -v 5.11.0 || true
    dkms build -m aipu -v 5.11.0 || true
    dkms install -m aipu -v 5.11.0 --force || true
    echo "[INFO] NPU driver installation complete."
else
    echo "[INFO] NPU driver installation skipped."
fi

# VPU Driver Installation
if [[ "\${INSTALL_VPU}" == "true" ]]; then
    echo "[INFO] Installing VPU drivers..."
    cd /opt/debs
    dpkg -i cix-vpu-driver*.deb || true
    dpkg -i cix-vpu-test*.deb || true

    echo "[INFO] Building VPU DKMS module..."
    apt install -y dkms || true
    dkms add -m cix-vpu-driver -v 1.0.0 || true
    dkms build -m cix-vpu-driver -v 1.0.0 || true
    dkms install -m cix-vpu-driver -v 1.0.0 --force || true
    echo "[INFO] VPU driver installation complete."
else
    echo "[INFO] VPU driver installation skipped."
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

if [[ "\${ENABLE_AUTOLOGIN}" == "true" ]]; then
    echo "[INFO] Configuring GDM3 autologin for \${NEW_USER}..."
    mkdir -p /etc/gdm3
    cat >> /etc/gdm3/custom.conf << EOF
[daemon]
AutomaticLoginEnable=true
AutomaticLogin=\${NEW_USER}
EOF
    echo "[INFO] GDM3 autologin enabled."
else
    echo "[INFO] GDM3 autologin disabled. Manual login required."
fi

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
  
  # deps
  sudo apt install dkms python3-pip # if not already installed
  
  # for GPU
  cd /opt/debs/cix-go
  sudo ./install.sh
  
  # for NPU
  cd /opt/debs
  sudo dpkg -i cix-npu-driver_*_arm64.deb
  sudo dpkg -i cix-noe-umd_*_arm64.deb
  sudo dkms add -m  aipu -v 5.11.0  
  sudo dkms build -m aipu -v 5.11.0 
  sudo dkms install -m aipu -v 5.11.0 --force

  # for VPU
  sudo dpkg -i cix-vpu-driver_*_arm64.deb
  sudo dpkg -i cix-vpu-test_*_arm64.deb
  sudo dkms add -m cix-vpu-driver -v 1.0.0
  sudo dkms build -m cix-vpu-driver -v 1.0.0
  sudo dkms install -m cix-vpu-driver -v 1.0.0 --force

  # for WLAN/Bluetooth
  sudo dpkg –i cix-wlan_xxx_arm64.deb
  sudo dpkg –i cix-bt-driver_xxx_arm64.deb
  sudo depmod -a

  # if not appearing in wifi
  sudo sed -i 's/NAME=\"$env{ID_NET_NAME}\"/NAME=\"$env{ID_NET_SLOT}\"/' /usr/lib/udev/rules.d/80-net-setup-link.rules
  sudo sed -i "/ACTION!=\"add|change|move\",/aENV{INTERFACE}==\"*p2p*\",  ENV{NM_UNMANAGED}=\"1\"" /usr/lib/udev/rules.d/85-nm-unmanaged.rules

KNOWN ISSUES (under investigation):
  - WebGL does not work in browsers
  - X11/SDDM/LightDM may fail to start the desktop environment
  - Some distros may have dependency issues with certain driver packages

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
    echo "  Ubuntu Bootloader Injector for Orange Pi6 V0.3"
    echo "=============================================="
    echo ""
    echo "UPDATE: Image chooser now supports custom images!"
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
