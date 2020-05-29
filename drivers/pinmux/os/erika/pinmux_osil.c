/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_pinmuxdrv_il
    @{

    @file pinmux_osil.c
    @brief PinMux driver Integration
    This source file contains the integration of PinMux driver to system
    @version 1.12 Imported from docx
*/

#include <osil/pinmux_osil.h>
#include <system.h>
#include <stddef.h>
#include "ee.h"

/**
    @name PinMux Integration IDs
    @{
    @brief PinMux driver integration design IDs
*/
#define BRCM_SWDSGN_PINMUX_SYS_CMD_REQ_PROC        (0x82U) /**< @brief #PINMUX_SysCmdReq */
/** @} */

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    Build SVC request
    if aPINMUXIO is not NULL
        ret = SVC_Request(&sysReqIO)
        if ret is BCM_ERR_OK
            ret = sysReqIO.response
        else ret = BCM_ERR_UNKNOWN

    return ret
    @endcode

    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_REQ_PROC
    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
int32_t PINMUX_SysCmdReq(PINMUX_CmdType aCmd, PINMUX_IOType *aPINMUXIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    sysReqIO.sysReqID = SVC_PMX_ID;
    sysReqIO.magicID = SVC_MAGIC_PMX_ID;
    sysReqIO.cmd = aCmd;
    sysReqIO.svcIO = (uint8_t *)aPINMUXIO;
    sysReqIO.response = BCM_ERR_UNKNOWN;

    if (NULL != aPINMUXIO) {
        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK != retVal) {
        retVal = BCM_ERR_UNKNOWN;
        } else if (BCM_ERR_OK != sysReqIO.response) {
            retVal = BCM_ERR_UNKNOWN;
        } else {
            retVal = aPINMUXIO->retVal;
        }
    }

    return retVal;
}

/** @} */
