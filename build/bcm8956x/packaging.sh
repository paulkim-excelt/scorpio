#!/bin/bash
################################################################################
#
# Copyright 2019 Broadcom Limited.  All rights reserved.
#
# This program is the proprietary software of Broadcom Limited and/or its
# licensors, and may only be used, duplicated, modified or distributed pursuant
# to the terms and conditions of a separate, written license agreement executed
# between you and Broadcom (an "Authorized License").
#
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
# LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
# IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
# 1. This program, including its structure, sequence and organization,
#    constitutes the valuable trade secrets of Broadcom, and you shall use all
#    reasonable efforts to protect the confidentiality thereof, and to use this
#    information only in connection with your use of Broadcom integrated
#    circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
#    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
#    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
#    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
#    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
#    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
#    SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
#    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
#    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################

set -e
shopt -s extglob
BLD_SCRIPT_DIR=$( cd "$( dirname "$0" )" && pwd )
BRCM_SDK_ROOT=$(readlink -f ${BLD_SCRIPT_DIR}/../..)


function usage
{
  echo "Usage: packaging.sh [-r -s -b <board_name1,board_name2> -i <Filename containing Customer vs component names separated by comma> -c <customer_name>]"
  echo "e.g. packaging.sh -r -s -b bcm89561_evk -i cust_config -c default"
  echo "cust_config would contain similar to below:"
  echo "default:lwip,drivers,utils"
  echo "config1:lwip,drivers"
}

board=
release="0"
safety="0"
tot_arg=$#

while getopts ":hrsb:i:c:" opt; do
  case $opt in
    r )
        release="1"
        ;;
    s )
        safety="1"
        ;;
    b )
		board=${OPTARG}
		;;
	i )
		ignore_comp=${OPTARG}
		;;
	c )
		customer=${OPTARG}
		;;
	\? )
		echo "Invalid Option: -${OPTARG}" 1>&2
		usage
		exit 1
		;;
	: )
		echo "Invalid Option: -${OPTARG} requires an argument" 1>&2
		usage
		exit 1
		;;
	h )
		usage
		exit 0
		;;
  esac
done

shift $((OPTIND -1))

