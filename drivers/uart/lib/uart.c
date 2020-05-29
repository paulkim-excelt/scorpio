/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_uartdrv_impl UART Client Design
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_impl
    @{

    @file drivers/uart/lib/uart.c
    @brief UART Client Design

    @version 0.1 Initial version
*/

#include <uart_osil.h>
#include <bcm_err.h>
#include <compiler.h>
#include <stddef.h>

/**
    @name UART Client Design IDs
    @{
    @brief Client Design IDs for UART

*/
#define BRCM_SWDSGN_UART_INIT_PROC          (0x80U) /**< @brief #UART_Init        */
#define BRCM_SWDSGN_UART_SEND_PROC          (0x81U) /**< @brief #UART_Send        */
#define BRCM_SWDSGN_UART_GET_TX_STATUS_PROC (0x82U) /**< @brief #UART_GetTxStatus */
#define BRCM_SWDSGN_UART_DEINIT_PROC        (0x83U) /**< @brief #UART_DeInit      */
#define BRCM_SWDSGN_UART_GET_STATS_PROC     (0x84U) /**< @brief #UART_GetStats    */
#define BRCM_SWDSGN_UART_SEND_BREAK_PROC    (0x85U) /**< @brief #UART_SendBreak   */
#define BRCM_SWDSGN_UART_CLR_BREAK_PROC     (0x86U) /**< @brief #UART_ClrBreak    */
#define BRCM_SWDSGN_UART_GET_STATE_PROC     (0x87U) /**< @brief #UART_GetState    */
/** @} */

/** @} */

/**
    @addtogroup grp_uartdrv_ifc
    @{
*/

/**
    @trace #BRCM_SWARCH_UART_INIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
void UART_Init(UART_HwIDType aId)
{
    int32_t ret;
    UART_IOType uartIO;

    uartIO.id = aId;
    uartIO.retVal = BCM_ERR_UNKNOWN;
    ret = UART_SysCmdReq(UART_CMD_INIT, &uartIO);
    if (BCM_ERR_OK == ret) {
    }
}

/**
    @trace #BRCM_SWARCH_UART_SEND_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
int32_t UART_Send(UART_HwIDType aId, const uint8_t *aData, uint32_t aLength)
{
    int32_t ret;
    UART_IOType uartIO;

    uartIO.id = aId;
    uartIO.data = aData;
    uartIO.size = aLength;
    uartIO.retVal = BCM_ERR_UNKNOWN;
    ret = UART_SysCmdReq(UART_CMD_SEND, &uartIO);
    return ret;
}

/**
    @trace #BRCM_SWARCH_UART_GET_TX_STATUS_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
UART_TxStatusType UART_GetTxStatus(UART_HwIDType aId)
{
    int32_t ret;
    UART_TxStatusType status = 0UL;
    UART_IOType uartIO;

    uartIO.id = aId;
    uartIO.retVal = BCM_ERR_UNKNOWN;
    uartIO.status = 0UL;
    ret = UART_SysCmdReq(UART_CMD_TX_STATUS, &uartIO);
    if (BCM_ERR_OK == ret) {
        status = uartIO.status;
    }
    return status;
}

/**
    @trace #BRCM_SWARCH_UART_SEND_BREAK_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
int32_t UART_SendBreak(UART_HwIDType aId)
{
    int32_t ret;
    UART_IOType uartIO;

    uartIO.id = aId;
    uartIO.retVal = BCM_ERR_UNKNOWN;
    ret = UART_SysCmdReq(UART_CMD_SEND_BREAK, &uartIO);
    return ret;
}

/**
    @trace #BRCM_SWARCH_UART_CLR_BREAK_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
int32_t UART_ClrBreak(UART_HwIDType aId)
{
    int32_t ret;
    UART_IOType uartIO;

    uartIO.id = aId;
    uartIO.retVal = BCM_ERR_UNKNOWN;
    ret = UART_SysCmdReq(UART_CMD_CLR_BREAK, &uartIO);
    return ret;
}

/**
    @trace #BRCM_SWARCH_UART_GET_STATE_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
int32_t UART_GetState(UART_HwIDType aId, UART_StateType * const aState)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    UART_IOType uartIO;

    if (NULL != aState) {
        uartIO.id = aId;
        uartIO.retVal = BCM_ERR_UNKNOWN;
        ret = UART_SysCmdReq(UART_CMD_GET_STATE, &uartIO);
        if (BCM_ERR_OK == ret) {
            *aState = uartIO.state;
        }
    }
    return ret;
}

/**
    @trace #BRCM_SWARCH_UART_GET_STATS_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
int32_t UART_GetStats(UART_HwIDType aId, UART_StatsType *const aStats)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    UART_IOType uartIO;

    if (NULL != aStats) {
        uartIO.id = aId;
        uartIO.retVal = BCM_ERR_UNKNOWN;
        ret = UART_SysCmdReq(UART_CMD_GET_STATS, &uartIO);
        if (BCM_ERR_OK == ret) {
            *aStats = uartIO.stats;
        }
    }
    return ret;
}

/**
    @trace #BRCM_SWARCH_UART_DEINIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
int32_t UART_DeInit(UART_HwIDType aId)
{
    int32_t ret;
    UART_IOType uartIO;

    uartIO.id = aId;
    uartIO.retVal = BCM_ERR_UNKNOWN;
    ret = UART_SysCmdReq(UART_CMD_DEINIT, &uartIO);
    return ret;
}

/** @} */
