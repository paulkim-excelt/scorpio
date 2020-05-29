/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
#include <string.h>
#include <ulog.h>
#include <bcm_err.h>
#include <mcu.h>
#include <clk.h>
#include <bcm_test.h>
#include <flash.h>
#include <atomic.h>
#include <utils.h>
#include <board.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel() ULOG_LVL_ERROR

/* Test IDs */
#define FLASH_CTRL0_IT1     (1UL)
#define FLASH_CTRL1_IT2     (2UL)

#define FLASHIT_DONE_IND    (0UL)
#define FLASHIT_ERR_IND     (1UL)

/* Macros for error reporting */
#define ATOMIC_CHECK_UPDATE(status, ret)                    \
{                                                           \
    if (BCM_AT_EXECUTING == (*status)) {                    \
        (*status) = ((int32_t)(ret));                       \
    }                                                       \
}
#define MARK_ERR(status, ret, line)                         \
{                                                           \
    ULOG_ERR("\nError: %d \t line %d", ret ,line);          \
    ATOMIC_CHECK_UPDATE((status), (ret))                    \
}
#define CHECK_ERR(status, ret, line)                        \
    if (BCM_ERR_OK != (ret))                                \
    {                                                       \
        MARK_ERR((status), (ret), (line));                  \
        goto err;                                           \
    }
#define CHECK_OPS_ERR(status, ind, ret, line)                                \
    if ((FLASH_OPS_RESULT_OK != (ret)) && (FLASHIT_DONE_IND != (ind)))       \
    {                                                                        \
        ULOG_ERR("\nOpsError: %ld \t Ind %lu \t line %ld", ret, ind, line);  \
        ATOMIC_CHECK_UPDATE((status), (ret));                                \
        goto err;                                                            \
    }

/* Test Status */
static int32_t FlashIT1_Status = BCM_AT_NOT_STARTED;
static int32_t FlashIT2_Status = BCM_AT_NOT_STARTED;

/* Flash page size */
#define PAGE_SIZE           (256UL)
/* Section page size */
#define SECTOR_SIZE         (64UL * 1024UL)

#define FLASHIT_TESTADDR        (1024UL * 1024UL)
#define FLASHIT_TEST_PAGES      (4UL)

static const MCU_ClkCfgIDType flash_clk_tbl[] = {
#ifdef ENABLE_FLASH0
    FLASH0_CLK_CFG_ID,
#endif /* ENABLE_FLASH0 */
#if (FLASH_MAX_HW_ID > 1)
#if ENABLE_FLASH1
    FLASH1_CLK_CFG_ID,
#endif /* ENABLE_FLASH1 */
#endif /* FLASH_MAX_HW_ID > 1 */
};

const MCU_ClkCfgIDType *flash_clk_table = flash_clk_tbl;
const uint32_t flash_clk_table_sz = (sizeof(flash_clk_tbl) /
                                    sizeof(MCU_ClkCfgIDType));

#ifdef ENABLE_FLASH0
/* Flashable (Erasable/Writable) sector list for Flash 0 */
#define FLASH0_FLSABLE_SEC_LIST0_START      (FLASH0_FLSABLE_SEC_START_ADDR)
#define FLASH0_FLSABLE_SEC_LIST0_SECTORS    (3UL)
static const FLASH_FlsableSecType fls0FlsableSecList[] = {
    {FLASH0_FLSABLE_SEC_LIST0_START, FLASH0_FLSABLE_SEC_LIST0_SECTORS},
};
#endif /* ENABLE_FLASH0 */

#if (FLASH_MAX_HW_ID > 1)
#ifdef ENABLE_FLASH1
/* Flashable (Erasable/Writable) sector list for Flash 1 */
#define FLASH1_FLSABLE_SEC_LIST0_START      (FLASH1_FLSABLE_SEC_START_ADDR)
#define FLASH1_FLSABLE_SEC_LIST0_SECTORS    (3UL)
static const FLASH_FlsableSecType fls1FlsableSecList[] = {
    {FLASH1_FLSABLE_SEC_LIST0_START, FLASH1_FLSABLE_SEC_LIST0_SECTORS},
};
#endif /* FLASH_MAX_HW_ID > 1 */
#endif /* ENABLE_FLASH1 */


