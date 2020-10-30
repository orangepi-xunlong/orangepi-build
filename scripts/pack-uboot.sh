#!/bin/bash
#
# SPDX-License-Identifier: GPL-2.0

function do_prepare()
{
	if [[ $BOARDFAMILY == "sun50iw6" ]]; then

                configs_file_list=(
                ${FILE}/*.fex
                ${FILE}/boot_package.cfg
                )

                boot_file_list=(
                ${BOOTDIR}/boot0_sdcard_${CHIP}.bin:${PACK_OUT}/boot0_sdcard.fex
                ${BOOTDIR}/u-boot-${CHIP}.bin:${PACK_OUT}/u-boot.fex
                ${FILE}/scp.bin:${PACK_OUT}/scp.fex
                ${FILE}/bl31.bin:${PACK_OUT}/monitor.fex
                ${SYS_CONFIG}:${PACK_OUT}/sys_config.fex
                )

                for file in ${configs_file_list[@]} ; do
                        cp -f $file ${PACK_OUT}/ 2> /dev/null
                done

                for file in ${boot_file_list[@]} ; do
                        cp -rf `echo $file | awk -F: '{print $1}'` \
                                `echo $file | awk -F: '{print $2}'` 2>/dev/null
                done

	else

	export PATH=$EXTER/packages/pack-uboot/${BOARDFAMILY}/tools/:$PATH

	cp ${EXTER}/packages/pack-uboot/${BOARDFAMILY}/bin/* .
	cp ${BOOTSOURCEDIR}/u-boot.bin u-boot.fex
	[[ $BOARDFAMILY != sun50iw9 ]] && cp ${BOOTSOURCEDIR}/boot0_sdcard.bin boot0_sdcard.fex

	fi
}

function do_ini_to_dts()
{
	if [[ $BOARDFAMILY == "sun50iw6" ]]; then

                local DTC_SRC_PATH=${LINUXSOURCEDIR}/arch/${ARCHITECTURE}/boot/dts/sunxi/
                local DTC_INI_FILE_BASE=${SYS_CONFIG}
                local DTC_INI_FILE=${DEST}/sys_config_fix.fex

                cp $DTC_INI_FILE_BASE $DTC_INI_FILE
                sed -i "s/\(\[dram\)_para\(\]\)/\1\2/g" $DTC_INI_FILE
                sed -i "s/\(\[nand[0-9]\)_para\(\]\)/\1\2/g" $DTC_INI_FILE
                DTC_SRC_FILE=${EXTER}/chips/${CHIP}/pack/bin/dtc_src_file

                dtc_alph -O dtb -o ${DEST}/sunxi.dtb    \
                        -b 0                    \
                        -i $DTC_SRC_PATH        \
                        -F $DTC_INI_FILE        \
                        $DTC_SRC_FILE 1>/dev/null 2>&1
                if [ $? -ne 0 ]; then
                        pack_error "Conver script to dts failed"
                        exit 1
                fi

	else

	local DTC_COMPILER=$EXTER/packages/pack-uboot/${BOARDFAMILY}/tools/dtc

	[[ ! -f $DTC_COMPILER ]] && exit_with_error "Script_to_dts: Can not find dtc compiler." 

	$DTC_COMPILER -p 2048 -W no-unit_address_vs_reg -@ -O dtb -o ${BOARD}-u-boot.dtb -b 0 ${BOARD}-u-boot.dts

	[[ $? -ne 0 ]] && exit_with_error "Conver script to dts failed."

       # It'is used for debug dtb
	$DTC_COMPILER -W no-unit_address_vs_reg -I dtb -O dts -o .${BOARD}-u-boot.dts ${BOARD}-u-boot.dtb 1>/dev/null 2>&1

	fi
}

function do_common()
{
	if [[ $BOARDFAMILY == "sun50iw9" ]]; then

		busybox unix2dos sys_config.fex
		script  sys_config.fex > /dev/null

		cp ${PACKOUT_DIR}/${BOARD}-u-boot.dtb sunxi.fex
		update_dtb sunxi.fex 4096

		update_boot0 boot0_sdcard.fex	sys_config.bin SDMMC_CARD > /dev/null
		update_uboot -no_merge u-boot.fex sys_config.bin > /dev/null

		[[ $? -ne 0 ]] && exit_with_error "update u-boot run error"

		#pack boot package
		busybox unix2dos boot_package.cfg
		dragonsecboot -pack boot_package.cfg > /dev/null

		[[ $? -ne 0 ]] && exit_with_error "dragon pack error"
	else

                unix2dos sys_config.fex 2>/dev/null
                script sys_config.fex > /dev/null
                cp sys_config.bin config.fex 2>/dev/null

                cp ${DEST}/sunxi.dtb sunxi.fex
                update_uboot_fdt u-boot.fex sunxi.fex u-boot.fex >/dev/null
                update_scp scp.fex sunxi.fex >/dev/null

                # Those files for Nand or Card
                update_boot0 boot0_sdcard.fex   sys_config.bin SDMMC_CARD > /dev/null
                update_uboot u-boot.fex sys_config.bin > /dev/null

                unix2dos boot_package.cfg 2>/dev/null
                dragonsecboot -pack boot_package.cfg 1>/dev/null 2>&1

                #Here, will check if need to used multi config.fex or not
                update_uboot_v2 u-boot.fex sys_config.bin ${LINUXFAMILY} 1>/dev/null 2>&1

	fi
}

do_pack_a64()
{
	cp -avf ${FILE}/* ${PACK_OUT}/ > /dev/null
	cp -avf $BOOTDIR/u-boot-sun50iw1p1.bin ${PACK_OUT}/u-boot.bin > /dev/null

	# Build binary device tree
	dtc -Odtb -o A64.dtb A64.dts 1>/dev/null 2>&1

	# Build sys_config.bin
	unix2dos sys_config.fex 1>/dev/null 2>&1
	script sys_config.fex 1>/dev/null 2>&1

	# Merge u-boot.bin infile outfile mode [secmonitor | secos | scp]
	merge_uboot  u-boot.bin  bl31.bin  u-boot-merged.bin secmonitor 1>/dev/null 2>&1
	merge_uboot  u-boot-merged.bin  scp.bin  u-boot-merged2.bin scp 1>/dev/null 2>&1

	# Merge uboot and dtb
	update_uboot_fdt u-boot-merged2.bin A64.dtb u-boot-with-dtb.bin 1>/dev/null 2>&1

	# Merge uboot and sys_config.fex
	update_uboot u-boot-with-dtb.bin sys_config.bin 1>/dev/null 2>&1

        cp ${PACK_OUT}/boot0.bin ${UBOOT_BIN}/boot0_sdcard_${CHIP}.bin 1>/dev/null 2>&1
        cp ${PACK_OUT}/u-boot-with-dtb.bin ${UBOOT_BIN}/u-boot-${CHIP}.bin 1>/dev/null 2>&1
        cp ${PACK_OUT}/A64.dtb ${UBOOT_BIN}/ 1>/dev/null 2>&1
}

do_pack_h3()
{

        cp $BOOTDIR/boot0_sdcard_${CHIP}.bin ${PACK_OUT}
        cp $BOOTDIR/u-boot-${CHIP}.bin ${PACK_OUT}
	cp $SYS_CONFIG ${PACK_OUT}/sys_config.fex

	fex2bin  sys_config.fex sys_config.bin
        update_boot0 boot0_sdcard_${CHIP}.bin sys_config.bin SDMMC_CARD 1>/dev/null 2>&1
        update_uboot u-boot-${CHIP}.bin sys_config.bin 1>/dev/null 2>&1

        cp boot0_sdcard_${CHIP}.bin $UBOOT_BIN/boot0_sdcard_${CHIP}.bin 
        cp u-boot-${CHIP}.bin $UBOOT_BIN/u-boot-${CHIP}.bin
        cp sys_config.bin $UBOOT_BIN/script.bin_${BOARD}
		cp sys_config.bin $EXTER/chips/${CHIP}/script/script.bin_${BOARD}
}

pack_uboot()
{
	PACKOUT_DIR=$SRC/.tmp/packout

	rm -rf ${PACKOUT_DIR}
	mkdir -p ${PACKOUT_DIR}
	cd ${PACKOUT_DIR}

	do_prepare
	do_ini_to_dts
	do_common
}
