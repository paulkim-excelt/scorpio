/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_flsmgr_il
    @{

    @file flsmgr_osil.c
    @brief Flash Manager integration
    This source file contains the integration of flash manager to system.
    @version 0.52 Imported from docx
*/

#include <stddef.h>
#include <system.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <flash_osil.h>
#include <flsmgr.h>
#include <flsmgr_osil.h>
#include <ptm_osil.h>
#include <board.h>
#include <osil/bcm_osil.h>
#include "ee.h"

#define FLASH_EVENTS        (FLASH0_EVENT | FLASH1_EVENT)
#define FLSMGR_0_EVENTS     (FLSMGR_0_MSGQ_EVENT | FLSMGR_0_CB_IND_EVENT)
#define FLSMGR_1_EVENTS     (FLSMGR_1_MSGQ_EVENT | FLSMGR_1_CB_IND_EVENT)
#define FLSMGR_EVENTS       (FLSMGR_0_EVENTS | FLSMGR_1_EVENTS)
#define PTM_EVENTS          (PTM_MSGQ_EVENT_ID | PTM_FLASH_OP_DONE_EVENT_ID)

/* Function prototypes */
static void *FLSMGR_GetMsg0(uint32_t aIdx);
static void *FLSMGR_GetMsg1(uint32_t aIdx);

/* Globals */
MSGQ_DEFINE_HDRQ(FLSMGR_0_MsgHdrQ);
#if (FLASH_MAX_HW_ID > 1)
MSGQ_DEFINE_HDRQ(FLSMGR_1_MsgHdrQ);
#endif

static MSGQ_Type FLSMGR_MsgQ[FLASH_MAX_HW_ID] =
{
    #if (FLASH_MAX_HW_ID == 0)
    #error "FLASH_MAX_HW_ID == 0"
    #endif
    #if (FLASH_MAX_HW_ID > 0)
    MSGQ_INIT(FLSMGR_SERVER_TASK_ID, FLSMGR_0_MSGQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE,
            FLSMGR_MsgType, &FLSMGR_0_MsgHdrQ, FLSMGR_GetMsg0),
    #endif
    #if (FLASH_MAX_HW_ID > 1)
    MSGQ_INIT(FLSMGR_SERVER_TASK_ID, FLSMGR_1_MSGQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE,
            FLSMGR_MsgType, &FLSMGR_1_MsgHdrQ, FLSMGR_GetMsg1),
    #endif
    #if (FLASH_MAX_HW_ID > 2)
    #error "FLASH_MAX_HW_ID > 2 is not supported by driver"
    #endif
};

static FLSMGR_MsgType FLSMGR_Msg[FLASH_MAX_HW_ID][MSGQ_SIZE];

static void FLSMGR_ReportError(uint8_t aInstanceID, uint8_t aApiID,
        int32_t aErr, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
        uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_FLM_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

static void *FLSMGR_GetMsg0(uint32_t aIdx)
{
    void *pMsg = NULL;

    if (MSGQ_SIZE > aIdx) {
        pMsg = (void *)(&FLSMGR_Msg[0UL][aIdx]);
    }

    return pMsg;
}

/* CERT-C 699926 */
#if (FLASH_MAX_HW_ID > 1)
static void *FLSMGR_GetMsg1(uint32_t aIdx)
{
    void *pMsg = NULL;

    if (MSGQ_SIZE > aIdx) {
        pMsg = (void *)(&FLSMGR_Msg[1UL][aIdx]);
    }

    return pMsg;
}
#endif

#if !defined(ENABLE_FLASH_TEST)
/**
    Client (CLIENT_TASK) shall wait for two events: FLASH_OPS_DONE_EVENT and
    FLASH_OPS_ERR_EVENT after initiating a flash asynchronous operation.After
    successful completion of flash operation client task shall be woken up by
    setting FLASH_OPS_DONE_EVENT from the #FLASH_OpsDoneInd callback function.

    Below is typical implementation of #FLASH_OpsDoneInd.
    @code{.c}
    void FLASH_OpsDoneInd(FLASH_HwIDType aID)
    {
        Signal(CLIENT_TASK, FLASH_OPS_DONE_EVENT);
    }
    @endcode
*/
void FLASH_OpsDoneInd(FLASH_HwIDType aID)
{
    int32_t status;

    if (FLASH_HW_ID_0 == aID) {
        status = BCM_SetEvent(FLSMGR_SERVER_TASK_ID, FLSMGR_0_CB_IND_EVENT);
        if (status != BCM_ERR_OK) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLASH_OPS_DONE_IND_PROC,
                    BCM_ERR_CUSTOM, 0UL, 0UL, 0UL, 0UL);
        }
    } else if (FLASH_HW_ID_1 == aID) {
        status = BCM_SetEvent(FLSMGR_SERVER_TASK_ID, FLSMGR_1_CB_IND_EVENT);
        if (status != BCM_ERR_OK) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLASH_OPS_DONE_IND_PROC,
                    BCM_ERR_CUSTOM, 0UL, 0UL, 0UL, 0UL);
        }
    } else {
        FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLASH_OPS_DONE_IND_PROC,
                BCM_ERR_INVAL_PARAMS, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    Client task shall wait for two events, say FLASH_OPS_DONE_EVENT and
    FLASH_OPS_ERR_EVENT after initiating a flash asynchronous operation. On
    error during flash operation, client task shall be woken up by setting
    FLASH_OPS_ERR_EVENT from the #FLASH_OpsErrInd callback function.

    Below is typical implementation of #FLASH_OpsErrInd:
    @code {.c}
    void FLASH_OpsErrInd(FLASH_HwIDType aID)
    {
        Signal(CLIENT_TASK, FLASH_OPS_ERR_EVENT);
    }
    @endcode
*/
void FLASH_OpsErrInd(FLASH_HwIDType aID)
{
    int32_t status;

    if (FLASH_HW_ID_0 == aID) {
        status = BCM_SetEvent(FLSMGR_SERVER_TASK_ID, FLSMGR_0_CB_IND_EVENT);
        if (status != BCM_ERR_OK) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLASH_OPS_ERR_IND_PROC,
                    BCM_ERR_CUSTOM, 0UL, 0UL, 0UL, 0UL);
        }
    } else if (FLASH_HW_ID_1 == aID){
        status = BCM_SetEvent(FLSMGR_SERVER_TASK_ID, FLSMGR_1_CB_IND_EVENT);
        if (status != BCM_ERR_OK) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLASH_OPS_ERR_IND_PROC,
                    BCM_ERR_CUSTOM, 0UL, 0UL, 0UL, 0UL);
        }
    } else {
        FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLASH_OPS_ERR_IND_PROC,
                BCM_ERR_INVAL_PARAMS, 0UL, 0UL, 0UL, 0UL);
    }
}
#endif