/* Flash Configurations */
static FLASH_CfgType FlashConfig[FLASH_MAX_HW_ID] = {
#ifdef ENABLE_FLASH0
    {
        .size = FLASH0_SIZE,
        .pageSize = PAGE_SIZE,
        .sectorSize = SECTOR_SIZE,
        .SPIMode = FLASH_SPI_MODE_3,
        .speed = FLASH0_SPEED,
        .readLane = FLASH0_READ_LANE_CFG,
        .flsableSecListSize = (sizeof(fls0FlsableSecList)/
                                sizeof(FLASH_FlsableSecType)),
        .flsableSecList = fls0FlsableSecList,
        .flsID = FLASH0_FLASH_ID,
    },
#else
    {0},
#endif
#if (FLASH_MAX_HW_ID > 1)
#ifdef ENABLE_FLASH1
    {
        .size = FLASH1_SIZE,
        .pageSize = PAGE_SIZE,
        .sectorSize = SECTOR_SIZE,
        .SPIMode = FLASH_SPI_MODE_3,
        .speed = FLASH1_SPEED,
        .readLane = FLASH1_READ_LANE_CFG,
        .flsableSecListSize = (sizeof(fls1FlsableSecList)/
                                sizeof(FLASH_FlsableSecType)),
        .flsableSecList = fls1FlsableSecList,
        .flsID = FLASH1_FLASH_ID,
    },
#else
    {0},
#endif
#endif

#if (FLASH_MAX_HW_ID > 2)
#error "FLASH_MAX_HW_ID > 2 not supported"
#endif
};

/* Buffers */
static uint8_t COMP_SECTION(".dma.bcm.test") FlashIT_TxBuf[PAGE_SIZE];
static uint8_t COMP_SECTION(".dma.bcm.test") FlashIT_RxBuf[FLASHIT_TEST_PAGES * PAGE_SIZE];
static uint32_t COMP_SECTION(".dma.bcm.test") FlashIT_DevID;

/* Flash OpsDoneInd */
void FLASH_OpsDoneInd(FLASH_HwIDType aID)
{
    if (FLASH_HW_ID_0 == aID) {
        (void)BCM_SetEvent(FlashIT1_Task, FlashIT_DoneEvent);
    } else {
        (void)BCM_SetEvent(FlashIT2_Task, FlashIT_DoneEvent);
    }
}

/* Flash OpsErrInd */
void FLASH_OpsErrInd(FLASH_HwIDType aID)
{
    if (FLASH_HW_ID_0 == aID) {
        (void)BCM_SetEvent(FlashIT1_Task, FlashIT_ErrEvent);
    } else {
        (void)BCM_SetEvent(FlashIT2_Task, FlashIT_ErrEvent);
    }
}

/* Helper function to wait, get and clear an event */
static uint32_t WaitGetClearEvent(TaskType aTaskID)
{
    uint32_t retVal;
    uint32_t loop = 1UL;
    BCM_EventMaskType mask = 0UL;
    while (1UL == loop) {
        (void)BCM_WaitEvent(FlashIT_DoneEvent | FlashIT_ErrEvent);
        (void)BCM_GetEvent(aTaskID, &mask);
        if ((mask & FlashIT_DoneEvent) != 0UL) {
            (void)BCM_ClearEvent(FlashIT_DoneEvent);
            retVal = FLASHIT_DONE_IND;
            loop = 0UL;
        } else if ((mask & FlashIT_ErrEvent) != 0UL) {
            (void)BCM_ClearEvent(FlashIT_ErrEvent);
            retVal = FLASHIT_ERR_IND;
            loop = 0UL;
        } else {
            loop = 1UL;
        }
    }
    return retVal;
}

