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

/** @brief MSGQ_EN_NOTIF_IT1
 *
 * This file implements MSGQ_EN_NOTIF_IT1, which tests the message queue with
 * client notifcation enabled.
 */

#include <ulog.h>
#include <msg_queue.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <atomic.h>
#include <osil/bcm_osil.h>
#include "ee.h"

#include "msg_queue_qt.h"

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#define CLIENT1_PKT_STATUS_BIT  (0UL)
#define CLIENT2_PKT_STATUS_BIT  (1UL)
#define PKT_STATUS_OK           (3UL)

#define MARK_ERR(aRetVal, aLineNum, aStatus)                 \
{                                                            \
    ULOG_ERR("\nError: %d \t line %d", aRetVal ,aLineNum);   \
    if (BCM_AT_EXECUTING == (aStatus)) {                     \
        (aStatus) = (aRetVal);                               \
    }                                                        \
}

static volatile int32_t MsgQQT1_ServerStatus = BCM_AT_NOT_STARTED;
static volatile int32_t MsgQQT1_Client1Status = BCM_AT_NOT_STARTED;
static volatile int32_t MsgQQT1_Client2Status = BCM_AT_NOT_STARTED;
static volatile uint32_t PktCnt_Status = 0UL;

/* define message struct */
typedef struct {
    uint32_t cmd;
    uint32_t resp;
} Msg;

static Msg MsgQQT1_msg[MSGQ_SIZE];


/* Define GetMessage function */
static void* MsgQQT1_GetMessage(uint32_t aIdx)
{
    void* pRet;

    if (aIdx < MSGQ_SIZE) {
        pRet = (void *)(&MsgQQT1_msg[aIdx]);
    } else {
        pRet = (void *) NULL;
    }
    return pRet;
}

/* Allocate the Message queue */
MSGQ_DEFINE_V2(MsgQQT1_msgQ, MsgQQT1_ServerTask, MsgQQT1_TriggerServer,
            MSGQ_CLIENT_SIGNAL_ENABLE, Msg, MsgQQT1_msgHdrQ, MsgQQT1_GetMessage, ".data.MsgQQT1_msgHdrQ");

/* Global variables */
static uint32_t server_resp = 500000;
static uint32_t client1_msg = 0;
static uint32_t client2_msg = 10000;

TASK(MsgQQT1_ServerTask)
{
    int32_t ret;
    BCM_EventMaskType mask;
    uint32_t idx;
    uint32_t whileCondition = 1UL;

    do {
        /* Wait for message reception signal from queue */
        (void)BCM_WaitEvent(MsgQQT1_TriggerServer);
        (void)BCM_GetEvent(MsgQQT1_ServerTask, &mask);
        (void)BCM_ClearEvent(mask);
        if (0UL != (mask & MsgQQT1_TriggerServer)) {
            /* Get message from queue */
            ret = MSGQ_GetMsgIdx(&MsgQQT1_msgQ, &idx);
            if (ret == BCM_ERR_OK) {
                /* Process the message - Change the data */
                MsgQQT1_msg[idx].resp = server_resp;
                server_resp++;

                /* Mark the message as completed */
                ret = MSGQ_CompleteMsgIdx(&MsgQQT1_msgQ, idx);
                if (BCM_ERR_OK != ret) {
                    MARK_ERR(ret, __LINE__, MsgQQT1_ServerStatus);
                    whileCondition = 0UL;
                    /*break;
                    * Since while condition is now false, so break statement is not required*/
                }
            } else if (BCM_ERR_BUSY != ret) {
                MARK_ERR(ret, __LINE__, MsgQQT1_ServerStatus);
                whileCondition = 0UL;
                /*break;
                * Since while condition is now false, so break statement is not required*/
            } else {
                continue;
            }
        }
    } while(1UL == whileCondition);
    (void)BCM_TerminateTask();
}

