/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_utils_err BCM Error Types
    @ingroup grp_utils

    @addtogroup grp_utils_err
    @{
    @section sec_err_overview Overview
    All Components use these errors as error return types.
    <BR>All Components use BCM_ReportError function for reporting errors.

    @file bcm_err.h
    @brief BCM Error Types
    This header file contains the interface functions for error types
    and error reporting function.
    @version 1.0 Initial Version
*/


#ifndef BCM_ERR_H
#define BCM_ERR_H

#include <stdint.h>
#include "bcm_comp.h"
/**
    @name BCM Error API IDs
    @{
    @brief API IDs for BCM Error APIs
*/
#define BRCM_SWARCH_BCM_ERR_MACRO         (0x00U)   /**< @brief #BCM_ERR_OK           */
#define BRCM_SWARCH_BCM_MACRO             (0x01U)   /**< @brief #BCM_LOGMASK_KERNEL   */
#define BRCM_SWARCH_BCM_ERR_REPORT_PROC   (0x02U)   /**< @brief #BCM_ReportError      */
#define BRCM_SWARCH_BCM_ERROR_TYPE        (0x03U)   /**< @brief #BCM_ErrorType        */
#define BRCM_SWARCH_BCM_ERR_CUSTOM_MACRO  (0x04U)   /**< @brief #BCM_ERR_MAX_CUSTOM_VALUES */
#define BRCM_SWARCH_BCM_GET_ERROR_PROC    (0x05U)   /**< @brief #BCM_GetError         */
/** @} */

/**
    @name BCM Error Codes
    @{
    @brief Error return values

    @trace #BRCM_SWREQ_BCM_ERR_CODES
*/
#define BCM_ERR_OK              (0x0)  /**< @brief No Error                          */
#define BCM_ERR_BUSY            (0x1)  /**< @brief Device or resource busy           */
#define BCM_ERR_NODEV           (0x2)  /**< @brief No device found                   */
#define BCM_ERR_NOT_FOUND       (0x3)  /**< @brief Not Found                         */
#define BCM_ERR_NOMEM           (0x4)  /**< @brief Out of memory                     */
#define BCM_ERR_NOSUPPORT       (0x5)  /**< @brief Not supported                     */
#define BCM_ERR_INVAL_PARAMS    (0x6)  /**< @brief Invalid argument                  */
#define BCM_ERR_INVAL_MAGIC     (0x7)  /**< @brief Invalid magic number              */
#define BCM_ERR_INVAL_STATE     (0x8)  /**< @brief Invalid state                     */
#define BCM_ERR_INVAL_BUF_STATE (0x9)  /**< @brief Invalid buffer state              */
#define BCM_ERR_EAGAIN          (0xA)  /**< @brief Try again                         */
#define BCM_ERR_TIME_OUT        (0xB)  /**< @brief Timeout                           */
#define BCM_ERR_UNINIT          (0xC)  /**< @brief Device or resource not initialized*/
#define BCM_ERR_CANCELLED       (0xD)  /**< @brief Cancel request success            */
#define BCM_ERR_DATA_INTEG      (0xE)  /**< @brief Data integrity error              */
#define BCM_ERR_AUTH_FAILED     (0xF)  /**< @brief Authentication error              */
#define BCM_ERR_VERSION         (0x10) /**< @brief Wrong version of hw/sw            */
#define BCM_ERR_BUS_FAILURE     (0x11) /**< @brief Bus Failure                       */
#define BCM_ERR_NACK            (0x12) /**< @brief NACK error                        */
#define BCM_ERR_MAX_ATTEMPS     (0x13) /**< @brief Maximum num of attempts           */
#define BCM_ERR_UNKNOWN         (0x14) /**< @brief Unknown error                     */
#define BCM_ERR_CUSTOM          (0x15) /**< @brief Module specific error             */
#define BCM_ERR_NOPERM          (0x16) /**< @brief Permission denied                 */
/** @} */

/**
    @name Masks for KLOG/ULOG
    @{
    @brief Mask for KLOG and ULOG in Component ID

    When an error is reported by a component, this bit in the component will identify whether the error is reported
    from Privileged context or Unprivileged context

    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
#define BCM_LOGMASK_KERNEL (0x8000U)  /**< @brief mask for routing the error to KLOG */
#define BCM_LOGMASK_USER   (0x7FFFU)  /**< @brief mask for routing the error to ULOG */
/** @} */

/** @brief Report a component error to the system

    This API is called by components to report an error to the system.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCompID     16-bit component ID
    @param[in]      aInstanceID 8-bit instance ID (starting from 0)
    @param[in]      aApiID      8-bit component specific API or interface ID
    @param[in]      aErrorID    8-bit error ID
    @param[in]      aNumInts    Number of valid entries in aValues
    @param[in]      aValues     Pointer to 32-bit data for print/debug

    @return void

    @post None

    @trace #BRCM_SWREQ_BCM_ERR_REPORT

    @limitations None
*/
void BCM_ReportError(BCM_CompIDType aCompID,
                       uint8_t aInstanceID,
                       uint8_t aApiID,
                       int32_t aErrorID,
                       uint32_t aNumInts,
                       const uint32_t *const aValues);

/**
    @brief Error reporting - maximum custom values

    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
#define BCM_ERR_MAX_CUSTOM_VALUES   (4UL)

/**
    @brief Error information Type

    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
typedef struct _BCM_ErrorType {
    BCM_CompIDType comp;
    uint8_t instance;
    uint8_t api;
    int32_t error;
    uint32_t custom[BCM_ERR_MAX_CUSTOM_VALUES];
} BCM_ErrorType;

/** @brief Get error information

    This API is Console/Shell to log errors

    @behavior Sync, Non-reentrant

    @pre None

    @param[out]     aError     Error Type pointer to be filled

    @return  #BCM_ERR_OK        Error type is filled
             #BCM_ERR_NOMEM     No Errors in memory

    @post None

    @trace #BRCM_SWREQ_BCM_ERR_REPORT

    @limitations None
*/
int32_t BCM_GetError(BCM_ErrorType *aError);

#endif /* BCM_ERR_H */
/** @} */

