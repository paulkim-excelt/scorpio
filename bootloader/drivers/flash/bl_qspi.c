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
    @addtogroup grp_bl_flash_impl
    @{
    @file bl_qspi.c
    @brief QSPI driver implementation

    @version 0.30 Imported from docx
*/

#include <string.h>
#include <bl_utils.h>
#include <bl_bcm_err.h>
#include <bl_log.h>
#include <bl_chip_config.h>
#include <bl_flash.h>
#include "bl_qspi.h"

#define BRCM_DSGN_BL_QSPI_REPORT_ERROR_PROC           (0x80U) /**< @brief #BL_QSPI_ReportError */

/* Macros */
#define BL_RAF_WORD_LEN                (4UL)
#define BL_LEN_TO_RAF_WORDS(readLen)   ((readLen) / BL_RAF_WORD_LEN)

#define BL_RAF_WATERMARK_64_WORDS      (0x0UL)
#define BL_RAF_WATERMARK_32_WORDS      (0x1UL)
#define BL_RAF_WATERMARK_16_WORDS      (0x2UL)
#define BL_RAF_WATERMARK_8_WORDS       (0x3UL)

/* Macros of SPBR value */
#define BL_SPBR_VAL_1                  (1UL)
#define BL_SPBR_VAL_2                  (2UL)
#define BL_SPBR_VAL_8                  (8UL)
#define BL_SPBR_VAL_32                 (32UL)

#define BL_QSPI_BSPI_TIMEOUT_COUNT    (200UL)
#define BL_QSPI_MSPI_TIMEOUT_COUNT    (10000UL)

#define BL_GetModuleLogLevel()         (BL_LOG_LVL_INFO)
static BL_QSPI_RegsType *const BL_QSPI_REGS[BL_QSPI_MAX_HW_ID] =
{
    (BL_QSPI_RegsType *)BL_QSPI0_BASE,
#if (BL_QSPI_MAX_HW_ID > 1UL)
    (BL_QSPI_RegsType *)BL_QSPI1_BASE,
#endif
#if (BL_QSPI_MAX_HW_ID > 2UL)
#error "BL_QSPI_MAX_HW_ID > 2 is not suported"
#endif
};

static BL_COMP_NEVER_INLINE void BL_QSPI_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_QSP_ID, aInstanceID, BRCM_DSGN_BL_QSPI_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static int32_t BL_QSPI_Wait(volatile uint32_t *const aReg,
                            uint32_t aMask,
                            uint32_t aIsCompEqZero,
                            uint32_t aTimeOutCnt)
{
    uint32_t i = aTimeOutCnt;
    int32_t retVal = BL_BCM_ERR_OK;

    while (i > 0UL) {
        if (BL_TRUE == aIsCompEqZero) {
            if (((*aReg) & aMask) != 0UL) {
                break;
            }
        } else {
            if (((*aReg) & aMask) == 0UL) {
                break;
            }
        }
        i--;
    }
    if (0UL == i) {
        retVal = BL_BCM_ERR_TIME_OUT;
    }

    return retVal;
}

/** @brief Helper function to stop a RAF read transaction.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID

    @retval     #BL_BCM_ERR_OK         RAF read transcation stopped successfully.
    @retval     #BL_BCM_ERR_TIME_OUT   Time out

    @post None

    @trace  #BRCM_DSGN_BL_FLASH_QSPI_RAF_STOP_PROC  #BRCM_ARCH_BL_FLASH_QSPI_RAF_STOP

    @limitations None

    @code{.c}
    Stop RAF read session from QSPI RAF control register
	Wait till RAF control register’s clear bit is cleared by hardware
    @endcode
*/
static int32_t BL_QSPI_RafStop(BL_QSPI_HwIDType aID)
{
    int32_t retVal;

    /* Stop RAF read session */
    BL_QSPI_REGS[aID]->RAF_CTRL &= (~RAF_CTRL_START_MASK);
    BL_QSPI_REGS[aID]->RAF_CTRL |= RAF_CTRL_CLEAR_MASK;
    /* Wait till clear bit is cleared by Hardware */
    retVal = BL_QSPI_Wait(&(BL_QSPI_REGS[aID]->RAF_CTRL), RAF_CTRL_CLEAR_MASK,
                        BL_FALSE, BL_QSPI_BSPI_TIMEOUT_COUNT);
    if (BL_BCM_ERR_OK != retVal) {
        BL_QSPI_ReportError(retVal, aID, 0UL, 0UL, 0UL, __LINE__);
    }

    return retVal;
}

/** @brief Helper function to perform a RAF read.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aAddr       Flash address
    @param[out]     aData       Pointer to output data buffer
    @param[in]      aLen        Length in bytes

    @retval     #BL_BCM_ERR_OK         RAF read transcation successfully completed
    @retval     #BL_BCM_ERR_BUSY       QSPI RAF is busy
    @retval     #BL_BCM_ERR_TIME_OUT   Time out
    @retval     Others              As returned by #BL_QSPI_RafStop

    @post None

    @trace  #BRCM_DSGN_BL_FLASH_QSPI_RAF_READ_PROC  #BRCM_ARCH_BL_FLASH_QSPI_RAF_READ

    @limitations None

    @image html bl_flash_flowchart_qspi_raf_read.jpg
*/
static int32_t BL_QSPI_RafRead(BL_QSPI_HwIDType aID,
                               uint32_t aAddr,
                               uint8_t *const aData,
                               uint32_t aLen)
{
    uint32_t rafWords;
    uint32_t rafStatus;
    uint32_t fifoEmpty;
    uint32_t fifoData;
    uint32_t rafBusy;
    uint32_t i;
    uint32_t count = 0UL;
    uint32_t remain = aLen;
    int32_t retVal = BL_BCM_ERR_OK;
    uint32_t lineNo = __LINE__;

    /* Check for RAF Busy */
    if ((BL_QSPI_REGS[aID]->RAF_STATUS & RAF_STATUS_SESSION_BUSY_MASK) != 0UL) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_BUSY;
        goto err;
    }

    /* set the RAF start address */
    BL_QSPI_REGS[aID]->RAF_START_ADDR = aAddr;

    rafWords = BL_LEN_TO_RAF_WORDS(aLen);
    if ((aLen & 0x3UL) != 0UL) {
        rafWords += 1UL;
    }
    BL_QSPI_REGS[aID]->RAF_NUM_WORDS = rafWords;

    /* Set WaterMark to FIFO full */
    BL_QSPI_REGS[aID]->RAF_WATERMARK = (BL_RAF_WATERMARK_64_WORDS &
            RAF_WATERMARK_FULLNESS_MASK);

    /* flush RAF buffer */
    BL_QSPI_REGS[aID]->RAF_CTRL = RAF_CTRL_CLEAR_MASK;

    /* start RAF session */
    BL_QSPI_REGS[aID]->RAF_CTRL = RAF_CTRL_START_MASK;

    /* Wait till start bit is cleared by Hardware */
    retVal = BL_QSPI_Wait(&(BL_QSPI_REGS[aID]->RAF_CTRL), RAF_CTRL_START_MASK,
                        BL_FALSE, BL_QSPI_BSPI_TIMEOUT_COUNT);
    if (BL_BCM_ERR_OK != retVal) {
        lineNo = __LINE__;
        goto err;
    }

    /* Read the data from FIFO */
    do {
        rafStatus = BL_QSPI_REGS[aID]->RAF_STATUS;
        fifoEmpty = (rafStatus & RAF_STATUS_FIFO_EMPTY_MASK);
        rafBusy = (rafStatus & RAF_STATUS_SESSION_BUSY_MASK);
        if (fifoEmpty == 0UL) {
            /* FIFO is not empty */
            fifoData = BL_QSPI_REGS[aID]->RAF_READ_DATA;
            if (remain >= BL_RAF_WORD_LEN) {
                *((uint32_t *)(aData + count)) = fifoData;
                count += BL_RAF_WORD_LEN;
                remain -= BL_RAF_WORD_LEN;
            } else {
                for (i = 0UL; i < remain; i++) {
                    aData[count + i] = (uint8_t)fifoData & 0xFFU;
                    fifoData = fifoData >> 8UL;
                }
                remain = 0UL;
                count = count + i;
            }
        } else {
            if (rafBusy == 0UL) {
                break;
            }
        }
    } while (count < aLen);

    retVal = BL_QSPI_RafStop(aID);
    if (BL_BCM_ERR_OK != retVal) {
        lineNo = __LINE__;
    }
err:
    if (BL_BCM_ERR_OK != retVal) {
        BL_QSPI_ReportError(retVal, aID, 0UL, 0UL, 0UL, lineNo);
    }
    return retVal;
}

