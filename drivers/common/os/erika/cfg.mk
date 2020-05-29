# /*****************************************************************************
# Copyright 2016-2019 Broadcom Limited.  All rights reserved.
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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#******************************************************************************/

ifeq ($(call iseeopt, __BCM__), yes)

INCLUDE_PATH += $(BRCM_SDK_ROOT)/drivers

EE_SRCS += $(BRCM_SDK_ROOT)/drivers/mcu/os/erika/mcu_osil.c

EE_SRCS += $(BRCM_SDK_ROOT)/drivers/uart/os/erika/uart_osil.c
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/uart/os/erika/uconsole_osil.c

ifeq ($(call iseeopt, ENABLE_AVT), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/avt/os/erika/avt_osil.c
endif   # ENABLE_CAN_MCAN

ifeq ($(call iseeopt, ENABLE_CAN_MCAN), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/can/os/erika/can_osil.c
endif   # ENABLE_CAN_MCAN

ifeq ($(call iseeopt, ENABLE_FLASH), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/flash/os/erika/flash_osil.c
endif   # ENABLE_FLASH

ifeq ($(call iseeopt, ENABLE_OTP), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/otp/os/erika/otp_osil.c
endif   # ENABLE_OTP

ifeq ($(call iseeopt, ENABLE_CCT), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/ccu/os/erika/ccu_osil.c
endif   # ENABLE_CCT

ifeq ($(call iseeopt, ENABLE_PWM), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/pwm/os/erika/pwm_osil.c
endif   # ENABLE_PWM

ifeq ($(call iseeopt, ENABLE_LIN), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/lin/os/erika/lin_osil.c
endif   # ENABLE_LIN

ifeq ($(call iseeopt, ENABLE_GPIO_GIO_V1), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/gpio/os/erika/gpio_osil.c
endif   # ENABLE_GPIO_GIO_V1

ifeq ($(call iseeopt, ENABLE_GPIO_GIO_V2), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/gpio/os/erika/gpio_osil.c
endif   # ENABLE_GPIO_GIO_V2

ifeq ($(call iseeopt, ENABLE_PINMUX), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/pinmux/os/erika/pinmux_osil.c
endif   # ENABLE_GPIO_GIO_V1

ifeq ($(call iseeopt, ENABLE_VTMON), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/vtmon/os/erika/vtmon_osil.c
endif   # ENABLE_VTMON

ifeq ($(call iseeopt, ENABLE_WATCHDOG_SP805), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/watchdog/os/erika/wdt_osil.c
endif   # ENABLE_WATCHDOG_SP805

ifeq ($(call iseeopt, ENABLE_DMA330), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/dma/os/erika/dma_osil.c
endif   # ENABLE_DMA330

ifeq ($(call iseeopt, ENABLE_TIMER_SP804), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/timer/os/erika/gpt_osil.c
endif   # ENABLE_TIMER_SP804

ifeq ($(call iseeopt, ENABLE_ADC), yes)
EE_SRCS +=$(BRCM_SDK_ROOT)/drivers/adc/os/erika/adc_osil.c
endif   # ENABLE_ADC

ifeq ($(call iseeopt, ENABLE_SPI_PL022), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/spi/os/erika/spi_osil.c
endif   # ENABLE_SPI_PL022

ifeq ($(call iseeopt, ENABLE_IPC), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/os/erika/ipc_osil.c
endif   # ENABLE_IPC

ifeq ($(call iseeopt, ENABLE_IIC_BSC), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/i2c/os/erika/i2c_osil.c
endif   # ENABLE_IIC_BSC

ifeq ($(call iseeopt, ENABLE_SD), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/sd/os/erika/sdhc_osil.c
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/sd/os/erika/sd_osil.c
endif   # ENABLE_SD

ifeq ($(call iseeopt, ENABLE_DBGMEM), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/drivers/dbgmem/os/erika/dbgmem_osil.c
endif   # ENABLE_DBGMEM

endif   # __BCM__
