/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
    @defgroup grp_flashdrv_qspi_impl QSPI Driver
    @ingroup grp_flashdrv

    @addtogroup grp_flashdrv_qspi_impl
    @{
    @file qspi.c
    @brief Flash driver's QSPI driver implementation.
    This source file implements QSPI driver implmentation for flash driver.

*/
#include <string.h>
#include <bcm_err.h>
#include <utils.h>
#include <system.h>
#include <flash_osil.h>
#include <qspi_rdb.h>

/**
    @name Design IDs
    @{
    @brief QSPI internal functions IDs for error reporting
*/
#define BRCM_SWDSGN_FLASH_QSPI_CMD_STATE_TYPE       ()      /**< @brief #FLASH_QSPICmdStateType */
#define BRCM_SWDSGN_FLASH_QSPI_CMDINFO_TYPE         ()      /**< @brief #FLASH_QSPICmdInfoType  */
#define BRCM_SWDSGN_FLASH_QSPI_RWDEV_TYPE           ()      /**< @brief #FLASH_QSPIDevRWType    */
#define BRCM_SWDSGN_FLASH_QSPI_REGS_GLOBAL          ()      /**< @brief #FLASH_QSPIRegs         */
#define BRCM_SWDSGN_FLASH_QSPI_DEVRW_GLOBAL         ()      /**< @brief #FLASH_QSPIDevRW        */
#define BRCM_SWDSGN_FLASH_QSPI_TIMEOUT_COUNT_MACRO  ()      /**< @brief #FLASH_QSPI_TIMEOUT_COUNT*/
#define BRCM_SWDSGN_FLASH_QSPI_REPORTERROR_PROC      (0xD0U) /**< @brief #FLASH_QSPIReportError  */
#define BRCM_SWDSGN_FLASH_QSPI_POLLONBIT_PROC        (0xD1U) /**< @brief #FLASH_QSPIPollOnBit    */
#define BRCM_SWDSGN_FLASH_QSPI_READSTART_PROC        (0xD2U) /**< @brief #FLASH_QSPIReadStart */
#define BRCM_SWDSGN_FLASH_QSPI_READDATA_PROC         (0xD3U) /**< @brief #FLASH_QSPIReadData */
#define BRCM_SWDSGN_FLASH_QSPI_READSTOP_PROC         (0xD4U) /**< @brief #FLASH_QSPIReadStop */
#define BRCM_SWDSGN_FLASH_QSPI_XFERSTART_PROC        (0xD5U) /**< @brief #FLASH_QSPIXferStart */
#define BRCM_SWDSGN_FLASH_QSPI_XFERREADDATA_PROC     (0xD6U) /**< @brief #FLASH_QSPIXferReadData */
#define BRCM_SWDSGN_FLASH_QSPI_XFERHALT_PROC         (0xD7U) /**< @brief #FLASH_QSPIXferHalt */
#define BRCM_SWDSGN_FLASH_QSPI_XFERSTOP_PROC         (0xD8U) /**< @brief #FLASH_QSPIXferStop */
#define BRCM_SWDSGN_FLASH_QSPI_XFERSTATEHANDLER_PROC (0xD9U) /**< @brief #FLASH_QSPIXferStateHandler */
#define BRCM_SWDSGN_FLASH_QSPI_READSTATEHANDLER_PROC (0xDAU) /**< @brief #FLASH_QSPIReadStateHandler */
#define BRCM_SWDSGN_FLASH_QSPI_STATEHANDLER_INT_PROC (0xDBU) /**< @brief #FLASH_QSPIStateHandlerInternal */
#define BRCM_SWDSGN_FLASH_QSPI_INITINTERNAL_PROC     (0xDCU) /**< @brief #FLASH_QSPIInitInternal */
#define BRCM_SWDSGN_FLASH_QSPI_CFGRDLANE_INT_PROC    (0xDDU) /**< @brief #FLASH_QSPICfgReadLaneInternal */
#define BRCM_SWDSGN_FLASH_QSPI_XFERINTERNAL_PROC     (0xDEU) /**< @brief #FLASH_QSPIXferInternal */
#define BRCM_SWDSGN_FLASH_QSPI_READINTERNAL_PROC     (0xDFU) /**< @brief #FLASH_QSPIReadInternal */
#define BRCM_SWDSGN_FLASH_QSPI_STATEHANDLER_PROC     (0xE0U) /**< @brief #FLASH_QSPIStateHandler */
#define BRCM_SWDSGN_FLASH_QSPI_IRQHANDLER_PROC       (0xE1U) /**< @brief #FLASH_BusIRQHandler */
#define BRCM_SWDSGN_FLASH_QSPI_ISBUSY_PROC           (0xE2U) /**< @brief #FLASH_IsQSPIBusy */
#define BRCM_SWDSGN_FLASH_SYSCMDHANDLER_PROC         (0xE3U) /**< @brief #FLASH_SysCmdHandler */
#define BRCM_SWDSGN_FLASH_SVCIO_TYPE                 (0xE4U) /**< @brief #FLASH_SVCIOType */
/** @} */

/**
    @{
    @brief QSPI command state type
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
typedef uint32_t FLASH_QSPICmdStateType;
#define FLASH_QSPI_CMD_STATE_START      (0UL)   /**< @brief QSPI command state start */
#define FLASH_QSPI_CMD_STATE_READ_DATA  (1UL)   /**< @brief QSPI command state read data */
#define FLASH_QSPI_CMD_STATE_STOP       (2UL)   /**< @brief QSPI command state stop */
#define FLASH_QSPI_CMD_STATE_EXIT       (3UL)   /**< @brief QSPI command state exit */
/** @} */

/**
    QSPI command information structure
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
typedef struct _FLASH_QSPICmdInfoType {
    const uint8_t *     cmdTxData;      /**< @brief Pointer to transmit data buffer passed during QSPI command call */
    uint8_t *           cmdRxData;      /**< @brief Pointer to receive data buffer passed during QSPI command call */
    uint32_t            cmdAddr;        /**< @brief Flash address passed during QSPI command call */
    uint32_t            cmdLen;         /**< @brief Length passed during QSPI command call */
    FLASH_ChipSelectAssertType   cmdFlag;        /**< @brief Xfer flag passed during QSPI command call */
    const uint8_t *     tmpTxData;      /**< @brief Pointer to transmit data buffer during command processing */
    uint8_t *           tmpRxData;      /**< @brief Pointer to receive data buffer during command processing */
    uint32_t            tmpAddr;        /**< @brief Flash address during command processing */
    uint32_t            tmpTxLen;       /**< @brief Transmit length during command processing */
    uint32_t            tmpTxLenRem;    /**< @brief Transmit length remaining during command processing */
    uint32_t            tmpTxRdIdx;     /**< @brief Transmit read index during command processing */
    uint32_t            tmpTxWdIdx;     /**< @brief Transmit write index during command processing */
    uint32_t            tmpRxLen;       /**< @brief Transmit length during command processing */
    uint32_t            tmpRxLenRem;    /**< @brief Transmit length remaining during command processing */
    uint32_t            tmpRxRdIdx;     /**< @brief Transmit read index during command processing */
    uint32_t            tmpRxWrIdx;     /**< @brief Transmit write index during command processing */
    FLASH_ChipSelectAssertType   tmpFlag;        /**< @brief Xfer flag passed during command processing */
} FLASH_QSPICmdInfoType;

