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

/**
    @defgroup grp_bl_flash_impl Implementation
    @ingroup grp_bl_flash

    @addtogroup grp_bl_flash_impl
    @{
    @file bl_flash.c
    @brief Flash Driver Implementation
    This source file contains the implementation of Flash driver

    @version 0.30 Imported from docx
*/

#include <string.h>
#include <bl_utils.h>
#include <bl_chip_config.h>
#include <bl_compiler.h>
#include <bl_bcm_err.h>
#include <bl_log.h>
#include <bl_flash.h>
#include "bl_qspi.h"

#define BRCM_DSGN_BL_FLASH_REPORT_ERROR_PROC           (0x90U) /**< @brief #BL_FLASH_ReportError */

/**
    @name FLASH Commands
    @{
    @brief List of flash commands

    @note
    - These are common flash commands followed by many flash manufacturers.
    - Apart for these commands, there are extended commands which differs among manufacturers
    - These command shall be in correspondence with the flashes mentioned in #BL_FLASH_FlsIDType. For detailed signal
    description, refer to the corresponding data sheets.

    @trace #BRCM_DSGN_BL_FLASH_CMDS_MACRO #BRCM_REQ_BL_FLASH_CMDS
 */
#define BL_FLASH_CMD_READ              (0x03)  /**< @brief Read */
#define BL_FLASH_CMD_FAST_READ         (0x0B)  /**< @brief Fast read */
#define BL_FLASH_CMD_RDID              (0x9F)  /**< @brief Read ID */
#define BL_FLASH_CMD_WREN              (0x06)  /**< @brief Write enable */
#define BL_FLASH_CMD_WRDI              (0x04)  /**< @brief Write disable */
#define BL_FLASH_CMD_RDSR              (0x05)  /**< @brief Read status */
#define BL_FLASH_CMD_RCFR              (0x35)  /**< @brief Read configuration */
#define BL_FLASH_CMD_WRSR              (0x01)  /**< @brief Write status */
#define BL_FLASH_CMD_PP                (0x02)  /**< @brief Page program */
#define BL_FLASH_CMD_SSE               (0x20)  /**< @brief Sub-sector erase */
#define BL_FLASH_CMD_SE                (0xD8)  /**< @brief Sector erase */
#define BL_FLASH_CMD_BE                (0xC7)  /**< @brief Bulk erase */
#define BL_FLASH_CMD_RES               (0xAB)  /**< @brief Read electronic signature */
#define BL_FLASH_CMD_DOFR              (0x3B)  /**< @brief Dual output fast read */
#define BL_FLASH_CMD_DIOFR             (0xBB)  /**< @brief Dual input/ouput fast read */
#define BL_FLASH_CMD_QOFR              (0x6B)  /**< @brief Quad output fast read */
#define BL_FLASH_CMD_QIOFR             (0xEB)  /**< @brief Quad input/output fast read*/
#define BL_FLASH_CMD_READ_FS_REG       (0x70)  /**< @brief Read flag status register */
/** @} */

/** Flag status register bit mask */
#define BL_FLAG_STATUS_PROG_ERASE_CONTR_MASK   (0x80U)
#define BL_FLAG_STATUS_PROTECTION_ERR_MASK     (0x01U)
#define BL_FLAG_STATUS_ERASE_ERR_MASK          (0x20U)
#define BL_FLAG_STATUS_PROG_ERR_MASK           (0x10U)
#define BL_FLAG_STATUS_ERR_MASK    (BL_FLAG_STATUS_PROTECTION_ERR_MASK |\
        BL_FLAG_STATUS_ERASE_ERR_MASK |\
        BL_FLAG_STATUS_PROG_ERR_MASK)

/* Flash status register bit fields */
#define BL_FLASH_STATUS_WIP_MASK       (0x1U)   /**< WIP Bit mask */
#define BL_FLASH_STATUS_WEL_MASK       (0x2U)   /**< WEL Bit mask */

#define BL_ADDRESS23_16(flashAddr)     (((flashAddr) >> 16UL) & 0xFFUL)
#define BL_ADDRESS15_8(flashAddr)      (((flashAddr) >>  8UL) & 0xFFUL)
#define BL_ADDRESS7_0(flashAddr)       ((flashAddr) & 0xFFUL)

#define BL_DUMMY_DATA                  (0x0)

