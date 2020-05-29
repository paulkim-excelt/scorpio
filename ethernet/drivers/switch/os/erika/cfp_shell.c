/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    constitutes the valuable trade secrCFP of Broadcom, and you shall use all
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

/** @defgroup grp_cfp_il_shell Shell
    @ingroup grp_cfp_il

    @addtogroup grp_cfp_il_shell
    @{
    @section sec_cfp_il_shell Shell
    This page describes the CFP shell

    @file cfp_shell.c
    @brief Shell commands for CFP
    Shell commands to configure and query status/statistics
    @version 0.1 Initial version
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <utils.h>
#include "cfp_drv.h"

#define GetModuleLogLevel() ULOG_LVL_INFO

/** @brief Convert CFP L3 framing to string

    Convert CFP L3 framing to string

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aL3Framing               CFP L3 framing

    @return      String representation of CFP L3 framing

    @post None

    @limitations None

    @code{.unparsed}
    CFP_L3FRAMING_IPV4  -> "IPv4"
    CFP_L3FRAMING_IPV6  -> "IPv6"
    CFP_L3FRAMING_NONIP -> "non-IP"
    @endcode
*/
static char* CFP_L3FramingToStr(const uint32_t aL3Framing)
{
    switch(aL3Framing)
    {
        case CFP_L3FRAMING_IPV4:
            return "IPv4";
        case CFP_L3FRAMING_IPV6:
            return "IPv6";
        case CFP_L3FRAMING_NONIP:
            return "non-IP";
        default:
            return "Error-Format";
    }
}

/** @brief Convert CFP format to string

    Convert CFP format to string

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aFormat                CFP format

    @return      String representation of CFP format

    @post None

    @limitations None

    @code{.unparsed}
    0UL -> "IPv4"
    1UL -> "IPv6"
    2UL -> "non-IP"
    @endcode
*/
static char* CFP_BaseToStr(const uint32_t aBase)
{
    switch(aBase)
    {
        case 0UL:
            return "SOP";
        case 2UL:
            return "endL2Hdr";
        case 3UL:
            return "endL3Hdr";
        default:
            return "Error-Base";
    }
}

/** @brief Display CFP Status

    Display CFP TCAM summary (from S/W)

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aArgString               Command string
    @param[in]   aArgStringLen            Command string length

    @return      void

    @post None

    @limitations None

    @code{.unparsed}
    @endcode
*/
static void CFP_Status(char* aArgString, uint32_t aArgStringLen)
{
    CFP_RuleType config;
    uint32_t     row;
    uint32_t     udf;
    uint32_t     base;
    uint32_t     offset;
    int32_t      ret;

    for (row = 0UL; row < CFP_MAX_RULES; ++row) {
        ret = CFP_GetRowConfig(0UL, row, &config);
        if (BCM_ERR_OK == ret) {
            ULOG_INFO("\n%u) Slice %u, %s\n    UDFs: ",
                      row, (config.rowAndSlice & CFP_ROWANDSLICE_SLICE_MASK) >> CFP_ROWANDSLICE_SLICE_SHIFT,
                      CFP_L3FramingToStr(config.key.l3Framing));
            for (udf = 0UL; udf < config.key.numEnabledUDFs; ++udf) {
                    base   = (config.key.udfList[udf].baseAndOffset & CFP_UDF_BASE_MASK) >> CFP_UDF_BASE_SHIFT;
                    offset = 2UL * ((config.key.udfList[udf].baseAndOffset & CFP_UDF_OFFSET_MASK) >> CFP_UDF_OFFSET_SHIFT);

                    ULOG_INFO("%u) %u bytes from %s = 0x%x, ", udf, offset, CFP_BaseToStr(base), config.key.udfList[udf].value);
            }
            ULOG_INFO("\n\n");
        }
    }
}

/**
    @code{.unparsed}
    Parse input command string if not NULL
    if command is "status":
        Invoke CFP_Status()
    else
        show help
    @endcode
*/
void CFP_Shell(uint32_t aConsoleID,
               char* aArgString,
               uint32_t aArgStringLen)
{
    if (NULL != aArgString) {

        if (0 == strncmp(aArgString, "status", 6UL)) {
            CFP_Status(aArgString, aArgStringLen);
        } else {
            CONSOLE_Write(aConsoleID, "cfp status\n");
        }

    } else {
        CONSOLE_Write(aConsoleID, "cfp status\n");
    }
}
/** @} */
