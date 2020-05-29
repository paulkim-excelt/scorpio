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

#include <stdlib.h>
#include <string.h>
#include <ee.h>
#include <console.h>
#include <bcm_test.h>
#include <ulog.h>
#include <utils.h>
#include <bcm_time.h>
#include <nif.h>
#include <circ_queue.h>
#include <osil/bcm_osil.h>
#include "ets.h"
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
    .clockMode = ETS_CLOCKMODE_GM,
    .avnuSyncAbsenceTimeout = 0U,
    .gmRateRatio = 0x40000000,
#if (ETS_MAX_INTERFACES > 1)
    .numInterfaces = 8U,
#else
    .numInterfaces = 1U,
#endif
    .intfCfg = {
        {
            .hwPortNum = 0U,
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
#if (ETS_MAX_INTERFACES > 1)
        {
            .hwPortNum = 1U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_1S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 2U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 3U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 4U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 5U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 6U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 8U,
            .asCapable = ETS_ADMINMODE_DISABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        }
#endif
    }
};

static volatile int32_t etscomp_qt_result = BCM_AT_NOT_STARTED;

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
        if ((ret == BCM_ERR_OK) && (cmdResp->ret != BCM_ERR_OK)) {
            ret = cmdResp->ret;
        }
    }

    return ret;
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
  # Activate Test Task
  # Activate ETS Protocol Task
  # Initialize ETS
  # De-initialize ETS
  # Terminate Test Task

@endcode
*/

