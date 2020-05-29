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

#include <stdint.h>
#include <crc.h>
#include <bcm_err.h>

static uint32_t EDC_BitReverse(uint32_t aValue)
{
    uint32_t val = aValue;

    val = ((val & 0x55555555UL) << 1UL) | ((val >> 1UL) & 0x55555555UL);
    val = ((val & 0x33333333UL) << 2UL) | ((val >> 2UL) & 0x33333333UL);
    val = ((val & 0x0F0F0F0FUL) << 4UL) | ((val >> 4UL) & 0x0F0F0F0FUL);
    val = (val << 24UL) | ((val & 0xFF00UL) << 8UL) |
        ((val >> 8UL) & 0xFF00UL) | (val >> 24UL);

    return val;
}

uint32_t BCM_CRC32(const uint8_t *const aAddr,
                   uint32_t aLen,
                   uint32_t aPoly)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = EDC_BitReverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ aPoly;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return EDC_BitReverse(~crc);
}
