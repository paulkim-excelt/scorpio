/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
#define IRQ_HANDLER(num)            \
    .word irq_handler ## num            \

#define VECTOR(x)           \
    .word x + 1

/*=====================================================
    Externs
=======================================================*/

        .extern __main_stack__


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
    VECTOR(EE_cortex_mx_default_reset_ISR)
    VECTOR(EE_CORTEX_MX_NMI_ISR)
    VECTOR(EE_CORTEX_MX_HARD_FAULT_ISR)
    VECTOR(EE_CORTEX_MX_MPU_FAULT_ISR)
    VECTOR(EE_CORTEX_MX_BUS_FAULT_ISR)
    VECTOR(EE_CORTEX_MX_USAGE_FAULT_ISR)
    VECTOR(reserved_handler)
    VECTOR(reserved_handler)
    VECTOR(reserved_handler)
    VECTOR(reserved_handler)
    VECTOR(EE_cortex_mx_svc_ISR)
    VECTOR(EE_CORTEX_MX_DEBUG_MONITOR_ISR)
    VECTOR(reserved_handler)
    VECTOR(EE_cortex_mx_pendsv_ISR)
    VECTOR(EE_CORTEX_MX_SYSTICK_ISR)
    VECTOR(irq_handler) /* IRQ[0] */
#if defined(ENABLE_IPC)
    VECTOR(IPC_IRQChann0Handler) /* IRQ[1] */
#else
    VECTOR(irq_handler) /* IRQ[1] */
#endif
    VECTOR(irq_handler) /* IRQ[2] */
    VECTOR(irq_handler) /* IRQ[3] */
    VECTOR(irq_handler) /* IRQ[4] */
    VECTOR(irq_handler) /* IRQ[5] */
    VECTOR(irq_handler) /* IRQ[6] */
#if defined(ENABLE_ETH_TIME)
    VECTOR(ETHER_1588_IRQHandler) /* IRQ[7] */
#else
    VECTOR(irq_handler)         /* IRQ[7] */
#endif
    VECTOR(irq_handler)         /* IRQ[8] */
#if defined(ENABLE_FLASH)
    VECTOR(FLASH_IRQHandler0)
#else
    VECTOR(irq_handler)         /* IRQ[9] */
#endif
    VECTOR(UART_IRQHandler0)    /* IRQ[10] */
    VECTOR(irq_handler)         /* IRQ[11] */
#if defined(ENABLE_TIMER_SP804)
    VECTOR(TIM_IRQHandler0)
    VECTOR(TIM_IRQHandler1)
#else
    VECTOR(irq_handler) /* IRQ[12] */
    VECTOR(irq_handler) /* IRQ[13] */
#endif
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
#if defined(ENABLE_SPI_PL022)
    VECTOR(SPI1_IRQHandler)     /* IRQ[17] */
#else
    VECTOR(irq_handler)         /* IRQ[17] */
#endif
    VECTOR(irq_handler)         /* IRQ[18] */
    VECTOR(irq_handler)         /* IRQ[19] */
    VECTOR(GPIO_IRQ1Handler)    /* IRQ[20] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
#if defined(ENABLE_ETH) && defined(ENABLE_ETH_AMAC)
    VECTOR(ETHER_IRQHandler)    /* IRQ[23] */
#else
    VECTOR(irq_handler)
#endif
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
#if defined(ENABLE_ETH_SWITCH)
    VECTOR(ETHERSWT_Port0LinkIRQHandler)    /* IRQ[27] */
    VECTOR(ETHERSWT_Port1LinkIRQHandler)    /* IRQ[28] */
    VECTOR(ETHERSWT_Port2LinkIRQHandler)    /* IRQ[29] */
    VECTOR(ETHERSWT_Port3LinkIRQHandler)    /* IRQ[30] */
    VECTOR(ETHERSWT_Port4LinkIRQHandler)    /* IRQ[31] */
    VECTOR(ETHERSWT_Port5LinkIRQHandler)    /* IRQ[32] */
    VECTOR(ETHERSWT_Port6LinkIRQHandler)    /* IRQ[33] */
    VECTOR(ETHERSWT_Port8LinkIRQHandler)    /* IRQ[34] */
#else
    VECTOR(irq_handler)                     /* IRQ[27] */
    VECTOR(irq_handler)                     /* IRQ[28] */
    VECTOR(irq_handler)                     /* IRQ[29] */
    VECTOR(irq_handler)                     /* IRQ[30] */
    VECTOR(irq_handler)                     /* IRQ[31] */
    VECTOR(irq_handler)                     /* IRQ[32] */
    VECTOR(irq_handler)                     /* IRQ[33] */
    VECTOR(irq_handler)                     /* IRQ[34] */
#endif
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[40] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[50] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[70] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[80] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[90] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[100] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[110] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[120] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[130 */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[140] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[150] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler) /* IRQ[160] */
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)
    VECTOR(irq_handler)

    .size  __vectors, .-__vectors

