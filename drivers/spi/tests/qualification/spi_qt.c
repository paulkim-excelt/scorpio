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
#include "ee.h"
#include <spi_osil.h>
#include <utils.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>
#include <gpio.h>

#define GetModuleLogLevel()     (ULOG_LVL_ERROR)

#ifdef ENABLE_HIPC
/* Macros for SPI opcode generation */
#define MAX_BUF_SZ                      (16)
#define DEFAULT_DEVICE_ID               (0)
#define SPI_OPCODE_PHYADDR_SHIFT        (6)
#define SPI_OPCODE_PHYADDR(a)           ((a) << SPI_OPCODE_PHYADDR_SHIFT)
#define SPI_OPCODE_RDWR_SHIFT           (5)
#define SPI_OPCODE_WR                   (1 << SPI_OPCODE_RDWR_SHIFT)
#define SPI_OPCODE_RD                   (0 << SPI_OPCODE_RDWR_SHIFT)
#define SPI_OPCODE_INC_SHIFT            (4)
#define SPI_OPCODE_AUTO_INC             (1 << SPI_OPCODE_INC_SHIFT)
#define SPI_OPCODE_NO_INC               (0 << SPI_OPCODE_INC_SHIFT)
#define SPI_OPCODE_RD_WAIT_SHIFT        (2)
#define SPI_OPCODE_RD_WAIT_MASK         (0x3 << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_0            (0 << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_2            (1 << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_4            (2 << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_6            (3 << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_TZ_SZ_MASK           (3)
#define SPI_OPCODE_TX_SZ_8              (0)
#define SPI_OPCODE_TX_SZ_16             (1)
#define SPI_OPCODE_TX_SZ_32             (2)
#define SPI_OPCODE_TX_SZ_64             (3)

/* TODO: use RDB */
#define IND_ACC_RETRY                   (10)
#define IND_ACC_TX_SZ_8                 (0)
#define IND_ACC_TX_SZ_16                (1)
#define IND_ACC_TX_SZ_32                (2)
#define IND_ACC_TX_SZ_64                (3)
#define IND_ACC_RD                      (0 << 2)
#define IND_ACC_WR                      (1 << 2)
#define IND_ACC_AUTO_INC                (1 << 3)
#define IND_ACC_NO_AUTO_INC             (0 << 3)
#define IND_ACC_COMMIT                  (1 << 4)
#define IND_ACC_COMMIT_ON_RDWR          (1 << 5)
#define IND_ACC_DONE                    (1 << 7)

#endif /* ENABLE_HIPC */

#define SPI_IT1                 (1UL)
static int32_t SPIIT1_Result = BCM_AT_NOT_STARTED;

#ifdef ENABLE_HIPC
#define SPI_DATA_PKTS_NUM       (1UL)
uint8_t srcBuf[64];
uint8_t destBuf[64];

volatile uint32_t testAddr;
extern uint8_t __ipc_end__[];

SPI_DataPktType SPI_DataPktList[SPI_DATA_PKTS_MAX] = {
    {
        .id = 0,
        .dataWidth = 8,
        .defaultTxValue = 0x0,
        .srcBufMaxSize = sizeof(srcBuf),
        .destBufMaxSize = sizeof(destBuf),
        .xferSize = 0,
        .srcBuf = NULL,
        .destBuf = NULL,
    },
};

SPI_DataPktIDType SPI_DataPktIdxList[] = {
    0U,
};

const SPI_XferPktType SPI_XferPktList0[] = {
    {
        .id = 0U,
        .hwID = 1U,
        .frameFormat = SPI_FRM_FORMAT_MOTOROLA,
        .bitRate = 25000000UL,
        .priority = SPI_XFER_PKT_PRIO_MAX - 1,
        .dataPktIdxList = SPI_DataPktIdxList,
        .dataPktIdxListSz = (sizeof(SPI_DataPktIdxList) / sizeof(SPI_DataPktIDType)),
    },
};

const SPI_SeqType SPI_SeqList[] = {
    {
        .id = 0U,
        .xferPktList = SPI_XferPktList0,
        .xferPktListSz = (sizeof(SPI_XferPktList0) / sizeof(SPI_XferPktType)),
        .isInterruptible = TRUE,
    },
};

