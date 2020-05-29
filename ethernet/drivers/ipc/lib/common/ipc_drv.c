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
 File Name:  IPC_drv.c
 Descritpion: Implements the IPC driver
 ******************************************************************************/

#include <stdint.h>
#include <ipc.h>
#include <ipc_osil.h>
#include <utils.h>
#include <ipc_hwif.h>
#include "ipc_plat.h"

#define IPC_KLOG_ID                     (BCM_IPC_ID | BCM_LOGMASK_KERNEL)

#define IPC_DRV_API_ID_INIT             (0x80U)
#define IPC_DRV_API_ID_DEINIT           (0x81U)
#define IPC_DRV_API_ID_ISR              (0x82U)
#define IPC_DRV_API_ID_SEND             (0x93U)
#define IPC_DRV_API_ID_RECEIVE          (0x84U)

/**
 * IPC header offsets
 */
#define IPC_HDR_MAGIC_INDEX        (0UL)
#define IPC_HDR_CHKSUM_INDEX       (1UL)
#define IPC_HDR_COMMAND_INDEX      (2UL)
#define IPC_HDR_LENGTH_INDEX       (3UL)
#define IPC_HDR_LAST_INDEX         (4UL)

/**
 * Target status register masks and shifts
 */
#define IPC_TARGET_STAT_RSVD_SHIFT      (11UL)
#define IPC_TARGET_STAT_RSVD_MASK       (0xF100U)
#define IPC_TARGET_STAT_PRI_SHIFT       (8UL)
#define IPC_TARGET_STAT_PRI_MASK        (0x0700U)
#define IPC_TARGET_STAT_PRI_REBOOT      (7U)
#define IPC_TARGET_STAT_PRI_6           (6U)
#define IPC_TARGET_STAT_PRI_5           (5U)
#define IPC_TARGET_STAT_PRI_4           (4U)
#define IPC_TARGET_STAT_PRI_3           (3U)
#define IPC_TARGET_STAT_PRI_2           (2U)
#define IPC_TARGET_STAT_PRI_1           (1U)
#define IPC_TARGET_STAT_PRI_PAUSE       (0U)
#define IPC_TARGET_STAT_WR_SHIFT        (4UL)
#define IPC_TARGET_STAT_WR_MASK         (IPC_MAX_PTR_MASK << IPC_TARGET_STAT_WR_SHIFT)
#define IPC_TARGET_STAT_RD_SHIFT        (0UL)
#define IPC_TARGET_STAT_RD_MASK         (IPC_MAX_PTR_MASK << IPC_TARGET_STAT_RD_SHIFT)


/**
 * Host status register masks and shifts
 */
#define IPC_HOST_STAT_RSVD_SHIFT        (8UL)
#define IPC_HOST_STAT_RSVD_MASK         (0xFF00U)
#define IPC_HOST_STAT_WR_SHIFT          (4UL)
#define IPC_HOST_STAT_WR_MASK           (IPC_MAX_PTR_MASK << IPC_HOST_STAT_WR_SHIFT)
#define IPC_HOST_STAT_RD_SHIFT          (0UL)
#define IPC_HOST_STAT_RD_MASK           (IPC_MAX_PTR_MASK << IPC_HOST_STAT_RD_SHIFT)


typedef uint32_t IPC_ChannStateType;
#define IPC_CHANNSTATE_UNINIT     (0x0UL)
#define IPC_CHANNSTATE_INIT       (0x1UL)

typedef struct _IPC_DrvRWDataType {
    IPC_ChannStateType state;
} IPC_DrvRWDataType;

static IPC_DrvRWDataType COMP_SECTION(".data.drivers") IPC_DrvRWData[IPC_MAX_CHANNELS] = {
#if (IPC_MAX_CHANNELS == 0)
#error "IPC_MAX_CHANNELS == 0"
#elif (IPC_MAX_CHANNELS > 0)
    {
        .state = IPC_CHANNSTATE_UNINIT,
    }
#elif (IPC_MAX_CHANNELS > 1)
    {
        .state = IPC_CHANNSTATE_UNINIT,
    }
#elif (IPC_MAX_CHANNELS > 2)
    {
        .state = IPC_CHANNSTATE_UNINIT,
    }
#else
#error (IPC_MAX_CHANNELS > 3)
#endif
};

void IPC_DrvReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                      uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_IPC_ID & BCM_LOGMASK_USER, aInstanceID, aApiID, aErr, 4UL, values);
}