/** @brief This API is to transfer data using QSPI controller

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aTxData     Pointer to transmit data buffer
    @param[out]     aRxData     Pointer to receive data buffer
    @param[in]      aLen        Length of data buffer
    @param[in]      aFlag       Transfer flag

    @retval     #BL_BCM_ERR_OK             Transfer successfully completed
    @retval     #BL_BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BL_BCM_ERR_UNKNOWN        Unknown error

    @post None

    @trace  #BRCM_DSGN_BL_FLASH_BL_QSPI_XFER_PROC  #BRCM_ARCH_BL_FLASH_QSPI_XFER

    @limitations None

    @image html bl_flash_flowchart_qspi_xfer.jpg
*/
int32_t BL_QSPI_Xfer(BL_QSPI_HwIDType aID,
                     const uint8_t *const aTxData,
                     uint8_t *const aRxData,
                     uint32_t aLen,
                     BL_QSPI_XferFlagType aFlag)
{
    uint32_t i;
    uint32_t j;
    uint32_t tmpTxLen;
    uint32_t tmpIdx = 0UL;
    uint32_t remain = aLen;
    uint32_t lineNo = __LINE__;
    int32_t retVal = BL_BCM_ERR_OK;

    if ((aID >= BL_QSPI_MAX_HW_ID) || (aTxData == NULL)) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (aLen > BL_MSPI_FIFO_LEN) {
        tmpTxLen = BL_MSPI_FIFO_LEN;
    } else {
        tmpTxLen = aLen;
    }

    /* Enable MSPI */
    BL_QSPI_REGS[aID]->BSPI_MAST_N_BOOT_CTRL |= BSPI_MAST_N_BOOT_CTRL_BIT_MASK;

    do {
        for (i = 0UL; i < tmpTxLen; i++) {
            BL_QSPI_REGS[aID]->MSPI_TXRAM[(i * 2UL)] = aTxData[i + tmpIdx];
            if ((remain < BL_MSPI_FIFO_LEN) && (i == (tmpTxLen - 1UL)) &&
                    (aFlag == BL_QSPI_XFER_STOP)) {
                BL_QSPI_REGS[aID]->MSPI_CDRAM[i] = 0UL;
            } else {
                BL_QSPI_REGS[aID]->MSPI_CDRAM[i] = MSPI_CDRAM_CMD_CONTINUE_MASK;
            }
        }
        /* Program the Start and end QPs */
        BL_QSPI_REGS[aID]->MSPI_NEWQP = 0UL;
        BL_QSPI_REGS[aID]->MSPI_ENDQP = (i - 1UL);

        /* Write lock enable */
        BL_QSPI_REGS[aID]->MSPI_WRITE_LOCK |= MSPI_WRITE_LOCK_WRITELOCK_MASK;

        /* Set chip select assert continue */
        BL_QSPI_REGS[aID]->MSPI_SPCR2 = MSPI_SPCR2_CONT_AFTER_CMD_MASK|
            MSPI_SPCR2_SPE_MASK;
        /* wait for MSPI completion */
        retVal = BL_QSPI_Wait(&(BL_QSPI_REGS[aID]->MSPI_STATUS),
                MSPI_STATUS_SPIF_MASK, BL_TRUE, BL_QSPI_MSPI_TIMEOUT_COUNT);
        if (BL_BCM_ERR_OK != retVal) {
            lineNo = __LINE__;
            break;
        }

        /* clear the SPIF bit */
        BL_QSPI_REGS[aID]->MSPI_STATUS &= ~MSPI_STATUS_SPIF_MASK;
        /* read the RX FIFO */
        for (j = 0UL; (j < tmpTxLen) && (NULL != aRxData); j++) {
            aRxData[j + tmpIdx] =
                (uint8_t)(BL_QSPI_REGS[aID]->MSPI_RXRAM[(2UL * j) + 1UL]);
        }
        remain -= tmpTxLen;
        tmpIdx += tmpTxLen;
        if (remain > BL_MSPI_FIFO_LEN) {
            tmpTxLen = BL_MSPI_FIFO_LEN;
        } else {
            tmpTxLen = remain;
        }
    } while (remain > 0UL);

    if (aFlag == BL_QSPI_XFER_STOP) {
        /* Stop the MSPI and disable it */
        BL_QSPI_REGS[aID]->MSPI_WRITE_LOCK &= (~MSPI_WRITE_LOCK_WRITELOCK_MASK);
        BL_QSPI_REGS[aID]->MSPI_SPCR2 &= ~MSPI_SPCR2_SPE_MASK;
        BL_QSPI_REGS[aID]->BSPI_MAST_N_BOOT_CTRL &=
            (~BSPI_MAST_N_BOOT_CTRL_BIT_MASK);
    }
err:
    if (BL_BCM_ERR_OK != retVal) {
        BL_QSPI_ReportError(retVal, aID, 0UL, 0UL, 0UL, lineNo);
    }
    return retVal;
}

