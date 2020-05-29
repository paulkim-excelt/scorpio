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
    @addtogroup grp_eth_switch_il
    @{

    @file switch_osil.c
    @brief Ethernet Switch Driver Integration
    This source file contains the integration of Ethernet Switch Driver to
    system
    @version 0.51 Imported from docx
*/
#include <rpc_async.h>
#include <eth_switch.h>
#include <switch_cfg.h>
#include <osil/eth_switch_osil.h>
#include <eth_xcvr.h>
#include <comms_osil.h>
#include <bcm_err.h>
#include <osil/bcm_osil.h>
#include "ee.h"

#define ETHERSWT_ID_0               (0UL)

/**
    @name Design IDs
    @{
    @brief Design IDs
*/
#define BRCM_SWDSGN_ETHERSWT_SYSCMDHANDLER_PROC (0x80U)   /**< @brief #ETHERSWT_SysCmdHandler  */
/** @} */

/**
    @code{.unparsed}
    BCM_ReportError();
    @endcode
*/
static void ETHERSWT_ErrorReport(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                                 uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), 0U, aApiID,
                    aErr, 4UL, values);
}

/**
    @code{.unparsed}
    @endcode
*/
void ETHERSWT_IntgLinkStateChangeInd(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType aLinkState)
{
    int32_t retVal;
    uint32_t lineNo;
    ETHERSWT_PortLinkInfoType portLinkInfo;
    ETHERSWT_PayloadType notifPayload;
    notifPayload.portLinkInfo = &portLinkInfo;

    /* Raise callback to the client */
    ETHERSWT_LinkStateChangeInd(aSwtID, aPortID, aLinkState);

    /* Raise async notification */
    portLinkInfo.port = aPortID;
    portLinkInfo.linkState = aLinkState;
    retVal = ETHERSWT_GetXcvrStats(aSwtID, aPortID, &portLinkInfo.portStats);
    lineNo = __LINE__;
    if (BCM_ERR_OK == retVal) {
        retVal = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_SWT_ID,
                ETHERSWT_EVENT_PORT_LINK_INFO,
                notifPayload.u8Ptr,
                sizeof(ETHERSWT_PortLinkInfoType));
        lineNo = __LINE__;
    }
    if (BCM_ERR_OK != retVal) {
        ETHERSWT_ErrorReport(
                BRCM_SWARCH_ETHERSWT_INTG_LINK_STATE_CHANGE_IND_PROC,
                retVal, aPortID, aLinkState, 0UL, lineNo);
    }
}

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    if aIO is not NULL {
        Build SVC request
        ret = SVC_Request(&sysReqIO)
        if ret is not BCM_ERR_OK
            ret = BCM_ERR_UNKNOWN
        else if sysReqIO.response is not BCM_ERR_OK
            ret = sysReqIO.response
        else
            ret = aIO->retVal
    }

    return ret
    @endcode
*/
int32_t ETHERSWT_SysCmdReq(ETHERSWT_CmdType aCmd, EthSwtIO *const aIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_SWT_ID;
        sysReqIO.magicID = SVC_MAGIC_SWT_ID;
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
*/
typedef union _EthSwt_SVCIOType {
    uint8_t *data;
    EthSwtIO *io;
} EthSwt_SVCIOType;

/**
    @code{.c}
    if aSysIO is not NULL {
        if aMagicID is SVC_MAGIC_SWT_ID {
            aSysIO.retVal = ETHERSWT_CmdHandler(aCmd, aSysIO)
        } else {
            aSysIO.retVal = BCM_ERR_INVAL_MAGIC
        }
    }
    @endcode
*/
//! [Usage of ETHERSWT_CmdHandler]
void ETHERSWT_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    EthSwt_SVCIOType ethSwitch;
    ethSwitch.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_SWT_ID == aMagicID) {
            ethSwitch.io->retVal = ETHERSWT_CmdHandler(aCmd, ethSwitch.io);
        } else {
            ethSwitch.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
}
//! [Usage of ETHERSWT_CmdHandler]

//! [Usage of ETHERSWT_LinkIRQHandler]
ISR2(ETHERSWT_Port0LinkIRQHandler)
{
    StatusType status = E_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 0UL);
    status = SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port1LinkIRQHandler)
{
    StatusType status = E_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 1UL);
    status = SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port2LinkIRQHandler)
{
    int32_t status = BCM_ERR_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 2UL);
    status = BCM_SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (BCM_ERR_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port3LinkIRQHandler)
{
    int32_t status = BCM_ERR_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 3UL);
    status = BCM_SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (BCM_ERR_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port4LinkIRQHandler)
{
    StatusType status = E_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 4UL);
    status = SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port5LinkIRQHandler)
{
    StatusType status = E_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 5UL);
    status = SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port6LinkIRQHandler)
{
    StatusType status = E_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 6UL);
    status = SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}

ISR2(ETHERSWT_Port8LinkIRQHandler)
{
    StatusType status = E_OK;
    ETHERSWT_LinkIRQHandler(ETHERSWT_ID_0, 8UL);
    status = SetEvent(ETHERSWT_IRQ_PROCESS_TASK, ETHERSWT_PORT_LINK_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}
//! [Usage of ETHERSWT_LinkIRQHandler]
/** @} */
