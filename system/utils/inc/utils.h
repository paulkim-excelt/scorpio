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
    @defgroup grp_utils_misc Miscellaneous
    @ingroup grp_utils

    @addtogroup grp_utils_misc
    @{
    @section sec_comp_overview Overview
    This file specifies common utility functions (miscellaneous) APIs

    @file utils.h
    @brief Common utility functiions

    @version 1.3 Export comments from docx
*/

#ifndef UTILS_H
#define UTILS_H

#include <arm_utils.h>
#include <stdint.h>
#include <compiler.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <bcm_utils.h>
/**
    @brief Find minimum of two values @a a and @a b

    @trace #BRCM_ARCH_UTILS_COMP_MAX_MACRO #BRCM_REQ_UTILS_COMP_MAX
*/
#define MIN(a, b)                   (((a) > (b)) ? (b) : (a))

/**
    @brief Find maximum of two values @a a and @a b

    @trace #BRCM_ARCH_UTILS_COMP_MAX_MACRO #BRCM_REQ_UTILS_COMP_MAX
*/
#define MAX(a, b)                   (((a) > (b)) ? (a) : (b))

/**
    @name Utils_COMP_State IDs
    @{
    @brief Define false and true

    @trace #BRCM_ARCH_UTILS_STATE_TYPE #BRCM_REQ_UTILS_GROUPMACRO
*/
#define FALSE                       (0UL)   /**< @brief FALSE Macro */
#define TRUE                        (1UL)   /**< @brief TRUE Macro */
/** @} */

/** @brief Modulo increment

    This API does modulo increment of the value. Range of Modulo incremented value will be
    from 0 to (aMax - 1UL).    If aValue = (aMax - 1UL), incremented value will be 0.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aValue      Value to be modulo incremented
    @param[in]   aMax        Max value for modulo increment

    @retval     Modulo incremented value

    @post None

    @trace  #BRCM_ARCH_UTILS_MISC_MODINC_PROC  #BRCM_REQ_UTILS_MEDIA_COMPONENT
*/
COMP_INLINE uint32_t ModInc(uint32_t aValue, uint32_t aMax)
{
    uint32_t tmpVal = aValue + 1UL;

    if (aMax == tmpVal) {
        tmpVal = 0UL;
    }

    return tmpVal;
}

/** @brief Modulo decrement

    This API does modulo decrement of the value. Range of Modulo decremented value will
    be from 0 to (aMax - 1UL). If aValue = 0, decremented value will be (aMax - 1UL).

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aValue      Value to be modulo decremented
    @param[in]   aMax        Max value for modulo decrement

    @retval     Modulo decremented value

    @post None

    @trace  #BRCM_ARCH_UTILS_MISC_MODDEC_PROC  #BRCM_REQ_UTILS_MEDIA_COMPONENT
*/
COMP_INLINE uint32_t ModDec(uint32_t aValue, uint32_t aMax)
{
    uint32_t tmpVal;

    if (0UL == aValue) {
        tmpVal = aMax - 1UL;
    } else {
        tmpVal = aValue - 1UL;
    }

    return tmpVal;
}

#endif /* UTILS_H */
/** @} */