/** @brief This API performs read operation using QSPI controller

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aAddr       Address from which data has to be read
    @param[out]     aData       Pointer to receive data buffer
    @param[in]      aLen        Length of data buffer

    @retval     #BL_BCM_ERR_OK             Read successful
    @retval     #BL_BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BL_BCM_ERR_TIME_OUT       Timeout happened during start
    @retval     #BL_BCM_ERR_UNKNOWN        Unknown error

    @post None

    @trace  #BRCM_DSGN_BL_FLASH_QSPI_READ_PROC  #BRCM_ARCH_BL_FLASH_QSPI_READ

    @limitations None

    @code{.c}
    uint32_t tmpBufIdx = 0UL;

	Validate aID

    If aAddr is 4 byte aligned:
		BL_QSPI_RafRead(aID, aAddr, aData, aLen)
	else
		alignAddr = (aAddr & (~0x3UL));
		BL_QSPI_RafRead(aID, alignAddr, tmpBuf, BL_RAF_WORD_LEN);
		alignOff = aAddr & 0x3UL;
		for (i = alignOff, (i < BL_RAF_WORD_LEN and tmpBufIdx < aLen), i++)
			aData[tmpBufIdx++] = tmpBuf[i];
		Increment alignAddr by  BL_RAF_WORD_LEN
		if (aLen > tmpBufIdx)
			BL_QSPI_RafRead(aID, alignAddr, &aData[tmpBufIdx],
			             aLen - tmpBufIdx)
    @endcode
*/
int32_t BL_QSPI_Read(BL_QSPI_HwIDType aID,
                     uint32_t aAddr,
                     uint8_t *const aData,
                     uint32_t aLen)
{
    uint32_t alignAddr;
    uint32_t alignOff;
    uint32_t i;
    uint32_t tmpBufIdx = 0UL;
    uint8_t tmpBuf[BL_RAF_WORD_LEN] = {0U};
    int32_t retVal = BL_BCM_ERR_OK;

    if (aID >= BL_QSPI_MAX_HW_ID) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /**
     * If aAddr is not 4-byte aligned,
     * we need to split the read transaction
     * since BL_QSPI_RafRead() only reads the
     * aligned addresses
     */
    alignOff = aAddr & 0x3UL;
    if (alignOff == 0UL) {
        retVal = BL_QSPI_RafRead(aID, aAddr, aData, aLen);
    } else {
        alignAddr = (aAddr & (~0x3UL));
        retVal = BL_QSPI_RafRead(aID, alignAddr, tmpBuf, BL_RAF_WORD_LEN);
        if (retVal == BL_BCM_ERR_OK) {
            for (i = alignOff; ((i < BL_RAF_WORD_LEN) && (tmpBufIdx < aLen)); i++) {
                aData[tmpBufIdx++] = tmpBuf[i];
            }
            alignAddr += BL_RAF_WORD_LEN;
            if (aLen > tmpBufIdx) {
                retVal = BL_QSPI_RafRead(aID, alignAddr, &aData[tmpBufIdx],
                        (aLen - tmpBufIdx));
            }
        }
    }
err:
    return retVal;
}

