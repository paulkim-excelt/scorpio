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

#include <ee.h>
#include <console.h>
#include <bcm_test.h>
#include <ulog.h>
#include <utils.h>
#include <osil/bcm_osil.h>
#include "ets_osil.h"

#define GetModuleLogLevel()  ULOG_LVL_INFO
#define CHECK_ERR(ret,line)  if (BCM_ERR_OK != ret) {\
                                ULOG_ERR("\n Error: %d \t line %d",ret,line); \
                                etscomp_qt_result = ret; goto end; \
                             }

static ETS_ConfigType ets_config =
{
    .magicId = ETS_CONFIG_MAGIC_ID,
    .adminMode = ETS_ADMINMODE_ENABLE,
    .clockMode = ETS_CLOCKMODE_SLAVE,
    .boundaryModeEnable = ETS_ADMINMODE_ENABLE,
    .avnuSyncAbsenceTimeout = 0U,
    .gmRateRatio = 0x40000000,
    .numInterfaces = 8U,
    .intfCfg = {
        {
            .hwPortNum = 0U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_SLAVE,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
#if (ETS_MAX_INTERFACES > 1)
        {
            .hwPortNum = 1U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 2U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 3U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 4U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 5U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 6U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 8U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_INF,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_INF,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        }
#endif
    }
};


extern uint16_t syncSeqNum;
uint32_t numOfRecordsCaptured;
static volatile int32_t etscomp_qt_result = BCM_AT_NOT_STARTED;
static volatile int32_t result = BCM_ERR_OK;

static int32_t ETS_SendCmdAndCheckResp(ETS_CmdRespType* const cmdResp)
{
    int32_t               ret = BCM_ERR_OK;
    const MSGQ_MsgHdrType *hdr = NULL;

    ret = ETS_SendMsg(cmdResp, SystemEvent0, &hdr);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("\n ETS_SendMsg Error: %d Cmd: %u", ret, cmdResp->cmdId);
    } else {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_ClearEvent(SystemEvent0);

        ret = ETS_RecvResp(hdr, cmdResp);
    }

    return ret;
}

uint32_t ETSPortToIndex(uint32_t port)
{
    uint32_t idx;

    for (idx = 0; idx < ETS_MAX_INTERFACES; idx++) {
        if (ets_config.intfCfg[idx].hwPortNum == port) {
            break;
        }
    }

    return idx;
}

static int32_t MsgToIndex(uint32_t isTX, ETS_MsgType msgType, uint32_t* msgIdx)
{
    int32_t retVal = BCM_ERR_OK;
    switch (msgType) {
        case ETS_MSG_SYNC:
            *msgIdx = 0UL;
            break;
        case ETS_MSG_FOLLOWUP:
            *msgIdx = 1UL;
            break;
        case ETS_MSG_PDELAY_REQ:
            *msgIdx = (isTX == 1UL) ? 4UL: 2UL ;
            break;
        case ETS_MSG_PDELAY_RESP:
            *msgIdx = (isTX == 1UL) ? 5UL: 3UL ;
            break;
        case ETS_MSG_PDELAY_RESP_FOLLOWUP:
            *msgIdx = (isTX == 1UL) ? 7UL: 6UL ;
            break;
        default:
            *msgIdx = 0xFFFFFFFF;
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
    }
    return retVal;
}

int32_t ValidateRecord(ETS_RecordsListType* const aRecord)
{
            int32_t retVal = BCM_ERR_OK;
            uint32_t portIdx;
            uint32_t i;
            uint32_t validRecords;
            uint32_t msgIdx;
            uint32_t idx;
            uint32_t flag;
            uint32_t portAndRecNum;

            validRecords = aRecord->numRecords;
            for (i = 0UL; i < validRecords; ++i) {
                    /* Check the latest record for each port */
                    numOfRecordsCaptured++;
                    portAndRecNum = aRecord->records[i].portAndRecNum;
                    portIdx = (portAndRecNum & ETS_RECORD_PORT_NUM_MASK) >> ETS_RECORD_PORT_NUM_SHIFT;
                    idx = ETSPortToIndex(portIdx);
                    if (idx >= ETS_MAX_INTERFACES) {
                        retVal = BCM_ERR_INVAL_PARAMS;
                        ULOG_ERR("idx is: %lu , which is greater than max allowed value: %lu \n", idx, (ETS_MAX_INTERFACES - 1));
                        goto end;
                    }
                    flag = aRecord->records[i].flag;
                    retVal = MsgToIndex((flag & ETS_RECORD_IS_TX_MASK) >> ETS_RECORD_IS_TX_SHIFT,
                                        (flag & ETS_RECORD_PACKET_TYPE_MASK) >> ETS_RECORD_PACKET_TYPE_SHIFT,
                                        &msgIdx);
                    if (retVal != BCM_ERR_OK) {
                        ULOG_ERR("msgIdx invalid: %lu\n", msgIdx);
                        goto end;
                    }

                    /* Check Rx Sync Packet */
                    if(0UL == msgIdx) {
                         if(syncSeqNum != ((aRecord->records[i].flag & ETS_RECORD_SEQ_NUM_MASK) >> ETS_RECORD_SEQ_NUM_SHIFT)) {
                             retVal = BCM_ERR_DATA_INTEG;
                             ULOG_ERR("***ERROR***\n");
                             ULOG_ERR("Port No::  : %lu\n", idx);
                             ULOG_ERR("SYNC SeqNo Received : %lu\n", (aRecord->records[i].flag & ETS_RECORD_SEQ_NUM_MASK) >> ETS_RECORD_SEQ_NUM_SHIFT);
                             ULOG_ERR("SYNC SeqNo Expected : %lu\n", syncSeqNum);
                             goto end;
                         }
                    }
                    /* Check Rx FollowUp Packet */
                    if(1UL == msgIdx) {
                         if(syncSeqNum != ((aRecord->records[i].flag & ETS_RECORD_SEQ_NUM_MASK) >> ETS_RECORD_SEQ_NUM_SHIFT)) {
                             retVal = BCM_ERR_DATA_INTEG;
                             ULOG_ERR("***ERROR***\n");
                             ULOG_ERR("Port No::  : %lu\n", idx);
                             ULOG_ERR("FollowUp SeqNo Received : %lu\n", (aRecord->records[i].flag & ETS_RECORD_SEQ_NUM_MASK) >> ETS_RECORD_SEQ_NUM_SHIFT);
                             ULOG_ERR("FollowUp SeqNo : %lu\n", syncSeqNum);
                             goto end;

                         }
                    }
            }

end:

    if(retVal != BCM_ERR_OK){
         result = retVal;
    }
    return retVal;
}