/* Maximum length of flash command supported */
#define BL_FLASH_CMD_MAX_LEN           (4UL)

#define BL_GetModuleLogLevel()         (BL_LOG_LVL_INFO)

/**
    @name BL_FLASH_StateType
    @{
    @brief State of the Flash driver

    @trace #BRCM_DSGN_BL_FLASH_STATE_TYPE #BRCM_REQ_BL_FLASH_STATE
 */
typedef uint32_t BL_FLASH_StateType; /**< @brief typedef for flash state */
#define BL_FLASH_STATE_UNINIT  (0UL) /**< @brief Uninit state */
#define BL_FLASH_STATE_INIT    (1UL) /**< @brief Init state */
/** @} */

/**
    @brief Flash RW device type

    @trace #BRCM_DSGN_BL_FLASH_RW_DEV_TYPE #BRCM_REQ_BL_FLASH_RW_DEV
 */
typedef struct {
    BL_FLASH_StateType state;       /**< @brief State of the Flash driver */
    uint32_t flsID;              /**< @brief Flash device Identification */
    const BL_FLASH_CfgType *config; /**< @brief Flash configuration structure */
} BL_FLASH_RWDevType;

/**
 * List of supported flashes
 */
static const uint32_t BL_FLASH_FlsIDTbl[] =
    {BL_WINBOND_W25Q16,
     BL_WINBOND_W25Q64CV,
     BL_MICRON_N25Q512A,
     BL_MACRONIX_MX25R32,
     BL_MACRONIX_MX25L32,
     BL_MICRON_M25P16,
     BL_S25FL064P};
static const uint32_t BL_FLASH_FlsIDTblSz = sizeof(BL_FLASH_FlsIDTbl)/sizeof(uint32_t);
static BL_FLASH_RWDevType BL_FLASH_RWDev[BL_FLASH_MAX_HW_ID] = {
#if (BL_FLASH_MAX_HW_ID == 0)
#error "BL_FLASH_MAX_HW_ID == 0"
#endif
    {
        .state = BL_FLASH_STATE_UNINIT,
    },
#if (BL_FLASH_MAX_HW_ID > 1UL)
    {
        .state = BL_FLASH_STATE_UNINIT,
    },
#endif
#if (BL_FLASH_MAX_HW_ID > 2)
#error "BL_FLASH_MAX_HW_ID > 2 is not suported"
#endif
};

static BL_COMP_NEVER_INLINE void BL_FLASH_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_QSP_ID, aInstanceID, BRCM_DSGN_BL_FLASH_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static int32_t BL_FLASH_ReadReg(BL_FLASH_HwIDType aID,
                                uint8_t aCmd,
                                uint32_t aCmdLen,
                                uint8_t *const aData)
{
    uint8_t cmdData[BL_FLASH_CMD_MAX_LEN];
    uint8_t rxData[BL_FLASH_CMD_MAX_LEN] = {0U};
    uint32_t i;
    int32_t retVal = BL_BCM_ERR_OK;

    if (aCmdLen > BL_FLASH_CMD_MAX_LEN) {
        retVal = BL_BCM_ERR_NOMEM;
    } else {

        cmdData[0] = aCmd;
        cmdData[1] = BL_DUMMY_DATA;
        cmdData[2] = BL_DUMMY_DATA;
        cmdData[3] = BL_DUMMY_DATA;

        retVal = BL_QSPI_Xfer(aID, cmdData, rxData, aCmdLen, BL_QSPI_XFER_STOP);
        if (retVal == BL_BCM_ERR_OK) {
            for (i = 0UL; i < (aCmdLen - 1UL); i++) {
                aData[i] = rxData[i + 1UL];
            }
        }
    }
    return retVal;
}

static int32_t BL_FLASH_ReadStatus(BL_FLASH_HwIDType aID,
                                   uint8_t *const aStatus)
{
    return BL_FLASH_ReadReg(aID, BL_FLASH_CMD_RDSR, 2UL, aStatus);
}

