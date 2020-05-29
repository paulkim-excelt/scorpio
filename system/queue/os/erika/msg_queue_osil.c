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
    @addtogroup grp_msgq_il Integration
    @{

    @file msg_queue_osil.c
    @brief Message Queue Integration
    This source file contains the integration of Messageq Queue to system.
    @version 1.10 Imported from docx
*/

#include "ee.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <system.h>
#include <atomic.h>
#include <bcm_err.h>
#include <msg_queue.h>
#include <osil/msg_queue_osil.h>
#include <osil/bcm_osil.h>

/**
    @name Message Queue Integration Design IDs
    @{
    @brief Design IDs for Message Queue Integration
*/
#define BRCM_SWDSGN_MSGQ_SYSCMD_PROC          (0x80U)   /**< @brief #MSGQ_SysCmdReq */
#define BRCM_SWDSGN_MSGQ_CMD_HANDLER_PROC     (0x81U)   /**< @brief #MSGQ_CmdHandler*/
#define BRCM_SWDSGN_MSGQ_IOTYPE_PROC          (0x82U)   /**< @brief #MSGQ_SVCIOType */
/** @} */

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    if aMsgQIO is not NULL {
        Build SVC request
        ret = SVC_Request(&msgQSysReqIO)
        if ret is BCM_ERR_OK {
            if msgQSysReqIO.response is BCM_ERR_OK {
                ret = aMsgQIO->retVal
            } else {
                retVal = msgQSysReqIO.response
            }
        }
        if (ret is BCM_ERR_OK and (aMsgQIO->signalID and aMsgQIO->signalMask
           are valid)) {
           Signal(aMsgQIO->signalID, aMsgQIO->signalMask)
        }
    }
    return ret
    @endcode
    @trace #BRCM_SWARCH_MSGQ_SYS_CMD_REQ_PROC
    @trace #BRCM_SWREQ_MSGQ_USER_INTERFACE
*/
int32_t MSGQ_SysCmdReq(const uint32_t aCmd,
                        MSGQ_IOType * const aMsgQIO)
{
    SVC_RequestType msgQSysReqIO;
    int32_t retVal = BCM_ERR_UNKNOWN;

    if (NULL != aMsgQIO) {
        aMsgQIO->retVal = BCM_ERR_UNKNOWN;

        msgQSysReqIO.sysReqID = SVC_MSG_ID;
        msgQSysReqIO.magicID = SVC_MAGIC_MSG_ID;
        msgQSysReqIO.cmd = aCmd;
        msgQSysReqIO.svcIO = (uint8_t*)aMsgQIO;
        msgQSysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&msgQSysReqIO);
        if (BCM_ERR_OK == retVal) {
            if (BCM_ERR_OK == msgQSysReqIO.response) {
                retVal = aMsgQIO->retVal;
            } else {
                retVal = msgQSysReqIO.response;
            }
        }
        if ((BCM_ERR_OK == retVal) && (MSGQ_INVAL_CLIENTID != aMsgQIO->signalID)
            && (MSGQ_NO_CLIENTMASK != aMsgQIO->signalMask)) {
            /* Signal server for notifying reception of message */
            (void)BCM_SetEvent(aMsgQIO->signalID, aMsgQIO->signalMask);
        }
    }
    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_MSGQ_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_MSGQ_KERNEL_HANDLER
*/
typedef union _MSGQ_SVCIOType {
    uint8_t *data;
    MSGQ_IOType *io;
} MSGQ_SVCIOType;

/**
    @code{.c}
    if aMagic is BCM_MSG_SVC_MAGIC_ID {
        if aMsgQSysIO.msgQIO is not NULL {
            GetTaskID(&taskID)
            MsgQ_IntCmdHandler(aCmd, taskID, aMsgQSysIO.msgQIO)
        }
    } else {
        aMsgQSysIO.msgQIO->retVal = BCM_ERR_INVAL_MAGIC
    }
    @endcode
    @trace #BRCM_SWARCH_MSGQ_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_MSGQ_KERNEL_HANDLER
*/
void MSGQ_CmdHandler(uint32_t aMagic, uint32_t aCmd, uint8_t * aMsgQSysIO)
{
    MSGQ_SVCIOType msgq;
    msgq.data = aMsgQSysIO;

    if ((NULL != aMsgQSysIO) && (aMagic == SVC_MAGIC_MSG_ID)) {
        TaskType TaskID = INVALID_TASK;

        (void)GetTaskID(&TaskID);
        MSGQ_IntCmdHandler(aCmd, TaskID, msgq.io);
    }
}

/** @} */