/** @brief API to initialize QSPI controller.

    This API initializes QSPI controller. Initialize controller and verify the presence of Flash(in case of external
    flash)

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aConfig     Pointer to configuration structure

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_FLASH_QSPI_INIT_PROC  #BRCM_ARCH_BL_FLASH_QSPI_INIT

    @limitations None

    @code{.c}
    Configure QSPI speed:
		Set MSPI clock to 6.25MHz (Input clock is assumed to be 400MHz)
	Configure QSPI in master mode
	Configure SPI mode as per aConfig-> SPIMode
	Enable and configure QSPI read lane asper aConfig->readLane
    @endcode
*/
void BL_QSPI_Init(BL_QSPI_HwIDType aID,
                  BL_QSPI_ConfigType *const aConfig)
{
    uint32_t regVal;

    /* Configure QSPI speed */
    /* Input clock is assumed to be 400MHz. So, MSPI clock set to 6.25MHz */
    BL_QSPI_REGS[aID]->MSPI_SPCR0_LSB =
                    (BL_SPBR_VAL_32 & MSPI_SPCR0_LSB_SPBR_MASK);

    /* Configure QSPI in master mode */
    regVal = MSPI_SPCR0_MSB_MSTR_MASK;

    /* Configure SPI mode */
    switch (aConfig->SPIMode) {
    case BL_FLASH_SPI_MODE_0:
        regVal &= (~MSPI_SPCR0_MSB_CPOL_MASK);
        regVal &= (~MSPI_SPCR0_MSB_CPHA_MASK);
        break;
    case BL_FLASH_SPI_MODE_1:
        regVal &= (~MSPI_SPCR0_MSB_CPOL_MASK);
        regVal |= MSPI_SPCR0_MSB_CPHA_MASK;
        break;
    case BL_FLASH_SPI_MODE_2:
        regVal |= MSPI_SPCR0_MSB_CPOL_MASK;
        regVal &= (~MSPI_SPCR0_MSB_CPHA_MASK);
        break;
    case BL_FLASH_SPI_MODE_3:
        regVal |= MSPI_SPCR0_MSB_CPOL_MASK;
        regVal |= MSPI_SPCR0_MSB_CPHA_MASK;
        break;
    default:
        break;
    }

    BL_QSPI_REGS[aID]->MSPI_SPCR0_MSB = regVal;

    /* Enable QSPI read lane */
    BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL = 0UL;
    switch (aConfig->readLane) {
    case BL_FLASH_READ_LANE_SINGLE:
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL &=
                                    ~(BSPI_STRAP_OR_CTRL_DUAL_SGL_MASK);
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL &=
                                    ~(BSPI_STRAP_OR_CTRL_QUAD_MASK);
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL |=
                                    BSPI_STRAP_OR_CTRL_OR_MASK;
        break;
    case BL_FLASH_READ_LANE_DUAL:
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL &=
                                    ~(BSPI_STRAP_OR_CTRL_QUAD_MASK);
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL |=
                                    BSPI_STRAP_OR_CTRL_DUAL_SGL_MASK;
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL |=
                                    BSPI_STRAP_OR_CTRL_OR_MASK;
        break;
    case BL_FLASH_READ_LANE_QUAD:
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL &=
                                    ~(BSPI_STRAP_OR_CTRL_DUAL_SGL_MASK);
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL |=
                                    BSPI_STRAP_OR_CTRL_QUAD_MASK;
        BL_QSPI_REGS[aID]->BSPI_STRAP_OVERRIDE_CTRL |=
                                    BSPI_STRAP_OR_CTRL_OR_MASK;
        break;
    default:
        break;
    }
}

/** @} */
