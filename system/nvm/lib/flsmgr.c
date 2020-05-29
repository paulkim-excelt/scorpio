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
#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>
#include <system.h>
#include <flsmgr.h>
#include <flash.h>
#include <chip_config.h>
#include <osil/flsmgr_osil.h>
#include <utils.h>

/* FLSMGR internal function IDs */
/** FLSMGR_UninitStateHandler ID */
#define FLSMGR_UNINIT_STATE_HANDLER_API_ID  (0x80U)
/** FLSMGR_IdleStateHandler ID */
#define FLSMGR_IDLE_STATE_HANDLER_API_ID    (0x81U)
/** FLSMGR_BusyStateHandler ID */
#define FLSMGR_BUSY_STATE_HANDLER_API_ID    (0x82U)

/* Macros for flash manager commands */
#define FLSMGR_CMD_DEINIT                   (1UL)   /**< Deinitialise command */
#define FLSMGR_CMD_READ                     (2UL)   /**< Read command */
#define FLSMGR_CMD_CFG_READ_LANE            (3UL)   /**< Configure read lane command */
#define FLSMGR_CMD_WRITE                    (4UL)   /**< Write command */
#define FLSMGR_CMD_ERASE                    (5UL)   /**< Erase command */
#define FLSMGR_CMD_COMPARE                  (6UL)   /**< Compare command */
#define FLSMGR_CMD_BLANK_CHECK              (7UL)   /**< Blank check command */

/* Flash manager component type */
typedef struct {
    FLSMGR_StateType        state;
    FLSMGR_MsgType *        currMsg;
    uint32_t isCfgLaneDone;
    uint32_t isCfgLaneStarted;
} FLSMGR_CompType;

static FLSMGR_CompType COMP_SECTION(".data.flsmgr")
FLSMGR_Comp[FLASH_MAX_HW_ID] = {
    #if (FLASH_MAX_HW_ID == 0)
    #error "FLASH_MAX_HW_ID == 0"
    #endif
    #if (FLASH_MAX_HW_ID > 0)
    {.state = FLSMGR_STATE_UNINIT, .currMsg = NULL, .isCfgLaneDone = FALSE,
    .isCfgLaneStarted = FALSE},
    #endif
    #if (FLASH_MAX_HW_ID > 1)
    {.state = FLSMGR_STATE_UNINIT, .currMsg = NULL, .isCfgLaneDone = FALSE,
    .isCfgLaneStarted = FALSE},
    #endif
    #if (FLASH_MAX_HW_ID > 2)
    #error "FLASH_MAX_HW_ID > 2 is not supported by driver"
    #endif
};