static int32_t BL_FLASH_WaitWIP(BL_FLASH_HwIDType aID)
{
    uint8_t status;
    uint8_t flagReg = 0;
    uint8_t wipStatus;
    int32_t retVal = BL_BCM_ERR_OK;

    do {
        retVal = BL_FLASH_ReadStatus(aID, &status);
        if (retVal == BL_BCM_ERR_OK) {
            wipStatus = status & BL_FLASH_STATUS_WIP_MASK;
            if (BL_MICRON_N25Q512A == BL_FLASH_RWDev[aID].flsID) {
                retVal = BL_FLASH_ReadReg(aID, BL_FLASH_CMD_READ_FS_REG, 2UL, &flagReg);
                if (retVal == BL_BCM_ERR_OK) {
                    if ((flagReg & BL_FLAG_STATUS_ERR_MASK) != 0U) {
                        retVal = BL_BCM_ERR_UNKNOWN;
                        BL_FLASH_ReportError(retVal, aID, 0UL, 0UL, 0UL, __LINE__);
                        break;
                    }
                    if (((flagReg & BL_FLAG_STATUS_PROG_ERASE_CONTR_MASK) != 0U)
                            && (wipStatus == 0U)) {
                        break;
                    }
                }
            } else {
               if (wipStatus == 0UL) {
                   break;
               }
            }
        }
    } while (retVal == BL_BCM_ERR_OK);

    return retVal;
}

static int32_t BL_FLASH_WriteEnable(BL_FLASH_HwIDType aID)
{
    int32_t retVal = BL_BCM_ERR_OK;
    uint8_t status;
    uint8_t cmdData[2UL];

    /* send Write enable command */
    cmdData[0] = BL_FLASH_CMD_WREN;
    retVal = BL_QSPI_Xfer(aID, cmdData, NULL, 1UL, BL_QSPI_XFER_STOP);

    if (retVal == BL_BCM_ERR_OK) {
        do {
            retVal = BL_FLASH_ReadStatus(aID, &status);
        } while ((retVal == BL_BCM_ERR_OK) &&
                ((status & BL_FLASH_STATUS_WEL_MASK) == 0U));
    }
    return retVal;
}

static int32_t BL_FLASH_Erase(BL_FLASH_HwIDType aID,
                              BL_FLASH_AddrType aAddr,
                              BL_FLASH_LenType aLen,
                              uint32_t aIsSubSector)
{
    uint32_t eraseSec;
    BL_FLASH_LenType eraseSecSize;
    uint32_t i;
    uint8_t cmdData[5UL];
    int32_t retVal = BL_BCM_ERR_OK;
    uint32_t tmpAddr = aAddr;
    uint32_t lineNo = __LINE__;

    if (BL_FLASH_MAX_HW_ID <= aID) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (BL_FLASH_STATE_UNINIT == BL_FLASH_RWDev[aID].state) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_UNINIT;
        goto err;
    }

    if ((0UL == aLen) || (BL_FLASH_RWDev[aID].config->size < (aAddr + aLen))) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (BL_TRUE == aIsSubSector) {
        eraseSecSize = BL_FLASH_RWDev[aID].config->subSectorSize;
    } else {
        eraseSecSize = BL_FLASH_RWDev[aID].config->sectorSize;
    }

    if (eraseSecSize == 0UL) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check aAddr and aLen for flash sector size alignment */
    if (((aAddr % eraseSecSize) != 0UL) || ((aLen % eraseSecSize) != 0UL)) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    eraseSec = aLen / eraseSecSize;

    for (i = 0UL; i < eraseSec; i++) {
        /* issue Write enable */
        retVal = BL_FLASH_WriteEnable(aID);
        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        /* Send sector erase command */
        cmdData[0] = (BL_TRUE == aIsSubSector) ? BL_FLASH_CMD_SSE : BL_FLASH_CMD_SE;
        cmdData[1] = (uint8_t)BL_ADDRESS23_16(tmpAddr);
        cmdData[2] = (uint8_t)BL_ADDRESS15_8(tmpAddr);
        cmdData[3] = (uint8_t)BL_ADDRESS7_0(tmpAddr);
        retVal = BL_QSPI_Xfer(aID, cmdData, NULL, 4UL, BL_QSPI_XFER_STOP);
        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        /* wait for erase to complete */
        retVal = BL_FLASH_WaitWIP(aID);
        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        tmpAddr +=  eraseSecSize;
    }

err:
    if (retVal != BL_BCM_ERR_OK) {
        BL_FLASH_ReportError(retVal, aID, 0UL, 0UL, 0UL, lineNo);
    }
    return retVal;
}