/* Test the flash */
static void FlashTest(FLASH_HwIDType aID, int32_t * const aStatus, TaskType aTaskID)
{
    int32_t retVal;
    uint32_t errInd;
    FLASH_StatusType flashStatus;
    FLASH_OpsResultType opsRes;

    ULOG_ERR("FlashTest (aID: %d): START\n", aID);
    /* Initialize Flash */
    FLASH_Init(aID, &FlashConfig[aID]);

    /* Verfiy current flash status as FLASH_STATUS_IDLE */
    retVal = FLASH_GetStatus(aID, &flashStatus);
    CHECK_ERR(aStatus, retVal, __LINE__);
    if (FLASH_STATUS_IDLE != flashStatus) {
        ATOMIC_CHECK_UPDATE((aStatus), (BCM_ERR_UNKNOWN));
        ULOG_ERR("\nError: Flash controller not IDLE %d", __LINE__);
        goto err;
    }

    /* Configure Read lane */
    retVal = FLASH_CfgReadLane(aID);
    if (BCM_ERR_OK == retVal) {
        errInd = WaitGetClearEvent(aTaskID);
        retVal = FLASH_GetOpsResult(aID, &opsRes);
        CHECK_ERR(aStatus, retVal, __LINE__);
        CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);
    }

    /* Read device ID */
    retVal = FLASH_ReadID(aID, &FlashIT_DevID);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);

    /* Read Id using FLASH_RegReadWrite API */
    BCM_MemSet(FlashIT_TxBuf, 0x0, 4UL);
    BCM_MemSet(FlashIT_RxBuf, 0x0, 4UL);
    FlashIT_TxBuf[0] = (uint8_t)0x9F;
    retVal = FLASH_RegReadWrite(aID, FlashIT_RxBuf, FlashIT_TxBuf, 4UL);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);
    if (FlashIT_DevID != (((uint32_t)FlashIT_RxBuf[1] << 16UL)
                | ((uint32_t)FlashIT_RxBuf[2] << 8UL)
                | (uint32_t)FlashIT_RxBuf[3])) {
        MARK_ERR(aStatus, BCM_ERR_UNKNOWN, __LINE__);
        goto err;
    }

    /* Erase 1 sector of flash */
    retVal = FLASH_Erase(aID, FLASHIT_TESTADDR, SECTOR_SIZE);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);

    /* do a blank check of the erased sector */
    retVal = FLASH_BlankCheck(aID, FLASHIT_TESTADDR, SECTOR_SIZE);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);

    /* write a known pattern to one page of the flash */
    BCM_MemSet(FlashIT_TxBuf, 0xA5, sizeof(FlashIT_TxBuf));
    retVal = FLASH_Write(aID, FLASHIT_TESTADDR, FlashIT_TxBuf, PAGE_SIZE);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);

    /* Read back the updated page and verity the write */
    retVal = FLASH_Read(aID, FLASHIT_TESTADDR, FlashIT_RxBuf, PAGE_SIZE);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);

    if (0 != memcmp(FlashIT_TxBuf, FlashIT_RxBuf, PAGE_SIZE)) {
        MARK_ERR(aStatus, BCM_ERR_UNKNOWN, __LINE__);
        goto err;
    }

    /* Use FLASH_Compare to verify the presvious write operation */
    retVal = FLASH_Compare(aID, FLASHIT_TESTADDR, FlashIT_TxBuf,  PAGE_SIZE);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);

    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    CHECK_OPS_ERR(aStatus, errInd, opsRes, __LINE__);

    /* Read 3 sectors from the flash */
    retVal = FLASH_Read(aID, FLASHIT_TESTADDR, FlashIT_RxBuf,
            (FLASHIT_TEST_PAGES * PAGE_SIZE));
    CHECK_ERR(aStatus, retVal, __LINE__);

    /* Cancel the last read operation */
    retVal = FLASH_Cancel(aID);
    CHECK_ERR(aStatus, retVal, __LINE__);

    errInd = WaitGetClearEvent(aTaskID);
    if ( errInd != FLASHIT_ERR_IND) {
        ULOG_ERR("\nErrorInd Not Received: %\t line %ld", opsRes, __LINE__);
        ATOMIC_CHECK_UPDATE((aStatus), (errInd));
    }

    /* verify that FLASH_OPS_RESULT_CANCELLED is received */
    retVal = FLASH_GetOpsResult(aID, &opsRes);
    CHECK_ERR(aStatus, retVal, __LINE__);
    if (FLASH_OPS_RESULT_CANCELLED != opsRes) {
        ULOG_ERR("\nOpsError: %ld \t line %ld", opsRes, __LINE__);
        ATOMIC_CHECK_UPDATE((aStatus), (opsRes));
        goto err;
    }

err:
    ULOG_ERR("FlashTest (aID: %d): END\n", aID);
    FLASH_DeInit(aID);
    return;
}

/* Task for FLASH_CTRL0_IT1 */
TASK(FlashIT1_Task)
{
    FlashTest(FLASH_HW_ID_0, &FlashIT1_Status, FlashIT1_Task);
    if(BCM_AT_EXECUTING == FlashIT1_Status) {
        FlashIT1_Status = BCM_ERR_OK;
    }
    (void)BCM_TerminateTask();
}

/* Task for FLASH_CTRL1_IT2 */
TASK(FlashIT2_Task)
{
    FlashTest(FLASH_HW_ID_1, &FlashIT2_Status, FlashIT2_Task);
    if(BCM_AT_EXECUTING == FlashIT2_Status) {
        FlashIT2_Status = BCM_ERR_OK;
    }
    (void)BCM_TerminateTask();
}

/* Start Test */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex)
    {
#ifdef ENABLE_FLASH0
        case FLASH_CTRL0_IT1:
            FlashIT1_Status = BCM_AT_EXECUTING;
            (void)BCM_ActivateTask(FlashIT1_Task);
            break;
#endif
#if (FLASH_MAX_HW_ID > 1)
#ifdef ENABLE_FLASH1
        case FLASH_CTRL1_IT2:
            FlashIT2_Status = BCM_AT_EXECUTING;
            (void)BCM_ActivateTask(FlashIT2_Task);
            break;
#endif
#endif
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

/* Get Test results */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex)
    {
#ifdef ENABLE_FLASH0
    case FLASH_CTRL0_IT1:
        retVal = FlashIT1_Status;
        break;
#endif
#if (FLASH_MAX_HW_ID > 1)
#ifdef ENABLE_FLASH1
    case FLASH_CTRL1_IT2:
        retVal = FlashIT2_Status;
        break;
#endif
#endif
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

void ApplicationInit(void)
{
    /* Dummy Init */
}
