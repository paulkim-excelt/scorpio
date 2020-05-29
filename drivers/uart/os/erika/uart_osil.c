/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
/**
    @addtogroup grp_uartdrv_il
    @{

    @file uart_osil.c
    @version 0.1 Initial Version
*/

#include <osil/uart_osil.h>
#include "ee.h"

/**
    @name UART Integration Design IDs
    @{
    @brief Integration Design IDs for UART
*/
#define BRCM_SWDSGN_UART_SYS_CMD_REQUEST_PROC   (0xC0UL) /**< @brief #UART_SysCmdReq    */
#define BRCM_SWDSGN_UART_IRQ_HANDLER_PROC       (0xC1UL) /**< @brief #UART_IRQHandler   */
#define BRCM_SWDSGN_UART_IRQ_HANDLER0_PROC      (0xC2UL) /**< @brief #UART_IRQHandler0  */
#define BRCM_SWDSGN_UART_IRQ_HANDLER1_PROC      (0xC3UL) /**< @brief #UART_IRQHandler1  */
#define BRCM_SWDSGN_UART_IRQ_HANDLER2_PROC      (0xC4UL) /**< @brief #UART_IRQHandler2  */
/** @} */

/**
    @trace #BRCM_SWARCH_UART_SYS_CMD_REQUEST_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
*/
int32_t UART_SysCmdReq(UART_CmdType aCmd, UART_IOType *const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t ret = BCM_ERR_UNKNOWN;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_URT_ID;
        sysReqIO.magicID = SVC_MAGIC_URT_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t *)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;
        ret = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK == ret) {
            if (BCM_ERR_OK == sysReqIO.response) {
                ret = aIO->retVal;
            } else {
                ret = sysReqIO.response;
            }
        }
    }

    return ret;
}

/**
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER0_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_HANDLER
*/
ISR2(UART_IRQHandler0)
{
    UART_IRQHandler(0UL);
}

/**
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER1_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_HANDLER
*/
ISR2(UART_IRQHandler1)
{
    UART_IRQHandler(1UL);
}

/**
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER2_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_HANDLER
*/
ISR2(UART_IRQHandler2)
{
    UART_IRQHandler(2UL);
}

/** @} */