function contains() {
  local n=$#
  local value=${!n}
  for ((i=1;i < $#; i++)) {
    #echo "i: ${i}"
	if [ "${!i}" == "${value}" ]; then
	  #echo "y"
	  return 0
	fi
  }
  #echo "n"
  return 1
}
family=bcm8956x
allboardpaths=(`find ${BRCM_SDK_ROOT}/build/${family}/board -mindepth 1 -maxdepth 1 -type d`)
boarddir="${BRCM_SDK_ROOT}/build/${family}/board/"
allboards=("${allboardpaths[@]/${boarddir}/}") #replace $boarddir with nothing in $allboardpaths[@] to get only board names
# If board input argument not provided, it would build and package for all boards.
# Search for all board directory in build/${family}/board folder and generate a list
# of all boards to be built and packaged.
if [[ $board ]]; then
  echo "Board: ${board}"
else
  usage
  buildall=true
  echo ${allboards[@]}
fi

# If customer config file and customer name is given as input argument, then parse the customer config file
# to get list of libraries for which source code would be delivered.
if [[ $board ]] && [[ $tot_arg -gt 2 ]]; then
  if [[ $ignore_comp ]] && [[ $customer ]]; then
    echo "Ignore file: $ignore_comp"
    echo "Customer: $customer"
    default=false
	cust_line=`awk -F: -v keyword="$customer" '$1 == keyword {$1=$1; print}' $ignore_comp`
	if [[ $cust_line ]]; then
	  echo "Cust_line: $cust_line"
	else
	  echo "Customer $customer doesn't exist in $ignore_comp"
	  exit 1
	fi
  else
    usage
    exit 1
  fi
else
  default=true
  echo "Packaging with default"
fi

if [[ $default = true ]]; then
  echo "Packaging with default"
else
  echo "Customer for packaging: $customer"
fi

# Create an array of board lists to be built and packaged
if [[ $buildall = true ]]; then
  echo "Building for all boards..."
  boardlist=("${allboards[@]}")
else
  IFS=',' read -a boardlist <<< "${board}"
fi

for eboard in "${allboards[@]}"
do
    if [ `echo ${board} | grep -c $eboard` -eq 0 ];then
        echo Deleting $boarddir/$eboard
        rm -rf $boarddir/$eboard
    fi
done

cd ${BRCM_SDK_ROOT}/build/
#create sw_version.txt file which contains the latest release label
sw_version=`git for-each-ref --sort=-taggerdate --format=%\(refname:short\) refs/tags --merged | grep -m1 BCM8956X_REL | awk -F"_REL_" '{print $NF}'`
echo "SW_IMAGE_VERSION:=${sw_version}" > ${BRCM_SDK_ROOT}/build/${family}/sw_version.txt
echo "Boards: ${boardlist[@]}"
LIBTYPE="gcc"
make clean
for eboard in "${boardlist[@]}"
do
  echo "$eboard"
  make board=${eboard} cust=1
done

prebuilt_dir=${BRCM_SDK_ROOT}/prebuilt
#lib_dir=${BRCM_SDK_ROOT}/out/${board}/lib

echo ${BLD_SCRIPT_DIR}
echo ${BRCM_SDK_ROOT}

alllibpaths=(`find ${BRCM_SDK_ROOT}/out/lib/ -mindepth 1 -maxdepth 1 -type d`)
libdir="${BRCM_SDK_ROOT}/out/lib/"
allchips=("${alllibpaths[@]/${libdir}/}") #replace $boarddir with nothing in $allboardpaths[@] to get only chip names

# Delete all libraries for which source code needs to provided.
if [[ $default = false ]]; then
	cust_array=($cust_line)
	echo "component list: ${cust_array[1]}"
	IFS=',' read -a ignore_list <<< "${cust_array[1]}"
	for echip in "${allchips[@]}"
	do
		for element in "${ignore_list[@]}"
		do
			echo "$element"
			skiplib=lib${echip}_${element}.a
			rm ${BRCM_SDK_ROOT}/out/lib/${echip}/${skiplib}
			echo "Removing Lib as Source needs to be delivered: ${skiplib}"
		done
	done
fi

# generate list of all components present in $BRCM_SDK_ROOT, parse for type "lib". If the library name
# exists in ${ignore_list[@]}, then do nothing as source code needs to be delivered for that.
# if library name doesn't exists, then delete all source files except comp.mk & os under that component
# library directory
complist=( $(find ${BRCM_SDK_ROOT} -name comp.mk) )
for comp in "${complist[@]}"
do
  comp_name=$(cat ${comp} | grep "BRCM_COMP_NAME\ \+\:=" | sed -n 's/BRCM_COMP_NAME\s\+:=\s\+\(.*\)/\1/p')
  comp_type=$(cat ${comp} | grep "BRCM_COMP_TYPE\ \+\:=" | sed -n 's/BRCM_COMP_TYPE\s\+:=\s\+\(.*\)/\1/p')
  comp_type=$(echo ${comp_type} | sed 's/^ *//g' | sed 's/ *$//g')
  if [[ "${comp_type}" == "lib" ]]; then
    if contains "${ignore_list[@]}" "${comp_name}"; then
	  echo "${comp_name} LIB Component as source"
	else
      echo "${comp_name} sources can be removed"
      comp_dir=$( dirname ${comp} )
      cd ${comp_dir}
      rm -rf lib
      echo "${comp_name} directory ${comp_dir}"
    fi
  fi
done

echo "Copy libraries to prebuilt directory"

for echip in "${allchips[@]}"
do
	mkdir -p ${prebuilt_dir}/${echip}/lib/${LIBTYPE}
	cp -r ${BRCM_SDK_ROOT}/out/lib/${echip}/lib* ${prebuilt_dir}/${echip}/lib/${LIBTYPE}/
	echo "Copy bootloader lib"
	for board in "${boardlist[@]}"
	do
		mkdir -p ${prebuilt_dir}/${echip}/bootloader_lib/${board}
		cp -rf ${BRCM_SDK_ROOT}/out/bootloader/${board}/lib/lib* ${prebuilt_dir}/${echip}/bootloader_lib/${board}/
	done
done
echo "Delete out directory..."
rm -rf ${BRCM_SDK_ROOT}/out

echo "Delete tests/unit and tests/autotest from all gits"
gitlist=( $(find ${BRCM_SDK_ROOT} -name .git -type d) )
find ${BRCM_SDK_ROOT} -name unit | grep tests | xargs rm -rf
find ${BRCM_SDK_ROOT} -name autotest | grep tests | xargs rm -rf

echo "Delete .gitignore files"
$(find ${BRCM_SDK_ROOT} -type f -name '.gitignore' -exec rm -rf {} \;)

echo "Delete .repo directory"
rm -rf ${BRCM_SDK_ROOT}/.repo

echo "Delete .git directory"
$(find ${BRCM_SDK_ROOT} -type d -name '.git' -exec rm -rf {} +)

echo "Clean up each directory to remove un-necessary files for release"

rm -rf ${BRCM_SDK_ROOT}/applications/avrcvr
rm -rf ${BRCM_SDK_ROOT}/applications/avxmtr
rm -rf ${BRCM_SDK_ROOT}/applications/face_detect_demo
rm -rf ${BRCM_SDK_ROOT}/applications/optflow_demo
rm -rf ${BRCM_SDK_ROOT}/applications/ped_detect_demo
rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidrcvr
rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidxmtr
rm -rf ${BRCM_SDK_ROOT}/applications/stitch_app
rm -rf ${BRCM_SDK_ROOT}/applications/tests
rm -rf ${BRCM_SDK_ROOT}/applications/vidrcvr
rm -rf ${BRCM_SDK_ROOT}/applications/vidxcvr
rm -rf ${BRCM_SDK_ROOT}/applications/vidxmtr
rm -rf ${BRCM_SDK_ROOT}/applications/viewfinder
rm -rf ${BRCM_SDK_ROOT}/applications/framework
rm -rf ${BRCM_SDK_ROOT}/applications/camera_tuning
rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidrcvr_wifi
rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidxmtr_wifi
rm -rf ${BRCM_SDK_ROOT}/applications/vidxmtr_ext
rm -rf ${BRCM_SDK_ROOT}/applications/vidxmtr720p60
rm -rf ${BRCM_SDK_ROOT}/applications/vidxmtr1080p30
rm -rf ${BRCM_SDK_ROOT}/applications/csitx_bayer_stream

rm -rf ${BRCM_SDK_ROOT}/bootloader/comms
rm -rf ${BRCM_SDK_ROOT}/bootloader/security
rm -rf ${BRCM_SDK_ROOT}/bootloader/tests
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/avt
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/ddr
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/ethernet/bl_eth_amac.c
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/ethernet/bl_eth_amac.h
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/ethernet/bl_eth_unimac.c
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/ethernet/bl_eth_unimac.h
rm -rf ${BRCM_SDK_ROOT}/bootloader/drivers/gpio
rm -rf ${BRCM_SDK_ROOT}/bootloader/arch/arm/cortex_rx
rm -rf ${BRCM_SDK_ROOT}/bootloader/include/gpio.h
rm -rf ${BRCM_SDK_ROOT}/bootloader/include/avt.h
rm -rf ${BRCM_SDK_ROOT}/bootloader/include/eth_xcvr.h
rm -rf ${BRCM_SDK_ROOT}/bootloader/include/ethernet.h
rm -rf ${BRCM_SDK_ROOT}/bootloader/utils/bcm_time.c
rm -rf ${BRCM_SDK_ROOT}/bootloader/bcm8956x/chip/common/bl_xcvr_cfg.c

rm -rf ${BRCM_SDK_ROOT}/cpu/arm/lib/cortex_rx
rm -rf ${BRCM_SDK_ROOT}/cpu/arm/inc/cortex_rx.h
rm -rf ${BRCM_SDK_ROOT}/system/arch/arm/cortex_rx
rm -rf ${BRCM_SDK_ROOT}/system/common/os/erika/display.c
rm -rf ${BRCM_SDK_ROOT}/system/common/os/erika/inet_cfg.c
rm -rf ${BRCM_SDK_ROOT}/system/include/inet_cfg.h
rm -rf ${BRCM_SDK_ROOT}/system/tests/integration/avt
rm -rf ${BRCM_SDK_ROOT}/system/display

# Delete the input RDB files
rm -rf ${BRCM_SDK_ROOT}/system/bcm8956x/rdb

#Driver git packaging
rm -rf ${BRCM_SDK_ROOT}/drivers/adc
rm -rf ${BRCM_SDK_ROOT}/drivers/can/
rm -rf ${BRCM_SDK_ROOT}/drivers/ccu/
rm -rf ${BRCM_SDK_ROOT}/drivers/dma
rm -rf ${BRCM_SDK_ROOT}/drivers/gpio/gpio_gio_v2.c
rm -rf ${BRCM_SDK_ROOT}/drivers/pwm
rm -rf ${BRCM_SDK_ROOT}/drivers/vtmon
rm -rf ${BRCM_SDK_ROOT}/drivers/avt
rm -rf ${BRCM_SDK_ROOT}/drivers/include/adc.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/avt.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/can.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/ccu.h
#rm -rf ${BRCM_SDK_ROOT}/drivers/include/dma.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/i2c.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/pca9673.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/pwm.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/spi.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/vtmon.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/adc_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/avt_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/can_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/ccu_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/dma_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/i2c_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/pwm_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/spi_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/vtmon_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/avt/os/erika/avt_osil.c
rm -rf ${BRCM_SDK_ROOT}/drivers/i2c/os/erika/i2c_osil.c
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/adc
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/avt
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/can
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/dma
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/i2c
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/pwm
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/spi
rm -rf ${BRCM_SDK_ROOT}/drivers/tests/integration/vtmon
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/sdhc_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/sd_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/sd.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/sdhc.h
rm -rf ${BRCM_SDK_ROOT}/drivers/sd/os/erika/sdhc_osil.c
rm -rf ${BRCM_SDK_ROOT}/drivers/sd/os/erika/sd_osil.c
rm -rf ${BRCM_SDK_ROOT}/drivers/include/lin.h
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/lin_osil.h
rm -rf ${BRCM_SDK_ROOT}/drivers/lin
rm -rf ${BRCM_SDK_ROOT}/drivers/i2c
rm -rf ${BRCM_SDK_ROOT}/drivers/sd

# ethernet/drivers packaging
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/controller/lib/gamac.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/controller/lib/eth_time_v1.c

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/examples
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/config_scripts
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/batchfiles
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/testcase
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/old

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/altera_nios2
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/atmel_atmega
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/atmel_atxmega
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/atmel_sam3
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5643l
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5644a
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5668
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5674f
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/hs12xs
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/infineon_common_tc2Yx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/infineon_tc26x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/infineon_tc27x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/mico32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/microchip_dspic
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/microchip_pic32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/msp430
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/nordic_nrf51x22
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/nxp_lpcxpresso_lpc12xx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/renesas_r5f5210x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/samsung_ks32c50100
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/st_sta2051
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/st_stm32_stm32f4xx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/tc179x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/ti_stellaris_lm4f232xxxx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/unibo_mparm
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/x86

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/arduino_uno
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/arm_evaluator7t
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/atmel_stk500
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/atmel_stk600
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/axiom_mpc5674fxmb
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ee_easylab
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ee_flex
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ee_miniflex
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/esi_risc
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/fpg-eye
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/freescale_xpc564xl
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/hs12xs_demo9s12xsfame
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/infineon_tc1775b
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/infineon_tc1796b
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/infineon_TriBoard_TC2X5
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ipermob_board_v2
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ipermob_db_pic32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ipermob_mb_pic32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/lattice_xp2_ev_board
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/microchip_dspicdem11plus
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/microchip_esk
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/microchip_explorer16
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/msp430
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/nordic_pca
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/renesas_rskrx210
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ti_stellaris_lm4f232xxxx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/twrs12g128
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/unibo_mparm
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/utmost
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/xbow_mib5x0

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/amazing
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/arduino
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/as
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/atmel
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/atmel802_15_4
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/cal
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/console
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/drivers
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/ieee802154
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/lwip
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/memory
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/microchip
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/misc
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/MiWiP2P
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/MiWiP2Pv2
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/network
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/nordic
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/nxp
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/qpc_v521
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/readme.txt
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/scicos
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/st
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/StellarisWare

rm -rf ${BRCM_SDK_ROOT}/host/utils/tests
rm -rf ${BRCM_SDK_ROOT}/host/utils/hipc/bcm5300x
rm -rf ${BRCM_SDK_ROOT}/test

#Back-up broadcom pcie driver
cp -r ${BRCM_SDK_ROOT}/host/ns2/kernel/drivers/net/ethernet/broadcom/xgbe ${BRCM_SDK_ROOT}/host/ns2
rm -rf ${BRCM_SDK_ROOT}/host/ns2/kernel
rm -rf ${BRCM_SDK_ROOT}/host/ns2/rootfs
rm -rf ${BRCM_SDK_ROOT}/host/ns2/u-boot

rm -rf ${BRCM_SDK_ROOT}/tools/common/rdbgen
rm -rf ${BRCM_SDK_ROOT}/tools/common/map_parser

rm -rf ${BRCM_SDK_ROOT}/ethernet/1722/utils/host/src/brcm_1722_rcvr.c

#delete avb git
rm -rf ${BRCM_SDK_ROOT}/avb
rm -rf ${BRCM_SDK_ROOT}/tools/flash
#remove automation git
rm -rf ${BRCM_SDK_ROOT}/automation
rm -rf ${BRCM_SDK_ROOT}/demo
rm -rf ${BRCM_SDK_ROOT}/host/keystone
find ${BRCM_SDK_ROOT}/build/bcm8956x/board/ -name demo.mk | xargs rm -rf
find ${BRCM_SDK_ROOT}/build/bcm8956x/board/ -name ut.mk | xargs rm -rf
find ${BRCM_SDK_ROOT} -name bcm8953x | xargs rm -rf
find ${BRCM_SDK_ROOT} -name bcm8910x | xargs rm -rf
find ${BRCM_SDK_ROOT} -name bcm8908x | xargs rm -rf

#Remove empty directories from gits
find ${BRCM_SDK_ROOT}/drivers -type d -empty -delete
find ${BRCM_SDK_ROOT}/ethernet/common -type d -empty -delete

cd ${BRCM_SDK_ROOT}/build/

# Verify that build compiles fine with source package.
for eboard in "${boardlist[@]}"
do
    make clean
    make board=${eboard} cust=1
done
#run doxygen build to generate documentation
make clean
make family=bcm8956x doc cust=1 release=${release} safety=${safety}
#remove process git after generating doxygen documentation
rm -rf ${BRCM_SDK_ROOT}/process
rm -rf ${BRCM_SDK_ROOT}/build/doc.mk
rm -rf ${BRCM_SDK_ROOT}/build/bcm8956x/bcm8956x_doc.mk
#remove all docref folders after generating documentation
find ${BRCM_SDK_ROOT} -name docref | xargs rm -rf
find ${BRCM_SDK_ROOT} -name doc | grep -v erika-sdk | grep -v 'system/bcm8956x/doc' | grep -v out | xargs rm -rf
#delete all files under "system/bcm8956x/doc" except "system/bcm8956x/doc/configs"
find ${BRCM_SDK_ROOT}/system/bcm8956x/doc -type f | grep -v 'system/bcm8956x/doc/configs' | xargs rm -rf
find ${BRCM_SDK_ROOT}/system/bcm8956x/doc -type d -empty -delete

cd ${BLD_SCRIPT_DIR}