static int32_t SPI_GenPktWrite32(uint32_t slaveId, uint32_t addr, uint32_t data)
{
    int32_t len = 0;
    srcBuf[len++] = SPI_OPCODE_PHYADDR(slaveId) | SPI_OPCODE_WR | SPI_OPCODE_NO_INC | SPI_OPCODE_RD_WAIT_0 | SPI_OPCODE_TX_SZ_32;
    srcBuf[len++] = (addr >> 24) & 0xff;
    srcBuf[len++] = (addr >> 16) & 0xff;
    srcBuf[len++] = (addr >> 8) & 0xff;
    srcBuf[len++] = (addr & 0xff);
    srcBuf[len++] = (data >> 24) & 0xff;
    srcBuf[len++] = (data >> 16) & 0xff;
    srcBuf[len++] = (data >> 8) & 0xff;
    srcBuf[len++] = (data & 0xff);
    return len;
}

static int32_t SPI_GenPktRead32(uint32_t slaveId, uint32_t addr)
{
    int32_t len = 0;
    int i;

    srcBuf[len++] = SPI_OPCODE_PHYADDR(slaveId) | SPI_OPCODE_RD | SPI_OPCODE_NO_INC | SPI_OPCODE_RD_WAIT_6 | SPI_OPCODE_TX_SZ_32;
    srcBuf[len++] = (addr >> 24) & 0xff;
    srcBuf[len++] = (addr >> 16) & 0xff;
    srcBuf[len++] = (addr >> 8) & 0xff;
    srcBuf[len++] = (addr & 0xff);
    /* wait states as per opcode */
    for (i =0; i < ((srcBuf[0] & SPI_OPCODE_RD_WAIT_MASK) >>
        SPI_OPCODE_RD_WAIT_SHIFT) * 2; i++) {
        srcBuf[len++] = 0x0;
    }
    return len + 4;
}

int32_t SPI_IpcSyncPktXfer(uint32_t len)
{
    uint32_t i = 0;
    int32_t retVal = BCM_ERR_OK;

    memset(destBuf, 0xEE, sizeof(destBuf));

    for (i = 0UL; i < SPI_DATA_PKTS_NUM; i++) {
        retVal = SPI_SetupBuf(i, srcBuf, destBuf, len);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("SPI_SetupBuf failed %d\n", retVal);
            goto err;
        }
    }

    /* HW expects minimum 300ns delay after last CS inactive state.
     * Delay due to SW operation is more than 1.5us so not putting any
     * explict delay. Applicable for subsequent tests as well  */
    GPIO_WriteChannel(GPIO_CHANNEL_4, GPIO_LEVEL_LOW);
    retVal = SPI_SyncXfer(0U);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("SPI_SeqSyncXfer failed %d\n", retVal);
        GPIO_WriteChannel(GPIO_CHANNEL_4, GPIO_LEVEL_HIGH);
        goto err;
    }
    GPIO_WriteChannel(GPIO_CHANNEL_4, GPIO_LEVEL_HIGH);

err:
    return retVal;
}

int32_t SPI_IpcXfer()
{
    uint32_t len = 0;
    int32_t retVal = BCM_ERR_OK;
    uint64_t recvData = 0;

    testAddr = ((uint32_t)__ipc_end__ - 4);
    memset((void *)testAddr, 0xdd, 8);
    SPIIT1_Result = BCM_ERR_OK;

    /* Test 32 bit Write */
    memset(srcBuf, 0x0, sizeof(srcBuf));
    len = SPI_GenPktWrite32(0,(uint32_t)testAddr, 0x11223344);
    if (BCM_ERR_OK != SPI_IpcSyncPktXfer(len)){
        ULOG_ERR("SPI 32bit write failed %d\n", retVal);
        goto err;
    }

    /* Test 32 bit Read */
    memset(srcBuf, 0x0, sizeof(srcBuf));
    len = SPI_GenPktRead32(0,(uint32_t)testAddr);
    if (BCM_ERR_OK != SPI_IpcSyncPktXfer(len)){
        ULOG_ERR("SPI 32bit read failed %d\n", retVal);
        goto err;
    }
    recvData = (destBuf[len-4] << 24) |
               (destBuf[len-3] << 16) |
               (destBuf[len-2] << 8) |
               (destBuf[len-1]);
    if (0x11223344 != (recvData & 0xFFFFFFFF)) {
        goto err;
    }

    return retVal;

err:
    SPIIT1_Result = BCM_ERR_UNKNOWN;
    retVal = BCM_ERR_UNKNOWN;
    return retVal;
}