TASK(TestTask)
{
    int32_t               ret;
    ETS_CmdRespType       cmdResp;

    BCM_ActivateTask(ETS_ProtocolTask);

    /* Configure gPTP for GM mode */
    cmdResp.cmdId            = ETS_CMD_INIT;
    cmdResp.ret              = BCM_ERR_UNKNOWN;
    cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
    BCM_MemCpy(cmdResp.payloadPtr.config, &ets_config, sizeof(ETS_ConfigType));

    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);
    ULOG_INFO("\nTesting gPTP in GM mode\n");

    /* Let enough time elapse for 2 Syncs to be sent out */
    Delay(120UL);

    /* Set the global time */
    cmdResp.cmdId                        = ETS_CMD_SETGLOBALTIME;
    cmdResp.payloadPtr.u8Ptr             = cmdResp.payload;
    cmdResp.payloadPtr.time->seconds     = 100000ULL;
    cmdResp.payloadPtr.time->nanoseconds = 0UL;
    cmdResp.ret                          = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);
    ULOG_INFO("Setting the global time\n");

    /* Fetch the global status */
    cmdResp.cmdId            = ETS_CMD_GETGLOBALSTATUS;
    cmdResp.ret              = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
    /* Verify the global status */
    if ((cmdResp.payloadPtr.globalStatus->gmStatus != ETS_GMSTATUS_PRESENT) ||
        (cmdResp.payloadPtr.globalStatus->clockState != ETS_CLOCKSTATE_UPDATE_GM)) {
        ULOG_ERR("Error: global status unexpected in GM mode %u %u\n",
                cmdResp.payloadPtr.globalStatus->gmStatus, cmdResp.payloadPtr.globalStatus->clockState);
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified global status\n");

    if ((cmdResp.payloadPtr.globalStatus->networkTime.seconds != 100005ULL) ||
        (cmdResp.payloadPtr.globalStatus->networkTimeState != ETS_TIMESTAMPSTATE_SYNC)) {
        ULOG_ERR("Error: current time unexpected %lu:%lu:%lu\n",
                cmdResp.payloadPtr.globalStatus->networkTime.seconds,
                cmdResp.payloadPtr.globalStatus->networkTime.nanoseconds,
                cmdResp.payloadPtr.globalStatus->networkTimeState);
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified network time\n");

    /* Fetch the interface status */
    cmdResp.cmdId                       = ETS_CMD_GETINTFSTATUS;
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    cmdResp.payloadPtr.portStatus->num  = 0UL;
    cmdResp.ret                         = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    /* Verify the interface status */
    //FIXME: Verify stats too
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    if ((cmdResp.payloadPtr.portStatus->status.isMeasuringPdelay != 0UL) ||
        (cmdResp.payloadPtr.portStatus->status.currentLogSyncInterval != ETS_INITLOGSYNC_125MS) ||
        (cmdResp.payloadPtr.portStatus->status.currentLogPdelayInterval != ETS_INITLOGPDELAY_INF)/* ||
        (cmdResp.payloadPtr.portStatus->status.syncTxTimestamp.seconds == 0ULL) ||
        (cmdResp.payloadPtr.portStatus->status.pDelayT1.seconds == 0ULL)*/) {
        ULOG_ERR("Error: interface status unexpected %lu:%ld:%lu:%lu:%lu\n",
                cmdResp.payloadPtr.portStatus->status.isMeasuringPdelay,
                cmdResp.payloadPtr.portStatus->status.currentLogSyncInterval,
                cmdResp.payloadPtr.portStatus->status.currentLogPdelayInterval,
                cmdResp.payloadPtr.portStatus->status.syncTxTimestamp.seconds,
                cmdResp.payloadPtr.portStatus->status.pDelayT1.seconds);
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified interface status in GM mode\n");

    /* Set config */
    cmdResp.cmdId            = ETS_CMD_SETCONFIG;
    cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
    BCM_MemCpy(cmdResp.payloadPtr.config, &ets_config, sizeof(ETS_ConfigType));
    cmdResp.payloadPtr.config->avnuSyncAbsenceTimeout = 1UL;
    cmdResp.payloadPtr.config->clockMode = ETS_CLOCKMODE_SLAVE;
    cmdResp.payloadPtr.config->intfCfg[0U].operLogSyncInterval   = ETS_OPERLOGSYNC_1S;
    cmdResp.payloadPtr.config->intfCfg[0U].initLogPdelayInterval = ETS_INITLOGPDELAY_1S;
    cmdResp.payloadPtr.config->intfCfg[0U].operLogPdelayInterval = ETS_OPERLOGPDELAY_1S;
    cmdResp.payloadPtr.config->intfCfg[0U].role                  = ETS_ROLE_SLAVE;
    cmdResp.payloadPtr.config->intfCfg[0U].nbrPropDelay          = 200UL;
    cmdResp.payloadPtr.config->intfCfg[0U].initLogSyncInterval   = ETS_INITLOGSYNC_31_25MS;
    cmdResp.payloadPtr.config->intfCfg[0U].allowedLostResponses  = 1UL;
    cmdResp.ret                                                  = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);
    ULOG_INFO("\nSwitching to slave mode....\n");

    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    BCM_MemSet(cmdResp.payloadPtr.config, 0U, sizeof(ETS_ConfigType));

    /* Fetch the global config */
    cmdResp.cmdId = ETS_CMD_GETCONFIG;
    cmdResp.ret   = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    /* Verify the global config */
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    if ((cmdResp.payloadPtr.config->clockMode !=
        ETS_CLOCKMODE_SLAVE
        ) || (cmdResp.payloadPtr.config->avnuSyncAbsenceTimeout != 1UL) ||
        (cmdResp.payloadPtr.config->intfCfg[0U].role != ETS_ROLE_SLAVE) ||
        (cmdResp.payloadPtr.config->intfCfg[0U].initLogPdelayInterval != ETS_INITLOGPDELAY_1S) ||
        (cmdResp.payloadPtr.config->intfCfg[0U].operLogPdelayInterval != ETS_OPERLOGPDELAY_1S) ||
        (cmdResp.payloadPtr.config->intfCfg[0U].initLogSyncInterval != ETS_INITLOGSYNC_31_25MS) ||
        (cmdResp.payloadPtr.config->intfCfg[0U].allowedLostResponses != 1UL)) {
        ULOG_ERR("Error: config unexpected\n");
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified config\n");

    /* Wait for sync absence timeout to occur */
    Delay(400);

    /* Fetch the global status */
    cmdResp.cmdId = ETS_CMD_GETGLOBALSTATUS;
    cmdResp.ret   = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    /* Verify the global status */
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    if ((cmdResp.payloadPtr.globalStatus->gmStatus != ETS_GMSTATUS_STARTUP_ABSENT) ||
        (cmdResp.payloadPtr.globalStatus->clockState != ETS_CLOCKSTATE_INIT_SLAVE)) {
        ULOG_ERR("Error: global status unexpected %lu: %lu\n", cmdResp.payloadPtr.globalStatus->gmStatus,
        cmdResp.payloadPtr.globalStatus->clockState);
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified global status in slave mode\n");

    /* Fetch the interface status */
    cmdResp.cmdId                       = ETS_CMD_GETINTFSTATUS;
    cmdResp.payloadPtr.portStatus->num  = 0UL;
    cmdResp.ret                         = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    /* Verify the interface status */
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    if ((cmdResp.payloadPtr.portStatus->status.isMeasuringPdelay != 0UL) ||
        (cmdResp.payloadPtr.portStatus->status.currentLogSyncInterval != ETS_INITLOGSYNC_31_25MS) ||
        (cmdResp.payloadPtr.portStatus->status.currentLogPdelayInterval != ETS_INITLOGPDELAY_1S) ||
        (cmdResp.payloadPtr.portStatus->stats.pDelayReqTxCount == 0UL)) {
        ULOG_ERR("Error: interface status unexpected\n");
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified interface status in slave mode\n");

    /* Reset interface stats */
    cmdResp.cmdId            = ETS_CMD_RESETSTATS;
    *cmdResp.payloadPtr.port = 0UL;
    cmdResp.ret              = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    /* Fetch interface stats */
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    cmdResp.cmdId                       = ETS_CMD_GETINTFSTATUS;
    cmdResp.payloadPtr.portStatus->num  = 0UL;
    cmdResp.ret                         = BCM_ERR_UNKNOWN;
    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    /* Verify stats */
    cmdResp.payloadPtr.u8Ptr            = cmdResp.payload;
    if (cmdResp.payloadPtr.portStatus->stats.pDelayReqTxCount > 1UL) {
        ULOG_ERR("Error: interface stats unexpected after clearing %lu\n", cmdResp.payloadPtr.portStatus->stats.pDelayReqTxCount);
        etscomp_qt_result = BCM_ERR_INVAL_STATE;
        goto end;
    }
    ULOG_INFO("Verified interface stats after clearing\n");

end:
    /* De-initialize gPTP */
    cmdResp.cmdId = ETS_CMD_DEINIT;
    cmdResp.ret = BCM_ERR_UNKNOWN;

    ret = ETS_SendCmdAndCheckResp(&cmdResp);
    CHECK_ERR(ret, __LINE__);

    BCM_SetEvent(ETS_ProtocolTask, ShutdownEvent);
    ULOG_INFO("\nTest completed\n");
    if (BCM_AT_EXECUTING == etscomp_qt_result) {
        etscomp_qt_result = BCM_ERR_OK;
    }

    BCM_TerminateTask();
}
/** @} */

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        etscomp_qt_result = BCM_AT_EXECUTING;
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