static void FLSMGR_ReportError(uint8_t aInstanceID, uint8_t aApiID,
        int32_t aErr, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
        uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_FLM_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

COMP_INLINE int32_t FLSMGR_UninitStateHandler(FLSMGR_DevIDType aID,
                                         FLSMGR_MsgType * const aMsg)
{
    int32_t retVal = BCM_ERR_OK;
    FLASH_OpsResultType flashOpResult;

    if (FLSMGR_CMD_CFG_READ_LANE == aMsg->cmd) {
        if (FLSMGR_Comp[aID].isCfgLaneDone != TRUE) {
            if (FLSMGR_Comp[aID].isCfgLaneStarted != TRUE) {
                FLSMGR_Comp[aID].currMsg = aMsg;
                retVal = FLASH_CfgReadLane(aID);
                if (BCM_ERR_OK == retVal) {
                    FLSMGR_Comp[aID].isCfgLaneStarted = TRUE;
                    retVal = BCM_ERR_EAGAIN;
                } else if (BCM_ERR_NOSUPPORT == retVal) {
                    aMsg->opResult = FLSMGR_OP_RESULT_SUCCESS;
                    FLSMGR_Comp[aID].state = FLSMGR_STATE_IDLE;
                    FLSMGR_Comp[aID].currMsg = NULL;
                    retVal = BCM_ERR_OK;
                } else {
                    aMsg->opResult = FLSMGR_OP_RESULT_FAILED;
                    FLSMGR_Comp[aID].currMsg = NULL;
                    retVal = BCM_ERR_OK;
                }
            } else {
                /* Check the result of flash operation */
                retVal = FLASH_GetOpsResult(aID, &flashOpResult);
                if (BCM_ERR_OK != retVal) {
                    goto err;
                }
                if (FLASH_OPS_RESULT_OK == flashOpResult) {
                    aMsg->opResult = FLSMGR_OP_RESULT_SUCCESS;
                } else if (FLASH_OPS_RESULT_FAIL == flashOpResult) {
                    aMsg->opResult = FLSMGR_OP_RESULT_FAILED;
                } else if (FLASH_OPS_RESULT_PENDING == flashOpResult) {
                    retVal = BCM_ERR_EAGAIN;
                    goto err;
                } else {
                    retVal = BCM_ERR_UNKNOWN;
                    goto err;
                }
                FLSMGR_Comp[aID].state = FLSMGR_STATE_IDLE;
                FLSMGR_Comp[aID].currMsg = NULL;
                FLSMGR_Comp[aID].isCfgLaneDone = TRUE;
            }
        }
    } else {
        /* It is possible that there are message queued
           in message queue in between queuing of deinit
           message and it processing. So, for these message
           result shall be marked failed. */
        aMsg->opResult = FLSMGR_OP_RESULT_FAILED;
    }

err:
    return retVal;
}

static int32_t FLSMGR_IdleStateHandler(FLSMGR_DevIDType aID,
                                       FLSMGR_MsgType * const aMsg)
{
    int32_t retVal;
    FLASH_StatusType flashStatus;

    /* Issue command to flash driver */
    FLSMGR_Comp[aID].state = FLSMGR_STATE_BUSY;
    FLSMGR_Comp[aID].currMsg = aMsg;
    switch(aMsg->cmd) {
    case FLSMGR_CMD_READ:
        retVal = FLASH_Read(aID, aMsg->addr, aMsg->bufOut, aMsg->len);
        break;
    case FLSMGR_CMD_WRITE:
        retVal = FLASH_Write(aID, aMsg->addr, aMsg->bufIn, aMsg->len);
        break;
    case FLSMGR_CMD_ERASE:
        retVal = FLASH_Erase(aID, aMsg->addr, aMsg->len);
        break;
    case FLSMGR_CMD_COMPARE:
        retVal = FLASH_Compare(aID, aMsg->addr, aMsg->bufIn, aMsg->len);
        break;
    case FLSMGR_CMD_BLANK_CHECK:
        retVal = FLASH_BlankCheck(aID, aMsg->addr, aMsg->len);
        break;
    case FLSMGR_CMD_DEINIT:
        FLSMGR_Comp[aID].state = FLSMGR_STATE_IDLE;
        FLSMGR_Comp[aID].currMsg = NULL;
        FLASH_DeInit(aID);
        retVal = FLASH_GetStatus(aID, &flashStatus);
        if (BCM_ERR_OK == retVal) {
            if (FLASH_STATUS_UNINIT == flashStatus) {
                aMsg->opResult = FLSMGR_OP_RESULT_SUCCESS;
                FLSMGR_Comp[aID].state = FLSMGR_STATE_UNINIT;
                FLSMGR_Comp[aID].isCfgLaneDone = FALSE;
                FLSMGR_Comp[aID].isCfgLaneStarted = FALSE;
                retVal = BCM_ERR_OK;
            } else {
                retVal = BCM_ERR_INVAL_STATE;
                FLSMGR_ReportError((uint8_t)aID,
                        FLSMGR_IDLE_STATE_HANDLER_API_ID, retVal,
                        aMsg->cmd, flashStatus, 0UL, 0UL);
            }
        }
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        FLSMGR_ReportError((uint8_t)aID, FLSMGR_IDLE_STATE_HANDLER_API_ID,
                retVal, aMsg->cmd, 0UL, 0UL, 0UL);
        break;
    }

    if (BCM_ERR_OK != retVal) {
        FLSMGR_ReportError((uint8_t)aID, FLSMGR_IDLE_STATE_HANDLER_API_ID,
                retVal, aMsg->cmd, 0UL, 0UL, 0UL);
        FLSMGR_Comp[aID].state = FLSMGR_STATE_IDLE;
        FLSMGR_Comp[aID].currMsg = NULL;
        aMsg->opResult = FLSMGR_OP_RESULT_FAILED;
        retVal = BCM_ERR_OK;
    } else {
        if (FLSMGR_CMD_DEINIT != aMsg->cmd){
            retVal = BCM_ERR_EAGAIN;
        }
    }

    return retVal;
}

static int32_t FLSMGR_BusyStateHandler(FLSMGR_DevIDType aID,
                                       FLSMGR_MsgType * const aMsg)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    FLASH_OpsResultType flashOpResult;

    if (FLSMGR_Comp[aID].currMsg == aMsg) {
        /* Check the result of flash operation */
        retVal = FLASH_GetOpsResult(aID, &flashOpResult);
        if (BCM_ERR_OK != retVal) {
            goto err;
        }

        if (FLASH_OPS_RESULT_OK == flashOpResult) {
            aMsg->opResult = FLSMGR_OP_RESULT_SUCCESS;
        } else if (FLASH_OPS_RESULT_FAIL == flashOpResult) {
            aMsg->opResult = FLSMGR_OP_RESULT_FAILED;
        } else if (FLASH_OPS_RESULT_PENDING == flashOpResult) {
            retVal = BCM_ERR_EAGAIN;
            goto err;
        } else {
            retVal = BCM_ERR_UNKNOWN;
            FLSMGR_ReportError((uint8_t)aID, FLSMGR_BUSY_STATE_HANDLER_API_ID,
                    retVal, flashOpResult, 0UL, 0UL, 0UL);
            goto err;
        }

        FLSMGR_Comp[aID].state = FLSMGR_STATE_IDLE;
        FLSMGR_Comp[aID].currMsg = NULL;
        retVal = BCM_ERR_OK;
    } else {
        FLSMGR_ReportError((uint8_t)aID, FLSMGR_BUSY_STATE_HANDLER_API_ID,
                retVal, (uint32_t)FLSMGR_Comp[aID].currMsg, (uint32_t)aMsg, 0UL, 0UL);
    }
err:
    return retVal;
}

