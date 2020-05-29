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

/**
    @defgroup grp_uconsoledrv_impl UCONSOLE Design
    @ingroup grp_uartdrv

    @addtogroup grp_uconsoledrv_impl
    @{

    @file drivers/uart/lib/uconsole.c
    @brief UCONSOLE Client Design

    @version 0.1 Initial version
*/

#include <stdlib.h>
#include <string.h>
#include <uart.h>
#include <utils.h>
#include <uconsole.h>
#include <uconsole_osil.h>
#include <circ_queue.h>

/**
    @name UCONSOLE Design IDs
    @{
    @brief Design IDs for UCONSOLE

*/
#define BRCM_SWDSGN_UCONSOLE_TXDATAQ_GLOBAL     (0xA0U) /**< @brief #UCONSOLE_TxDataQ    */
#define BRCM_SWDSGN_UCONSOLE_TXBUFFER_GLOBAL    (0xA1U) /**< @brief #UCONSOLE_CurTxBuffer*/
#define BRCM_SWDSGN_UCONSOLE_READ_PROC          (0xA2U) /**< @brief #UCONSOLE_Read       */
#define BRCM_SWDSGN_UCONSOLE_WRITE_PROC         (0xA3U) /**< @brief #UCONSOLE_Write      */
/** @} */


/**
    @trace #BRCM_SWARCH_UCONSOLE_WRITE_PROC
    @trace #BRCM_SWREQ_UCONSOLE_COMMUNICATION
*/
CIRCQ_V2_DEFINE(UCONSOLE_TxDataQ, 10UL, uint8_t, UCONSOLE_TxDataBuffer, UCONSOLE_TxDataIdx,
                CIRCQ_OVERWRITE_ENABLE, ".data.uconsole");
/**
    @trace #BRCM_SWARCH_UCONSOLE_WRITE_PROC
    @trace #BRCM_SWREQ_UCONSOLE_COMMUNICATION
*/
uint8_t UCONSOLE_CurTxBuffer[64UL];

/** @} */

/**
    @addtogroup grp_uconsoledrv_ifc
    @{
*/

/**
    @trace #BRCM_SWARCH_UCONSOLE_READ_PROC
    @trace #BRCM_SWREQ_UCONSOLE_COMMUNICATION
*/
int32_t UCONSOLE_Read(char * const aCmdStr, uint32_t *aSize)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    UCONSOLE_IOType uconsoleIO;

    if (NULL != aSize) {
        uconsoleIO.data = aCmdStr;
        uconsoleIO.size = *aSize;
        uconsoleIO.retVal = BCM_ERR_UNKNOWN;
        ret = UCONSOLE_SysCmdReq(UCONSOLE_CMD_READ, &uconsoleIO);
        if (ret == BCM_ERR_OK) {
            *aSize = uconsoleIO.size;
        }
    }

    return ret;
}

/**
    @trace #BRCM_SWARCH_UCONSOLE_WRITE_PROC
    @trace #BRCM_SWREQ_UCONSOLE_COMMUNICATION
*/
int32_t UCONSOLE_Write(UART_HwIDType aHWId, const char * const aBuffer, uint32_t aSize)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t status = 0UL;
    uint32_t numRead;

    if ((aBuffer != NULL) && (aSize > 0UL)) {
        (void)CIRCQ_Push(&UCONSOLE_TxDataQ, aBuffer, aSize);
        ret = BCM_ERR_OK;
    } else if (0UL == aSize) {
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK == ret) {
        status = UART_GetTxStatus(aHWId);
        if ((status & UART_TX_STATUS_BUFFER_IDLE) == UART_TX_STATUS_BUFFER_IDLE) {
            numRead = CIRCQ_Pop(&UCONSOLE_TxDataQ, (char*)UCONSOLE_CurTxBuffer, 64UL);
            if (numRead > 0UL) {
                ret = UART_Send(aHWId, UCONSOLE_CurTxBuffer, numRead);
                if (ret != BCM_ERR_OK) {
                    ret = BCM_ERR_UNKNOWN;
                }
            }
        }
    }
    return ret;
}

/** @} */
