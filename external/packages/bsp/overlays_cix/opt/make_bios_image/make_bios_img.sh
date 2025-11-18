#!/bin/bash

# BIOS镜像制作脚本

set -e

# 配置变量
IMAGE_NAME="opi6plus_bios_image_for_linux_$(date +"%Y%m%d").img"
IMAGE_SIZE="100"  # 100MB镜像
MOUNT_POINT="/tmp/bios_mount"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

info() { echo -e "${GREEN}[INFO]${NC} $1"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 检查依赖
check_dependencies() {
    local deps=("dosfstools" "parted")
    for dep in "${deps[@]}"; do
        if ! command -v $dep &> /dev/null; then
            info "安装依赖: $dep"
            sudo apt-get install -y $dep
        fi
    done
}

# 清理函数
cleanup() {
    if mountpoint -q "$MOUNT_POINT"; then
        sudo umount "$MOUNT_POINT" 2>/dev/null || true
    fi
    if [ -n "$LOOP_DEVICE" ]; then
        sudo losetup -d "$LOOP_DEVICE" 2>/dev/null || true
    fi
    if [ -d "$MOUNT_POINT" ]; then
        sudo rmdir "$MOUNT_POINT" 2>/dev/null || true
    fi
}

trap cleanup EXIT

main() {
    info "开始制作BIOS镜像..."

    check_dependencies

    if [ ! -d "bios_flash" ] || [ ! -d "EFI" ]; then
        error "缺少必要的文件夹: bios_flash 或 EFI"
        exit 1
    fi

    # 删除已存在的镜像文件
    [ -f "$IMAGE_NAME" ] && rm -f "$IMAGE_NAME"

    # 1. 创建100MB镜像文件
    info "创建${IMAGE_SIZE}MB空白镜像..."
    dd if=/dev/zero of="$IMAGE_NAME" bs=1M count=$IMAGE_SIZE status=progress
    sync

    # 2. 使用parted创建分区
    info "创建分区表和分区..."
    sudo parted "$IMAGE_NAME" --script mklabel msdos
    sudo parted "$IMAGE_NAME" --script mkpart primary fat32 1MiB 100%
    sudo parted "$IMAGE_NAME" --script set 1 boot on

    sync
    sleep 2

    # 3. 设置循环设备
    info "设置循环设备..."
    LOOP_DEVICE=$(sudo losetup --find --show --partscan "$IMAGE_NAME")
    PARTITION="${LOOP_DEVICE}p1"
    info "使用循环设备: $LOOP_DEVICE"
    info "分区: $PARTITION"

    sleep 2

    # 4. 使用wipefs清除文件系统签名（更安全的方法）
    info "清除文件系统签名..."
    if command -v wipefs > /dev/null; then
        sudo wipefs -a "$PARTITION"
    else
        warn "wipefs不可用，跳过清除签名步骤"
    fi

    # 5. 格式化分区
    info "使用gparted相同命令格式化..."
    sudo mkfs.fat -F32 -v -I -n "BIOSFLASH" "$PARTITION"

    # 6. 挂载分区
    info "挂载分区..."
    sudo mkdir -p "$MOUNT_POINT"
    sudo mount "$PARTITION" "$MOUNT_POINT"
    
    # 7. 复制文件
    info "复制bios_flash文件夹..."
    sudo cp -r bios_flash "$MOUNT_POINT/"
    info "复制EFI文件夹..."
    sudo cp -r EFI "$MOUNT_POINT/"

    # 8. 验证文件
    info "验证文件复制..."
    if [ -d "$MOUNT_POINT/bios_flash" ] && [ -d "$MOUNT_POINT/EFI" ]; then
        info "✅ 文件复制成功"
        echo "=== 文件列表 ==="
        ls -la "$MOUNT_POINT"
        echo ""
        echo "=== 文件大小 ==="
        du -sh "$MOUNT_POINT"/*
    else
        error "❌ 文件复制失败"
        exit 1
    fi

    # 9. 同步并卸载
    info "同步数据..."
    sync
    sudo umount "$MOUNT_POINT"
    sudo losetup -d "$LOOP_DEVICE"

    # 10. 最终验证
    info "最终验证..."
    echo "=== 分区信息 ==="
    sudo parted "$IMAGE_NAME" --script print
    
    echo "=== 文件系统信息 ==="
    sudo losetup --find --show --partscan "$IMAGE_NAME" > /tmp/loopdev
    LOOP_DEVICE=$(cat /tmp/loopdev)
    echo "文件系统检查:"
    sudo fsck.fat -v "${LOOP_DEVICE}p1" || true
    sudo losetup -d "$LOOP_DEVICE"
    rm -f /tmp/loopdev

    echo ""
    info "✅ 镜像制作完成: $IMAGE_NAME"
    info "📊 文件大小: $(du -h $IMAGE_NAME | cut -f1)"
    echo ""
    info "🔥 烧录命令:"
    echo "sudo dd if=$IMAGE_NAME of=/dev/sdX bs=4M status=progress oflag=sync"
    echo ""
    info "💡 测试方法:"
    echo "1. 烧录到U盘: sudo dd if=$IMAGE_NAME of=/dev/sdX bs=4M status=progress oflag=sync"
    echo "2. 在Linux中检查: ls /media/\$USER/BIOSFLASH/"
    echo "3. 在Windows中应该能看到bios_flash和EFI文件夹"
    echo ""
    warn "⚠️  注意: 将 /dev/sdX 替换为正确的U盘设备路径！"
}

echo "=================================================="
info "   BIOS镜像制作脚本"
echo "=================================================="

info "当前目录结构:"
tree -L 2

echo ""
read -p "是否继续? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 0
fi

main