#else /* ENABLE_HIPC */

#define SEQ0_EVENT              SystemEvent0
#define SEQ1_EVENT              SystemEvent1
#define SEQ_EVENTS              (SEQ0_EVENT | SEQ1_EVENT)
#define SPI_DATA_PKTS_NUM       (3UL)

uint8_t srcBuf[25];
uint8_t destBuf[25];

static uint32_t XferPktCbCnt;
static uint32_t SeqCbCnt;

void SPI_XferPkt0EndCb(void)
{
    XferPktCbCnt++;
}

void SPI_XferPkt1EndCb(void)
{
    XferPktCbCnt++;
}
void SPI_XferPkt2EndCb(void)
{
    XferPktCbCnt++;
}
void SPI_XferPkt3EndCb(void)
{
    XferPktCbCnt++;
}
void SPI_Seq0EndCb(void)
{
    SeqCbCnt++;
    SetEvent(SPIIT_Task, SEQ0_EVENT);
}
void SPI_Seq1EndCb(void)
{
    SeqCbCnt++;
    SetEvent(SPIIT_Task, SEQ1_EVENT);
}
SPI_DataPktType SPI_DataPktList[SPI_DATA_PKTS_MAX] = {
    {
        .id = SPI_DATA_PKTS_NUM - 3UL,
        .dataWidth = 8,
        .defaultTxValue = 55,
        .srcBufMaxSize = 12,
        .destBufMaxSize = 12,
        .xferSize = 0,
        .srcBuf = NULL,
        .destBuf = NULL,
    },
    {
        .id = SPI_DATA_PKTS_NUM - 2UL,
        .dataWidth = 12,
        .defaultTxValue = 33,
        .srcBufMaxSize = 12,
        .destBufMaxSize = 12,
        .xferSize = 0,
        .srcBuf = NULL,
        .destBuf = NULL,
    },
    {
        .id = SPI_DATA_PKTS_NUM - 1UL,
        .dataWidth = 16,
        .defaultTxValue = 11,
        .srcBufMaxSize = 12,
        .destBufMaxSize = 12,
        .xferSize = 0,
        .srcBuf = NULL,
        .destBuf = NULL,
    },
};

SPI_DataPktIDType SPI_DataPktIdxList[] = {
    0U,
    1U,
    2U,
};

const SPI_XferPktType SPI_XferPktList0[] = {
    {
        .id = 0U,
        .hwID = 1U,
        .frameFormat = SPI_FRM_FORMAT_MOTOROLA,
        .bitRate = 25000000UL,
        .priority = SPI_XFER_PKT_PRIO_MAX - 1,
        .dataPktIdxList = SPI_DataPktIdxList,
        .dataPktIdxListSz = (sizeof(SPI_DataPktIdxList) / sizeof(SPI_DataPktIDType)),
        .cb = SPI_XferPkt0EndCb,
    },
    {
        .id = 1U,
        .hwID = 1U,
        .frameFormat = SPI_FRM_FORMAT_MOTOROLA,
        .bitRate = 2000000UL,
        .priority = SPI_XFER_PKT_PRIO_MAX - 2 ,
        .dataPktIdxList = SPI_DataPktIdxList,
        .dataPktIdxListSz = (sizeof(SPI_DataPktIdxList) / sizeof(SPI_DataPktIDType)),
        .cb = SPI_XferPkt1EndCb,
    },
    {
        .id = 2U,
        .hwID = 1U,
        .frameFormat = SPI_FRM_FORMAT_MOTOROLA,
        .bitRate = 4000000UL,
        .priority = SPI_XFER_PKT_PRIO_MAX - 3,
        .dataPktIdxList = SPI_DataPktIdxList,
        .dataPktIdxListSz = (sizeof(SPI_DataPktIdxList) / sizeof(SPI_DataPktIDType)),
        .cb = SPI_XferPkt2EndCb,
    },
};

