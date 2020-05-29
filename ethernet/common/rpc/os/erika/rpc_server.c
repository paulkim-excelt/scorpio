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

#include <ee.h>
#include <ipc.h>
#include <ipc_osil.h>
#include <rpc_async_osil.h>
#include <etherswt_ipc.h>
#include <sys_ipc_cmds.h>
#include <imgl_ipc_cmds.h>
#include <rpc_cmds.h>
#include <utils.h>
#include <board.h>
#include <init.h>
#include <bcm_time.h>
#include <lw_queue.h>
#ifdef ENABLE_ETS
#include <ets_ipc.h>
#endif
#ifdef ENABLE_ETH_SWITCH
#include <comms_ipc.h>
#include <eth_switch.h>
#endif

#include <rpc_loader_osil.h>
#include <rpc_cmd_queue_osil.h>

#include <osil/sys_ipc_osil.h>
#include <osil/bcm_osil.h>
#include <comms.h>
#include <mcu_ext.h>

#define RPC_CHANNEL_EVENTS    (IPC_CHANN0_EVENT | IPC_CHANN1_EVENT)

#ifdef __BCM89564G__
static uint32_t RPC_ServerModeMaster = FALSE;
#endif

/**
    @code{.unparsed}
    BCM_ReportError();
    @endcode
*/
static void RPC_ServerReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                            uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};

    BCM_ReportError(BCM_RPC_ID, 0U, aApiID,
                    aErr, 4UL, values);
}

/** @brief RPC server initialization

    This is the initialization routine for RPC server. This call
    initializes the RPC driver.

    @behavior Sync, Non-rentrant

    @pre None

    @param void

    Return values are documented in reverse-chronological order
    @retval void

    @post None
*/
static void RPC_ServerInit()
{
    uint8_t i;
#ifdef __BCM89564G__
    int32_t retVal;
    MCU_ExtendedInfoType stackingInfo;
#endif

    /* Initialize the RPC driver */
    for (i = 0UL; i < RPC_MAX_CHANNELS; i++) {
        IPC_Init(IPC_ChannCfg[i].ID);
    }

#ifdef __BCM89564G__
    retVal = MCU_GetExtendedInfo(&stackingInfo);

    if ((BCM_ERR_OK == retVal) && (TRUE == stackingInfo.stackingEn) &&
            (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode)) {
        RPC_ServerModeMaster = TRUE;
    }
#endif
    return;
}

/** @brief RPC Command Send


    @behavior Sync, Non-rentrant

    @pre None

    @param void

    @post None
*/
int32_t RPC_Send(uint8_t aID, uint32_t aCmd, uint8_t *aMsg, uint32_t aLen)
{
    int32_t retval;

    retval = IPC_Send(aID,aCmd,aMsg,aLen);
    return retval;
}

/** @brief RPC Command Send


    @behavior Sync, Non-rentrant

    @pre None

    @param void

    @post None
*/
int32_t RPC_GetChannelMode(RPC_ChannModeType aMode)
{
    return aMode;
}

#if defined(__BCM89564G__) && (!defined(ENABLE_IPC_S2M_INTR))
void RPC_AlarmCb(void)
{
    if (TRUE == RPC_ServerModeMaster) {
        SetEvent(RPC_ServerTask, IPC_CHANN1_EVENT);
    }
}
#endif /* defined(__BCM89564G__) && (!defined(ENABLE_IPC_S2M_INTR)) */

