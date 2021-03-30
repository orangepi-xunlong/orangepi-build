#!/bin/bash
#
# SPDX-License-Identifier: GPL-2.0

function do_prepare()
{
	export PATH=$EXTER/packages/pack-uboot/${BOARDFAMILY}/tools/:$PATH
	cp ${EXTER}/packages/pack-uboot/${BOARDFAMILY}/bin/* . -r
	cp sys_config/sys_config_${BOARD}.fex sys_config.fex
}

function do_ini_to_dts()
{
	local DTC_COMPILER=$EXTER/packages/pack-uboot/${BOARDFAMILY}/tools/dtc
	[[ ! -f $DTC_COMPILER ]] && exit_with_error "Script_to_dts: Can not find dtc compiler."

	if [[ $BOARDFAMILY =~ sun50iw2|sun50iw6|sun50iw9 ]]; then

		#Disbale noisy checks
		local DTC_FLAGS="-W no-unit_address_vs_reg"

		if [[ $BOARDFAMILY =~ sun50iw2 ]]; then
			$DTC_COMPILER ${DTC_FLAGS} -O dtb -o ${BOARD}-u-boot.dtb -b 0 dts/${BOARD}-u-boot.dts >/dev/null 2>&1
		else
			$DTC_COMPILER -p 2048 ${DTC_FLAGS} -@ -O dtb -o ${BOARD}-u-boot.dtb -b 0 dts/${BOARD}-u-boot.dts >/dev/null 2>&1
		fi

		[[ $? -ne 0 ]] && exit_with_error "dtb: Conver script to dts failed."

		# It'is used for debug dtb
		$DTC_COMPILER ${DTC_FLAGS} -I dtb -O dts -o .${BOARD}-u-boot.dts ${BOARD}-u-boot.dtb >/dev/null 2>&1
	fi
}

function do_common()
{
	unix2dos sys_config.fex > /dev/null 2>&1
	script  sys_config.fex  > /dev/null 2>&1
	cp ${PACKOUT_DIR}/${BOARD}-u-boot.dtb sunxi.fex
	[[ $BOARDFAMILY == sun50iw2 ]] && update_uboot_fdt u-boot.fex sunxi.fex u-boot.fex >/dev/null

	if [[ $BOARDFAMILY =~ sun50iw6|sun50iw9 ]]; then
		update_dtb sunxi.fex 4096 >/dev/null
	fi
	
	if [[ $BOARDFAMILY =~ sun50iw6|sun50iw2 ]]; then
		cp -f sys_config.bin config.fex
		update_scp scp.fex sunxi.fex > /dev/null 2>&1
	fi

	update_boot0 boot0_sdcard.fex	sys_config.bin SDMMC_CARD > /dev/null
	if [[ $BOARDFAMILY =~ sun50iw6|sun50iw9 ]]; then
		update_uboot -no_merge u-boot.fex sys_config.bin > /dev/null
	elif [[ $BOARDFAMILY =~ sun50iw2 ]]; then
		update_uboot u-boot.fex sys_config.bin > /dev/null
	fi
	[[ $? -ne 0 ]] && exit_with_error "update u-boot run error"

	#pack boot package
	unix2dos boot_package.cfg > /dev/null 2>&1
	dragonsecboot -pack boot_package.cfg > /dev/null
	[[ $? -ne 0 ]] && exit_with_error "dragon pack error"

	#Here, will check if need to used multi config.fex or not
	if [[ $BOARDFAMILY == sun50iw2 ]]; then
		update_uboot_v2 u-boot.fex sys_config.bin ${CHIP_BOARD} 1>/dev/null 2>&1
	fi
}

do_pack_h3()
{
	cp $BOOTDIR/boot0_sdcard_${CHIP}.bin ${PACK_OUT}
	cp $BOOTDIR/u-boot-${CHIP}.bin ${PACK_OUT}
	cp $SYS_CONFIG ${PACK_OUT}/sys_config.fex

	fex2bin  sys_config.fex sys_config.bin
	update_boot0 boot0_sdcard_${CHIP}.bin sys_config.bin SDMMC_CARD >/dev/null 2>&1
	update_uboot u-boot-${CHIP}.bin sys_config.bin >/dev/null 2>&1

	cp boot0_sdcard_${CHIP}.bin $UBOOT_BIN/boot0_sdcard_${CHIP}.bin 
	cp u-boot-${CHIP}.bin $UBOOT_BIN/u-boot-${CHIP}.bin
	cp sys_config.bin $UBOOT_BIN/script.bin_${BOARD}
	cp sys_config.bin $EXTER/chips/${CHIP}/script/script.bin_${BOARD}
}

pack_uboot()
{
	PACKOUT_DIR=$SRC/.tmp/packout
	cd ${PACKOUT_DIR}

	do_prepare
	do_ini_to_dts
	do_common
}
