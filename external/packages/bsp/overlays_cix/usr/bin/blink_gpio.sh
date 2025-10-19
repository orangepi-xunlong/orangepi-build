#!/bin/bash

# 检查至少提供一个 GPIO 引脚
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 gpio_number1 [gpio_number2 ... gpio_numberN]"
    exit 1
fi

# 导出和设置方向为输出
for gpio in "$@"; do
    if [ ! -e "/sys/class/gpio/gpio${gpio}" ]; then
        echo "Exporting GPIO $gpio"
        echo $gpio > /sys/class/gpio/export
    fi
    echo "Setting GPIO $gpio direction to out"
    echo "out" > /sys/class/gpio/gpio${gpio}/direction
done

# 闪烁逻辑
while true; do
    # 设置 GPIO 高电平
    for gpio in "$@"; do
        echo 1 > /sys/class/gpio/gpio${gpio}/value
    done
    sleep 1  # 调整为所需的闪烁时间间隔

    # 设置 GPIO 低电平
    for gpio in "$@"; do
        echo 0 > /sys/class/gpio/gpio${gpio}/value
    done
    sleep 1  # 调整为所需的闪烁时间间隔
done
