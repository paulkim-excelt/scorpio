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
    @addtogroup grp_ptm_il
    @{

    @file ptm_osil.c
    @brief PTM Integration
    This source file contains the integration of PTM to system
    @version 1.21 Imported from docx
*/

#include <stdint.h>
#include <osil/ptm_osil.h>
#include <osil/flsmgr_osil.h>
#include <msg_queue.h>
#include <system.h>
#include "ee.h"

static PTM_MsgType PTM_Msg[MSGQ_SIZE];

static void PTM_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_PTM_ID, 0U, aApiID, aErr, 4UL, values);
}

/* Initialize MSGQ header */
MSGQ_DEFINE_HDRQ(PTM_MSGHDRQ);
MSGQ_DEFINE(PTM_MSGQ, FLSMGR_SERVER_TASK_ID, PTM_MSGQ_EVENT_ID, MSGQ_CLIENT_SIGNAL_ENABLE,
                PTM_MsgType, &PTM_MSGHDRQ, PTM_GetMsg);


#if defined(ENABLE_FLSMGR)
/**
    @code{.c}
    return FLSMGR_Read(aFlashID, aAddr, aData, aLen,
                       PTM_FLASH_OP_DONE_EVENT_ID, aMsgHdr)
    @endcode
*/
int32_t PTM_ReadFlash(PTM_FlashIDType aFlashID,
                      uint32_t aAddr,
                      uint32_t aLen,
                      uint8_t *const aData,
                      const MSGQ_MsgHdrType ** const aMsgHdr)
{
    return FLSMGR_Read(aFlashID, aAddr, aData, aLen,
                       PTM_FLASH_OP_DONE_EVENT_ID, aMsgHdr);
}

/**
    @code{.c}
    return FLSMGR_Write(aFlashID, aAddr, aData, aLen,
                       PTM_FLASH_OP_DONE_EVENT_ID, aMsgHdr)
    @endcode
*/
int32_t PTM_WriteFlash(PTM_FlashIDType aFlashID,
                      uint32_t aAddr,
                      uint32_t aLen,
                      uint8_t *const aData,
                      const MSGQ_MsgHdrType ** const aMsgHdr)
{
    return FLSMGR_Write(aFlashID, aAddr, aData, aLen,
                       PTM_FLASH_OP_DONE_EVENT_ID, aMsgHdr);
}

/**
    @code{.c}
    return FLSMGR_Erase(aFlashID, aAddr, aLen,
                       PTM_FLASH_OP_DONE_EVENT_ID, aMsgHdr)
    @endcode
*/
int32_t PTM_EraseFlash(PTM_FlashIDType aFlashID,
                      uint32_t aAddr,
                      uint32_t aLen,
                      const MSGQ_MsgHdrType ** const aMsgHdr)
{
    return FLSMGR_Erase(aFlashID, aAddr, aLen,
                       PTM_FLASH_OP_DONE_EVENT_ID, aMsgHdr);
}

/**
    @code{.c}
    ret = FLSMGR_GetOpResult(aFlashID, &opsRes, aMsgHdr)
    if ret is not BCM_ERR_OK
        opsRes = FLSMGR_OP_RESULT_FAILED

    return ret
    @endcode
*/
FLSMGR_OpResultType PTM_FlashGetOpsResult(PTM_FlashIDType aFlashID,
                                          const MSGQ_MsgHdrType *const aMsgHdr)
{
    int32_t err;
    FLSMGR_OpResultType opsRes;

    err = FLSMGR_GetOpResult(aFlashID, &opsRes, aMsgHdr);
    if (BCM_ERR_OK != err) {
        opsRes = FLSMGR_OP_RESULT_FAILED;
    }

    return opsRes;
}
#endif

/**
    @code{.c}
    *msg = NULL

    if MSGQ_SIZE is greater than aIdx {
        msg = (void *)(&PTM_Msg[aIdx])
    }

    return msg
    @endcode
*/
void *PTM_GetMsg(uint32_t aIdx)
{
    void *msg = NULL;

    if (MSGQ_SIZE > aIdx) {
        msg = (void *)(&PTM_Msg[aIdx]);
    }

    return msg;
}

/**
    @code{.c}
    return MSGQ_SendMsg(&PTM_MSGQ, (void *)aMsg, aClientMask, aHdr)
    @endcode
*/
int32_t PTM_SendMsg(PTM_MsgType * const aMsg,
                    const uint32_t aClientMask,
                    const MSGQ_MsgHdrType** const aHdr)
{
    return MSGQ_SendMsg(&PTM_MSGQ, (void *)aMsg, aClientMask, aHdr);
}

/**
    @code{.c}
    return MSGQ_RecvMsg(&PTM_MSGQ, aHdr, (void *)aMsg)
    @endcode
*/
int32_t PTM_RecvMsg(const MSGQ_MsgHdrType *const aHdr,
                    PTM_MsgType *const aMsg)
{
    return MSGQ_RecvMsg(&PTM_MSGQ, aHdr, (void *)aMsg);
}

//! [Usage of PTM_ProcessMsg]
/**
    @code{.c}
    if PTM_STATE_UNINIT is PTM_GetState() {
        ret = PTM_ProcessMsg(NULL)
    }
    if (ret is BCM_ERR_OK) {
        do {
            ret = MSGQ_GetMsgIdx(&PTM_MSGQ, &idx)
            if ret is BCM_ERR_OK {
                msg = &PTM[idx]
                ret = PTM_ProcessMsg(msg)
            }
            if ret is not BCM_ERR_BUSY {
                ret = MSGQ_GetMsgIdx(&PTM_MSGQ, &idx)
                if ret is BCM_ERR_OK {
                    ret = MSGQ_CompleteMsgIdx(&PTM_MSGQ, idx)
                }
            }
        } while (ret is BCM_ERR_OK)
    }
    @endcode
*/
void PTM_EventHandler(void)
{
    int32_t err = BCM_ERR_OK;
    uint32_t idx = 0UL;
    PTM_MsgType *msg = NULL;

    if (PTM_STATE_UNINIT == PTM_GetState()) {
        err = PTM_ProcessMsg(NULL);
    }
    if (BCM_ERR_OK == err) {
        /* To process all the messages in message queue */
        do {
            err = MSGQ_GetMsgIdx(&PTM_MSGQ, &idx);
            if (BCM_ERR_OK == err) {
                msg = &PTM_Msg[idx];
                err = PTM_ProcessMsg(msg);
            }
            if (BCM_ERR_BUSY != err) {
                err = MSGQ_GetMsgIdx(&PTM_MSGQ, &idx);
                if (BCM_ERR_OK == err) {
                    err = MSGQ_CompleteMsgIdx(&PTM_MSGQ, idx);
                    if (BCM_ERR_OK != err) {
                        PTM_ReportError(0U, err, 0UL, 0UL, 0UL, 0UL);
                    }
                }
            }
        } while (BCM_ERR_OK == err);
    }
}
//! [Usage of PTM_ProcessMsg]
/** @} */
