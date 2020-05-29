/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
#include <bl_edc.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>

/**
    @code{.c}
    crc = BL_EDC_CalculateCRC(aAddr, aLen)
    if crc != aCRC
        return BL_BCM_ERR_DATA_INTEG
    else
        return BL_BCM_ERR_OK
    @endcode
*/
int32_t BL_EDC_ValidateCRC(const uint8_t *const aAddr,
                           uint32_t aLen,
                           uint64_t aCRC)
{
    int32_t err;
    uint32_t crc;

    crc = BL_EDC_CalculateCRC(aAddr, aLen, BL_EDC_CRC_INIT_SEED);
    if (crc != aCRC) {
        err = BL_BCM_ERR_DATA_INTEG;
    } else {
        err = BL_BCM_ERR_OK;
    }

    return err;
}

/**
    @code{.c}
    i = 0
    crc = 0
    while aLen != 0
        byte = aAddr[i]
        byte = BL_EDC_BitReverse(byte)
        for j = 0 to 7
            if (crc ^ byte) < 0
                crc = (crc << 1) ^ CRC32_POLY
            else
                crc = crc << 1
            byte = byte << 1
        i++
        aLen--
    crc = BL_EDC_BitReverse(~crc)

    return crc;
    @endcode
*/
uint32_t BL_EDC_CalculateCRC(const uint8_t *const aAddr,
                             uint32_t aLen,
                             uint32_t aSeedCRC)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;
    /* Polynomial bit reverse is performed to avoid bit
     * reversal of each byte and bit reversal of
     * penaultimate CRC value to generate final CRC.
     */
    uint32_t polyBitReverse = BL_BitReverse32(BL_CRC32_POLY);

    i = 0UL;
    crc = ~(aSeedCRC);

    if ((polyBitReverse & 0xFUL) == 0UL) {
        uint32_t crc_table[16UL] =  {
            0x0UL,
            (polyBitReverse >> 3UL),
            (polyBitReverse >> 2UL),
            (polyBitReverse >> 2UL) ^ (polyBitReverse >> 3UL),
            (polyBitReverse >> 1UL),
            (polyBitReverse >> 1UL) ^ (polyBitReverse >> 3UL),
            (polyBitReverse >> 1UL) ^ (polyBitReverse >> 2UL),
            (polyBitReverse >> 1UL) ^ (polyBitReverse >> 2UL) ^ (polyBitReverse >> 3UL),
            (polyBitReverse >> 0UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 3UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 2UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 2UL) ^ (polyBitReverse >> 3UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 1UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 1UL) ^ (polyBitReverse >> 3UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 1UL) ^ (polyBitReverse >> 2UL),
            (polyBitReverse >> 0UL) ^ (polyBitReverse >> 1UL) ^ (polyBitReverse >> 2UL) ^ (polyBitReverse >> 3UL),
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
                    crc = (crc >> 1UL) ^ polyBitReverse;
                } else {
                    crc = crc >> 1UL;
                }
                byte = byte >> 1UL;
            }
            i++;
            aLen--;
        }
    }

    crc = (~crc);

    return crc;
}

/**
    @code{.c}
    addr = aAddr
    temp = aAddr
    sum = 0

    count = aLen / sizeof(uint32_t)
    remain = aLen & 0x3
    for i = 0 to count
        sum += *(temp++)
    addr = &addr[count * sizeof(uint32_t)]
    if remain != 0UL
        for i = 0 to remain
            sum += *(addr++)
    if ~sum != aChcksm
        return BL_BCM_ERR_DATA_INTEG
    else
        return BL_BCM_ERR_OK
    @endcode
*/
int32_t BL_EDC_ValidateChcksm(uint8_t *const aAddr,
                              uint32_t aLen,
                              uint64_t aChcksm)
{
    int32_t err;
    uint32_t i;
    uint32_t remain;
    uint32_t count;
    uint8_t *addr = aAddr;
    uint32_t *temp = (uint32_t *)aAddr;
    uint64_t sum = 0ULL;

    count = aLen / sizeof(uint32_t);
    remain = aLen & 0x3UL;
    for (i = 0UL; i < count; i++) {
        sum += *(temp++);
    }
    addr = &addr[count * sizeof(uint32_t)];
    if (remain != 0UL) {
        for(i = 0UL; i < remain; i++) {
            sum += *(addr++);
        }
    }
    if (~sum != aChcksm) {
        err = BL_BCM_ERR_DATA_INTEG;
    } else {
        err = BL_BCM_ERR_OK;
    }

    return err;
}