/**
    @code{.c}
    ret = BCM_ERR_INVAL_PARAMS
    if (aID < FLASH_MAX_HW_ID) and (aMsg is not NULL) and (aMsgHdr is not NULL)
        ret = MSGQ_SendMsg(&FLSMGR_MsgQ[aID], aMsg, aClientMask, aMsgHdr)

    return ret
    @endcode
*/
int32_t FLSMGR_SendMsg(FLSMGR_DevIDType aID,
                     FLSMGR_MsgType * const aMsg,
                     const uint32_t aClientMask,
                     const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((FLASH_MAX_HW_ID > aID) && (aMsg != NULL) && (NULL != aMsgHdr)) {
        retVal = MSGQ_SendMsg(&FLSMGR_MsgQ[aID], aMsg, aClientMask, aMsgHdr);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_IL_SEND_MSG_PROC, retVal,
                    0UL, 0UL, 0UL, 0UL);
        }
    }

    return retVal;
}

/**
    @code{.c}
    ret = BCM_ERR_INVAL_PARAMS
    if (aID < FLASH_MAX_HW_ID) and (aMsgHdr is not NULL) and (aMsg is not NULL)
        ret = MSGQ_TryCancelMsg(&FLSMGR_MsgQ[aID], aMsgHdr, aMsg)

    return ret
    @endcode
*/
int32_t FLSMGR_TryCancelMsg(FLSMGR_DevIDType aID,
                      const FLSMGR_MsgHdrType* const aMsgHdr,
                      FLSMGR_MsgType * const aMsg)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((FLASH_MAX_HW_ID > aID) && (aMsgHdr != NULL) && (NULL != aMsg)) {
        retVal = MSGQ_TryCancelMsg(&FLSMGR_MsgQ[aID], aMsgHdr, aMsg);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_IL_TRY_CANCEL_MSG_PROC,
                    retVal, 0UL, 0UL, 0UL, 0UL);
        }
    }

    return retVal;
}

/**
    @code{.c}
    ret = BCM_ERR_INVAL_PARAMS
    if (aID < FLASH_MAX_HW_ID) and (aMsgHdr is not NULL) and (aMsg is not NULL)
        ret = MSGQ_RecvMsg(&FLSMGR_MsgQ[aID], aMsgHdr, aMsg)

    return ret
    @endcode
*/
int32_t FLSMGR_RecvMsg(FLSMGR_DevIDType aID,
                    const FLSMGR_MsgHdrType * const aMsgHdr,
                    FLSMGR_MsgType * const aMsg)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((FLASH_MAX_HW_ID > aID) && (aMsgHdr != NULL) && (NULL != aMsg)) {
        retVal = MSGQ_RecvMsg(&FLSMGR_MsgQ[aID], aMsgHdr, aMsg);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_IL_RECV_MSG_PROC, retVal,
                    0UL, 0UL, 0UL, 0UL);
        }
    }

    return retVal;
}