static int32_t IPC_DrvBusRead(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *const aData, uint32_t aLen, IPC_AccWidthType aWidth)
{
    IPC_BusHwIDType busHwID = IPC_ChannCfg[aID].busInfo.hwID;
    uint16_t slaveID = IPC_ChannCfg[aID].busInfo.slaveID;
    IPC_BusType busType = IPC_ChannCfg[aID].busInfo.busType;
    int32_t ret = BCM_ERR_OK;
    uint32_t tempAddr = aAddr;
    uint8_t * dataPtr = aData;
    uint32_t cnt = 0;

    if (IPC_BUS_MEMMAP == busType) {
        do {
            switch (aWidth) {
                case IPC_ACCESS_WIDTH_8:
                    *((uint8_t *)dataPtr) = *((volatile uint8_t *)tempAddr);
                    dataPtr += 1UL;
                    tempAddr += 1UL;
                    cnt += 1UL;
                    break;
                case IPC_ACCESS_WIDTH_16:
                    *((uint16_t *)dataPtr) = *((volatile uint16_t *)tempAddr);
                    dataPtr += 2UL;
                    tempAddr += 2UL;
                    cnt += 2UL;
                    break;
                case IPC_ACCESS_WIDTH_32:
                    *((uint32_t*)dataPtr) = *((volatile uint32_t *)tempAddr);
                    dataPtr += 4UL;
                    tempAddr += 4UL;
                    cnt += 4UL;
                    break;
                case IPC_ACCESS_WIDTH_64:
                    *((uint64_t*)dataPtr) = *((volatile uint64_t *)tempAddr);
                    dataPtr += 8UL;
                    tempAddr += 8UL;
                    cnt += 8UL;
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
            }
        } while(cnt < aLen);
    } else {
        ret = IPC_ChannCfg[aID].busInfo.fnTbl->read(busHwID, slaveID, aAddr, aData, aLen, aWidth);
    }

    return ret;
}

static int32_t IPC_DrvBusWrite(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *const aData, uint32_t aLen, IPC_AccWidthType aWidth)
{
    IPC_BusHwIDType busHwID = IPC_ChannCfg[aID].busInfo.hwID;
    uint16_t slaveID = IPC_ChannCfg[aID].busInfo.slaveID;
    IPC_BusType busType = IPC_ChannCfg[aID].busInfo.busType;
    int32_t ret = BCM_ERR_OK;
    uint32_t tempAddr = aAddr;
    uint8_t * dataPtr = aData;
    uint32_t cnt = 0;

    if (IPC_BUS_MEMMAP == busType) {
        do {
            switch (aWidth) {
                case IPC_ACCESS_WIDTH_8:
                    *((volatile uint8_t *)tempAddr) =  *((volatile uint8_t *)dataPtr);
                    dataPtr += 1UL;
                    tempAddr += 1UL;
                    cnt += 1UL;
                    break;
                case IPC_ACCESS_WIDTH_16:
                    *((volatile uint16_t *)tempAddr) =  *((volatile uint16_t *)dataPtr);
                    dataPtr += 2UL;
                    tempAddr += 2UL;
                    cnt += 2UL;
                    break;
                case IPC_ACCESS_WIDTH_32:
                    *((volatile uint32_t *)tempAddr) =  *((volatile uint32_t *)dataPtr);
                    dataPtr += 4UL;
                    tempAddr += 4UL;
                    cnt += 4UL;
                    break;
                case IPC_ACCESS_WIDTH_64:
                    *((volatile uint64_t *)tempAddr) =  *((volatile uint64_t *)dataPtr);
                    dataPtr += 8UL;
                    tempAddr += 8UL;
                    cnt += 8UL;
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
            }
        } while(cnt < aLen);
    } else {
        ret = IPC_ChannCfg[aID].busInfo.fnTbl->write(busHwID, slaveID, aAddr, aData, aLen, aWidth);
    }

    return ret;
}

static uint32_t IPC_DrvCalChecksum(uint32_t aMagic, uint32_t aCmd, uint32_t *aMsg, uint32_t aLen)
{
    uint32_t i;
    uint32_t chksum = 0UL;
    for (i = 0UL; i < (aLen >> 2UL); i++) {
        chksum += aMsg[i];
    }
    chksum += aMagic;
    chksum += aLen;
    chksum += aCmd;
    return (~chksum) + 1UL;
}

