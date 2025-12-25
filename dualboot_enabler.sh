#!/bin/bash
set -e

YELLOW='\033[1;33m'
NC='\033[0m'

enable_dualboot() {
    echo -e "${YELLOW}Select the disk of the CURRENT OS (where GRUB.CFG will be edited):"
    lsblk -d -o NAME,SIZE,MODEL | grep -v "^NAME"
    read -p "Enter device name (e.g., nvme0n1): " current_device_name

    CURRENT_DEVICE="/dev/${current_device_name}"
    if [[ ! -b "${CURRENT_DEVICE}" ]]; then
        echo "Device ${CURRENT_DEVICE} does not exist!"
        exit 1
    fi

    if [[ "${current_device_name}" == nvme* ]]; then
        CURRENT_PART1="${CURRENT_DEVICE}p1"
    else
        CURRENT_PART1="${CURRENT_DEVICE}1"
    fi

    mkdir -p /mnt/ESP
    mount "${CURRENT_PART1}" /mnt/ESP

    echo -e "${YELLOW}Select the disk where the OTHER OS is installed:"
    lsblk -d -o NAME,SIZE,MODEL | grep -v "^NAME"
    read -p "Enter device name (e.g., sda, nvme0n1): " other_device_name

    OTHER_DEVICE="/dev/${other_device_name}"
    if [[ ! -b "${OTHER_DEVICE}" ]]; then
        echo "Device ${OTHER_DEVICE} does not exist!"
        umount /mnt/ESP
        exit 1
    fi

    if [[ "${other_device_name}" == nvme* ]]; then
        OTHER_PART1="${OTHER_DEVICE}p1"
    else
        OTHER_PART1="${OTHER_DEVICE}1"
    fi

    read -p "Enter the name for the boot menu entry: " os_name

    # suffix for vfat
    echo -e "${YELLOW}Enter a suffix for the Image file (e.g., 'UBUNTU', 'ARCH')${NC}"
    echo "This will create Image-<SUFFIX> on the current ESP"
    echo "VFAT limitation: use uppercase, no spaces, max 8 chars recommended"
    read -p "Image suffix (leave empty to skip Image copy): " image_suffix

    mkdir -p /mnt/OTHER_ESP
    mount "${OTHER_PART1}" /mnt/OTHER_ESP

    OTHER_GRUB_CFG=""
    if [[ -f "/mnt/OTHER_ESP/GRUB/GRUB.CFG" ]]; then
        OTHER_GRUB_CFG="/mnt/OTHER_ESP/GRUB/GRUB.CFG"
    elif [[ -f "/mnt/OTHER_ESP/ubuntu/ESP/GRUB/GRUB.CFG" ]]; then
        OTHER_GRUB_CFG="/mnt/OTHER_ESP/ubuntu/ESP/GRUB/GRUB.CFG"
    else
        echo "Error: GRUB.CFG not found on other OS partition!"
        umount /mnt/OTHER_ESP
        umount /mnt/ESP
        exit 1
    fi

    echo "Available menu entries from other OS:"
    grep "^menuentry" "${OTHER_GRUB_CFG}" | nl -v 0
    read -p "Enter the number of the menuentry to copy (default: 0): " entry_num
    entry_num=${entry_num:-0}

    MENUENTRY_CONTENT=$(awk -v target=$entry_num '
        /^menuentry/ {
            if (count == target) {
                in_target = 1
                print
                next
            }
            count++
        }
        in_target {
            print
            if (/^}/) exit
        }
    ' "${OTHER_GRUB_CFG}")

    if [[ -z "${MENUENTRY_CONTENT}" ]]; then
        echo "Error: Could not extract menuentry content! (entry ${entry_num} does not exist)"
        umount /mnt/OTHER_ESP
        rmdir /mnt/OTHER_ESP
        umount /mnt/ESP
        exit 1
    fi

    # activate image (+suffox)
    if [[ -n "${image_suffix}" ]]; then
        image_suffix=$(echo "${image_suffix}" | tr '[:lower:]' '[:upper:]' | tr -cd 'A-Z0-9-' | cut -c1-8)
        NEW_IMAGE_NAME="IMAGE-${image_suffix}"

        if [[ -f "/mnt/OTHER_ESP/Image" ]]; then
            echo "Copying Image from other OS as ${NEW_IMAGE_NAME}..."
            cp "/mnt/OTHER_ESP/Image" "/mnt/ESP/${NEW_IMAGE_NAME}"
            echo "Image copied successfully."
        elif [[ -f "/mnt/OTHER_ESP/IMAGE" ]]; then
            echo "Copying IMAGE from other OS as ${NEW_IMAGE_NAME}..."
            cp "/mnt/OTHER_ESP/IMAGE" "/mnt/ESP/${NEW_IMAGE_NAME}"
            echo "Image copied successfully."
        else
            echo -e "${YELLOW}Warning: No Image found on other ESP, skipping copy${NC}"
            NEW_IMAGE_NAME=""
        fi
    else
        NEW_IMAGE_NAME=""
    fi

    umount /mnt/OTHER_ESP
    rmdir /mnt/OTHER_ESP

    if [[ -n "${NEW_IMAGE_NAME}" ]]; then
        MENUENTRY_CONTENT=$(echo "${MENUENTRY_CONTENT}" | sed "s|linux /[Ii][Mm][Aa][Gg][Ee][^ ]*|linux /${NEW_IMAGE_NAME}|g")
    fi

    if [[ "${other_device_name}" == nvme* ]]; then
        SUGGESTED_ROOTFS="${other_device_name}p2"
    else
        SUGGESTED_ROOTFS="${other_device_name}2"
    fi

    echo -e "${YELLOW}Rootfs device selection (fixes boot issues with PARTUUID only)${NC}"
    echo "1) Use suggested: ${SUGGESTED_ROOTFS}"
    echo "2) Enter manually"
    echo "3) Skip (keep PARTUUID only)"
    read -p "Choice [1-3]: " rootfs_choice

    case ${rootfs_choice} in
        1)
            rootfs_device="${SUGGESTED_ROOTFS}"
            ;;
        2)
            read -p "Enter rootfs device (e.g., sda2, nvme0n1p2): " rootfs_device
            ;;
        *)
            rootfs_device=""
            ;;
    esac

    if [[ -n "${rootfs_device}" ]]; then
        MENUENTRY_CONTENT=$(echo "${MENUENTRY_CONTENT}" | sed "s|rootwait|noresume root=/dev/${rootfs_device} rootwait|g")
        echo "Added: noresume root=/dev/${rootfs_device}"
    fi

    GRUB_CFG="/mnt/ESP/GRUB/GRUB.CFG"
    if [[ ! -f "${GRUB_CFG}" ]]; then
        echo "GRUB.CFG not found!"
        umount /mnt/ESP
        exit 1
    fi

    # Blame VFAT
    TMP_DIR="/tmp/grub_edit_$$"
    mkdir -p "${TMP_DIR}"
    TMP_GRUB="${TMP_DIR}/GRUB.CFG"

    cp "${GRUB_CFG}" "${TMP_GRUB}"

    TIMESTAMP=$(date +%Y%m%d%H%M%S)
    BACKUP_FILE="${TMP_DIR}/GRUB.CFG_${TIMESTAMP}.bak"
    cp "${TMP_GRUB}" "${BACKUP_FILE}"

    current_timeout=$(grep "^set timeout=" "${TMP_GRUB}" | sed 's/set timeout=//')
    echo "Current timeout: ${current_timeout} seconds"
    read -p "Enter new timeout in seconds: " new_timeout
    sed -i "s/^set timeout=.*/set timeout=${new_timeout}/" "${TMP_GRUB}"

    if grep -q "^menuentry '[0-9]" "${TMP_GRUB}"; then
        max_num=$(grep "^menuentry '[0-9]" "${TMP_GRUB}" | sed "s/menuentry '\([0-9]*\).*/\1/" | sort -n | tail -1)
        next_num=$((max_num + 1))
    else
        next_num=1
    fi

    MENUENTRY_FILE="${TMP_DIR}/new_menuentry.txt"
    echo "${MENUENTRY_CONTENT}" > "${MENUENTRY_FILE}"

    sed -i "s/^menuentry '[^']*'/menuentry '${next_num} ${os_name}'/" "${MENUENTRY_FILE}"

    awk '
    BEGIN { last_menuentry_line = 0; in_menuentry = 0 }
    /^menuentry/ { in_menuentry = 1 }
    in_menuentry && /^}/ { last_menuentry_line = NR; in_menuentry = 0 }
    { lines[NR] = $0 }
    END {
        for (i = 1; i <= NR; i++) {
            print lines[i]
        }
        if (last_menuentry_line > 0) {
            print ""
        }
    }
    ' "${TMP_GRUB}" > "${TMP_DIR}/GRUB.CFG.new"

    cat "${MENUENTRY_FILE}" >> "${TMP_DIR}/GRUB.CFG.new"
    mv "${TMP_DIR}/GRUB.CFG.new" "${TMP_GRUB}"

    # Create backup directory on ESP and copy backup
    BACKUP_DIR="/mnt/ESP/grub_backup"
    mkdir -p "${BACKUP_DIR}"
    cp "${BACKUP_FILE}" "${BACKUP_DIR}/"

    cp "${TMP_GRUB}" "${GRUB_CFG}"
    sync

    echo "Boot menu updated successfully."
    echo "Current menu entries:"
    grep -A 3 "^menuentry" "${GRUB_CFG}"

    rm -rf "${TMP_DIR}"

    umount /mnt/ESP
}