TASK(RPC_ServerTask)
{
    uint32_t i;
    uint32_t cmd = 0UL;
    uint32_t len = 0UL;
    int32_t retVal = BCM_ERR_OK;
    uint8_t cmdBuffer[RPC_MSG_PAYLOAD_SZ];
    RPC_ChannIDType channID;
    RPC_LoaderRpcMsgType imglMsg;
    RPC_CmdQRpcMsgType cmdQMsg;

    BCM_EventMaskType mask;
    BCM_EventMaskType rpcEvents = (RPC_CHANNEL_EVENTS);

    RPC_ServerInit();
    RPC_IpcLoaderInit();
    RPC_CmdQInit();
    COMMS_NotifyState(COMMS_SUBSYSTEM_IPC, COMMS_SUBSYSTEM_STATE_INIT);

    while (1UL) {

        (void)BCM_WaitEvent(rpcEvents |
                            RPC_CMDQ_EVENT |
                            RPC_ASYNC_EVENT |
                            RPC_LOADER_EVENT |
                            ShutdownEvent);
        (void)BCM_GetEvent(RPC_ServerTask, &mask);
        (void)BCM_ClearEvent(mask);

        if (mask & RPC_CHANNEL_EVENTS) {
            /* When host consumes an Async message, it notifies the
               target. We use this opportunity to see if there are
               any pending Async messages waiting to be pushed to RPC */
            mask |= RPC_ASYNC_EVENT;

            while (1UL) {
                if ((mask & IPC_CHANN0_EVENT) == IPC_CHANN0_EVENT) {
                    channID = 0UL;
                } else if ((mask & IPC_CHANN1_EVENT) == IPC_CHANN1_EVENT) {
                    channID = 1UL;
                } else {
                    RPC_ServerReportError(0UL, BCM_ERR_INVAL_PARAMS, __LINE__, 0UL, 0UL, 0UL);
                    break;
                }

                retVal = IPC_Receive(channID, &cmd, cmdBuffer, sizeof(cmdBuffer), &len);
                if (BCM_ERR_NOMEM == retVal) {
                    break;
                }
                if (retVal == BCM_ERR_OK) {
                    /* incoming message received. process it */
                    if (RPC_CHANN_MODE_MASTER == RPC_GetChannelMode(IPC_ChannCfg[channID].mode)) {
                        if (BCM_GROUPID_IMGL == RPC_GET_GROUPID(cmd)) {
                            imglMsg.payLoad = cmdBuffer;
                            imglMsg.payLoadSize = len;
                            imglMsg.cmd = cmd;
                            imglMsg.id = channID;
                            RPC_IpcLoaderMasterModeMsgHandler(&imglMsg);
                            retVal = BCM_ERR_OK;
                        } else {
                            if (0UL == (cmd & RPC_CMD_ASYNC_MASK)) {
                                int32_t respCode = cmdBuffer[3];
                                respCode = (respCode << 8UL) | cmdBuffer[2];
                                respCode = (respCode << 8UL) | cmdBuffer[1];
                                respCode = (respCode << 8UL) | cmdBuffer[0];
                                cmdQMsg.payLoad = cmdBuffer + 4UL;
                                cmdQMsg.payLoadSize = len - 4UL;
                                cmdQMsg.cmd = cmd;
                                cmdQMsg.id = channID;
                                cmdQMsg.error = (int32_t)respCode;
                                cmdQMsg.channel = RPC_CHANNEL_REMOTE;
                                RPC_CmdQAddResponse(&cmdQMsg);
                                retVal = BCM_ERR_OK;
                            } else {
                            /* Handle ASYNC Messages */
                            }
                        }
                    } else {
                        /* Received a command from Master/Host. Process it */
                        if (BCM_GROUPID_IMGL == RPC_GET_GROUPID(cmd)) {
                            imglMsg.payLoad = cmdBuffer;
                            imglMsg.payLoadSize = len;
                            imglMsg.cmd = cmd;
                            imglMsg.id = channID;
                            RPC_IpcLoaderSlaveModeMsgHandler(&imglMsg);
                            retVal = BCM_ERR_OK;
                        } else {
                            cmdQMsg.payLoad = cmdBuffer;
                            cmdQMsg.payLoadSize = len;
                            cmdQMsg.cmd = cmd;
                            cmdQMsg.id = channID;
                            cmdQMsg.error = BCM_ERR_OK;
                            cmdQMsg.channel = RPC_CHANNEL_REMOTE;
                            retVal = RPC_CmdQAddCmd(&cmdQMsg);
                        }
                        if (BCM_ERR_OK != retVal) {
                            RPC_Send(channID, cmd, (uint8_t*)&retVal, sizeof(retVal));
                        }
                    }
                }
            }
        }

        if (mask & RPC_CMDQ_EVENT) {
            RPC_CmdQEventHandler();
        }

        if (mask & RPC_LOADER_EVENT) {
            RPC_IpcLoaderEventHandler();
        }

        if (mask & RPC_ASYNC_EVENT) {
            RPC_AsyncProcessEvents();
        }

        if (mask & ShutdownEvent) {
            break;
        }
    }

    RPC_IpcLoaderDeInit();

    /* TODO: reboot request to slave if applicable */
    for (i = 0UL; i < RPC_MAX_CHANNELS; i++) {
        IPC_DeInit(IPC_ChannCfg[i].ID);
    }

    COMMS_NotifyState(COMMS_SUBSYSTEM_IPC, COMMS_SUBSYSTEM_STATE_UNINIT);

    (void)BCM_TerminateTask();

    return;
}
