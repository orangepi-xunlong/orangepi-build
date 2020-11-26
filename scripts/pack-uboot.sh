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
	if [[ $BOARDFAMILY =~ sun50iw6|sun50iw9 ]]; then

		local DTC_COMPILER=$EXTER/packages/pack-uboot/${BOARDFAMILY}/tools/dtc
		[[ ! -f $DTC_COMPILER ]] && exit_with_error "Script_to_dts: Can not find dtc compiler." 
		#Disbale noisy checks
		local DTC_FLAGS="-W no-unit_address_vs_reg"

		$DTC_COMPILER -p 2048 ${DTC_FLAGS} -@ -O dtb -o ${BOARD}-u-boot.dtb -b 0 dts/${BOARD}-u-boot.dts >/dev/null 2>&1

		[[ $? -ne 0 ]] && exit_with_error "dtb: Conver script to dts failed."

		# It'is used for debug dtb
		$DTC_COMPILER ${DTC_FLAGS} -I dtb -O dts -o .${BOARD}-u-boot.dts ${BOARD}-u-boot.dtb >/dev/null 2>&1
	fi
}

function do_common()
{
	busybox unix2dos sys_config.fex
	script  sys_config.fex  >/dev/null
	cp ${PACKOUT_DIR}/${BOARD}-u-boot.dtb sunxi.fex
	update_dtb sunxi.fex 4096 >/dev/null
	
	if [[ $BOARDFAMILY == "sun50iw6" ]]; then
		cp -f   sys_config.bin config.fex
		update_scp scp.fex sunxi.fex > /dev/null 2>&1
	fi

	update_boot0 boot0_sdcard.fex	sys_config.bin SDMMC_CARD > /dev/null
	update_uboot -no_merge u-boot.fex sys_config.bin > /dev/null
	[[ $? -ne 0 ]] && exit_with_error "update u-boot run error"

	#pack boot package
	busybox unix2dos boot_package.cfg
	dragonsecboot -pack boot_package.cfg > /dev/null
	[[ $? -ne 0 ]] && exit_with_error "dragon pack error"
}

do_pack_a64()
{
	cp -avf ${FILE}/* ${PACK_OUT}/ > /dev/null
	cp -avf $BOOTDIR/u-boot-sun50iw1p1.bin ${PACK_OUT}/u-boot.bin > /dev/null

	# Build binary device tree
	dtc -Odtb -o A64.dtb A64.dts >/dev/null 2>&1

	# Build sys_config.bin
	unix2dos sys_config.fex >/dev/null 2>&1
	script sys_config.fex >/dev/null 2>&1

	# Merge u-boot.bin infile outfile mode [secmonitor | secos | scp]
	merge_uboot  u-boot.bin  bl31.bin  u-boot-merged.bin secmonitor >/dev/null 2>&1
	merge_uboot  u-boot-merged.bin  scp.bin  u-boot-merged2.bin scp >/dev/null 2>&1

	# Merge uboot and dtb
	update_uboot_fdt u-boot-merged2.bin A64.dtb u-boot-with-dtb.bin >/dev/null 2>&1

	# Merge uboot and sys_config.fex
	update_uboot u-boot-with-dtb.bin sys_config.bin >/dev/null 2>&1

        cp ${PACK_OUT}/boot0.bin ${UBOOT_BIN}/boot0_sdcard_${CHIP}.bin >/dev/null 2>&1
        cp ${PACK_OUT}/u-boot-with-dtb.bin ${UBOOT_BIN}/u-boot-${CHIP}.bin >/dev/null 2>&1
        cp ${PACK_OUT}/A64.dtb ${UBOOT_BIN}/ >/dev/null 2>&1
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
