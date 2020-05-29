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
 File Name: ethxcvr_osil.c
 Descritpion: This file implements OSIL for Ethernet Transceiver driver.
******************************************************************************/

#include <system.h>
#include <eth_xcvr.h>
#include <osil/ethxcvr_osil.h>
#include <comms_osil.h>
#include <chip_config.h>
#include "ee.h"
#include "ee_internal.h"

/**
    @name ETHXCVR OSIL Design IDs
    @{
    @brief Design IDs for ETHXCVR OSIL
*/
#define BRCM_SWDSGN_ETHXCVR_SYSCMDREQ_PROC     (0x82U) /**< @brief #ETHXCVR_SysCmdReq */
#define BRCM_SWDSGN_ETHXCVR_SVCIO_TYPE         (0x82U) /**< @brief #ETHXCVR_SVCIOType */
#define BRCM_SWDSGN_ETHXCVR_SYSCMDHANDLER_PROC (0x82U) /**< @brief #ETHXCVR_SysCmdHandler */
/** @} */

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    Build SVC command structure #SVC_RequestType
    sysReqIO.sysReqID = BCM_XVR_SVC_ID
    sysReqIO.magicID = BCM_XVR_SVC_MAGIC_ID
    sysReqIO.cmd = aCmd
    sysReqIO.sysIO.ethIO = aIO
    sysReqIO.response = BCM_ERR_UNKNOWN

    Send SVC command
    ret = SVC_Request(&sysReqIO)
    if ret is not BCM_ERR_OK
        ret = BCM_ERR_UNKNOWN
    else if sysReqIO.response is not BCM_ERR_OK
        ret = BCM_ERR_UNKNOWN
    else
        ret = aIO->retVal
    return ret
    @endcode

    @trace #BRCM_SWARCH_ETHXCVR_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
int32_t ETHXCVR_SysCmdReq(ETHXCVR_CmdType aCmd, ETHXCVR_IOType *const aIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_XVR_ID;
        sysReqIO.magicID = SVC_MAGIC_XVR_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t*)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
        } else if (BCM_ERR_OK != sysReqIO.response) {
            retVal = BCM_ERR_UNKNOWN;
        } else {
            retVal = aIO->retVal;
        }
    }

    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_ETHXCVR_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef union _ETHXCVR_SVCIOType {
    uint8_t    *data;
    ETHXCVR_IOType *io;
} ETHXCVR_SVCIOType;


/**
    @code{.c}
    if aSysIO.ethIO is not NULL
        if aMagicID is SVC_MAGIC_XVR_ID
            aSysIO.ethIO.retVal = ETHXCVR_CmdHandler(aCmd, aSysIO.ethIO)
        else
            aSysIO.ethIO.retVal = BCM_ERR_INVAL_MAGIC
    @endcode

    @trace #BRCM_SWARCH_ETHXCVR_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
//! [Usage of ETHXCVR_CmdHandler]
void ETHXCVR_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    ETHXCVR_SVCIOType ether;
    ether.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_XVR_ID == aMagicID) {
            ether.io->retVal = ETHXCVR_CmdHandler(aCmd, ether.io);
        } else {
            ether.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
}
//! [Usage of ETHXCVR_CmdHandler]

/**
    @trace #BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER0_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
#ifdef ENABLE_ETH_BRPHY
//! [Usage of ETHXCVR_LinkIRQHandler0]
ISR2(ETHXCVR_LinkIRQHandler0)
{
    StatusType status = E_OK;
    ETHXCVR_LinkIRQHandler(0U);
    status = SetEvent(ETH_IRQ_PROCESS_TASK, ETHER_XCVR_LINK_EVENT);
    if (E_OK != status) {
        const uint32_t values[4] = {(uint32_t)status, 0UL, 0UL, 0UL};
        BCM_ReportError(BCM_XVR_ID, 0U, BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER0_PROC, BCM_ERR_INVAL_PARAMS, 4UL, values);
    }
}
//! [Usage of ETHXCVR_LinkIRQHandler0]
#endif /* ENABLE_ETH_BRPHY */

/* Nothing past this line */