/**
    @code{.c}
    validate arguments aID, aAddr and aLen
	Validate controller status
	Verify aAddr + Len < sectorSize
	Verify sectorSize is non-zero
	Verify aAddr and aLen is aligned to sector size
	tmpAddr = aAddr
	sectors = aLen / sectorSize
	For ( i=0, i < sectors; ++i)
		Issue write enable
		Send Sector erase command:
			cmdData[0] = BL_FLASH_CMD_SE;
        		cmdData[1] = (uint8_t)BL_ADDRESS23_16(tmpAddr)
        		cmdData[2] = (uint8_t)BL_ADDRESS15_8(tmpAddr)
        		cmdData[3] = (uint8_t)BL_ADDRESS7_0(tmpAddr)
			BL_QSPI_Xfer(aID, cmdData, NULL, 4UL, BL_QSPI_XFER_STOP)
		Wait for erase to complete
		Increment tmpAddr by sectorSize
	Report errors (if any)
    @endcode
*/
int32_t BL_FLASH_EraseSector(BL_FLASH_HwIDType aID,
                             BL_FLASH_AddrType aAddr,
                             BL_FLASH_LenType aLen)
{
    return BL_FLASH_Erase(aID, aAddr, aLen, BL_FALSE);
}

/**
    @code{.c}
    Validate arguments aID, aAddr and aLen
	Validate controller status
	Verify aAddr + Len < subSectorSize
	Verify sectorSize is non-zero
	Verify aAddr and aLen is aligned to sub-sector size

	tmpAddr = aAddr
    subSectors = aLen / subSectorSize
    For (i = 0UL; i < subSectors; i++) {
        Issue Write enable
        cmdData[] = {BL_FLASH_CMD_SSE, BL_ADDRESS23_16(tmpAddr),
			BL_ADDRESS15_8(tmpAddr), BL_ADDRESS7_0(tmpAddr)}
        BL_QSPI_Xfer(aID, cmdData, NULL, 4UL, BL_QSPI_XFER_STOP)
        Wait for erase to complete
	Increment tmpAddr by subSectorSize
	Report errors (if any)
    @endcode
*/
int32_t BL_FLASH_EraseSubSector(BL_FLASH_HwIDType aID,
                                BL_FLASH_AddrType aAddr,
                                BL_FLASH_LenType aLen)
{
    return BL_FLASH_Erase(aID, aAddr, aLen, BL_TRUE);
}