TASK(MsgQQT1_TaskClient1)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;
    Msg msgCmd;
    Msg msgResp;
    const MSGQ_MsgHdrType* pHdr;

    do {
        /* Set cmd = 1, resp = 0 */
        msgCmd.cmd = client1_msg;
        msgCmd.resp = 0;
        client1_msg++;

        /* Send message to server/queue */
        ret = MSGQ_SendMsg(&MsgQQT1_msgQ, &msgCmd, MsgQQT1_TriggerClient1, &pHdr);
        if (ret == BCM_ERR_OK) {
            /* Wait for message completion signal from server/queue */
            (void)BCM_WaitEvent(MsgQQT1_TriggerClient1);
            (void)BCM_GetEvent(MsgQQT1_TaskClient1, &mask);
            (void)BCM_ClearEvent(MsgQQT1_TriggerClient1);

            /* Receive response from server/queue */
            ret = MSGQ_RecvMsg(&MsgQQT1_msgQ, pHdr, &msgResp);
        }
        if (ret != BCM_ERR_OK) {
            MARK_ERR(ret, __LINE__, MsgQQT1_Client1Status);
            break;
        }
    } while(client1_msg < 10UL);
    if (client1_msg == 10UL) {
        ATOMIC_SetBit(&PktCnt_Status, CLIENT1_PKT_STATUS_BIT);
    } else {
        ULOG_ERR("Client1 failed at (%u)\n", client1_msg);
    }
    (void)BCM_TerminateTask();
}

TASK(MsgQQT1_TaskClient2)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;
    Msg msgCmd;
    Msg msgResp;
    const MSGQ_MsgHdrType* pHdr;

    do {
        /* Set cmd = 1, resp = 0 */
        msgCmd.cmd = client2_msg;
        msgCmd.resp = 0;
        client2_msg++;

        /* Send message to server/queue */
        ret = MSGQ_SendMsg(&MsgQQT1_msgQ, &msgCmd, MsgQQT1_TriggerClient2, &pHdr);
        if (ret == BCM_ERR_OK) {
            /* Wait for message completion signal from server/queue */
            (void)BCM_WaitEvent(MsgQQT1_TriggerClient2);
            (void)BCM_GetEvent(MsgQQT1_TaskClient2, &mask);
            (void)BCM_ClearEvent(MsgQQT1_TriggerClient2);

            /* Receive response from server/queue */
            ret = MSGQ_RecvMsg(&MsgQQT1_msgQ, pHdr, &msgResp);
        }
        if (ret != BCM_ERR_OK) {
            MARK_ERR(ret, __LINE__, MsgQQT1_Client2Status);
            break;
        }
    } while(client2_msg < 10010UL);
    if (client2_msg == 10010UL) {
        ATOMIC_SetBit(&PktCnt_Status, CLIENT2_PKT_STATUS_BIT);
    } else {
        ULOG_ERR("Client2 failed at (%u)\n", client2_msg);
        MARK_ERR(BCM_ERR_DATA_INTEG, __LINE__, MsgQQT1_Client2Status);
    }
    (void)BCM_TerminateTask();
}

extern void MsgQQT1_Start(void)
{
    server_resp = 500000;
    client1_msg = 0UL;
    client2_msg = 10000UL;
    MsgQQT1_ServerStatus = BCM_AT_EXECUTING;
    (void)BCM_ActivateTask(MsgQQT1_ServerTask);
    /* Activate the client task */
    MsgQQT1_Client1Status = BCM_AT_EXECUTING;
    (void)BCM_ActivateTask(MsgQQT1_TaskClient1);
    MsgQQT1_Client2Status = BCM_AT_EXECUTING;
    (void)BCM_ActivateTask(MsgQQT1_TaskClient2);
}

int32_t MsgQQT1_GetResult(void)
{
    int32_t ret;

    if (BCM_AT_EXECUTING != MsgQQT1_ServerStatus) {
        ret = MsgQQT1_ServerStatus;
    } else if (BCM_AT_EXECUTING != MsgQQT1_Client1Status) {
        ret = MsgQQT1_Client1Status;
    } else if (BCM_AT_EXECUTING != MsgQQT1_Client2Status) {
        ret = MsgQQT1_Client2Status;
    } else {
        ret = BCM_AT_EXECUTING;
    }

    if (BCM_AT_EXECUTING == ret) {
        if (PKT_STATUS_OK == PktCnt_Status) {
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_UNKNOWN;
        }
    }
    return ret;
}