static int32_t IPC_DrvVerifyChannCfg(IPC_ChannIDType aID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((IPC_CHANN_MODE_SLAVE != IPC_ChannCfg[aID].mode) &&
            (IPC_CHANN_MODE_MASTER != IPC_ChannCfg[aID].mode)) {
        goto err;
    }

    if ((IPC_ChannCfg[aID].sizeLog2 > IPC_BUF_LOG2_MAX_SIZE) ||
            (IPC_ChannCfg[aID].cntLog2 > IPC_BUF_LOG2_MAX_CNT)) {
        goto err;
    }
    if (IPC_BUS_PCIE < IPC_ChannCfg[aID].busInfo.busType) {
        goto err;
    }
    if (IPC_BUS_MEMMAP != IPC_ChannCfg[aID].busInfo.busType) {
        if ((NULL == IPC_ChannCfg[aID].busInfo.fnTbl->init) ||
                (NULL == IPC_ChannCfg[aID].busInfo.fnTbl->deInit) ||
                (NULL == IPC_ChannCfg[aID].busInfo.fnTbl->read) ||
                (NULL == IPC_ChannCfg[aID].busInfo.fnTbl->write)) {
            goto err;
        }
    }
    ret = BCM_ERR_OK;
err:
    return ret;
}

int32_t IPC_DrvWriteDirect(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *aData, IPC_AccWidthType aWidth)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t busHwID;

    if ((IPC_MAX_CHANNELS > aID) && (NULL != aData)) {
        if (IPC_DrvRWData[aID].state == IPC_CHANNSTATE_INIT) {
            if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
                busHwID = IPC_ChannCfg[aID].busInfo.hwID;
                ret = IPC_DrvBusWrite(busHwID, aAddr, aData, 1 << (aWidth - 1), aWidth);
            } else {
                ret = BCM_ERR_NOSUPPORT;
            }
        } else {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t IPC_DrvReadDirect(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *aData, IPC_AccWidthType aWidth)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t busHwID;

    if ((IPC_MAX_CHANNELS > aID) && (NULL != aData)) {
        if (IPC_DrvRWData[aID].state == IPC_CHANNSTATE_INIT) {
            if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
                busHwID = IPC_ChannCfg[aID].busInfo.hwID;
                ret = IPC_DrvBusRead(busHwID, aAddr, aData, 1 << (aWidth - 1), aWidth);
            } else {
                ret = BCM_ERR_NOSUPPORT;
            }
        } else {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

static int32_t IPC_DrvPause(IPC_ChannIDType aID)
{
    uint16_t regVal16;
    int32_t ret = BCM_ERR_OK;

    if ((IPC_MAX_CHANNELS > aID) && (IPC_DrvRWData[aID].state == IPC_CHANNSTATE_INIT)) {
        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
            /* Update spare registers to indicate that IPC is no more active */
            regVal16 = *IPC_TARGET_STAT_REG;
            regVal16 &= ~IPC_TARGET_STAT_PRI_MASK;
            regVal16 |= (IPC_TARGET_STAT_PRI_PAUSE << IPC_TARGET_STAT_PRI_SHIFT);
            *IPC_TARGET_STAT_REG = regVal16;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

static int32_t IPC_DrvResume(IPC_ChannIDType aID)
{
    uint16_t regVal16;
    int32_t ret = BCM_ERR_OK;

    if (IPC_MAX_CHANNELS > aID) {
        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
            /* Update spare registers to indicate that IPC is active again */
            regVal16 = *IPC_TARGET_STAT_REG;
            regVal16 &= ~IPC_TARGET_STAT_PRI_MASK;
            regVal16 |= (IPC_TARGET_STAT_PRI_1 << IPC_TARGET_STAT_PRI_SHIFT);
            *IPC_TARGET_STAT_REG = regVal16;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

static int32_t IPC_DrvSend(IPC_ChannIDType aID, uint32_t aCmd, uint8_t *aMsg, uint32_t aLen)
{
    uint16_t wPtr;
    uint16_t rPtr;
    uint32_t idx;
    uint32_t *ipcBuf;
    uint8_t *ipcBufBase;
    uint16_t ipcMsgCnt;
    uint16_t ipcMsgSz;
    uint16_t hostReg;
    uint16_t targetReg;
    uint8_t  cntRollOverMask;
    uint16_t prio;
    uint8_t  busHwID;
    uint32_t buffer[128];
    uint32_t line = __LINE__;
    uint32_t err0 = 0UL, err1 = 0UL, err2 = 0UL;
    int32_t ret = BCM_ERR_OK;
    uint32_t alignedLen = aLen;

    alignedLen += 3UL;
    alignedLen &= ~3UL;


    if (IPC_MAX_CHANNELS <= aID) {
        line = __LINE__;
        err0 = IPC_MAX_CHANNELS;
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (IPC_DrvRWData[aID].state != IPC_CHANNSTATE_INIT) {
        line = __LINE__;
        err0 = IPC_DrvRWData[aID].state;
        ret = BCM_ERR_INVAL_STATE;
        goto err;
    }

    ret = IPC_PlatGetBuffInfo(aID, &ipcBufBase, &ipcMsgCnt, &cntRollOverMask, &ipcMsgSz);
    if (ret != BCM_ERR_OK) {
        line = __LINE__;
        goto err;
    }

    if (aLen > (ipcMsgSz - IPC_MSG_HDR_SIZE)) {
        line = __LINE__;
        err0 = aLen;
        err1 = ipcMsgSz - IPC_MSG_HDR_SIZE;
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
        if (aLen > sizeof(buffer)) {
            line = __LINE__;
            err0 = aLen;
            err1 = sizeof(buffer);
            ret = BCM_ERR_INVAL_PARAMS;
            goto err;
        }
    }

    busHwID = IPC_ChannCfg[aID].busInfo.hwID;

    /* read target/host registers */
    ret = IPC_DrvBusRead(busHwID, (uint32_t)IPC_TARGET_STAT_REG, (uint8_t *)&targetReg, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        err0 = busHwID;
        goto err;
    }
    ret = IPC_DrvBusRead(busHwID, (uint32_t)IPC_HOST_STAT_REG, (uint8_t *)&hostReg, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        err0 = busHwID;
        goto err;
    }

    /* In master mode, check if target IPC priority is set
     * to reboot or pause. if set, we can not send
     * message
     */
    if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
        prio = (targetReg & IPC_TARGET_STAT_PRI_MASK) >> IPC_TARGET_STAT_PRI_SHIFT;
        if ((prio == IPC_TARGET_STAT_PRI_REBOOT) ||
                (IPC_TARGET_STAT_PRI_PAUSE == prio)) {
            line = __LINE__;
            err0 = targetReg;
            ret = BCM_ERR_BUSY;
            goto err;
        }
        rPtr = (targetReg & IPC_TARGET_STAT_RD_MASK) >> IPC_TARGET_STAT_RD_SHIFT;
        wPtr = (hostReg & IPC_HOST_STAT_WR_MASK) >> IPC_HOST_STAT_WR_SHIFT;
        idx = (wPtr & (ipcMsgCnt - 1));
    } else {
        rPtr = (hostReg & IPC_HOST_STAT_RD_MASK) >> IPC_HOST_STAT_RD_SHIFT;
        wPtr = (targetReg & IPC_TARGET_STAT_WR_MASK) >> IPC_TARGET_STAT_WR_SHIFT;
        idx = (wPtr & (ipcMsgCnt - 1)) + ipcMsgCnt;
    }

    /* Check if there a slot available for send */
    if (((rPtr + ipcMsgCnt) & IPC_MAX_PTR_MASK) != wPtr) {
        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
            ipcBuf = (uint32_t *)&ipcBufBase[(idx * ipcMsgSz)];
        } else {
            ipcBuf = buffer;
        }

        /* Memcpy */
        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
            BCM_MemSet(ipcBuf, 0U, ipcMsgSz);
        } else {
            BCM_MemSet(ipcBuf, 0U, sizeof(buffer));
        }
        BCM_MemCpy(&ipcBuf[IPC_MSG_HDR_SIZE >> 2UL], aMsg, aLen);
        if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
            ipcBuf[IPC_HDR_MAGIC_INDEX] = IPC_CMDRESPMAGIC_CMD;
        } else {
            ipcBuf[IPC_HDR_MAGIC_INDEX] = IPC_CMDRESPMAGIC_RESP;
        }
        ipcBuf[IPC_HDR_CHKSUM_INDEX] = IPC_DrvCalChecksum(ipcBuf[IPC_HDR_MAGIC_INDEX], aCmd,
                                            &ipcBuf[IPC_HDR_LAST_INDEX], aLen);
        ipcBuf[IPC_HDR_COMMAND_INDEX] = aCmd;
        ipcBuf[IPC_HDR_LENGTH_INDEX] = aLen;

        if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
            ret = IPC_DrvBusWrite(busHwID, (uint32_t)&ipcBufBase[idx * ipcMsgSz],
                    (uint8_t*)ipcBuf, alignedLen + IPC_MSG_HDR_SIZE, IPC_ACCESS_WIDTH_32);
            if (BCM_ERR_OK != ret) {
                line = __LINE__;
                err0 = busHwID;
                err1 = (uint32_t)&ipcBufBase[idx * ipcMsgSz];
                err2 = aLen + IPC_MSG_HDR_SIZE;
                goto err;
            }
        }

        /* Update write pointer in register */

        wPtr++;
        wPtr &= cntRollOverMask;

        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
            targetReg &= ~IPC_TARGET_STAT_WR_MASK;
            targetReg |= ((wPtr << IPC_TARGET_STAT_WR_SHIFT) & IPC_TARGET_STAT_WR_MASK);
            ret = IPC_DrvBusWrite(busHwID, (uint32_t)IPC_TARGET_STAT_REG, (uint8_t *)&targetReg,
                    sizeof(uint16_t),  IPC_ACCESS_WIDTH_16);
        } else {
            hostReg &= ~IPC_HOST_STAT_WR_MASK;
            hostReg |= ((wPtr << IPC_HOST_STAT_WR_SHIFT) & IPC_HOST_STAT_WR_MASK);
            ret = IPC_DrvBusWrite(busHwID, (uint32_t)IPC_HOST_STAT_REG, (uint8_t *)&hostReg,
                    sizeof(uint16_t),  IPC_ACCESS_WIDTH_16);
        }
        if (BCM_ERR_OK != ret) {
            line = __LINE__;
            err0 = IPC_ChannCfg[aID].mode;
            err1 = busHwID;
            goto err;
        }
        /* Raise a signal */
        IPC_PlatSetIntr(aID);
    } else {
        ret = BCM_ERR_NOMEM;
    }

err:
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOMEM != ret)) {
        IPC_DrvReportError(aID, IPC_DRV_API_ID_SEND, ret, err0, err1, err2, line);
    }

    return ret;
}