/**
    QSPI read-write device type
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
typedef struct _FLASH_QSPIDevRWType {
    FLASH_BusCmdType    currCmd;        /**< @brief command being processed */
    FLASH_QSPICmdStateType   currCmdState;   /**< @brief Current state of current command */
    FLASH_QSPICmdInfoType    cmdInfo;        /**< @brief current command information */
    uint32_t            irqCnt;     /**< @brief Interrupt count */
} FLASH_QSPIDevRWType;

/**
    @brief QSPI register structure global variable
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static QSPI_RDBType * const FLASH_QSPIRegs[QSPI_MAX_HW_ID] =
{
#if (QSPI_MAX_HW_ID == 0)
#error "QSPI_MAX_HW_ID == 0"
#endif
    (QSPI_RDBType *)QSPI0_BASE,
#if (QSPI_MAX_HW_ID > 1)
    (QSPI_RDBType *)QSPI1_BASE,
#endif
#if (QSPI_MAX_HW_ID > 2)
#error "QSPI_MAX_HW_ID > 2 is not supported by the driver"
#endif
};

/**
    @brief QSPI read-write device global variable
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static FLASH_QSPIDevRWType FLASH_QSPIDevRW[QSPI_MAX_HW_ID] COMP_SECTION(".data.drivers") =
    {
        #if (QSPI_MAX_HW_ID == 0)
        #error "QSPI_MAX_HW_ID == 0"
        #endif
        {
            .currCmd = FLASH_BUS_CMD_INVALID,
            .irqCnt = 0UL,
        },
        #if (QSPI_MAX_HW_ID > 1)
        {
            .currCmd = FLASH_BUS_CMD_INVALID,
            .irqCnt = 0UL,
        },
        #endif
        #if (QSPI_MAX_HW_ID > 2)
        #error "QSPI_MAX_HW_ID > 2 is not supported by the driver"
        #endif
    };

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_QSPI_TIMEOUT_COUNT    (200UL)   /**< @brief QSPI timeout count */

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static void FLASH_QSPIReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_QSP_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIPollOnBit(volatile uint32_t * const aReg,
                            uint32_t aMask, uint32_t aIsSet)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i, regVal;

    for (i = 0UL; i < FLASH_QSPI_TIMEOUT_COUNT; i++) {
        regVal = (*aReg & aMask);
        if ((uint32_t)((TRUE == aIsSet) ? (regVal != 0UL) : (regVal == 0UL))
                == 1UL) {
            break;
        }
    }

    if (FLASH_QSPI_TIMEOUT_COUNT == i) {
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIReadStart(FLASH_HwIDType aID)
{
    int32_t retVal;
    uint32_t addr = FLASH_QSPIDevRW[aID].cmdInfo.cmdAddr;
    uint32_t rdlen = FLASH_QSPIDevRW[aID].cmdInfo.cmdLen;

    /* Check for RAF Busy */
    if ((FLASH_QSPIRegs[aID]->raf_status & QSPI_RAF_STATUS_SESSION_BUSY_MASK) != 0UL) {
        retVal = BCM_ERR_BUSY;
        FLASH_QSPIReportError((uint8_t)aID, BRCM_SWDSGN_FLASH_QSPI_READSTART_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
        goto err;
    }

    /* Identify startAddr which is multiple of 4UL, aligned
     * to QSPI_RAF_WORD_LEN
     */
    if ((addr & 0x3UL) == 0UL) {
        FLASH_QSPIDevRW[aID].cmdInfo.tmpAddr = addr;
        FLASH_QSPIDevRW[aID].cmdInfo.tmpRxRdIdx = 0UL;
    } else {
        FLASH_QSPIDevRW[aID].cmdInfo.tmpAddr = (addr & (~0x3UL));
        FLASH_QSPIDevRW[aID].cmdInfo.tmpRxRdIdx = (addr & 0x3UL);
    }

    /* Set RAF start address */
    FLASH_QSPIRegs[aID]->raf_start_addr = FLASH_QSPIDevRW[aID].cmdInfo.tmpAddr;

    /* Remaining length to be read */
    FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLenRem = rdlen;

    /* End address */
    addr += rdlen;

    /* Identify read length which is multiple of 4, aligned
     * to QSPI_RAF_WORD_LEN
     */
    if ((addr & 0x3UL) == 0UL) {
        FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLen =
                        (addr - FLASH_QSPIDevRW[aID].cmdInfo.tmpAddr);
    } else {
        FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLen = (((addr & (~0x3UL)) + QSPI_RAF_WORD_LEN)
                                            - FLASH_QSPIDevRW[aID].cmdInfo.tmpAddr);
    }

    FLASH_QSPIRegs[aID]->raf_num_words =
                    QSPI_RAF_LEN_TO_WORDS(FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLen);

    /* Set WaterMark to FIFO full */
    FLASH_QSPIRegs[aID]->raf_watermark = (QSPI_RAF_WATERMARK_64_WORDS
                                    & QSPI_RAF_WATERMARK_FULLNESS_WATERMARK_MASK);

    /* Flash RAF buffer */
    FLASH_QSPIRegs[aID]->raf_ctrl = QSPI_RAF_CTRL_CLEAR_MASK;

    /* Start RAF read session */
    FLASH_QSPIRegs[aID]->raf_ctrl = QSPI_RAF_CTRL_START_MASK;

    /* Wait till start bit is cleared by Hardware */
    retVal = FLASH_QSPIPollOnBit(&(FLASH_QSPIRegs[aID]->raf_ctrl),
                            QSPI_RAF_CTRL_START_MASK, FALSE);

    if (BCM_ERR_TIME_OUT == retVal) {
        FLASH_QSPIReportError((uint8_t)aID, BRCM_SWDSGN_FLASH_QSPI_READSTART_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIReadData(FLASH_HwIDType aID)
{
    uint32_t status;
    uint32_t isFifoEmpty;
    uint32_t fifoData;
    uint32_t rdCnt = 0UL;
    uint8_t * rxData = (FLASH_QSPIDevRW[aID].cmdInfo.cmdRxData);
    uint32_t * rxRdIdx = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpRxRdIdx);
    uint32_t * rxWrIdx = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpRxWrIdx);
    uint32_t * rxLenRem = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLenRem);

    do {
        status = FLASH_QSPIRegs[aID]->raf_status;
        isFifoEmpty = ((status & QSPI_RAF_STATUS_FIFO_EMPTY_MASK)
                        >> QSPI_RAF_STATUS_FIFO_EMPTY_SHIFT);
        if (isFifoEmpty == 0UL) {
            /* FIFO not empty */
            fifoData = FLASH_QSPIRegs[aID]->raf_read_data;

            /* Read for unaligned address */
            if (0UL != *rxRdIdx) {
                while ((*rxRdIdx) < QSPI_RAF_WORD_LEN) {
                    rxData[(*rxWrIdx)] =
                        (uint8_t)((fifoData >> (8UL * (*rxRdIdx))) & 0xFFUL);
                    (*rxWrIdx)++;
                    (*rxRdIdx)++;
                    (*rxLenRem)--;
                    rdCnt++;
                }
               *rxRdIdx = 0UL;
            } else {
                if (*rxLenRem >= QSPI_RAF_WORD_LEN) {
                    /* Read for aligned address and length */
                    *((uint32_t *)(&rxData[(*rxWrIdx)])) = fifoData;
                    (*rxWrIdx) += QSPI_RAF_WORD_LEN;
                    (*rxLenRem)  -= QSPI_RAF_WORD_LEN;
                    rdCnt += QSPI_RAF_WORD_LEN;
                } else {
                    /* Read for unaligned length */
                    while ((*rxLenRem) != 0UL) {
                        rxData[(*rxWrIdx)] = (uint8_t)(fifoData & 0xFFUL);
                        fifoData = fifoData >> 8UL;
                        (*rxWrIdx)++;
                        (*rxLenRem)--;
                        rdCnt++;
                    }
                }
            }
        } else {
            /* No more data in FIFO. Wait till FIFO threshold interrupt
               or Session done interrupt
            */
            break;
        }
        /* Read less than or equal to QSPI_RAF_FIFO_LEN bytes at a time */
    } while (((*rxLenRem) != 0UL) && (rdCnt < QSPI_RAF_FIFO_LEN));

    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIReadStop(FLASH_HwIDType aID)
{
    int32_t retVal;

    /* Stop RAF read session */
    FLASH_QSPIRegs[aID]->raf_ctrl &= (~QSPI_RAF_CTRL_START_MASK);
    FLASH_QSPIRegs[aID]->raf_ctrl |= QSPI_RAF_CTRL_CLEAR_MASK;
    /* Wait till clear bit is cleared by Hardware */
    retVal = FLASH_QSPIPollOnBit(&(FLASH_QSPIRegs[aID]->raf_ctrl),
                            QSPI_RAF_CTRL_CLEAR_MASK, FALSE);
    if (BCM_ERR_TIME_OUT == retVal) {
        FLASH_QSPIReportError((uint8_t)aID, BRCM_SWDSGN_FLASH_QSPI_READSTOP_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_QSPIXferStart(FLASH_HwIDType aID)
{
    uint32_t i;
    const uint8_t * txData = (FLASH_QSPIDevRW[aID].cmdInfo.cmdTxData);
    uint32_t * txLen = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLen);
    uint32_t * txRdIdx = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpTxRdIdx);
    uint32_t * isCont = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpFlag);

    /* Enable MSPI */
    FLASH_QSPIRegs[aID]->bspi_registers_mast_n_boot_ctrl |=
                            QSPI_BMNBC_BSPI_REGISTERS_MAST_N_BOOT_CTRL_MAST_N_BOOT_MASK;

    /* Fill the TX FIFO and cdram registers*/
    for (i = 0UL; (i < *txLen) && (NULL != txData); i++) {
        (&FLASH_QSPIRegs[aID]->mspi_txram00)[(i * 2UL)] = txData[(*txRdIdx)];
        (*txRdIdx)++;
        if (i == (*txLen - 1UL)) {
            if (*isCont != FLASH_CS_ASSERT_STOP) {
                (&FLASH_QSPIRegs[aID]->mspi_cdram00)[i] = QSPI_MSPI_CDRAM_CMD_CONTINUE;

            } else {
                (&FLASH_QSPIRegs[aID]->mspi_cdram00)[i] = 0UL;
            }
        } else {
            (&FLASH_QSPIRegs[aID]->mspi_cdram00)[i] = QSPI_MSPI_CDRAM_CMD_CONTINUE;
        }
    }

    /* Program the Start and end QPs */
    FLASH_QSPIRegs[aID]->mspi_newqp = 0UL;
    FLASH_QSPIRegs[aID]->mspi_endqp = (i - 1UL);

    /* Write lock enable */
    FLASH_QSPIRegs[aID]->mspi_write_lock |= QSPI_MSPI_WRITE_LOCK_WRITELOCK_MASK;

    /* Set chip select assert continue */
    FLASH_QSPIRegs[aID]->mspi_spcr2 = QSPI_MSPI_SPCR2_CONT_AFTER_CMD_MASK
                                | QSPI_MSPI_SPCR2_SPE_MASK;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_QSPIXferReadData(FLASH_HwIDType aID)
{
    uint32_t i;
    uint8_t * rxData = (FLASH_QSPIDevRW[aID].cmdInfo.cmdRxData);
    uint32_t * rxWrIdx = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpRxWrIdx);
    uint32_t * rdlen = &(FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLen);

    if ((FLASH_QSPIRegs[aID]->mspi_mspi_status & QSPI_MSPI_MSPI_STATUS_SPIF_MASK) == 0UL) {
        /* This should never happen */
        FLASH_QSPIReportError((uint8_t)aID, BRCM_SWDSGN_FLASH_QSPI_XFERREADDATA_PROC,
                BCM_ERR_CUSTOM, 0UL, 0UL, 0UL, 0UL);
        goto err;
    }

    /* clear the SPIF bit */
    FLASH_QSPIRegs[aID]->mspi_mspi_status &= (~QSPI_MSPI_MSPI_STATUS_SPIF_MASK);

    /* read the RX FIFO */
    for (i = 0UL; (i < (*rdlen)) && (NULL != rxData); i++) {
        rxData[(*rxWrIdx)] =
                (uint8_t)((&FLASH_QSPIRegs[aID]->mspi_rxram00)[(2UL * i) + 1UL]);
        (*rxWrIdx)++;
    }
err:
    return;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIXferHalt(FLASH_HwIDType aID)
{
    int32_t retVal;

    FLASH_QSPIRegs[aID]->mspi_spcr2 |= QSPI_MSPI_SPCR2_HALT_MASK;
    /* wait for HALT bit to be set */
    retVal = FLASH_QSPIPollOnBit(&(FLASH_QSPIRegs[aID]->mspi_mspi_status),
                            QSPI_MSPI_MSPI_STATUS_HALTA_MASK, TRUE);
    if (BCM_ERR_TIME_OUT == retVal) {
        FLASH_QSPIReportError((uint8_t)aID, BRCM_SWDSGN_FLASH_QSPI_XFERHALT_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    } else {
        /* clear HALTA bit */
        FLASH_QSPIRegs[aID]->mspi_mspi_status &= (~QSPI_MSPI_MSPI_STATUS_HALTA_MASK);
        /* Now its safe to disable the MSPI */
        FLASH_QSPIRegs[aID]->mspi_spcr2 &= (~QSPI_MSPI_SPCR2_SPE_MASK);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_QSPIXferStop(FLASH_HwIDType aID)
{
    if (FLASH_CS_ASSERT_STOP == FLASH_QSPIDevRW[aID].cmdInfo.cmdFlag) {
        FLASH_QSPIRegs[aID]->mspi_write_lock &= (~QSPI_MSPI_WRITE_LOCK_WRITELOCK_MASK);
        FLASH_QSPIRegs[aID]->bspi_registers_mast_n_boot_ctrl &=
                                (~QSPI_BMNBC_BSPI_REGISTERS_MAST_N_BOOT_CTRL_MAST_N_BOOT_MASK);
    }
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIXferStateHandler(FLASH_HwIDType aID,
                                uint32_t * const aIsTrigAgain)
{
    int32_t retVal;

    *aIsTrigAgain = FALSE;

    switch(FLASH_QSPIDevRW[aID].currCmdState) {
    case FLASH_QSPI_CMD_STATE_START: /* case 0: */
        /* Identify xfer length */
        if (QSPI_MSPI_FIFO_LEN >= FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem) {
            FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLen =
                                FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem;
            FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem = 0UL;
        } else {
            FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLen = QSPI_MSPI_FIFO_LEN;
            FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem -= QSPI_MSPI_FIFO_LEN;
        }

        /* Set xfer flag */
        if (FLASH_CS_ASSERT_CONTINUE == FLASH_QSPIDevRW[aID].cmdInfo.cmdFlag) {
            FLASH_QSPIDevRW[aID].cmdInfo.tmpFlag = FLASH_CS_ASSERT_CONTINUE;
        } else {
            if (0UL == FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem) {
                /* Last Xfer */
                FLASH_QSPIDevRW[aID].cmdInfo.tmpFlag = FLASH_CS_ASSERT_STOP;
            } else {
                /* Xfer to continue */
                FLASH_QSPIDevRW[aID].cmdInfo.tmpFlag = FLASH_CS_ASSERT_CONTINUE;
            }
        }

        /* Update state to next */
        FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_READ_DATA;
        FLASH_QSPIXferStart(aID);
        retVal = BCM_ERR_OK;
        break;
    case FLASH_QSPI_CMD_STATE_READ_DATA: /* case 1: */
        if (0UL == FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem) {
            /* Data Transfer complete */
            FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_STOP;
        } else {
            /* Transfer more data */
            FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_START;
        }

        FLASH_QSPIXferReadData(aID);
        *aIsTrigAgain = TRUE;
        retVal = BCM_ERR_OK;
        break;
    case FLASH_QSPI_CMD_STATE_STOP: /* case 2: */
        FLASH_QSPIXferStop(aID);
        FLASH_QSPIDevRW[aID].currCmd = FLASH_BUS_CMD_INVALID;
        retVal = BCM_ERR_OK;
        break;
    default:
        /* Shall never reach here */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIReadStateHandler(FLASH_HwIDType aID,
                                uint32_t * const aIsTrigAgain)
{
    int32_t retVal;

    *aIsTrigAgain = FALSE;

    switch(FLASH_QSPIDevRW[aID].currCmdState) {
    case FLASH_QSPI_CMD_STATE_START: /* case 0: */
        FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_READ_DATA;
        retVal = FLASH_QSPIReadStart(aID);
        break;
    case FLASH_QSPI_CMD_STATE_READ_DATA: /* case 1: */
        retVal = FLASH_QSPIReadData(aID);
        /* Read data complete */
        if ((BCM_ERR_OK == retVal)
            && (0UL == FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLenRem)) {
            FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_STOP;
            *aIsTrigAgain = TRUE;
        }
        break;
    case FLASH_QSPI_CMD_STATE_STOP: /* case 2: */
        retVal = FLASH_QSPIReadStop(aID);
        FLASH_QSPIDevRW[aID].currCmd = FLASH_BUS_CMD_INVALID;
        break;
    default:
        /* Shall never reach here */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIStateHandlerInternal(FLASH_HwIDType aID, uint32_t aIsErr)
{
    int32_t retVal;
    uint32_t isTrigAgain;

    if (TRUE == aIsErr) {
        switch (FLASH_QSPIDevRW[aID].currCmd) {
        case FLASH_BUS_CMD_READ:
            retVal = FLASH_QSPIReadStop(aID);
        break;
        case FLASH_BUS_CMD_XFER:
            retVal = FLASH_QSPIXferHalt(aID);
            FLASH_QSPIXferStop(aID);
        break;
        default:
            /* TODO: Shall never reach here. So, add core abort/crash */
            retVal = BCM_ERR_UNKNOWN;
        break;
        }
        if (BCM_ERR_OK == retVal) {
            FLASH_QSPIDevRW[aID].currCmd = FLASH_BUS_CMD_INVALID;
        }

    } else {
        switch (FLASH_QSPIDevRW[aID].currCmd) {
        case FLASH_BUS_CMD_READ:
            do {
                retVal = FLASH_QSPIReadStateHandler(aID, &isTrigAgain);
            } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));
            break;
        case FLASH_BUS_CMD_XFER:
            do {
                retVal = FLASH_QSPIXferStateHandler(aID, &isTrigAgain);
            } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));
            break;
        default:
            /* TODO: Shall never reach here. So, add core abort/crash */
            retVal = BCM_ERR_UNKNOWN;
            break;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIInitInternal(FLASH_HwIDType aID, const FLASH_CfgType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t regVal;

    /* Configure MSPI speed */
    FLASH_QSPIRegs[aID]->mspi_spcr0_lsb =
                    (64UL & QSPI_MSPI_SPCR0_LSB_SPBR_MASK);

    /* Configure QSPI in master mode */
    regVal = QSPI_MSPI_SPCR0_MSB_MSTR_MASK;

    /* Configure SPI mode */
    switch (aConfig->SPIMode) {
    case FLASH_SPI_MODE_0:
        regVal &= (~QSPI_MSPI_SPCR0_MSB_CPOL_MASK);
        regVal &= (~QSPI_MSPI_SPCR0_MSB_CPHA_MASK);
        break;
    case FLASH_SPI_MODE_1:
        regVal &= (~QSPI_MSPI_SPCR0_MSB_CPOL_MASK);
        regVal |= QSPI_MSPI_SPCR0_MSB_CPHA_MASK;
        break;
    case FLASH_SPI_MODE_2:
        regVal |= QSPI_MSPI_SPCR0_MSB_CPOL_MASK;
        regVal &= (~QSPI_MSPI_SPCR0_MSB_CPHA_MASK);
        break;
    case FLASH_SPI_MODE_3:
        regVal |= QSPI_MSPI_SPCR0_MSB_CPOL_MASK;
        regVal |= QSPI_MSPI_SPCR0_MSB_CPHA_MASK;
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (BCM_ERR_OK == retVal) {
        FLASH_QSPIRegs[aID]->mspi_spcr0_msb = regVal;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPICfgReadLaneInternal(FLASH_HwIDType aID,
                         FLASH_ReadLaneType aReadLane)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t regVal = 0UL;

    /* Enable QSPI read lane */
    switch (aReadLane) {
    case FLASH_READ_LANE_SINGLE:
        /* Clear bspi_registers_STRAP_OVERRIDE_CTRL_data_dual_n_sgl &
         * bspi_registers_STRAP_OVERRIDE_CTRL_data_quad bits
         * Set bspi_registers_STRAP_OVERRIDE_CTRL_override bit*/
        regVal = QSPI_BSOC_BSPI_REGISTERS_STRAP_OVERRIDE_CTRL_OVERRIDE_MASK;
        break;
    case FLASH_READ_LANE_DUAL:
        /* Clear bspi_registers_STRAP_OVERRIDE_CTRL_data_quad bit.
         * Set bspi_registers_STRAP_OVERRIDE_CTRL_data_dual_n_sgl
         * & bspi_registers_STRAP_OVERRIDE_CTRL_override bits */
        regVal = (QSPI_BSOC_BRSOCDDNS_MASK
                | QSPI_BSOC_BSPI_REGISTERS_STRAP_OVERRIDE_CTRL_OVERRIDE_MASK);
        break;
    case FLASH_READ_LANE_QUAD:
        /* Clear bspi_registers_STRAP_OVERRIDE_CTRL_data_dual_n_sgl bit.
         * Set bspi_registers_STRAP_OVERRIDE_CTRL_data_quad &
         * bspi_registers_STRAP_OVERRIDE_CTRL_override bits */
        regVal = (QSPI_BSOC_BSPI_REGISTERS_STRAP_OVERRIDE_CTRL_DATA_QUAD_MASK
                | QSPI_BSOC_BSPI_REGISTERS_STRAP_OVERRIDE_CTRL_OVERRIDE_MASK);
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (BCM_ERR_OK == retVal) {
        FLASH_QSPIRegs[aID]->bspi_registers_strap_override_ctrl = regVal;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_QSPIXferInternal(FLASH_HwIDType aID, const uint8_t * const aTxData,
                uint8_t * const aRxData, uint32_t aLen,
                FLASH_ChipSelectAssertType aFlag)
{
    int32_t retVal;
    uint32_t isTrigAgain = FALSE;

    /* Set command info */
    BCM_MemSet(&(FLASH_QSPIDevRW[aID].cmdInfo), 0x0, sizeof(FLASH_QSPICmdInfoType));
    FLASH_QSPIDevRW[aID].cmdInfo.cmdTxData = aTxData;
    FLASH_QSPIDevRW[aID].cmdInfo.cmdRxData = aRxData;
    FLASH_QSPIDevRW[aID].cmdInfo.cmdLen = aLen;
    FLASH_QSPIDevRW[aID].cmdInfo.cmdFlag = aFlag;
    FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLen = aLen;
    FLASH_QSPIDevRW[aID].cmdInfo.tmpTxLenRem = aLen;
    FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLen = aLen;
    FLASH_QSPIDevRW[aID].cmdInfo.tmpRxLenRem = aLen;
    FLASH_QSPIDevRW[aID].cmdInfo.tmpTxData = aTxData;
    FLASH_QSPIDevRW[aID].cmdInfo.tmpRxData = aRxData;

    /* Set command & command states */
    FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_START;
    FLASH_QSPIDevRW[aID].currCmd = FLASH_BUS_CMD_XFER;

    do {
        retVal = FLASH_QSPIXferStateHandler(aID, &isTrigAgain);
    } while((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static int32_t FLASH_QSPIReadInternal(FLASH_HwIDType aID, uint32_t aAddr,
                uint8_t * const aData, uint32_t aLen)
{
    int32_t retVal;
    uint32_t isTrigAgain;

    /* Set command info */
    BCM_MemSet(&(FLASH_QSPIDevRW[aID].cmdInfo), 0x0, sizeof(FLASH_QSPICmdInfoType));
    FLASH_QSPIDevRW[aID].cmdInfo.cmdRxData = aData;
    FLASH_QSPIDevRW[aID].cmdInfo.cmdAddr = aAddr;
    FLASH_QSPIDevRW[aID].cmdInfo.cmdLen = aLen;

    /* Set command & command states */
    FLASH_QSPIDevRW[aID].currCmdState = FLASH_QSPI_CMD_STATE_START;
    FLASH_QSPIDevRW[aID].currCmd = FLASH_BUS_CMD_READ;

    do {
        retVal = FLASH_QSPIReadStateHandler(aID, &isTrigAgain);
    } while ((TRUE == isTrigAgain) && (retVal == BCM_ERR_OK));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_IsQSPIBusy(FLASH_HwIDType aID)
{
    int32_t retVal;

    if (FLASH_BUS_CMD_INVALID == FLASH_QSPIDevRW[aID].currCmd) {
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_BUSY;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
*/
typedef union _FLASH_SVCIOType {
    uint8_t *data;
    FLASH_IOType *io;
} FLASH_SVCIOType;

/**
    @code{.c}
    if aSysIO.flashIO is NULL
        aSysIO.flashIO->retval = BCM_ERR_INVAL_MAGIC
    else
        aSysIO.flashIO->retval = FLASH_CmdHandler(aCmd, aSysIO.flashIO)
    @endcode

    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
*/
void FLASH_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{

    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    FLASH_SVCIOType flash;
    flash.data = aSysIO;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_QSP_ID) {
            switch (aCmd) {
            case FLASH_BUS_CMD_INIT:
                retVal = FLASH_QSPIInitInternal(flash.io->hwID, flash.io->config);
                break;
            case FLASH_BUS_CMD_READ:
                retVal = FLASH_QSPIReadInternal(flash.io->hwID, flash.io->addr,
                                                flash.io->rxBuf, flash.io->len);
                break;
            case FLASH_BUS_CMD_CFG_READ_LANE:
                retVal = FLASH_QSPICfgReadLaneInternal(flash.io->hwID, flash.io->readLane);
                break;
            case FLASH_BUS_CMD_XFER:
                retVal = FLASH_QSPIXferInternal(flash.io->hwID, flash.io->txBuf,
                                                flash.io->rxBuf, flash.io->len,
                                                flash.io->flag);
                break;
            case FLASH_BUS_CMD_STATEHANDLER:
                retVal = FLASH_QSPIStateHandlerInternal(flash.io->hwID, flash.io->isErr);
                break;
            case FLASH_BUS_CMD_GETSTATUS:
                retVal = FLASH_IsQSPIBusy(flash.io->hwID);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        flash.io->retVal = retVal;
    }
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_QSP_ID, 0xFF, BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC, retVal, 4UL, values);
    }
}

/**
    @trace #BRCM_SWARCH_FLASH_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
*/
void FLASH_BusIRQHandler(FLASH_HwIDType aID)
{
    uint32_t regVal;

    if (FLASH_MAX_HW_ID > aID) {
        /* Clear interrupts */
        regVal = (FLASH_QSPIRegs[aID]->raf_interrupt_lr_fullness_reached
                & QSPI_RILFR_RILFRSLFR_MASK);
        FLASH_QSPIRegs[aID]->raf_interrupt_lr_fullness_reached = regVal;

        regVal = (FLASH_QSPIRegs[aID]->raf_interrupt_lr_session_done & QSPI_RILSD_RILSDSLSD_MASK);
        FLASH_QSPIRegs[aID]->raf_interrupt_lr_session_done = regVal;

        regVal = (FLASH_QSPIRegs[aID]->mspi_interrupt_mspi_done & QSPI_MIMD_MSPI_INTERRUPT_MSPI_DONE_MSPI_DONE_MASK);
        FLASH_QSPIRegs[aID]->mspi_interrupt_mspi_done = regVal;

        /* Increment interrupt count */
        FLASH_QSPIDevRW[aID].irqCnt++;
    }
}
/** @} */