/**
    @code{.c}
    do {
        ret = MSGQ_GetMsgIdx(&FLSMGR_MsgQ[aID], &idx)
        if ret is BCM_ERR_BUSY
            break
        if ret is not BCM_ERR_OK
            break

        ret = FLSMGR_MsgHandler(aID, &FLSMGR_Msg[aID][idx])
        if ret is BCM_ERR_OK {
            ret = MSGQ_CompleteMsgIdx(&FLSMGR_MsgQ[aID], idx)
        }
    } while (ret is BCM_ERR_OK)
    @endcode
*/
void FLSMGR_EventHandler(FLSMGR_DevIDType aID)
{
    int32_t retVal;
    uint32_t idx;

    do {
        /* Get message queue index */
        retVal = MSGQ_GetMsgIdx(&FLSMGR_MsgQ[aID], &idx);
        if (BCM_ERR_BUSY == retVal) {
            /* No new message */
            break;
        }
        if (BCM_ERR_OK != retVal) {
            /* Something wrong happened */
            FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_IL_EVENT_HANDLER_PROC,
                    retVal, 0UL, 0UL, 0UL, 0UL);
            break;
        }

        /* Process message */
        retVal = FLSMGR_MsgHandler(aID, &FLSMGR_Msg[aID][idx]);

        /* Complete the message */
        if (BCM_ERR_OK == retVal) {
            retVal = MSGQ_CompleteMsgIdx(&FLSMGR_MsgQ[aID], idx);
            if (BCM_ERR_OK != retVal) {
                FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_IL_EVENT_HANDLER_PROC,
                        retVal, 0UL, 0UL, 0UL, 0UL);
                break;
            }
        }
    } while (BCM_ERR_OK == retVal);

    return ;
}

//! [Usage of FLASH_StateHandler]
static void FLSMGR_ServerEventHandler(EventMaskType aEventMask)
{
#if defined(ENABLE_FLASH)
    if ((aEventMask & FLASH0_EVENT) != 0UL) {
        FLASH_StateHandler(0UL);
    }
    if ((aEventMask & FLASH1_EVENT) != 0UL) {
        FLASH_StateHandler(1UL);
    }
#endif
#if defined(ENABLE_FLSMGR)
    if ((aEventMask & FLSMGR_0_EVENTS) != 0UL) {
        FLSMGR_EventHandler(0UL);
    }
    if ((aEventMask & FLSMGR_1_EVENTS) != 0UL) {
        FLSMGR_EventHandler(1UL);
    }
#endif
#if defined(ENABLE_PTM)
    if ((aEventMask & PTM_EVENTS) != 0UL) {
        PTM_EventHandler();
    }
#endif
}
//! [Usage of FLASH_StateHandler]

//! [Usage of PTM_Init]

/**
    @brief Initialize PTM Server
*/
static int32_t InitPTM(void)
{
    int32_t retVal;
    PTM_StateType ptmState;
    BCM_EventMaskType waitEvents = FLASH_EVENTS | FLSMGR_EVENTS | PTM_FLASH_OP_DONE_EVENT_ID;
    BCM_EventMaskType eventMask = 0U;

    retVal = PTM_Init(PTM_Cfg);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }
    /* Wait until PTM is initialized */
    do {
        ptmState = PTM_GetState();
        if (PTM_STATE_IDLE == ptmState) {
            retVal = BCM_ERR_OK;
            break;
        }
        if (PTM_STATE_ERROR == ptmState) {
            retVal = BCM_ERR_NOT_FOUND;
            break;
        }
        retVal = BCM_WaitEvent(waitEvents);
        if (BCM_ERR_OK != retVal) {
            break;
        }
        retVal = BCM_GetEvent(FLSMGR_Server, &eventMask);
        if (BCM_ERR_OK != retVal) {
            break;
        }
        retVal = BCM_ClearEvent((EventMaskType )eventMask & (EventMaskType)waitEvents);
        if (BCM_ERR_OK != retVal) {
            break;
        }
        FLSMGR_ServerEventHandler(eventMask & waitEvents);
    } while (1UL);

err:
    return retVal;
}
//! [Usage of PTM_Init]

