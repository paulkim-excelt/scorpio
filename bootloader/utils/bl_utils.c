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
/******************************************************************************
 File Name: utils.c
 Descritpion: This file implements common util functions.
******************************************************************************/
#include <stdint.h>
#include <bl_compiler.h>
#include <bl_utils.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

/**
    @code{.c}
    memcpy(aDest, aSrc, aSize)
    @endcode
*/
void BL_BCM_MemCpy(void *aDest,
                   const void *aSrc,
                   uint32_t aSize)
{
    (void)memcpy(aDest, aSrc, aSize);
}

/**
    @code{.c}
    memset(aStr, (int32_t)aVal, aSize)
    @endcode
*/
void BL_BCM_MemSet(void *aStr,
                   uint8_t aVal,
                   uint32_t aSize)
{
    (void)memset(aStr, (int32_t)aVal, aSize);
}

/**
    @code{.c}
    memmove(aDest, aSrc, aSize)
    @endcode
*/
void BL_BCM_MemMove(void *aDest,
                    const void *aSrc,
                    uint32_t aSize)
{
    (void)memmove(aDest, aSrc, aSize);
}

/**
    @code{.c}
    memcmp(aBuf, aBufRef, aSize)
    @endcode
*/
int32_t BL_BCM_MemCmp(const void *aBuf,
                      const void *aBufRef,
                      uint32_t aSize)
{
    return memcmp(aBuf, aBufRef, aSize);
}

/**
    @code{.c}
    strncpy(aDest, aSrc, aSize)
    @endcode
*/
void BL_BCM_StrnCpy(void *aDest,
                    const void *aSrc,
                    uint32_t aSize)
{
    (void)strncpy(aDest, aSrc, aSize);
}

/**
    @code{.c}
    value = strtol(aBuf, aEndPtr, aBase)
    if (**aEndPtr != '\0')
        value = LONG_MAX
    return value
    @endcode
*/
int32_t BL_BCM_StrToLong(char *aBuf,
                         char **aEndPtr,
                         int32_t aBase)
{
    int32_t value;
    value = strtol(aBuf, aEndPtr, aBase);
    if ((**aEndPtr != '\0')) {
        value = LONG_MAX;
    }
    return value;
}
/* Nothing past this line */