static int32_t IPC_DrvReceive(IPC_ChannIDType aID, uint32_t *aCmd, uint8_t *aMsg, uint32_t aInLen, uint32_t *aOutLen)
{
    uint16_t wPtr;
    uint16_t rPtr;
    uint32_t idx;
    uint32_t *ipcBuf;
    uint8_t *ipcBufBase;
    uint16_t ipcMsgCnt;
    uint16_t ipcMsgSz;
    uint16_t hostReg;
    uint16_t targetReg;
    uint8_t  cntRollOverMask;
    uint8_t  busHwID;
    uint32_t msgLen;
    uint32_t chksum;
    uint32_t header[IPC_HDR_LAST_INDEX];
    uint32_t line = __LINE__;
    uint32_t err0 = 0UL, err1 = 0UL, err2 = 0UL;
    int32_t ret = BCM_ERR_OK;

    if (IPC_MAX_CHANNELS <= aID) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (IPC_DrvRWData[aID].state != IPC_CHANNSTATE_INIT) {
        line = __LINE__;
        err0 = IPC_DrvRWData[aID].state;
        ret = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if ((NULL == aCmd) || (NULL == aMsg) || (NULL == aOutLen)) {
        line = __LINE__;
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    ret = IPC_PlatGetBuffInfo(aID, &ipcBufBase, &ipcMsgCnt, &cntRollOverMask, &ipcMsgSz);
    if (ret != BCM_ERR_OK) {
        line = __LINE__;
        goto err;
    }
    busHwID = IPC_ChannCfg[aID].busInfo.hwID;

    /* read target/host registers */
    ret = IPC_DrvBusRead(busHwID, (uint32_t)IPC_TARGET_STAT_REG, (uint8_t *)&targetReg, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        err0 = busHwID;
        goto err;
    }
    ret = IPC_DrvBusRead(busHwID, (uint32_t)IPC_HOST_STAT_REG, (uint8_t *)&hostReg, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        err0 = busHwID;
        goto err;
    }

    if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
        wPtr = (targetReg & IPC_TARGET_STAT_WR_MASK) >> IPC_TARGET_STAT_WR_SHIFT;
        rPtr = (hostReg & IPC_HOST_STAT_RD_MASK) >> IPC_HOST_STAT_RD_SHIFT;
        idx = (rPtr & (ipcMsgCnt - 1)) + ipcMsgCnt;
    } else {
        wPtr = (hostReg & IPC_HOST_STAT_WR_MASK) >> IPC_HOST_STAT_WR_SHIFT;
        rPtr = (targetReg & IPC_TARGET_STAT_RD_MASK) >> IPC_TARGET_STAT_RD_SHIFT;
        idx = (rPtr & (ipcMsgCnt - 1));
    }
    if (rPtr != wPtr) {
        /* read the header first */
        ipcBuf = (uint32_t *)&ipcBufBase[(idx * ipcMsgSz)];
        ret = IPC_DrvBusRead(busHwID, (uint32_t)ipcBuf, (uint8_t *)header, sizeof(header), IPC_ACCESS_WIDTH_32);
        if (BCM_ERR_OK != ret) {
            line = __LINE__;
            err0 = busHwID;
            goto err;
        }

        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
            if (IPC_CMDRESPMAGIC_CMD != header[IPC_HDR_MAGIC_INDEX]) {
                ret = BCM_ERR_INVAL_MAGIC;
                line = __LINE__;
                goto err;
            }
        } else {
            if (IPC_CMDRESPMAGIC_RESP != header[IPC_HDR_MAGIC_INDEX]) {
                ret = BCM_ERR_INVAL_MAGIC;
                line = __LINE__;
                goto err;
            }
        }
        msgLen = header[IPC_HDR_LENGTH_INDEX];
        /* read the payload */
        if (aInLen > msgLen) {
            ret = IPC_DrvBusRead(busHwID, ((uint32_t)(intptr_t)ipcBuf) + sizeof(header), aMsg,
                        msgLen, IPC_ACCESS_WIDTH_32);
            chksum = IPC_DrvCalChecksum(header[IPC_HDR_MAGIC_INDEX],
                    header[IPC_HDR_COMMAND_INDEX], (uint32_t*)aMsg, header[IPC_HDR_LENGTH_INDEX]);
            if (chksum == header[IPC_HDR_CHKSUM_INDEX]) {
                *aCmd = header[IPC_HDR_COMMAND_INDEX];
                *aOutLen = header[IPC_HDR_LENGTH_INDEX];
            } else {
                line = __LINE__;
                err0 = msgLen;
                err1 = chksum;
                ret = BCM_ERR_DATA_INTEG;
            }
            rPtr++;
            rPtr &= cntRollOverMask;

            if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
                targetReg &= ~IPC_TARGET_STAT_RD_MASK;
                targetReg |= (rPtr << IPC_TARGET_STAT_RD_SHIFT) & IPC_TARGET_STAT_RD_MASK;
                ret = IPC_DrvBusWrite(busHwID, (uint32_t)IPC_TARGET_STAT_REG, (uint8_t *)&targetReg,
                        sizeof(uint16_t),  IPC_ACCESS_WIDTH_16);
                line = __LINE__;
            } else {
                hostReg &= ~IPC_HOST_STAT_RD_MASK;
                hostReg |= (rPtr << IPC_HOST_STAT_RD_SHIFT) & IPC_HOST_STAT_RD_MASK;
                ret = IPC_DrvBusWrite(busHwID, (uint32_t)IPC_HOST_STAT_REG, (uint8_t *)&hostReg,
                        sizeof(uint16_t),  IPC_ACCESS_WIDTH_16);
                line = __LINE__;
            }
        } else {
            line = __LINE__;
            err0 = aInLen;
            err1 = msgLen;
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_NOMEM;
    }
err:
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOMEM != ret)) {
        IPC_DrvReportError(aID, IPC_DRV_API_ID_RECEIVE, ret, err0, err1, err2, line);
    }
    return ret;
}

