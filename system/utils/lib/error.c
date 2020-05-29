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
    @defgroup grp_utils_err_impl BCM Error Implementation
    @ingroup grp_utils

    @addtogroup grp_utils_err_impl
    @{

    @file error.c
    @brief Error reporting and Query
    This file implements Error Reporting function.

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <compiler.h>
#include <utils.h>
#include <bcm_err.h>
#include <stddef.h>
#include <circ_queue.h>

/**
    @name Error Reporting Design IDs
    @{
    @brief Design IDs for Error Reporting
*/
#define BRCM_SWDSGN_BCM_ERROR_QUEUE_GLOBAL    (0x98U)   /**< @brief #BCM_ErrorQ        */
#define BRCM_SWDSGN_BCM_ERR_REPORT_PROC       (0x99U)   /**< @brief #BCM_ReportError   */
#define BRCM_SWDSGN_BCM_GET_ERROR_PROC        (0x9AU)   /**< @brief #BCM_GetError      */
/** @} */

/**
    @brief Global Queue for Errors reported

    @anchor BCM_ErrorQ
    @trace #BRCM_SWARCH_BCM_ERR_REPORT_PROC
    @trace #BRCM_SWARCH_BCM_GET_ERROR_PROC
    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
CIRCQ_V2_DEFINE(BCM_ErrorQ, 5UL, BCM_ErrorType, BCM_ErrorBuffer, BCM_ErrorIdx,
                CIRCQ_OVERWRITE_DISABLE, ".data.error");

/** @} */
/**
    @addtogroup grp_utils_err
    @{
*/

/**
    @trace #BRCM_SWARCH_BCM_ERR_REPORT_PROC
    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
void BCM_ReportError(uint16_t aCompID,
                       uint8_t aInstanceID,
                       uint8_t aApiID,
                       int32_t aErrorID,
                       uint32_t aNumInts,
                       const uint32_t *const aValues)
{
    uint32_t i;
    BCM_ErrorType error = {
        .comp = aCompID,
        .instance = aInstanceID,
        .api = aApiID,
        .error = aErrorID,
    };
    if (NULL != aValues) {
        for (i = 0UL; (i < aNumInts) && (i < BCM_ERR_MAX_CUSTOM_VALUES); i++) {
            error.custom[i] = aValues[i];
        }
    }
    for (i = aNumInts; i < BCM_ERR_MAX_CUSTOM_VALUES; i++) {
        error.custom[i] = 0UL;
    }
    
    (void)CIRCQ_Push(&BCM_ErrorQ, (char*)&error, 1UL);
}

/**
    @trace #BRCM_SWARCH_BCM_ERR_REPORT_PROC
    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
int32_t BCM_GetError(BCM_ErrorType *aError)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t cnt = 0UL;

    if (NULL == aError) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        cnt = CIRCQ_Pop(&BCM_ErrorQ, (char*)aError, 1UL);
        if (cnt != 1UL) {
            ret = BCM_ERR_NOMEM;
        }
    }
    return ret;
}
/** @} */
