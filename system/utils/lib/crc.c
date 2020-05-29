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
    @defgroup grp_utils_crc_impl CRC Implementation
    @ingroup grp_utils

    @addtogroup grp_utils_crc_impl
    @{

    @file crc.c
    @brief CRC Implementation

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <stdlib.h>
#include <crc.h>
#include <cortex.h>

/**
    @name CRC Design IDs
    @{
    @brief Design IDs for CRC
*/
#define BRCM_SWDSGN_BCM_CRC32_PROC          (0x80U)   /**< @brief #BCM_CRC32        */
/** @} */

/** @} */

/**
    @addtogroup grp_utils_crc
    @{
*/

/**
    @trace #BRCM_SWARCH_BCM_CRC32_PROC
    @trace #BRCM_SWREQ_BCM_UTILS
*/
uint32_t BCM_CRC32(const uint8_t *const aAddr,
                   uint32_t aLen,
                   uint32_t aPoly)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;
    uint32_t polynomial = CORTEX_BitReverse(aPoly);

    i = 0UL;
    crc = 0UL;

    if ((polynomial & 0xFUL) == 0UL) {
        uint32_t crc_table[16UL] =  {
            0x0UL,
            (polynomial >> 3UL),
            (polynomial >> 2UL),
            (polynomial >> 2UL) ^ (polynomial >> 3UL),
            (polynomial >> 1UL),
            (polynomial >> 1UL) ^ (polynomial >> 3UL),
            (polynomial >> 1UL) ^ (polynomial >> 2UL),
            (polynomial >> 1UL) ^ (polynomial >> 2UL) ^ (polynomial >> 3UL),
            (polynomial >> 0UL),
            (polynomial >> 0UL) ^ (polynomial >> 3UL),
            (polynomial >> 0UL) ^ (polynomial >> 2UL),
            (polynomial >> 0UL) ^ (polynomial >> 2UL) ^ (polynomial >> 3UL),
            (polynomial >> 0UL) ^ (polynomial >> 1UL),
            (polynomial >> 0UL) ^ (polynomial >> 1UL) ^ (polynomial >> 3UL),
            (polynomial >> 0UL) ^ (polynomial >> 1UL) ^ (polynomial >> 2UL),
            (polynomial >> 0UL) ^ (polynomial >> 1UL) ^ (polynomial >> 2UL) ^ (polynomial >> 3UL),
        };
        while (aLen != 0UL) {
            byte = aAddr[i];
            byte = (byte ^ crc) & 0xFUL;
            crc = (crc >> 4UL) ^ crc_table[byte];
            byte = aAddr[i];
            byte = ((byte >> 4UL) ^ crc) & 0xFUL;
            crc = (crc >> 4UL) ^ crc_table[byte];
            i++;
            aLen--;
        }
    } else {
        while (aLen != 0UL) {
            byte = aAddr[i];
            for (j = 0UL; j <= 7UL; j++) {
                if (((crc ^ byte) & 1UL) != 0UL) {
                    crc = (crc >> 1UL) ^ polynomial;
                } else {
                    crc = crc >> 1UL;
                }
                byte = byte >> 1UL;
            }
            i++;
            aLen--;
        }
    }

    return ~crc;
}
/** @} */