static int32_t IPC_DrvDeinit(IPC_ChannIDType aID)
{
    uint16_t regVal16;
    int32_t ret = BCM_ERR_OK;

    if (IPC_MAX_CHANNELS > aID) {
        if (IPC_DrvRWData[aID].state == IPC_CHANNSTATE_INIT) {
            if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
                /* Update spare registers to indicate that IPC is no more active */
                regVal16 = *IPC_TARGET_STAT_REG;
                regVal16 &= ~IPC_TARGET_STAT_PRI_MASK;
                regVal16 |= (IPC_TARGET_STAT_PRI_PAUSE << IPC_TARGET_STAT_PRI_SHIFT);
                *IPC_TARGET_STAT_REG = regVal16;
            } else {
                /* Deinitialize the slave bus */
                IPC_ChannCfg[aID].busInfo.fnTbl->deInit(IPC_ChannCfg[aID].busInfo.hwID);
            }
            IPC_DrvRWData[aID].state = IPC_CHANNSTATE_UNINIT;
        } else {
            ret = BCM_ERR_INVAL_STATE;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    if (BCM_ERR_OK != ret) {
        IPC_DrvReportError(aID, IPC_DRV_API_ID_DEINIT, ret, IPC_DrvRWData[aID].state, 0UL, 0UL, 0UL);
    }
    return ret;
}

static int32_t IPC_DrvInit(IPC_ChannIDType aID)
{
    int32_t ret = BCM_ERR_OK;
    uint16_t regVal16;

    if (IPC_MAX_CHANNELS <= aID) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (IPC_DrvRWData[aID].state != IPC_CHANNSTATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
        goto err;
    }

    ret = IPC_DrvVerifyChannCfg(aID);
    if (BCM_ERR_OK != ret) {
        goto err;
    }

    if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
        /* Initialize the slave bus */
        if (IPC_BUS_MEMMAP != IPC_ChannCfg[aID].busInfo.busType) {
            IPC_ChannCfg[aID].busInfo.fnTbl->init(IPC_ChannCfg[aID].busInfo.hwID, &IPC_ChannCfg[aID].busInfo.config);
        }
    }

    if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
        ret = IPC_PlatSetBuffInfo(aID, IPC_ChannCfg[aID].sizeLog2, IPC_ChannCfg[aID].cntLog2);
        if (BCM_ERR_OK != ret) {
            goto err;
        }
        /* Inform the master that we are ready */
        regVal16 = *IPC_TARGET_STAT_REG;
        regVal16 &= ~IPC_TARGET_STAT_PRI_MASK;
        regVal16 |= (IPC_TARGET_STAT_PRI_1 << IPC_TARGET_STAT_PRI_SHIFT);
        *IPC_TARGET_STAT_REG = regVal16;
    }
    IPC_DrvRWData[aID].state = IPC_CHANNSTATE_INIT;
    IPC_PlatEnableIntr(aID);