const SPI_XferPktType SPI_XferPktList1[] = {
    {
        .id = 3U,
        .hwID = 1U,
        .frameFormat = SPI_FRM_FORMAT_MOTOROLA,
        .bitRate = 100000UL,
        .priority = SPI_XFER_PKT_PRIO_MAX,
        .dataPktIdxList = SPI_DataPktIdxList,
        .dataPktIdxListSz = (sizeof(SPI_DataPktIdxList) / sizeof(SPI_DataPktIDType)),
        .cb = SPI_XferPkt3EndCb,
    },
};

const SPI_SeqType SPI_SeqList[] = {
    {
        .id = 0U,
        .xferPktList = SPI_XferPktList0,
        .xferPktListSz = (sizeof(SPI_XferPktList0) / sizeof(SPI_XferPktType)),
        .isInterruptible = TRUE,
        .cb = SPI_Seq0EndCb,
    },
    {
        .id = 1U,
        .xferPktList = SPI_XferPktList1,
        .xferPktListSz = (sizeof(SPI_XferPktList1) / sizeof(SPI_XferPktType)),
        .isInterruptible = FALSE,
        .cb = SPI_Seq1EndCb,
    },
};
#endif /* ENABLE_HIPC */

const SPI_ConfigType SPI_Config = {
    .seqList = SPI_SeqList,
    .seqListSz = (sizeof(SPI_SeqList) / sizeof(SPI_SeqType)),
    .isSeqInterruptible = TRUE,
};

TASK(SPIIT_Task)
{
    int32_t retVal;
#ifndef ENABLE_HIPC
    uint32_t i;
    SPI_SeqStatusType seqStatus;
    BCM_EventMaskType mask;
    uint32_t Seq0Done = FALSE, Seq1Done = FALSE;
#endif  /* ENABLE_HIPC */

    SPI_Init();

#ifdef ENABLE_HIPC
    retVal = SPI_IpcXfer();
#else  /* ENABLE_HIPC */
    for (i = 0UL; i < 25; i++) {
        srcBuf[i] = i;
    }
    for (i = 0UL; i < SPI_DATA_PKTS_NUM; i++) {
        retVal = SPI_SetupBuf(i, srcBuf, destBuf, 10UL);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("SPI_SetupBuf failed %d\n", retVal);
            goto err;
        }
    }
    retVal = SPI_SyncXfer(0U);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("SPI_SeqSyncXfer failed %d\n", retVal);
        goto err;
    }
    BCM_MemSet(destBuf, 0U, sizeof(destBuf));
    retVal = SPI_AsyncXfer(0U);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("SPI_SeqAsyncXfer failed %d\n", retVal);
        goto err;
    }
    seqStatus = SPI_GetSeqStatus(0U);
    if ((SPI_SEQ_STATUS_OK != seqStatus) && (SPI_SEQ_STATUS_PENDING != seqStatus)) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    retVal = SPI_AsyncXfer(1U);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("SPI_SeqAsyncXfer failed %d\n", retVal);
        goto err;
    }
    seqStatus = SPI_GetSeqStatus(1U);
    if ((SPI_SEQ_STATUS_OK != seqStatus) && (SPI_SEQ_STATUS_PENDING != seqStatus)) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    SPI_Cancel(0U);
    seqStatus = SPI_GetSeqStatus(0U);
    if (SPI_SEQ_STATUS_CANCELLED != seqStatus) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    while((FALSE == Seq0Done) || (FALSE == Seq1Done)) {
        BCM_WaitEvent(SEQ_EVENTS);
        BCM_GetEvent(SPIIT_Task, &mask);
        if (mask & SEQ0_EVENT) {
            BCM_ClearEvent(SEQ0_EVENT);
            Seq0Done = TRUE;
        }
        if (mask & SEQ1_EVENT) {
            BCM_ClearEvent(SEQ1_EVENT);
            Seq1Done = TRUE;
        }
    }
    SPIIT1_Result = BCM_ERR_OK;

err:
#endif  /* ENABLE_HIPC */
    if (BCM_ERR_OK != retVal) {
        SPIIT1_Result = retVal;
    }
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex) {
    case SPI_IT1:
        if (BCM_AT_NOT_STARTED == SPIIT1_Result) {
            SPIIT1_Result = BCM_AT_EXECUTING;
            BCM_ActivateTask(SPIIT_Task);
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch(aIndex) {
    case SPI_IT1:
        retVal = SPIIT1_Result;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return retVal;
}

void ApplicationInit()
{
}