int32_t FLSMGR_MsgHandler(FLSMGR_DevIDType aID,
                        FLSMGR_MsgType * const aMsg)
{
    int32_t retVal;

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aMsg)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_IL_MSG_HANDLER_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    } else {
        /* Check Flash manager state */
        switch (FLSMGR_Comp[aID].state) {
        case FLSMGR_STATE_UNINIT:
            retVal = FLSMGR_UninitStateHandler(aID, aMsg);
            break;
        case FLSMGR_STATE_IDLE:
            retVal = FLSMGR_IdleStateHandler(aID, aMsg);
            break;
        case FLSMGR_STATE_BUSY:
            retVal = FLSMGR_BusyStateHandler(aID, aMsg);
            break;
        default:
            retVal = BCM_ERR_UNKNOWN;
            break;
        }
    }

    return retVal;
}

int32_t FLSMGR_Init(FLSMGR_DevIDType aID,
                 const FLSMGR_CfgType *const aConfig,
                 const uint32_t aClientMask,
                 const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal = BCM_ERR_OK;
#if !defined(ENABLE_FLASH_TEST)
    FLASH_StatusType flashStatus;
    FLSMGR_MsgType mesg = {0};
#endif

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aConfig) ||
            (aConfig->flashCfg == NULL) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_INIT_PROC, retVal, 0UL, 0UL,
                0UL, 0UL);
        goto err;
    }

    if (FLSMGR_STATE_UNINIT != FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_INVAL_STATE;
        FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_INIT_PROC, retVal,
                    FLSMGR_Comp[aID].state, 0UL, 0UL, 0UL);
        goto err;
    }

#if !defined(ENABLE_FLASH_TEST)
    /* Initialise flash driver */
    FLASH_Init(aID, &aConfig->flashCfg->config);

    retVal = FLASH_GetStatus(aID, &flashStatus);
    if (BCM_ERR_OK != retVal) {
        FLSMGR_ReportError((uint8_t)aID, BRCM_SWARCH_FLSMGR_INIT_PROC, retVal, 0UL, 0UL,
                0UL, 0UL);
        goto err;
    }

    if (FLASH_STATUS_IDLE == flashStatus) {
        /* Post a message to configure Flash Read Lane */
        mesg.cmd = FLSMGR_CMD_CFG_READ_LANE;
        retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);
    } else {
        retVal = BCM_ERR_UNKNOWN;
    }
#endif

err:
    return retVal;
}

int32_t FLSMGR_DeInit(FLSMGR_DevIDType aID,
                     const uint32_t aClientMask,
                     const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    mesg.cmd = FLSMGR_CMD_DEINIT;

    /* Post message to message queue */
    retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);

err:
    return retVal;
}

int32_t FLSMGR_GetState(FLSMGR_DevIDType aID,
                        FLSMGR_StateType * const aState)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aState)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    *aState = FLSMGR_Comp[aID].state;
err:
    return retVal;
}