err:
    return ret;
}

void IPC_IRQHandler(IPC_ChannIDType aID)
{
    /* Clear interrupt source */
    IPC_PlatClearIntr(aID);
}

typedef union _IPC_SVCIOType {
    uint8_t *data;
    IPCIO *io;
} IPC_SVCIOType;

/* SVC Command Handler */
void IPC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    int32_t ret = BCM_ERR_OK;
    IPC_SVCIOType ipc;
    ipc.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_IPC_ID == aMagicID) {
            switch(aCmd){
            case IPC_CMD_INIT:
                ret = IPC_DrvInit(ipc.io->channID);
                break;
            case IPC_CMD_DEINIT:
                ret = IPC_DrvDeinit(ipc.io->channID);
                break;
            case IPC_CMD_SEND:
                ret = IPC_DrvSend(ipc.io->channID, ipc.io->cmd, ipc.io->msg, ipc.io->inLen);
                break;
            case IPC_CMD_RECEIVE:
                ret = IPC_DrvReceive(ipc.io->channID, &ipc.io->cmd, ipc.io->msg, ipc.io->inLen, &ipc.io->outLen);
                break;
            case IPC_CMD_PAUSE:
                ret = IPC_DrvPause(ipc.io->channID);
                break;
            case IPC_CMD_RESUME:
                ret = IPC_DrvResume(ipc.io->channID);
                break;
            case IPC_CMD_READ_DIRECT:
                ret = IPC_DrvReadDirect(ipc.io->channID, ipc.io->addr, ipc.io->msg, ipc.io->width);
                break;
            case IPC_CMD_WRITE_DIRECT:
                ret = IPC_DrvWriteDirect(ipc.io->channID, ipc.io->addr, ipc.io->msg, ipc.io->width);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            ret = BCM_ERR_INVAL_MAGIC;
        }
        ipc.io->status = ret;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOMEM != ret)) {
        IPC_DrvReportError(0xFFUL, BRCM_ARCH_IPC_IL_CMD_HANDLER_PROC, ret, aMagicID, aCmd, (uint32_t)aSysIO, 0UL);
    }
}

