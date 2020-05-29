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
    @defgroup grp_uconsoledrv_drv_impl UCONSOLE Driver Design
    @ingroup grp_uartdrv

    @addtogroup grp_uconsoledrv_drv_impl
    @{

    @file drivers/uart/lib/uconsole_drv.c
    @brief UART Driver Design

    @version 0.1 Initial version
*/

#include <stdlib.h>
#include <stdint.h>
#include <svc.h>
#include <utils.h>
#include <uconsole.h>
#include <uconsole_osil.h>
#include <circ_queue.h>

/**
    @name UCONSOLE Design IDs
    @{
    @brief Design IDs for UCONSOLE

*/
#define BRCM_SWDSGN_UCONSOLE_RXDATAQ_GLOBAL         (0xA8U) /**< @brief #UCONSOLE_RxDataQ       */
#define BRCM_SWDSGN_UCONSOLE_DRV_READ_PROC          (0xA9U) /**< @brief #UCONSOLE_DrvRead       */
#define BRCM_SWDSGN_UCONSOLE_SVC_IO_TYPE            (0xAAU) /**< @brief #UCONSOLE_SVCIOType     */
#define BRCM_SWDSGN_UCONSOLE_RCVIND_PROC            (0xABU) /**< @brief #UCONSOLE_RcvIndication */
#define BRCM_SWDSGN_UCONSOLE_ERRIND_PROC            (0xABU) /**< @brief #UCONSOLE_ErrIndication */
#define BRCM_SWDSGN_UCONSOLE_SYS_CMD_HANDLER_PROC   (0xACU) /**< @brief #UCONSOLE_SysCmdHandler */
/** @} */

/**
    @trace #BRCM_SWARCH_UCONSOLE_RCV_INDICATION_PROC
    @trace #BRCM_SWREQ_UCONSOLE_COMMUNICATION
*/
CIRCQ_V2_DEFINE(UCONSOLE_RxDataQ, 6UL, uint8_t, UCONSOLE_RxDataBuffer, UCONSOLE_RxDataIdx,
                CIRCQ_OVERWRITE_DISABLE, ".data.drivers.uconsole");

/**
    @trace #BRCM_SWARCH_UCONSOLE_READ_PROC
    @trace #BRCM_SWARCH_UCONSOLE_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UCONSOLE_COMMUNICATION
*/
static int32_t UCONSOLE_DrvRead(char * const aCmdStr, uint32_t *aSize)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if ((aCmdStr != NULL) && (aSize != NULL) && (*aSize > 0UL)) {
        *aSize = CIRCQ_Pop(&UCONSOLE_RxDataQ, aCmdStr, *aSize);
        ret = BCM_ERR_OK;
    }
    return ret;
}

/**
    @trace #BRCM_SWARCH_UCONSOLE_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER
*/
typedef union _UCONSOLE_SVCIOType {
    uint8_t *data;
    UCONSOLE_IOType *io;
} UCONSOLE_SVCIOType;

/** @} */

/**
    @addtogroup grp_uconsoledrv_osil
    @{
*/

/**
    @trace #BRCM_SWARCH_UCONSOLE_RCV_INDICATION_PROC
    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER
*/
void UCONSOLE_RcvIndication(uint8_t * aData, uint32_t aSize)
{
    int32_t err = BCM_ERR_OK;
    uint32_t isEmpty;

    err = CIRCQ_Push(&UCONSOLE_RxDataQ, (char*)aData, aSize);
    if (BCM_ERR_OK == err) {
        err = CIRCQ_IsEmpty(&UCONSOLE_RxDataQ, &isEmpty);
        /* Send a Console receive indication only if the Q is non-empty */
        if ((BCM_ERR_OK == err) && (isEmpty == 0UL)) {
            UCONSOLE_NotifyRcvIndication();
        }
    }
}


/**
    @trace #BRCM_SWARCH_UCONSOLE_ERR_INDICATION_PROC
    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER
*/
void UCONSOLE_ErrIndication(UART_ErrorType aError, uint8_t aData)
{
}

/**
    @trace #BRCM_SWARCH_UCONSOLE_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER
*/
void UCONSOLE_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    UCONSOLE_SVCIOType uconsoleio;
    uconsoleio.data = aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_SHL_ID == aMagicID) {
            retVal = BCM_ERR_OK;
            switch (aCmd) {
            case UCONSOLE_CMD_READ:
                UCONSOLE_DrvRead(uconsoleio.io->data, &uconsoleio.io->size);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        uconsoleio.io->retVal = retVal;
    }
    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_SHL_ID, 0U, BRCM_SWARCH_UCONSOLE_SYS_CMD_HANDLER_PROC, retVal,
                4UL, values);
    }
}

/** @} */