set_default_boot() {
    echo "Select the disk for GRUB.CFG:"
    lsblk -d -o NAME,SIZE,MODEL | grep -v "^NAME"
    read -p "Enter device name (e.g., sda, nvme0n1): " device_name

    TARGET_DEVICE="/dev/${device_name}"
    if [[ ! -b "${TARGET_DEVICE}" ]]; then
        echo "Device ${TARGET_DEVICE} does not exist!"
        exit 1
    fi

    if [[ "${device_name}" == nvme* ]]; then
        PART1="${TARGET_DEVICE}p1"
    else
        PART1="${TARGET_DEVICE}1"
    fi

    mkdir -p /mnt/ESP
    mount "${PART1}" /mnt/ESP

    GRUB_CFG="/mnt/ESP/GRUB/GRUB.CFG"
    if [[ ! -f "${GRUB_CFG}" ]]; then
        echo "GRUB.CFG not found!"
        umount /mnt/ESP
        exit 1
    fi

    TMP_DIR="/tmp/grub_edit_$$"
    mkdir -p "${TMP_DIR}"
    TMP_GRUB="${TMP_DIR}/GRUB.CFG"
    cp "${GRUB_CFG}" "${TMP_GRUB}"
    TIMESTAMP=$(date +%Y%m%d%H%M%S)
    BACKUP_FILE="${TMP_DIR}/GRUB.CFG_${TIMESTAMP}.bak"
    cp "${TMP_GRUB}" "${BACKUP_FILE}"

    current_timeout=$(grep "^set timeout=" "${TMP_GRUB}" | sed 's/set timeout=//')
    echo "Current timeout: ${current_timeout} seconds"
    read -p "Enter new timeout in seconds: " new_timeout
    sed -i "s/^set timeout=.*/set timeout=${new_timeout}/" "${TMP_GRUB}"

    echo "Current menu entries:"
    grep "^menuentry" "${TMP_GRUB}" | nl -v 0
    read -p "Enter the number of the entry to set as default (will move to top): " target_entry

    # Dissection of all entries
    awk '
        /^menuentry/ {
            if (in_entry) close(outfile)
            in_entry = 1
            entry_num++
            outfile = "/tmp/grub_entry_" entry_num ".txt"
        }
        in_entry {
            print > outfile
            if (/^}/) {
                close(outfile)
                in_entry = 0
            }
        }
    ' "${TMP_GRUB}"

    total_entries=$(grep -c "^menuentry" "${TMP_GRUB}")

    if [[ ${target_entry} -lt 0 || ${target_entry} -ge ${total_entries} ]]; then
        echo "Error: Invalid entry number!"
        rm -rf "${TMP_DIR}"
        rm -f /tmp/grub_entry_*.txt
        umount /mnt/ESP
        exit 1
    fi

    # Rebuild GRUB.CFG
    sed -n '1,/^menuentry/p' "${TMP_GRUB}" | sed '$d' > "${TMP_DIR}/GRUB.CFG.new"

    selected_num=$((target_entry + 1))
    cat "/tmp/grub_entry_${selected_num}.txt" >> "${TMP_DIR}/GRUB.CFG.new"

    for ((i=1; i<=total_entries; i++)); do
        if [[ $i -ne ${selected_num} ]]; then
            echo "" >> "${TMP_DIR}/GRUB.CFG.new"
            cat "/tmp/grub_entry_${i}.txt" >> "${TMP_DIR}/GRUB.CFG.new"
        fi
    done

    mv "${TMP_DIR}/GRUB.CFG.new" "${TMP_GRUB}"

    # Create backup directory on Boot Partition.
    BACKUP_DIR="/mnt/ESP/grub_backup"
    mkdir -p "${BACKUP_DIR}"
    cp "${BACKUP_FILE}" "${BACKUP_DIR}/"
    cp "${TMP_GRUB}" "${GRUB_CFG}"
    sync

    echo "Default boot entry updated successfully."
    echo "Updated menu entries (top entry will boot by default):"
    grep "^menuentry" "${GRUB_CFG}"

    rm -rf "${TMP_DIR}"
    rm -f /tmp/grub_entry_*.txt

    umount /mnt/ESP
}

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root (sudo)"
    exit 1
fi

echo -e "${YELLOW}This script enables dual boot on Orange Pi 6.${NC}"

echo "Select option:"
echo "1) Make dualboot (add another OS to boot menu)"
echo "2) Set default boot (move menuentry to top, auto-boot)"
read -p "Enter choice [1-2]: " main_choice

case $main_choice in
    1)
        enable_dualboot
        ;;
    2)
        set_default_boot
        ;;
    *)
        echo "Invalid choice."
        exit 1
        ;;
esac
