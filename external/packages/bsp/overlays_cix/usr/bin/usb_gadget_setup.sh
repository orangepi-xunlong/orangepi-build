#!/bin/bash

# USB Gadget 配置脚本
# 功能：将设备配置为 USB NCM (网络控制模型) 设备

set -e  # 遇到任何错误立即退出

GADGET_DIR="/sys/kernel/config/usb_gadget/g1"

echo "正在设置 USB NCM Gadget..."

# 检查是否以 root 权限运行
if [ "$EUID" -ne  0 ]; then
    echo "错误：请使用 sudo 或以 root 用户运行此脚本"
    exit 1
fi

# 检查 configfs 是否已挂载
if ! mountpoint -q /sys/kernel/config; then
    echo "挂载 configfs..."
    mount -t configfs none /sys/kernel/config || {
        echo "错误：无法挂载 configfs"
        exit 1
    }
fi

# 清理现有配置（如果存在）
if [ -d "$GADGET_DIR" ]; then
    echo "清理现有 gadget 配置..."
    echo "" > "$GADGET_DIR/UDC" 2>/dev/null || true
    rm -f "$GADGET_DIR/configs/c.1/ncm.usb0"
    rmdir "$GADGET_DIR/configs/c.1/strings/0x409" 2>/dev/null || true
    rmdir "$GADGET_DIR/configs/c.1" 2>/dev/null || true
    rmdir "$GADGET_DIR/functions/ncm.usb0" 2>/dev/null || true
    rmdir "$GADGET_DIR/strings/0x409" 2>/dev/null || true
    rmdir "$GADGET_DIR" 2>/dev/null || true
    sleep 1
fi

# 创建 gadget 目录
echo "创建 gadget 目录..."
mkdir "$GADGET_DIR"

# 设置 USB 设备描述符
echo "设置设备描述符..."
echo 0x35b1 > "$GADGET_DIR/idVendor"
echo 0x0009 > "$GADGET_DIR/idProduct"
echo 0x0200 > "$GADGET_DIR/bcdDevice"
echo 0x0210 > "$GADGET_DIR/bcdUSB"
echo "super-speed-plus" > "$GADGET_DIR/max_speed"

# 设置字符串描述符
echo "设置字符串描述符..."
mkdir -m 0770 "$GADGET_DIR/strings/0x409"
echo "0123456789ABCDEF" > "$GADGET_DIR/strings/0x409/serialnumber"
echo "cix" > "$GADGET_DIR/strings/0x409/manufacturer"
echo "USB ncm Device" > "$GADGET_DIR/strings/0x409/product"

# 创建配置
echo "创建配置..."
mkdir -m 0770 "$GADGET_DIR/configs/c.1"
mkdir -m 0770 "$GADGET_DIR/configs/c.1/strings/0x409"
echo "ncm" > "$GADGET_DIR/configs/c.1/strings/0x409/configuration"

# 创建 NCM 功能
echo "创建 NCM 功能..."
mkdir "$GADGET_DIR/functions/ncm.usb0"

# 将功能链接到配置
echo "链接功能到配置..."
ln -s "$GADGET_DIR/functions/ncm.usb0" "$GADGET_DIR/configs/c.1/"

# 启用 gadget
echo "启用 USB 设备控制器..."
UDC_DEVICE=$(ls /sys/class/udc/ | head -n1)
if [ -n "$UDC_DEVICE" ]; then
    echo "$UDC_DEVICE" > "$GADGET_DIR/UDC"
    echo "已启用 UDC: $UDC_DEVICE"
else
    echo "警告：未找到可用的 UDC 设备"
fi

echo "USB NCM Gadget 配置完成！"
echo "设备现在应该被识别为 USB 网络设备"
