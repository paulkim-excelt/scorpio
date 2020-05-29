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
/******************************************************************************
 File Name: eth_osil.c
 Descritpion: This file implements OSIL for Ethernet driver.
******************************************************************************/

#include <system.h>
#include <ethernet.h>
#include <osil/eth_osil.h>
#include <comms_osil.h>
#if defined (ENABLE_NIF)
#include <nif.h>
#if defined (ENABLE_FQTSS) && !defined(ENABLE_ETHER_TEST) && !defined(ENABLE_WLAN)
#include <fqtss.h>
#endif
#endif
#include "ee.h"
#include "ee_internal.h"
#include <utils.h>
#if defined(ENABLE_MDIO)
#include <mdio.h>
#endif

const MDIO_CfgType ETHER_MdioCfg[] =
{
#if defined(ENABLE_MDIO)
    {
    .mode = MDIO_CTRLMODE_MASTER,
    .speed = 1000000UL,
    },
#if defined(__BCM8956X__)
    {
    .mode = MDIO_CTRLMODE_MASTER,
    .speed = 1000000UL,
    },
    {
    .mode = MDIO_CTRLMODE_MASTER,
    .speed = 1000000UL,
    },
#endif
#endif
};

int32_t ETHER_GetMdioConfig(const MDIO_CfgType **aCfg, uint32_t *aCount)
{
    int32_t retVal;

    if ((NULL == aCfg) || (NULL == aCount)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    *aCfg = &ETHER_MdioCfg[0];
    *aCount = sizeof(ETHER_MdioCfg)/sizeof(MDIO_CfgType);
    retVal = BCM_ERR_OK;

done:
    return retVal;
}

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    Build SVC command structure #SVC_RequestType
    sysReqIO.sysReqID = BCM_AMC_SVC_ID
    sysReqIO.magicID = BCM_AMC_SVC_MAGIC_ID
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
*/
int32_t ETHER_SysCmdReq(ETHER_CntlrIOCmdType aCmd, ETHER_IOType *const aIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_AMC_ID;
        sysReqIO.magicID = SVC_MAGIC_AMC_ID;
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
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    Build SVC command structure #SVC_RequestType
    sysReqIO.sysReqID = BCM_ETT_SVC_ID
    sysReqIO.magicID = BCM_ETT_SVC_MAGIC_ID
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
*/
int32_t ETHER_TimeSysCmdReq(ETHER_TimeIOCmdType aCmd, ETHER_TimeIOType *const aIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_ETT_ID;
        sysReqIO.magicID = SVC_MAGIC_ETT_ID;
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

#ifdef ENABLE_ETH
//! [Usage of ETHER_IntIRQHandler]
ISR2(ETHER_IRQHandler)
{
    StatusType status = E_OK;
    ETHER_IRQEventType irqEventMap = 0UL;
    EventMaskType eventMask = 0UL;

    ETHER_IntIRQHandler(0UL, &irqEventMap);

    if ((irqEventMap & ETHER_IRQEVENT_TIMER) != 0UL) {
#if defined (ENABLE_NIF)
#if defined (ENABLE_FQTSS) && !defined(ENABLE_ETHER_TEST) && !defined(ENABLE_WLAN)
        FQTSS_UpdateCredits();
        if (1UL == NIF_AreClientsWaitingForBuffers()) {
            eventMask |= ETHER_TIMER_EVENT;
        }
#endif
#endif
    }
    if ((irqEventMap & ETHER_IRQEVENT_PKT) != 0UL) {
        eventMask |= ETHER_PKT_EVENT;
    }

    if (eventMask != 0UL) {
        status = SetEvent(ETH_IRQ_PROCESS_TASK, eventMask);
        if (E_OK != status) {
            /* TODO: Add ASSERT/Crash */
        }
    }
}
//! [Usage of ETHER_IntIRQHandler]
#endif

#if defined(ENABLE_ETH_TIME)
//! [Usage of ETHER_TimeIRQHandler]
ISR2(ETHER_1588_IRQHandler)
{
    StatusType status = E_OK;
    ETHER_TimestampIRQHandler(0UL);
    status = SetEvent(ETH_IRQ_PROCESS_TASK, ETHER_TIMESTAMP_EVENT);
    if (E_OK != status) {
        /* TODO: Add ASSERT/Crash */
    }
}
//! [Usage of ETHER_TimeIRQHandler]
#endif

/* Nothing past this line */
