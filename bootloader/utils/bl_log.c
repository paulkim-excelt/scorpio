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

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <bl_chip_config.h>
#include <bl_utils.h>
#include <bl_uart.h>
#include <bl_log.h>
#include <log.h>

/* Macros */
#define BL_BASE_DECIMAL        (10UL)
#define BL_BASE_HEXADECIMAL    (16UL)
#define BL_LOG_BUF_SIZE        (128UL)
#define MAX_BUF_SIZE           (24UL)


static void BL_Log_PrintBuff(const char *const aBuf)
{
    uint32_t len;
    uint32_t i;

    len = strnlen(aBuf, BL_LOG_BUF_SIZE);
    for (i = 0UL; i < len; i++) {
        BL_UART_PutChar(BL_LOG_UART_HW_ID, aBuf[i]);
    }
}

/**
    @code{.c}
    len = strnlen(aBuf, 128)
    for i = 0 to len
        UART_PutChar(0, aBuf[i])
    @endcode
*/
void BL_Log_Printf(uint32_t aStrMaxSize,
                   const char *const aFmt,
                   ...)
{
    char aLogBuf[BL_LOG_BUF_SIZE] = {0};
    va_list args;
    va_start( args, aFmt );
    LOG_Printf(aLogBuf, aStrMaxSize, aFmt, args);
    va_end(args);
    BL_Log_PrintBuff(aLogBuf);
}
