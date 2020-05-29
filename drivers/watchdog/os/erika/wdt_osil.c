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

/**
    @addtogroup grp_wdtdrv_il
    @{

    @file wdt_osil.c
    @brief Watcdog driver Integration
    This source file contains the integration of Watchdog driver to system
    @version 1.01 Imported from docx
*/
#include <osil/wdt_osil.h>
#include <stddef.h>
#include "ee.h"

/**
    @name WDT Driver OSIL API IDs
    @{
    @brief API IDs for WDT OSIL driver
*/
#define BRCM_SWDSGN_WDT_SYSCMDREQ_PROC         (0x50U) /**< @brief #WDT_SysCmdReq */
/** @} */
/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    if aWDTIO is not NULL
        build SVC request
        ret =  SVC_Request(&sysReqIO)
        if ret is not BCM_ERR_OK
            ret = BCM_ERR_UNKNOWN
        else if  sysReqIO.response is not BCM_ERR_OK
            ret = sysReqIO.response
        else
            retVal = aWDTIO->retVal
    @endcode
    @trace  #BRCM_SWARCH_WDT_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
int32_t WDT_SysCmdReq(WDT_CmdType aCmd, WDT_IOType *const aWDTIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aWDTIO) {
        sysReqIO.sysReqID = SVC_WDT_ID;
        sysReqIO.magicID = SVC_MAGIC_WDT_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t *)aWDTIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
        } else if (BCM_ERR_OK != sysReqIO.response) {
            retVal = sysReqIO.response;
        } else {
            retVal = aWDTIO->retVal;
        }
    }

    return retVal;
}

/** @} */
