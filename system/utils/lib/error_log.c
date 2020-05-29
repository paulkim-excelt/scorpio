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
    @defgroup grp_utils_err_log_impl BCM Error Log Implementation
    @ingroup grp_utils

    @addtogroup grp_utils_err_log_impl
    @{

    Note: This module should be used only for Bootloader 

    @file system/utils/lib/error_log.c
    @brief Error reporting and Query
    This file implements Error Reporting function.

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <compiler.h>
#include <utils.h>
#include <bcm_err.h>
#include <stddef.h>
#include <bl_log.h>
#include <lw_queue.h>
#include <compiler.h>

#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)
/**
    @name Error Reporting Design IDs
    @{
    @brief Design IDs for Error Reporting
*/
#define BRCM_SWDSGN_BCM_ERR_REPORT_LOG_PROC       (0xA9U)   /**< @brief #BCM_ReportError   */
/** @} */

BCM_ErrorType aError_buf[15];
LWQ_BufferType aErr_LWQ = 0;
/**
    @addtogroup grp_utils_err_log_impl
    @{
*/

/**
    To be implemented to perform a log over UART and use a LWQ to push

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
    int32_t ret;
    static uint8_t err_wrIdx;
    static uint32_t LWQ_State = BCM_ERR_OK;
    uint8_t name[4UL] = {'\0','\0','\0','\0'};

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

    if (LWQ_State != BCM_ERR_NOMEM) {
        ret = LWQ_Push(&aErr_LWQ, err_wrIdx);
        if (ret == BCM_ERR_OK) {
            BCM_MemCpy((void *) &(aError_buf[err_wrIdx]), (const void *) &error, sizeof(BCM_ErrorType));
            err_wrIdx++;
        } else if (ret == BCM_ERR_NOMEM) {
            LWQ_State = BCM_ERR_NOMEM;
        }
    }

#if defined(BL_ENABLE_UART_PL011)
    name[0UL] = (uint8_t)(65U + ((error.comp & 0x7C00U) >> 10U));
    name[1UL] = (uint8_t)(65U + ((error.comp & 0x3E0U) >> 5U));
    name[2UL] = (uint8_t)(65U + (error.comp & 0x1FU));

    BL_LOG_ERR("[%s] [A:0x%02x] [I:0x%02x] [E:0x%02x] [DATA:0x%08x,0x%08x,0x%08x] [L:%d]\n",\
            name, error.api, error.instance, error.error, error.custom[0UL],\
            error.custom[1UL], error.custom[2UL], error.custom[3UL]);
#endif
}

/** @} */
