#!/bin/bash

#cvi_pinmux -w CAM_MCLK2/IIC7_SDA
#cvi_pinmux -w CAM_MCLK3/IIC7_SCL
#cvi_pinmux -w CAM_MCLK4/IIC8_SDA
#cvi_pinmux -w CAM_MCLK5/IIC8_SCL
#cvi_pinmux -w PAD_MIPI_RX16N/GPIO171
#cvi_pinmux -w PAD_MIPI_RX16P/GPIO170
#cvi_pinmux -w PAD_MIPI_RX17P/GPIO172
#cvi_pinmux -w PAD_MIPI_RX17N/GPIO173

echo 331 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio331/direction
echo 0 > /sys/class/gpio/gpio331/value
echo 1 > /sys/class/gpio/gpio331/value

echo 330 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio330/direction
echo 0 > /sys/class/gpio/gpio330/value
echo 1 > /sys/class/gpio/gpio330/value

echo 332 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio332/direction
echo 0 > /sys/class/gpio/gpio332/value
echo 1 > /sys/class/gpio/gpio332/value

echo 333 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio333/direction
echo 0 > /sys/class/gpio/gpio333/value
echo 1 > /sys/class/gpio/gpio333/value
