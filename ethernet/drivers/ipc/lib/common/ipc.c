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

/******************************************************************************
 File Name:  IPC.c
 Descritpion: This file implements API exposed to IPC's client
 ******************************************************************************/

#include <stdint.h>
#include <ipc.h>
#include <ipc_osil.h>
#include <bcm_err.h>
#include <utils.h>

void IPC_ReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                      uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_IPC_ID & BCM_LOGMASK_USER, aInstanceID, aApiID, aErr, 4UL, values);
}

void IPC_Init(IPC_ChannIDType aID)
{
    int32_t ret;
    IPCIO ipcIO = {
        .channID = aID,
        .status = BCM_ERR_UNKNOWN,
    };

    ret = IPC_SysCmdReq(IPC_CMD_INIT, &ipcIO);
    if (BCM_ERR_OK != ret) {
        IPC_ReportError(aID, BRCM_ARCH_IPC_INIT_PROC, ret, (uint32_t)__LINE__, (uint32_t)ipcIO.status, 0UL, 0UL);
    }
}

int32_t IPC_DeInit(IPC_ChannIDType aID)
{
    int32_t ret;
    IPCIO ipcIO = {
        .channID = aID,
        .status = BCM_ERR_UNKNOWN,
    };

    ret = IPC_SysCmdReq(IPC_CMD_DEINIT, &ipcIO);
    if (BCM_ERR_OK != ret) {
        IPC_ReportError(aID, BRCM_ARCH_IPC_DEINIT_PROC, ret, (uint32_t)__LINE__, (uint32_t)ipcIO.status, 0UL, 0UL);
    }
    return ipcIO.status;
}


int32_t IPC_Send(IPC_ChannIDType aID, uint32_t aCmd, uint8_t *aMsg, uint32_t aLen)
{
    IPCIO ipcIO = {
        .channID = aID,
        .status = BCM_ERR_UNKNOWN,
        .msg = aMsg,
        .cmd = aCmd,
        .inLen = aLen,
    };

    return IPC_SysCmdReq(IPC_CMD_SEND, &ipcIO);
}

int32_t IPC_Receive(IPC_ChannIDType aID, uint32_t* aCmd, uint8_t *aMsg, uint32_t aInLen, uint32_t *aOutLen)
{
    int32_t ret;
    IPCIO ipcIO = {
        .status = BCM_ERR_UNKNOWN,
        .channID = aID,
        .msg = aMsg,
        .inLen = aInLen,
    };

    if ((NULL != aCmd) && (NULL != aMsg) && (NULL != aOutLen)) {
        ret = IPC_SysCmdReq(IPC_CMD_RECEIVE, &ipcIO);
        if (BCM_ERR_OK == ret) {
            *aCmd = ipcIO.cmd;
            *aOutLen = ipcIO.outLen;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t IPC_Pause(IPC_ChannIDType aID)
{
    IPCIO ipcIO = {
        .channID = aID,
        .status = BCM_ERR_UNKNOWN,
    };

    return IPC_SysCmdReq(IPC_CMD_PAUSE, &ipcIO);
}

int32_t IPC_Resume(IPC_ChannIDType aID)
{
    IPCIO ipcIO = {
        .channID = aID,
        .status = BCM_ERR_UNKNOWN,
    };

    return IPC_SysCmdReq(IPC_CMD_RESUME, &ipcIO);
}

int32_t IPC_ReadDirect(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *aData, IPC_AccWidthType aWidth)
{
    IPCIO ipcIO = {
        .channID = aID,
        .addr = aAddr,
        .msg = aData,
        .width = aWidth,
        .status = BCM_ERR_UNKNOWN,
    };
    return IPC_SysCmdReq(IPC_CMD_READ_DIRECT, &ipcIO);
}

int32_t IPC_WriteDirect(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *aData, IPC_AccWidthType aWidth)
{
    IPCIO ipcIO = {
        .channID = aID,
        .addr = aAddr,
        .msg = aData,
        .width = aWidth,
        .status = BCM_ERR_UNKNOWN,
    };
    return IPC_SysCmdReq(IPC_CMD_WRITE_DIRECT, &ipcIO);
}