/**
    @brief Initialize Flash Manger
*/
static int32_t InitFlsMgr(void)
{
    int32_t err = BCM_ERR_UNKNOWN;
    uint32_t i;
    int32_t status = BCM_ERR_OK;
    BCM_EventMaskType waitEvents = FLASH_EVENTS | FLSMGR_EVENTS | FLSMGR_EVENT_ID;
    BCM_EventMaskType eventMask = 0U;
    FLSMGR_DevIDType flashID;
    const MSGQ_MsgHdrType const *pHdr = NULL;
    FLSMGR_OpResultType opsRes;

    /* Flash manager Initialization */
    for (i = 0UL; i < flsmgr_cfg_table_sz; i++) {
        flashID = flsmgr_cfg_table[i].flashCfg->hwID;
        err = FLSMGR_Init(flashID, &flsmgr_cfg_table[i], FLSMGR_EVENT_ID, &pHdr);
        if (BCM_ERR_OK != err) {
            break;
        }
        do {
            status = BCM_WaitEvent(waitEvents);
            if (BCM_ERR_OK != status) {
                break;
            }
            status = BCM_GetEvent(FLSMGR_Server, &eventMask);
            if (BCM_ERR_OK != status) {
                break;
            }
            status = BCM_ClearEvent(eventMask & waitEvents);
            if (BCM_ERR_OK != status) {
                break;
            }
            FLSMGR_ServerEventHandler((EventMaskType)eventMask & (EventMaskType)waitEvents);
        } while ((eventMask & FLSMGR_EVENT_ID) != FLSMGR_EVENT_ID);
        if (BCM_ERR_OK != status) {
            break;
        }
        err = FLSMGR_GetOpResult(flashID, &opsRes, pHdr);
        if ((BCM_ERR_OK != err) || (FLSMGR_OP_RESULT_SUCCESS != opsRes)) {
            FLSMGR_ReportError((uint8_t)flashID, BRCM_SWARCH_FLSMGR_IL_SERVER_TASK_MACRO, err,
                    opsRes, 0UL, 0UL, __LINE__);
            break;
        }
    }
    if (BCM_ERR_OK != err) {
        FLSMGR_ReportError(0U, BRCM_SWARCH_FLSMGR_IL_SERVER_TASK_MACRO, err, (uint32_t)status,
                0UL, 0UL, __LINE__);
    }

    return err;
}

TASK(FLSMGR_Server)
{
    int32_t status;
    BCM_EventMaskType waitEvents;
    BCM_EventMaskType eventMask = 0U;
    int32_t err = BCM_ERR_OK;

#if !defined(ENABLE_FLSMGR_TEST) && !defined(ENABLE_FLASH_TEST)
    uint32_t lineNo = __LINE__;
    /* Flash manager initialization */
    err = InitFlsMgr();
#if defined(ENABLE_PTM)
    if (BCM_ERR_OK == err) {
        /* PTM Initialization */
        err = InitPTM();
        lineNo = __LINE__;
    }
#endif
    if (BCM_ERR_OK != err) {
        SYS_NotifyState(SYS_SUBSYSTEM_NVM, SYS_SUBSYSTEM_STATE_ERROR);
        FLSMGR_ReportError(0U, BRCM_SWARCH_FLSMGR_IL_SERVER_TASK_MACRO, err, 0UL, 0UL, 0UL,
                lineNo);
    } else {
        SYS_NotifyState(SYS_SUBSYSTEM_NVM, SYS_SUBSYSTEM_STATE_INIT);
    }
#else
    SYS_NotifyState(SYS_SUBSYSTEM_NVM, SYS_SUBSYSTEM_STATE_ERROR);
#endif

    if (BCM_ERR_OK == err) {
#if !defined(ENABLE_FLSMGR_TEST) && !defined(ENABLE_FLASH_TEST)
        waitEvents = FLASH_EVENTS | FLSMGR_EVENTS | PTM_EVENTS;
#else
        waitEvents = FLASH_EVENTS | FLSMGR_EVENTS;
#endif
        do {
            status = BCM_WaitEvent(waitEvents | ShutdownEvent);
            if (BCM_ERR_OK != status) {
                break;
            }
            status = BCM_GetEvent(FLSMGR_Server, &eventMask);
            if (BCM_ERR_OK != status) {
                break;
            }
            status = BCM_ClearEvent(eventMask & waitEvents);
            if (BCM_ERR_OK != status) {
                break;
            }
            FLSMGR_ServerEventHandler((EventMaskType)eventMask & (EventMaskType)waitEvents);

            if (eventMask & ShutdownEvent) {
                (void)BCM_ClearEvent(eventMask & ShutdownEvent);
                break;
            }
        } while (1UL);
    }

    SYS_NotifyState(SYS_SUBSYSTEM_NVM, SYS_SUBSYSTEM_STATE_UNINIT);
    BCM_TerminateTask();
}

/**
    @code{.c}
    BCM_ActivateTask(FLSMGR_Server)
    @endcode
*/
void FLSMGR_ServerInit(void)
{
    int32_t status;

    status = BCM_ActivateTask(FLSMGR_Server);
    if (BCM_ERR_OK != status) {
        FLSMGR_ReportError(0U, BRCM_SWARCH_FLSMGR_IL_SERVER_TASK_MACRO, status, 0UL,
                0UL, 0UL, __LINE__);
    }
}
/** @} */