/**
    @code{.c}
    uint32_t tmpAddr = aAddr;
    uint8_t const *tmpBuf = aBuf;
    uint8_t cmdData[5UL];

	Validate arguments aID, aBuf, aAddr and aLen
	Validate controller status
	Verify aAddr + Len < sectorSize
	Verify sectorSize is non-zero
	Verify aAddr and aLen is aligned to page size

    numPages = aLen / pageSize;
    remain = aLen % pageSize;
    if remain != 0UL
        numPages += 1UL;

	tmpAddr = aAddr
	tmpBuf = aBuf
    For (i = 0UL; i < numPages; i++)
        Issue write enable
		cmdData[] = {BL_FLASH_CMD_PP, BL_ADDRESS23_16(tmpAddr),
			BL_ADDRESS15_8(tmpAddr), BL_ADDRESS7_0(tmpAddr)}
        BL_QSPI_Xfer(aID, cmdData, NULL, 4UL, BL_QSPI_XFER_CONTINUE)
        	if ((remain != 0UL) && (i == (numPages - 1UL)))
           BL_QSPI_Xfer(aID, tmpBuf, NULL, remain, BL_QSPI_XFER_STOP)
        else
           BL_QSPI_Xfer(aID, tmpBuf, NULL, pageSize, BL_QSPI_XFER_STOP);
		Wait for write to complete
        Increment tmpAddr by pageSize
       	Increment tmpBuf by pageSize
    @endcode
*/
int32_t BL_FLASH_Write(BL_FLASH_HwIDType aID,
                       BL_FLASH_AddrType aAddr,
                       const uint8_t *const aBuf,
                       BL_FLASH_LenType aLen)
{
    int32_t retVal = BL_BCM_ERR_UNKNOWN;
    uint32_t numPages;
    uint32_t remain;
    uint32_t i;
    uint32_t tmpAddr = aAddr;
    uint8_t const *tmpBuf = aBuf;
    uint8_t cmdData[5UL];
    uint32_t lineNo = __LINE__;

    if (BL_FLASH_MAX_HW_ID <= aID) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (BL_FLASH_STATE_UNINIT == BL_FLASH_RWDev[aID].state) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_UNINIT;
        goto err;
    }

    if ((0UL == aLen) || (NULL == aBuf)
            || (BL_FLASH_RWDev[aID].config->size < (aAddr + aLen))) {
        lineNo = __LINE__;
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check aAddr and aLen for flash page size alignment */
    if ((aAddr % BL_FLASH_RWDev[aID].config->pageSize) != 0UL) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto err;
    }

    numPages = aLen / BL_FLASH_RWDev[aID].config->pageSize;
    remain = aLen % BL_FLASH_RWDev[aID].config->pageSize;
    if (remain != 0UL) {
        numPages += 1UL;
    }

    /**
     * Flash write operation page by page
     * Enable WEL --> Send PP cmd + BL_ADDRESS
     * --> Page data --> Wait for WIP
     */
    for (i = 0UL; i < numPages; i++) {
        retVal = BL_FLASH_WriteEnable(aID);
        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        cmdData[0] = BL_FLASH_CMD_PP;
        cmdData[1] = (uint8_t)BL_ADDRESS23_16(tmpAddr);
        cmdData[2] = (uint8_t)BL_ADDRESS15_8(tmpAddr);
        cmdData[3] = (uint8_t)BL_ADDRESS7_0(tmpAddr);
        retVal = BL_QSPI_Xfer(aID, cmdData, NULL, 4UL, BL_QSPI_XFER_CONTINUE);
        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        if ((remain != 0UL) && (i == (numPages - 1UL))) {
            retVal = BL_QSPI_Xfer(aID, tmpBuf, NULL, remain, BL_QSPI_XFER_STOP);

        } else {
            retVal = BL_QSPI_Xfer(aID, tmpBuf, NULL, BL_FLASH_RWDev[aID].config->pageSize,
                    BL_QSPI_XFER_STOP);
        }

        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        retVal = BL_FLASH_WaitWIP(aID);
        if (retVal != BL_BCM_ERR_OK) {
            lineNo = __LINE__;
            break;
        }
        tmpAddr += BL_FLASH_RWDev[aID].config->pageSize;
        tmpBuf += BL_FLASH_RWDev[aID].config->pageSize;
    }

err:
    if (retVal != BL_BCM_ERR_OK) {
        BL_FLASH_ReportError(retVal, aID, 0UL, 0UL, 0UL, lineNo);
    }
    return retVal;
}

/**
    @code{.c}
    Validate arguments aID, aAddr, aBuf, aLen
	Validate Controller status
	Verify aAddr + aLen < flashSize

	BL_QSPI_Read(aID, aAddr, aBuf, aLen)
    @endcode
*/
int32_t BL_FLASH_Read(BL_FLASH_HwIDType aID,
                      BL_FLASH_AddrType aAddr,
                      uint8_t *const aBuf,
                      BL_FLASH_LenType aLen)
{

    int32_t retVal;
    uint32_t lineNo = __LINE__;

    if (BL_FLASH_MAX_HW_ID <= aID) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto err;
    }

    /* Check the controller status */
    if (BL_FLASH_STATE_UNINIT == BL_FLASH_RWDev[aID].state) {
        retVal = BL_BCM_ERR_UNINIT;
        lineNo = __LINE__;
        goto err;
    }

    if ((0UL == aLen) || (BL_FLASH_RWDev[aID].config->size < (aAddr + aLen))) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto err;
    }

    retVal = BL_QSPI_Read(aID, aAddr, aBuf, aLen);
    if (BL_BCM_ERR_OK != retVal) {
        lineNo = __LINE__;
    }
err:
    if (BL_BCM_ERR_OK != retVal) {
        BL_FLASH_ReportError(retVal, aID, 0UL, 0UL, 0UL, lineNo);
    }
    return retVal;
}

static BL_FLASH_FlsIDType BL_FLASH_ReadIDInternal(BL_FLASH_HwIDType aID)
{
    uint8_t rxData[3UL] = {0U};
    int32_t retVal;
    BL_FLASH_FlsIDType flashID = 0UL;

    retVal = BL_FLASH_ReadReg(aID, BL_FLASH_CMD_RDID, 4UL, rxData);
    if (retVal == BL_BCM_ERR_OK) {
        flashID = (uint32_t)rxData[0] << 16UL | (uint32_t)rxData[1] << 8UL |
            (uint32_t)rxData[2];
    }

    return flashID;
}

