/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

	.syntax unified
	.arch armv7-m
	.cpu cortex-m7
	.code 16
	.text

vector_reg:
    .word 0xE000ED08
/*=====================================================
	Macros
=======================================================*/
#define VECTOR(x)           \
    .word x + 1

/*=====================================================
	Externs
=======================================================*/

    .extern BL_EarlyInit
    .extern __main_stack__
    .extern main
    .extern ChipNMIFaultHandler
    .extern ChipHardFaultHandler
    .extern ChipBusFaultHandler
    .extern ChipMPUFaultHandler
    .extern ChipUsageFaultHandler
    .extern ChipSVCHandler
    .extern ChipDebugHandler
    .extern ChipPendSVHandler
    .extern ChipSysTickHandler
    .extern ChipIRQHandler

/*=============================================================================
=============================================================================*/
    .section .reset_func1
    BL CORTEX_MX_RESET_HANDLER

    .section .reset_func2
    BL CORTEX_MX_RESET_HANDLER

    .section .reset_func3
    BL CORTEX_MX_RESET_HANDLER
/*======================================================
    Exception Vectors

    Thumb mode code
    bit[0] of all exception vector table entries needs
    to be 1 (except entry 0 which is for main stack),
    since cortex-m EPSR.T bit is set/reset
    based on bit[0] of exception entry code. Since
    cortex-M supports only thumb mode, EPSR.T must
    be always 1. Trying to execute an instruction
    while EPSR.T bit is set 0, will lead to hardfault
=======================================================*/

    .section .vector_tbl
    .code   16
    .global  __vectors

__vectors:
    VECTOR(__main_stack__)
    VECTOR(CORTEX_MX_RESET_HANDLER)
    VECTOR(CORTEX_MX_NMI_FAULT)
    VECTOR(CORTEX_MX_HARD_FAULT)
    VECTOR(CORTEX_MX_MPU_FAULT)
    VECTOR(CORTEX_MX_BUS_FAULT)
    VECTOR(CORTEX_MX_USAGE_FAULT)
    VECTOR(CORTEX_MX_RSVD)
    VECTOR(CORTEX_MX_RSVD)
    VECTOR(CORTEX_MX_RSVD)
    VECTOR(CORTEX_MX_RSVD)
    VECTOR(CORTEX_MX_SVC_HANDLER)
    VECTOR(CORTEX_MX_DEBUG_MONITOR_HANDLER)
    VECTOR(CORTEX_MX_RSVD)
    VECTOR(CORTEX_MX_PENDSV_HANDLER)
    VECTOR(CORTEX_MX_SYSTICK_HANDLER)
    VECTOR(CORTEX_MX_IRQ_HANDLER)

/*=====================================================
	Reset handler
=======================================================*/

	.global  CORTEX_MX_RESET_HANDLER

CORTEX_MX_RESET_HANDLER:
	LDR r0, =__vectors
	LDR r1, =vector_reg
	LDR r1, [r1]
	STR r0, [r1]

/*=====================================================
	Setup stack
=======================================================*/
/* ideally processor will load MSP from exception vector table
   but since we can not modify the ROM table in FPGA image, we
   will reconfigure it again
   */
    LDR r0, =__main_stack__
	MSR MSP, r0

/*=====================================================
	Setup MPU regions
=======================================================*/
	BL	BL_EarlyInit

/*=====================================================
	C-Entry code
=======================================================*/
	/* Keep IRQ disabled */
	CPSID	i
C_ENTRY:
	blx     BL_main					// jump to main
LOOP:
	b       LOOP

/*=====================================================
	exception handlers
=======================================================*/
	.global CORTEX_MX_NMI_FAULT
	.global CORTEX_MX_HARD_FAULT
	.global CORTEX_MX_MPU_FAULT
	.global CORTEX_MX_BUS_FAULT
	.global CORTEX_MX_USAGE_FAULT
    .global CORTEX_MX_SVC_HANDLER
	.global CORTEX_MX_DEBUG_MONITOR_HANDLER
    .global CORTEX_MX_PENDSV_HANDLER
	.global CORTEX_MX_SYSTICK_HANDLER
	.global CORTEX_MX_IRQ_HANDLER
	.global CORTEX_MX_RSVD

CORTEX_MX_NMI_FAULT:
#ifdef CHIP_NMI_HANDLER
    b ChipNMIFaultHandler
#else
    b CORTEX_MX_NMI_FAULT
#endif

CORTEX_MX_HARD_FAULT:
#ifdef CHIP_HARD_FAULT_HANDLER
	b ChipHardFaultHandler
#else
    b CORTEX_MX_HARD_FAULT
#endif

CORTEX_MX_MPU_FAULT:
#ifdef CHIP_MPU_FAULT_HANDLER
	b ChipMPUFaultHandler
#else
    b CORTEX_MX_MPU_FAULT
#endif

CORTEX_MX_BUS_FAULT:
#ifdef CHIP_BUS_FAULT_HANDLER
	b ChipBusFaultHandler
#else
    b CORTEX_MX_BUS_FAULT
#endif

CORTEX_MX_USAGE_FAULT:
#ifdef CHIP_USAGE_FAULT_HANDLER
	b ChipUsageFaultHandler
#else
    b CORTEX_MX_USAGE_FAULT
#endif

CORTEX_MX_SVC_HANDLER:
#ifdef CHIP_SVC_HANDLER
	b ChipSVCHandler
#else
    b CORTEX_MX_SVC_HANDLER
#endif

CORTEX_MX_DEBUG_MONITOR_HANDLER:
#ifdef CHIP_DEBUG_HANDLER
	b ChipDebugHandler
#else
    b CORTEX_MX_DEBUG_MONITOR_HANDLER
#endif

CORTEX_MX_PENDSV_HANDLER:
#ifdef CHIP_PENDSV_HANDLER
    b ChipPendSVHandler
#else
    b CORTEX_MX_PENDSV_HANDLER
#endif

CORTEX_MX_SYSTICK_HANDLER:
#ifdef CHIP_SYSTICK_HANDLER
    b ChipSysTickHandler
#else
	b CORTEX_MX_SYSTICK_HANDLER
#endif

CORTEX_MX_IRQ_HANDLER:
#ifdef CHIP_IRQ_HANDLER
    b ChipIRQHandler
#else
	b CORTEX_MX_IRQ_HANDLER
#endif

CORTEX_MX_RSVD:
	b CORTEX_MX_RSVD
