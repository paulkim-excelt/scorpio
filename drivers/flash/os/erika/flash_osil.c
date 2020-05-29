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
    @addtogroup grp_flashdrv_il
    @{

    @file flash_osil.c
    @brief Flash driver system integration
    This source file contains the integration of flash driver to system
    @version 1.31 Imported from docx
*/

#include <flash.h>
#include <osil/flash_osil.h>
#include <osil/flsmgr_osil.h>
#include "ee.h"

/**
    @name Flash interface IDs
    @{
    @brief FLASH integration design IDs.
*/
#define BRCM_SWDSGN_FLASH_IRQHANDLER0_PROC         (0xE8U) /**< @brief #FLASH_IRQHandler0 API ID */
#define BRCM_SWDSGN_FLASH_IRQHANDLER1_PROC         (0xE9U) /**< @brief #FLASH_IRQHandler1 API ID */
#define BRCM_SWDSGN_FLASH_SYSCMDREQ_PROC           (0xEAU) /**< @brief #FLASH_SysCmdReq API ID */
#define BRCM_SWDSGN_FLASH_IRQHANDLER_PROC          (0xEBU) /**< @brief #FLASH_IRQHandler API ID */
#define BRCM_SWDSGN_FLASH_OSILREPORTERROR_PROC     (0xECU) /**< @brief #FLASH_OSILReportError */
/** @} */

/**
 * @trace #BRCM_SWARCH_FLASH_IRQHANDLER0_PROC
 * @trace #BRCM_SWARCH_FLASH_IRQHANDLER1_PROC
 * @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
 */
static void FLASH_OSILReportError(uint16_t aCompID, uint8_t aInstanceID,
        uint8_t aApiID, int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, aInstanceID, aApiID, aErr, 4UL, values);
}

/* Interrupt Handlers */
/**
 * @brief Flash IRQ 0 handler
 * @trace #BRCM_SWARCH_FLASH_IRQHANDLER0_PROC
 * @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
 */
ISR2(FLASH_IRQHandler0)
{
    StatusType status = E_OK;

    FLASH_BusIRQHandler(0UL);

    status = SetEvent(FLASH_IRQ_PROCESS_TASK, FLASH0_EVENT);
    if (E_OK != status) {
        FLASH_OSILReportError(BCM_QSP_ID, (uint8_t)0UL,
                BRCM_SWARCH_FLASH_IRQHANDLER0_PROC, 0UL, status, 0UL, 0UL, 0UL);
        /* TODO: Add ASSERT/Crash */
    }
}

/**
 * @brief Flash IRQ 1 handler
 * @trace #BRCM_SWARCH_FLASH_IRQHANDLER1_PROC
 * @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
 */
ISR2(FLASH_IRQHandler1)
{
    StatusType status = E_OK;

    FLASH_BusIRQHandler(1UL);

    status = SetEvent(FLASH_IRQ_PROCESS_TASK, FLASH1_EVENT);
    if (E_OK != status) {
        FLASH_OSILReportError(BCM_QSP_ID, (uint8_t)1UL,
                BRCM_SWARCH_FLASH_IRQHANDLER1_PROC, 0L, 0UL, 0UL, 0UL, 0UL);
        /* TODO: Add ASSERT/Crash */
    }
}

/**
    @code{.c}
    ret = BCM_ERR_INVAL_PARAMS
    Build SVC Request
    ret = SVC_Request(&sysReqIO)
    if (ret is BCM_ERR_OK) and (sysReqIO.response is BCM_ERR_OK)
        ret = aIO->retVal
    else
        ret = sysReqIO.response

    return ret
    @endcode
    @trace #BRCM_SWARCH_FLASH_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE
*/
int32_t FLASH_SysCmdReq(FLASH_BusCmdType aCmd, FLASH_IOType * const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (aIO != NULL) {
        sysReqIO.sysReqID = SVC_QSP_ID;
        sysReqIO.magicID = SVC_MAGIC_QSP_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t *)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK == retVal) {
            if (BCM_ERR_OK == sysReqIO.response) {
                retVal = aIO->retVal;
            } else {
                retVal = sysReqIO.response;
            }
        }
    }

    return retVal;
}

/** @} */