void Delay(uint32_t sysTicks)
{
    /* Wait for upto 'sysTicks' ticks */
    (void)BCM_SetRelAlarm(DelayTimerAlarm, sysTicks, 0U);

    (void)BCM_WaitEvent(SystemEvent0);
    (void)BCM_ClearEvent(SystemEvent0);

    /* Disable the alarm so that it can be reused */
    (void)BCM_CancelAlarm(DelayTimerAlarm);
}


void DelayTimerAlarmCb(void)
{
    SetEvent(TestTask, SystemEvent0);
}

/**
  @defgroup grp_ets_qt Qualification Tests
  @ingroup grp_ets

  @addtogroup grp_ets_qt
  @{

  @file code.c
  @brief ETS Library Integration Test
  This source file contains the qualification tests for ETS library
  @version 1.25 Import from MS Word document
*/

/** @brief Brief description of COMP_IT

  This test ....

  @code{.unparsed}
  # Activate ETS Protocol Task
  # Check record before start Validation
  # Initialize ETS notify record
  # Verify ETS notify record
  # Activate Test Task
  # De-initialize ETS
  # Terminate Test Task

@endcode
*/

TASK(TestTask)
{
    int32_t               ret;
    ETS_CmdRespType       cmdResp;

    BCM_ActivateTask(ETS_ProtocolTask);

    /* Initialize gPTP */
    cmdResp.cmdId            = ETS_CMD_INIT;
    cmdResp.ret              = BCM_ERR_UNKNOWN;
    cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
    BCM_MemCpy(cmdResp.payloadPtr.config, &ets_config, sizeof(ETS_ConfigType));
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);
    CONSOLE_Write(0UL, "\nStarted gPTP in Slave mode");

    /* Wait for some time to check false records */
    Delay(40UL);
    if(0UL != numOfRecordsCaptured) {
        ret = BCM_ERR_UNKNOWN;
        CHECK_ERR(ret, __LINE__);
        CONSOLE_Write(0UL, "\nRecords captured before ETS started\n");
    }
    /* Initialize ETS notify record*/
    cmdResp.cmdId = ETS_CMD_NOTIFY_RECORD;
    cmdResp.ret   = BCM_ERR_UNKNOWN;
    cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
    *cmdResp.payloadPtr.isNotifyingRecord = TRUE;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);

    /* Verify the ETS notify record */
    CHECK_ERR(ret, __LINE__);
    CONSOLE_Write(0UL, "\nVerified ETS  Records\n");

    BCM_ActivateTask(TestCommsTask);
    (void)BCM_WaitEvent(SystemEvent0);
    (void)BCM_ClearEvent(SystemEvent0);

    if(0UL == numOfRecordsCaptured) {
        ret = BCM_ERR_UNKNOWN;
        CHECK_ERR(ret, __LINE__);
        CONSOLE_Write(0UL, "\n No Records captured\n");
    }

end:
    ULOG_INFO("\n Test completed");
    if (BCM_AT_EXECUTING == etscomp_qt_result) {
        etscomp_qt_result = result;
    }
    BCM_SetEvent(ETS_ProtocolTask, ShutdownEvent);
    BCM_TerminateTask();
}
/** @} */

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if ((1UL == aIndex) && (BCM_AT_EXECUTING != etscomp_qt_result)) {
        etscomp_qt_result = BCM_AT_EXECUTING;
        numOfRecordsCaptured = 0UL;
        BCM_ActivateTask(TestTask);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = etscomp_qt_result;
    }
    return ret;
}

void ApplicationInit()
{
}