int32_t FLSMGR_Cancel(FLSMGR_DevIDType aID,
                      FLSMGR_OpResultType* const aOpResult,
                      const FLSMGR_MsgHdrType* const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Try cancelling message */
    retVal = FLSMGR_TryCancelMsg(aID, aMsgHdr, &mesg);
    if (BCM_ERR_OK == retVal) {
        *aOpResult = mesg.opResult;
    }

    if (BCM_ERR_BUSY == retVal) {
        retVal = BCM_ERR_OK;
        *aOpResult = FLSMGR_OP_RESULT_PENDING;
    }

err:
    return retVal;
}

int32_t FLSMGR_Read(FLSMGR_DevIDType aID,
                    FLSMGR_AddrType aAddr,
                    uint8_t * const aBuf,
                    FLSMGR_LenType aLen,
                    const uint32_t aClientMask,
                    const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aBuf)
            || (0UL == aLen) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    mesg.cmd = FLSMGR_CMD_READ;
    mesg.addr = aAddr;
    mesg.len = aLen;
    mesg.bufOut = aBuf;
    mesg.opResult = FLSMGR_OP_RESULT_PENDING;

    /* Post message to message queue */
    retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);

err:
    return retVal;
}

int32_t FLSMGR_Write(FLSMGR_DevIDType aID,
                        FLSMGR_AddrType aAddr,
                        const uint8_t * const aBuf,
                        FLSMGR_LenType aLen,
                        const uint32_t aClientMask,
                        const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aBuf)
            || (0UL == aLen) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    mesg.cmd = FLSMGR_CMD_WRITE;
    mesg.addr = aAddr;
    mesg.bufIn = aBuf;
    mesg.len = aLen;
    mesg.opResult = FLSMGR_OP_RESULT_PENDING;

    /* Post message to message queue */
    retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);

err:
    return retVal;
}

int32_t FLSMGR_Erase(FLSMGR_DevIDType aID,
                    FLSMGR_AddrType aAddr,
                    FLSMGR_LenType aLen,
                    const uint32_t aClientMask,
                    const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (0UL == aLen) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    mesg.cmd = FLSMGR_CMD_ERASE;
    mesg.addr = aAddr;
    mesg.len = aLen;
    mesg.opResult = FLSMGR_OP_RESULT_PENDING;

    /* Post message to message queue */
    retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);

err:
    return retVal;
}

int32_t FLSMGR_Compare(FLSMGR_DevIDType aID,
                        FLSMGR_AddrType aAddr,
                        const uint8_t * const aBuf,
                        FLSMGR_LenType aLen,
                        const uint32_t aClientMask,
                        const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aBuf)
            || (0UL == aLen) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    mesg.cmd = FLSMGR_CMD_COMPARE;
    mesg.addr = aAddr;
    mesg.bufIn = aBuf;
    mesg.len = aLen;
    mesg.opResult = FLSMGR_OP_RESULT_PENDING;

    /* Post message to message queue */
    retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);

err:
    return retVal;
}

int32_t FLSMGR_BlankCheck(FLSMGR_DevIDType aID,
                        FLSMGR_AddrType aAddr,
                        FLSMGR_LenType aLen,
                        const uint32_t aClientMask,
                        const FLSMGR_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (0UL == aLen) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if(FLSMGR_STATE_UNINIT == FLSMGR_Comp[aID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    mesg.cmd = FLSMGR_CMD_BLANK_CHECK;
    mesg.addr = aAddr;
    mesg.len = aLen;
    mesg.opResult = FLSMGR_OP_RESULT_PENDING;

    /* Post message to message queue */
    retVal = FLSMGR_SendMsg(aID, &mesg, aClientMask, aMsgHdr);

err:
    return retVal;
}

int32_t FLSMGR_GetOpResult(FLSMGR_DevIDType aID,
                           FLSMGR_OpResultType * const aOpResult,
                           const FLSMGR_MsgHdrType * const aMsgHdr)
{
    int32_t retVal;
    FLSMGR_MsgType mesg = {0};

    if ((aID >= FLASH_MAX_HW_ID) || (NULL == aOpResult) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Receive message from message queue */
    retVal = FLSMGR_RecvMsg(aID, aMsgHdr, &mesg);
    if (BCM_ERR_OK == retVal) {
        *aOpResult = mesg.opResult;
    }

    if (BCM_ERR_BUSY == retVal) {
        retVal = BCM_ERR_OK;
        *aOpResult = FLSMGR_OP_RESULT_PENDING;
    }

err:
    return retVal;
}
