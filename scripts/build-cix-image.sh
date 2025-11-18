#!/usr/bin/env bash

#  Copyright 2024 Cix Technology Group Co., Ltd.
#  All Rights Reserved.
#
#  The following programs are the sole property of Cix Technology Group Co., Ltd.,
#  and contain its proprietary and confidential information.
#

function replace_line_if_exist() {
    local src=$1
    local dst=$2
    local file=$3

    if [[ -e "$file" ]]; then
        set +E
        local res=`grep -n "${src}" "${file}"`
        #echo "res:$res"
        if [[ ${#res} -gt 0 ]]; then
            line=`echo "${res}" | cut -d ":" -f 1`
        fi
        set -E
        #echo "line: $line"
        if [[ $line -gt 0 ]]; then
            sed -i "${line}c${dst}" "${file}"
        fi
    fi
}

function create_cix_rootfs()
{
    display_alert "Preparing" "cix rootfs" "info"

    local PATH_OUT=${SRC}/output/cix
    local ROOT_FREE_SIZE=4096000
    rootfs_ext4=${PATH_OUT}/images/rootfs.ext4

    mkdir -p ${PATH_OUT}/images > /dev/null 2>&1

    mount --bind --make-private $SDCARD $MOUNT/

    local debianSize=`sudo du ${MOUNT} -d 0 -k | sudo awk -F ' ' '{print $1}'`
    debianSize=`expr $debianSize \* 13 / 10`
    if [[ ${debianSize} -lt 1024 ]]; then
        totalsize='10240'
    elif [[ ${debianSize} -lt 10240 ]]; then
        totalsize=`expr $debianSize \* 50 / 10`
    elif [[ ${debianSize} -lt 102400 ]]; then
        totalsize=`expr $debianSize \* 25 / 10`
    elif [[ ${debianSize} -lt 1024000 ]]; then
        totalsize=`expr $debianSize \* 20 / 10`
    else
        totalsize=`expr $debianSize + ${ROOT_FREE_SIZE:-4096000}`
    fi

    local count=`expr $totalsize / 4`
    root_uuid=$(uuidgen)
    boot_uuid=$(uuidgen | tr -d '-' | cut -c1-8 | tr 'a-f' 'A-F')
    formatted_boot_uuid="${boot_uuid:0:4}-${boot_uuid:4:4}"
    cat <<-END > ${SDCARD}/etc/fstab
# /etc/fstab: static file system information.
#
# Use 'blkid' to print the universally unique identifier for a device; this may
# be used with UUID= as a more robust way to name devices that works even if
# disks are added and removed. See fstab(5).
#
# <file system>                           <mount point>   <type>  <options>       <dump>  <pass>

END
    #echo "UUID=${formatted_boot_uuid} /boot vfat defaults,umask=0077 0 2" >> $MOUNT/etc/fstab
    #echo "UUID=$root_uuid / ext4 errors=remount-ro 0 1" >> $MOUNT/etc/fstab

    dd if=/dev/zero of="${rootfs_ext4}" bs=4k count=$count > /dev/null 2>&1
    #mkfs.ext4 -U ${uuid} -F -i 4096 "${rootfs_ext4}" -d "${MOUNT}" > /dev/null 2>&1
    mkfs.ext4 -F -i 4096 "${rootfs_ext4}" -d "${MOUNT}" > /dev/null 2>&1
    fsck.ext4 -pvfD "${rootfs_ext4}" > /dev/null 2>&1

    umount $MOUNT
    rm -rf $MOUNT
}

function create_cix_image()
{
    display_alert "Creating" "cix image" "info"

    local PATH_OUT=${SRC}/output/cix
    local gpt_btp=${PATH_OUT}/images/partition.bpt
    local SCRIPT_DIR=${EXTER}/cache/sources/component_cix-${BRANCH}

    if [[ $SELECTED_CONFIGURATION == "cli_standard" ]]; then
        IMAGE_TYPE=server
    elif [[ $SELECTED_CONFIGURATION == "cli_minimal" ]]; then
        IMAGE_TYPE=minimal
    else
        IMAGE_TYPE=desktop
    fi
    version="${BOARD^}_${REVISION}_${DISTRIBUTION,}_${RELEASE}_${IMAGE_TYPE}"${DESKTOP_ENVIRONMENT:+_$DESKTOP_ENVIRONMENT}"_linux$(grab_version "$LINUXSOURCEDIR")"
    mkdir -p ${SRC}/output/images/${version}
    local cix_image_name="${SRC}/output/images/${version}/${version}.img"

    cp -f "${SCRIPT_DIR}/debian/fb/partition.bpt" "${gpt_btp}" || {
        display_alert "Failed to copy BPT template" "${gpt_btp}" "err"
        return 1
    }

    local boot_start=$(expr $(sed -n '6p' ${gpt_btp} | awk '{print $2}') / 1024 / 1024)
    local boot_size=$(expr $(sed -n '11p' ${gpt_btp} | awk -F '"' '{print $4}' | awk '{print $1}'))

    if [[ ! -f "${rootfs_ext4}" ]]; then
        display_alert "Rootfs missing" "${rootfs_ext4}" "err"
        return 1
    fi

    local rootfs_ext4="${PATH_OUT}/images/rootfs.ext4"
    local root_size=`du -s -b ${rootfs_ext4} | awk '{print $1}'`
    root_size=`expr $root_size / 1024 / 1024 + 10`
    local total_size=`expr $root_size + $boot_size + $boot_start + 1`
    local root_start=`expr $boot_size + $boot_start`
    local boot_end=$root_start
    local root_end=$total_size

    #echo "total size: ${total_size}M"
    #echo "boot: ${boot_start}, ${boot_end}M"
    #echo "rootfs: ${root_start}, ${root_end}M"

    replace_line_if_exist "\"disk_size\":" "\ \ \ \ \ \ \ \ \"disk_size\": \"${total_size} MiB\"," "${gpt_btp}"
    sudo "${SCRIPT_DIR}/debian/fb/bpttool" make_table --input "${gpt_btp}" --output_gpt "${PATH_OUT}/images/partition-table.img" --output_json "${PATH_OUT}/partition-table.json"
    sudo chown ${USER}:${USER} "${PATH_OUT}/images/partition-table.img"

    boot_size=$(expr $boot_size \* 1024 \* 1024)
    #echo "boot_start: ${boot_start}Mib, boot_size: ${boot_size} bytes"

    cp "${SCRIPT_DIR}/debian/grub-post-silicon.cfg" "${PATH_OUT}/images/grub.cfg"
    local root_device_guid=$("${SCRIPT_DIR}/debian/cix_tool" --flash-tool -d "${PATH_OUT}/images/partition-table.img" | grep "PARTITION1, guid:" | awk -F ":" '{print $2}')
    sed -i "s:root=/dev/nvme0n1p2:root=PARTUUID=${root_device_guid}:g" "${PATH_OUT}/images/grub.cfg"
    sed -i "s:root=/dev/sda2:root=PARTUUID=${root_device_guid}:g" "${PATH_OUT}/images/grub.cfg"

    local dtb_args=()
    while IFS= read -r -d $'\0' dtb; do
        dtb_args+=("${dtb}" "/$(basename "${dtb}")")
    done < <(find "${SRC}/output/cix/" -maxdepth 1 -name "*.dtb" -print0)

    #sed -i '3cset default="0"' "${PATH_OUT}/images/grub.cfg"
    "${SCRIPT_DIR}/tools/mk-part-fat" \
        -o "${PATH_OUT}/images/boot.img" \
        -s "${boot_size}" \
        -l "ESP" \
        "${SCRIPT_DIR}/grub.efi" "/EFI/BOOT/BOOTAA64.EFI" \
        "${PATH_OUT}/images/grub.cfg" "/grub/grub.cfg" \
        "${SRC}/output/cix/Image" "/Image" \
        "${SCRIPT_DIR}/cix_binary/device/images/rootfs.cpio.gz" "/rootfs.cpio.gz" \
        "${dtb_args[@]}" > /dev/null 2>&1

    fatlabel -i "${PATH_OUT}/images/boot.img" "0x${boot_uuid}"

    dd if=/dev/zero of="${cix_image_name}" bs=1M count=$total_size > /dev/null 2>&1
    parted -s "${cix_image_name}" mklabel gpt > /dev/null 2>&1

    dd if="${PATH_OUT}/images/partition-table.img" of="${cix_image_name}" conv=notrunc,fsync bs=1M seek=0 > /dev/null 2>&1

    local boot_offset=`"${SCRIPT_DIR}/debian/cix_tool" --flash-tool -d "${PATH_OUT}/images/partition-table.img" -p boot | awk '{print $1}'`
    boot_offset=`expr $boot_offset \* 512 / 1024 / 1024`
    #echo "boot offset: $boot_offset M bytes"
    dd if="${PATH_OUT}/images/boot.img" of="${cix_image_name}" conv=notrunc,fsync bs=1M seek=$boot_offset > /dev/null 2>&1

    local rootfs_offset=`"${SCRIPT_DIR}/debian/cix_tool" --flash-tool -d "${PATH_OUT}/images/partition-table.img" -p root | awk '{print $1}'`
    rootfs_offset=`expr $rootfs_offset \* 512 / 1024 / 1024`
    #echo "root offset: $rootfs_offset M bytes"
    dd if="${rootfs_ext4}" of="${cix_image_name}" conv=notrunc,fsync bs=1M seek=$rootfs_offset > /dev/null 2>&1

    if [[ $COMPRESS_OUTPUTIMAGE == "" || $COMPRESS_OUTPUTIMAGE == no ]]; then
        COMPRESS_OUTPUTIMAGE="sha,img"
    elif [[ $COMPRESS_OUTPUTIMAGE == yes ]]; then
        COMPRESS_OUTPUTIMAGE="sha,xz"
    fi

    if [[ $COMPRESS_OUTPUTIMAGE == *xz* ]]; then
        display_alert "Compressing" "${cix_image_name}.xz" "info"
        # compressing consumes a lot of memory we don't have. Waiting for previous packing job to finish helps to run a lot more builds in parallel
        available_cpu=$(grep -c 'processor' /proc/cpuinfo)
        [[ ${available_cpu} -gt 8 ]] && available_cpu=8 # using more cpu cores for compressing is pointless
        available_mem=$(LC_ALL=c free | grep Mem | awk '{print $4/$2 * 100.0}' | awk '{print int($1)}') # in percentage

        pixz -7 -p ${available_cpu} -f $(expr ${available_cpu} + 2) < $cix_image_name > ${cix_image_name}.xz
        compression_type=".xz"
    fi

    if [[ $COMPRESS_OUTPUTIMAGE == *sha* ]]; then
        cd ${SRC}/output/images/${version}/
        display_alert "SHA256 calculating" "${version}.img${compression_type}" "info"
        sha256sum -b ${version}.img${compression_type} > ${version}.img${compression_type}.sha
    fi

    display_alert "Done building" "$cix_image_name" "info"
}
