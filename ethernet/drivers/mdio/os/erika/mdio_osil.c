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

/**
    @addtogroup grp_mdiodrv_il
    @{

    @file mdio_osil.c
    @brief MDIO driver system integration
    This source file contains the integration of mdio driver to system
    @version 0.01 Initial draft
*/

#include <mdio.h>
#include <osil/mdio_osil.h>
#include "ee.h"
#include <utils.h>
#include <comms_osil.h>

/**
    @name MDIO interface IDs
    @{
    @brief MDIO integration design IDs.
*/
#define BRCM_SWDSGN_MDIO_OSILREPORTERROR_PROC       (0xE8U) /**< @brief #MDIO_OSILReportError API ID */
#define BRCM_SWDSGN_MDIO_TIMERCB_PROC               (0xE9U) /**< @brief #MDIO_TimerCb API ID */
/** @} */

/**
 * @brief MDIO osil report error function
 * @trace #BRCM_SWARCH_MDIO_TIMERCB_PROC
 * @trace #BRCM_SWREQ_MDIO_OPERATIONS
 */
static void MDIO_OSILReportError(uint16_t aCompID, uint8_t aInstanceID,
        uint8_t aApiID, int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
 * @brief High resolution timer callback handler
 * @trace #BRCM_SWARCH_MDIO_TIMERCB_PROC
 * @trace #BRCM_SWREQ_MDIO_OPERATIONS
 */
void MDIO_TimerCb(void)
{
    StatusType status = E_OK;
    uint32_t isJobComp = FALSE;

    MDIO_StateHandler(&isJobComp);

    if (TRUE == isJobComp) {
        status = SetEvent(MDIO_STATE_HANDLER_TASK, MDIO_STATE_HANDLER_EVENT);
        if (E_OK != status) {
            MDIO_OSILReportError(BCM_MIO_ID, (uint8_t)0UL,
                    BRCM_SWARCH_MDIO_TIMERCB_PROC, 0L,
                    status, 0UL, 0UL, 0UL);
        }
    }
}

/** @} */