/**
    @code{.c}
    Validate argument aID
	cmdData[] = {BL_FLASH_CMD_RDID, BL_DUMMY_DATA, BL_DUMMY_DATA, BL_DUMMY_DATA}
	BL_QSPI_Xfer(aID, cmdData, rxData, 4UL, BL_QSPI_XFER_STOP)
	flashID = rxData[1] << 16UL | rxData[2] << 8UL | rxData[3]
	return flashID
    @endcode
*/
BL_FLASH_FlsIDType BL_FLASH_ReadID(BL_FLASH_HwIDType aID)
{
    BL_FLASH_FlsIDType flashID = 0UL;

    if (BL_FLASH_MAX_HW_ID <= aID) {
        goto err;
    }
    if (BL_FLASH_STATE_UNINIT == BL_FLASH_RWDev[aID].state) {
        goto err;
    }
    flashID = BL_FLASH_ReadIDInternal(aID);

err:
    return flashID;
}

/**
    @code{.c}
    Validate aID;
    Validate aConfig;

    Initialize QSPI controller
    QSPI_Config.SPIMode = aConfig->SPIMode;
    QSPI_Config.speed = aConfig->speed;
    QSPI_Config.readLane = aConfig->readLane;
    BL_QSPI_Init(aID, &QSPI_Config);

    Save the configuration structure in the corresponding BL_FLASH_RWDev
    BL_FLASH_RWDev[aID].flsID = flsID;
    BL_FLASH_RWDev[aID].config = aConfig;
    BL_FLASH_RWDev[aID].state = BL_FLASH_STATE_INIT;
    @endcode
*/
void BL_FLASH_Init(BL_FLASH_HwIDType aID,
                   const BL_FLASH_CfgType *const aConfig)
{
    BL_FLASH_FlsIDType flsID = 0UL;
    BL_QSPI_ConfigType QSPI_Config;
    int32_t retVal = BL_BCM_ERR_OK;
    uint32_t i;
    uint32_t lineNo = __LINE__;

    if ((BL_FLASH_MAX_HW_ID <= aID) || (NULL == aConfig)) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto err;
    }

    /* Check the controller status */
    if (BL_FLASH_STATE_UNINIT != BL_FLASH_RWDev[aID].state) {
        retVal = BL_BCM_ERR_INVAL_STATE;
        lineNo = __LINE__;
        goto err;
    }

    /* Initialize QSPI controller */
    QSPI_Config.SPIMode = aConfig->SPIMode;
    QSPI_Config.speed = aConfig->speed;
    QSPI_Config.readLane = aConfig->readLane;

    BL_QSPI_Init(aID, &QSPI_Config);

    /**
     * Read flash id and check if its supported by the driver. If flash is not
     * supported, flash driver initialization fails and flash driver will stay
     * in UNINIT state
     */
    flsID = BL_FLASH_ReadIDInternal(aID);
    for (i = 0UL; i < BL_FLASH_FlsIDTblSz; i++) {
        if (flsID == BL_FLASH_FlsIDTbl[i]) {
            break;
        }
    }
    if (BL_FLASH_FlsIDTblSz == i) {
        retVal = BL_BCM_ERR_NOSUPPORT;
        lineNo = __LINE__;
        goto err;
    }

    /* Quad lane not supported for Micron N25Q512A flash */
    if ((0UL != aID)
            && (BL_FLASH_READ_LANE_QUAD == aConfig->readLane)
            && (BL_WINBOND_W25Q16 != flsID)
            && (BL_WINBOND_W25Q64CV != flsID)) {
        retVal = BL_BCM_ERR_NOSUPPORT;
        lineNo = __LINE__;
        goto err;
    }

    BL_FLASH_RWDev[aID].flsID = flsID;
    BL_FLASH_RWDev[aID].config = aConfig;
    BL_FLASH_RWDev[aID].state = BL_FLASH_STATE_INIT;

err:
    if (BL_BCM_ERR_OK != retVal) {
        BL_FLASH_ReportError(retVal, aID, 0UL, 0UL, 0UL, lineNo);
    }
    return;
}

/** @} */
