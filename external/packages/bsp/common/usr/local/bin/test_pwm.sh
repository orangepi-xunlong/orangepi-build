#!/bin/bash

base_dir="/sys/devices/platform"
pwm_base="$1"
[[ ! -d "$base_dir"/"$pwm_base".pwm ]] && echo "pwm not found !" && exit

cd "$base_dir"/"$pwm_base".pwm/pwm/pwmchip*

echo 0 > export
echo 20000000 > pwm0/period
echo 10000000 > pwm0/duty_cycle
echo 1 > pwm0/enable
echo 0 > unexport
cd - 2>&1 > /dev/null
