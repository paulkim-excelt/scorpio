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
#include <string.h>
#include <chip_config.h>
#include <bcm_err.h>
#include <utils.h>
#include <compiler.h>
#include <ipc.h>
#include "ipc_pl022.h"
#include <bcm_time.h>
#include <gio_rdb.h>
#include <spi1_rdb.h>

#define SPI_MAX_HW_ID               (SPI1_MAX_HW_ID)
#define SPI_FIFO_SIZE               (8UL)
#define SPI_TIMEOUT_VAL_MAX         (100000UL)

#define BITRATE_CPSDVSR_CTRL_MIN    (196850UL)

/* Macros for SPI opcode generation */
#define SPI_OPCODE_PHYADDR_SHIFT        (6UL)
#define SPI_OPCODE_PHYADDR(a)           ((a) << SPI_OPCODE_PHYADDR_SHIFT)
#define SPI_OPCODE_RDWR_SHIFT           (5UL)
#define SPI_OPCODE_WR                   (1UL << SPI_OPCODE_RDWR_SHIFT)
#define SPI_OPCODE_RD                   (0UL << SPI_OPCODE_RDWR_SHIFT)
#define SPI_OPCODE_INC_SHIFT            (4UL)
#define SPI_OPCODE_AUTO_INC             (1UL << SPI_OPCODE_INC_SHIFT)
#define SPI_OPCODE_NO_INC               (0UL << SPI_OPCODE_INC_SHIFT)
#define SPI_OPCODE_RD_WAIT_SHIFT        (2UL)
#define SPI_OPCODE_RD_WAIT_MASK         (0x3UL << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_0            (0UL << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_2            (1UL << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_4            (2UL << SPI_OPCODE_RD_WAIT_SHIFT)
#define SPI_OPCODE_RD_WAIT_6            (3UL << SPI_OPCODE_RD_WAIT_SHIFT)

typedef uint8_t SPI_AccessWidthType;
#define SPI_ACCESS_WIDTH_8              (0UL)
#define SPI_ACCESS_WIDTH_16             (1UL)
#define SPI_ACCESS_WIDTH_32             (2UL)
#define SPI_ACCESS_WIDTH_64             (3UL)

#define DEFAULT_DEVICE_ID               (0UL)
#define SPI_READ_WAIT_BYTES             (SPI_OPCODE_RD_WAIT_6)
#define SPI_WRITE_WAIT_BYTES            (SPI_OPCODE_RD_WAIT_0)
#define SPI_SPEED                       (25000000UL)
#define SPI_FIFO_WIDTH_16_MASK          (0xFUL)
#define SPI_FIFO_WIDTH_8_MASK           (0x7UL)
#define SPI_SLAVE_ID_MAX                (0x3UL)    /* Slave ID is 2 bit value, 0-3 */
#define SPI_BUF_MAX_SIZE                (RPC_MSG_PAYLOAD_SZ + 16UL)

/**
    @brief Index of the SPI bus

    @trace #BRCM_ARCH_SPI_HW_ID_TYPE #BRCM_REQ_SPI_HW_ID
*/
typedef uint8_t SPI_HwIDType;

/**
    @name SPI_StateType
    @{
    @brief State of the SPI Driver

    @trace #BRCM_ARCH_SPI_STATE_TYPE #BRCM_REQ_SPI_STATE
*/
typedef uint32_t SPI_StateType;                 /**< @brief typedef for SPI states */
#define SPI_STATE_UNINIT                (0UL)   /**< @brief Uninitialized state */
#define SPI_STATE_INIT                  (1UL)   /**< @brief Idle state */
/** @} */

/**
    @brief Type for the SPI data buffer

    @trace #BRCM_ARCH_SPI_DATA_BUF_TYPE #BRCM_REQ_SPI_DATA_BUF
*/
typedef uint8_t SPI_DataBufType;

/**
    @name SPI_FRMFormatType
    @{
    @brief SPI Frame format

    @trace #BRCM_ARCH_SPI_FRM_FORMAT_TYPE #BRCM_REQ_SPI_FRM_FORMAT
*/
typedef uint16_t SPI_FRMFormatType;             /**< @brief typedef for SPI frame format */
#define SPI_FRM_FORMAT_MOTOROLA         (0U)    /**< @brief MOTOROLA slave */
#define SPI_FRM_FORMAT_TI               (1U)    /**< @brief TI slave */
#define SPI_FRM_FORMAT_NATIONAL         (2U)    /**< @brief NATIONAL microwire slave */
/** @} */


/**
    @brief Size of the SPI data buffer

    @trace #BRCM_ARCH_SPI_DATA_BUF_TYPE #BRCM_REQ_SPI_DATA_BUF
*/
typedef uint16_t SPI_DataSizeType;

/**
    @name SPI_DataWidthType
    @{
    @brief Width of the SPI data to be transferred

    @trace #BRCM_ARCH_SPI_DATA_WIDTH_TYPE #BRCM_REQ_SPI_DATA_WIDTH
*/
typedef uint32_t SPI_DataWidthType;             /**< @brief typedef for SPI data width */
#define SPI_DATA_WIDTH_8              (8UL)     /**< @brief 4 bits */
#define SPI_DATA_WIDTH_16             (16UL)    /**< @brief 16 bits */
/** @} */


/**
    @brief SPI configuration structure

    @trace #BRCM_ARCH_SPI_CONFIG_TYPE #BRCM_REQ_SPI_CONFIG
 */
typedef struct {
    SPI_HwIDType hwID;
    uint32_t bitRate;                   /**< @brief Bit Rate */
#define BITRATE_MIN                 (769UL)
#define BITRATE_MAX                 (25000000UL)
} SPI_ConfigType;

typedef struct {
    SPI_StateType state;
} SPI_DevType;


static uint8_t srcBuf[SPI_BUF_MAX_SIZE];
static uint8_t destBuf[SPI_BUF_MAX_SIZE];

static PL022_RegsType *const PL022_REGS = (PL022_RegsType *)SPI1_BASE;

static SPI_DevType COMP_SECTION(".data.drivers") SPIDrv_Dev = {
    .state = SPI_STATE_UNINIT,
};

#define IPC_ENABLE_CS_GPIO
#ifdef IPC_ENABLE_CS_GPIO
#define IPC_CS_GPIO     (0UL)
static GIO_CHAN_GRP_RDBType *const GPIO_G1_REGS = (GIO_CHAN_GRP_RDBType *)GIO1_BASE;

static void IPC_SpiGpioWriteCs(uint32_t aGpioNum, uint32_t aLevel)
{
    if (aGpioNum < 4UL) {
        if (1UL == aLevel) {
            GPIO_G1_REGS->dout |= (0x1U << aGpioNum);
        } else {
            GPIO_G1_REGS->dout &= ~(0x1UL << aGpioNum);
        }
    }
}

static void IPC_SpiGpioInit(uint32_t aGpioNum)
{
    if (aGpioNum < 4UL) {
        GPIO_G1_REGS->dout |= (0x1U << aGpioNum);
        GPIO_G1_REGS->drv_en |= (0x1U << aGpioNum);
    }
}

static void IPC_SpiGpioDeInit(uint32_t aGpioNum)
{
    if (aGpioNum < 4UL) {
        GPIO_G1_REGS->drv_en &= ~(0x1U << aGpioNum);
    }
}
#endif

static int32_t IPC_SpiPl022Init(const SPI_ConfigType *const aConfig)
{
    uint32_t ratio;
    uint32_t scr, cpsdvr;
    int32_t ret = BCM_ERR_OK;

    /* TODO: We will not be supporting bit rate lesse than 25M as incremental op will fail */
    if ((BITRATE_MIN > aConfig->bitRate) ||
        (BITRATE_MAX < aConfig->bitRate)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    ratio = (SPI_CLOCK / (aConfig->bitRate));
    if (BITRATE_CPSDVSR_CTRL_MIN <= aConfig->bitRate) {
        scr = 0UL;
        cpsdvr = ratio;
    } else {
        cpsdvr = 254UL;
        scr = (ratio / cpsdvr) - 1UL;
        if (0UL != (ratio % cpsdvr)) {
            scr = scr + 1UL;
        }
    }
    /* Mask the interrupts */
    PL022_REGS->SSPIMSC &= ~(SSPIMSC_TXIM_MASK | SSPIMSC_RXIM_MASK
                                          | SSPIMSC_RTIM_MASK | SSPIMSC_RORIM_MASK);
    PL022_REGS->SSPCR0 = ((scr << SSPCR0_SCR_SHIFT) & SSPCR0_SCR_MASK) |
                                        SPI_FIFO_WIDTH_8_MASK;

    PL022_REGS->SSPCPSR = cpsdvr;
    /* Enable SSP */
    PL022_REGS->SSPCR1 |= SSPCR1_SSE_MASK;
    PL022_REGS->SSPCR1 &= ~SSPCR1_MS_MASK;
    SPIDrv_Dev.state = SPI_STATE_INIT;
err:
    return ret;
}

static void IPC_SpiPl022DeInit(SPI_HwIDType aHwID)
{
    if (SPI_STATE_UNINIT != SPIDrv_Dev.state) {
        SPIDrv_Dev.state = SPI_STATE_UNINIT;
        /* Ideally this function will not get called while transfer is going on so RxFIFO will
           always be empty. Below code is redundant but extra check.
           Clear RX FIFO */
        while((PL022_REGS->SSPSR & SSPSR_RNE_MASK) == SSPSR_RNE_MASK) {
            PL022_REGS->SSPDR;
        }
        /* Power on reset state */
        PL022_REGS->SSPCR0 = 0UL;
        PL022_REGS->SSPCR1 = 0UL;
        PL022_REGS->SSPDR = 0UL;
        PL022_REGS->SSPCPSR = 0UL;
        PL022_REGS->SSPIMSC = 0UL;
    }
}

static void IPC_SpiPl022Xfer(SPI_HwIDType aHwID, uint32_t aXferSize)
{
    uint32_t rxIdx = 0UL;
    uint32_t txIdx = 0UL;
    volatile uint32_t statusReg;

    PL022_REGS->SSPDR = (srcBuf[txIdx++]);
    PL022_REGS->SSPDR = (srcBuf[txIdx++]);
    PL022_REGS->SSPDR = (srcBuf[txIdx++]);

    while ((txIdx < aXferSize) || (rxIdx < aXferSize)) {
        statusReg = PL022_REGS->SSPSR;
        if ((statusReg & SSPSR_TNF_MASK) && (txIdx < aXferSize)) {
            PL022_REGS->SSPDR = (srcBuf[txIdx++]);
        }

        if (statusReg & SSPSR_RNE_MASK) {
            destBuf[rxIdx++] = (PL022_REGS->SSPDR);
        }
    }
}

int32_t IPC_SpiValidateParam(uint32_t aHwID, IPC_AccWidthType aWidth, uint32_t aSlaveID, uint32_t aLen, uint8_t *const aData)
{
    int32_t ret = BCM_ERR_OK;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        ret = BCM_ERR_UNINIT;
        goto err;
    }

    if (SPI_SLAVE_ID_MAX < aSlaveID) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((aWidth < IPC_ACCESS_WIDTH_8) || (aWidth > IPC_ACCESS_WIDTH_64)){
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check length and allignment with aWidth */
    if ((0UL == aLen) ||
        (SPI_BUF_MAX_SIZE < aLen) ||
        (aLen & ((1 << (aWidth - 1UL)) - 1UL))) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

err:
    return ret;
}

int32_t IPC_SpiBusWrite(IPC_BusHwIDType aHwID,
                       uint16_t aSlaveID,
                       uint32_t aAddr,
                       uint8_t *const aData,
                       uint32_t aLen,
                       IPC_AccWidthType aWidth)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t xferSize;
    uint32_t cmdLen = (5UL + (SPI_WRITE_WAIT_BYTES >> 1UL)); /* 1 byte opCode + 4 byte Address + dummy bytes for read */
    uint32_t srcIdx = 0UL;
    uint32_t dataIdx = 0UL;
    uint32_t autoIncrement = SPI_OPCODE_AUTO_INC;
    uint32_t spiAccessWidth = aWidth - 1UL;
    uint32_t *tempPtr32;
    uint16_t *tempPtr16;
    uint64_t *tempPtr64;

    ret = IPC_SpiValidateParam(aHwID, aWidth, aSlaveID, aLen, aData);
    if (BCM_ERR_OK == ret) {
        if ((1 << spiAccessWidth) == aLen) {
            autoIncrement = SPI_OPCODE_NO_INC;
        }
        xferSize = cmdLen + aLen;    /* xferSize is actual number of bytes */
        srcBuf[srcIdx++] = SPI_OPCODE_PHYADDR(aSlaveID) | SPI_OPCODE_WR | autoIncrement | SPI_WRITE_WAIT_BYTES | spiAccessWidth;
        tempPtr32 = (uint32_t *)&(srcBuf[srcIdx]);
        *tempPtr32 = Host2BE32(aAddr);
        srcIdx += 4UL;

        while (srcIdx < xferSize) {
            dataIdx = srcIdx - cmdLen;
            switch (spiAccessWidth) {
                case SPI_ACCESS_WIDTH_8:
                    srcBuf[srcIdx] = aData[dataIdx];
                    break;
                case SPI_ACCESS_WIDTH_16:
                    tempPtr16 = (uint16_t *)&(srcBuf[srcIdx]);
                    *tempPtr16 = Host2BE16(*((uint16_t *)&(aData[dataIdx])));
                    break;
                case SPI_ACCESS_WIDTH_32:
                    tempPtr32 = (uint32_t *)&(srcBuf[srcIdx]);
                    *tempPtr32 = Host2BE32(*((uint32_t *)&(aData[dataIdx])));
                    break;
                case SPI_ACCESS_WIDTH_64:
                    tempPtr64 = (uint64_t *)&(srcBuf[srcIdx]);
                    *tempPtr64 = Host2BE64(*((uint64_t *)&(aData[dataIdx])));
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
            }
            srcIdx += (1 << spiAccessWidth);
        }

#ifdef IPC_ENABLE_CS_GPIO
        IPC_SpiGpioWriteCs(IPC_CS_GPIO, 0UL);
#endif
        IPC_SpiPl022Xfer(aHwID, xferSize);
#ifdef IPC_ENABLE_CS_GPIO
        IPC_SpiGpioWriteCs(IPC_CS_GPIO, 1UL);
#endif
        BCM_DelayNs(300UL);   /* CS must be inactive for minimum 300ns as per HW spec */
    }

    return ret;
}

int32_t IPC_SpiBusRead(IPC_BusHwIDType aHwID,
                       uint16_t aSlaveID,
                       uint32_t aAddr,
                       uint8_t *const aData,
                       uint32_t aLen,
                       IPC_AccWidthType aWidth)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t xferSize;
    uint32_t cmdLen = (5UL + (SPI_READ_WAIT_BYTES >> 1UL)); /* 1 byte OpCode + 4 byte Address + dummy bytes for read */
    uint32_t destIdx = 0UL;
    uint32_t dataIdx = 0UL;
    uint32_t autoIncrement = SPI_OPCODE_AUTO_INC;
    uint32_t spiAccessWidth = aWidth - 1UL;
    uint32_t *tempPtr32;
    uint16_t *tempPtr16;
    uint64_t *tempPtr64;

    ret = IPC_SpiValidateParam(aHwID, aWidth, aSlaveID, aLen, aData);
    if (BCM_ERR_OK == ret) {
        if ((1UL << spiAccessWidth) == aLen) {
            autoIncrement = SPI_OPCODE_NO_INC;
        }
        xferSize = cmdLen + aLen;    /* xferSize is actual number of bytes */
        srcBuf[0UL] = SPI_OPCODE_PHYADDR(aSlaveID) | SPI_OPCODE_RD | autoIncrement | SPI_READ_WAIT_BYTES | spiAccessWidth;
        tempPtr32 = (uint32_t *)&(srcBuf[1UL]);
        *tempPtr32 = Host2BE32(aAddr);

#ifdef IPC_ENABLE_CS_GPIO
        IPC_SpiGpioWriteCs(IPC_CS_GPIO, 0UL);
#endif
        IPC_SpiPl022Xfer(aHwID, xferSize);
#ifdef IPC_ENABLE_CS_GPIO
        IPC_SpiGpioWriteCs(IPC_CS_GPIO, 1UL);
#endif
        BCM_DelayNs(300UL);   /* CS must be inactive for minimum 300ns as per HW spec */

        while (dataIdx < aLen) {
            destIdx = dataIdx + cmdLen;
            switch (spiAccessWidth) {
                case SPI_ACCESS_WIDTH_8:
                    aData[dataIdx] = destBuf[destIdx];
                    break;
                case SPI_ACCESS_WIDTH_16:
                    tempPtr16 = (uint16_t *)&(destBuf[destIdx]);
                    *((uint16_t *)&(aData[dataIdx])) = Host2BE16(*tempPtr16);
                    break;
                case SPI_ACCESS_WIDTH_32:
                    tempPtr32 = (uint32_t *)&(destBuf[destIdx]);
                    *((uint32_t *)&(aData[dataIdx])) = Host2BE32(*tempPtr32);
                    break;
                case SPI_ACCESS_WIDTH_64:
                    tempPtr64 = (uint64_t *)&(destBuf[destIdx]);
                    *((uint64_t *)&(aData[dataIdx])) = Host2BE64(*tempPtr64);
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
            }
            dataIdx += (1UL << spiAccessWidth);
        }
    }
    return ret;
}

void IPC_SpiBusDeInit(IPC_BusHwIDType aID)
{
    if (SPI_STATE_INIT == SPIDrv_Dev.state) {
        IPC_SpiPl022DeInit(aID);
#ifdef IPC_ENABLE_CS_GPIO
        IPC_SpiGpioDeInit(IPC_CS_GPIO);
#endif
    }
}

static void IPC_SpiBusInit(IPC_BusHwIDType aID, const IPC_BusConfigType *const aConfig)
{
    /* TODO: Use SPI speed from aConfig or remove the param itself */
    SPI_ConfigType spiConfig = {
        .hwID = 1UL,
        .bitRate = SPI_SPEED,
    };

#ifdef IPC_ENABLE_CS_GPIO
    IPC_SpiGpioInit(IPC_CS_GPIO);
#endif
    if ((BCM_ERR_OK == IPC_SpiPl022Init(&spiConfig))) {
        SPIDrv_Dev.state = SPI_STATE_INIT;
    }
}

const IPC_BusFnTblType IPC_SpiFnTbl = {
    .init = IPC_SpiBusInit,
    .deInit = IPC_SpiBusDeInit,
    .read = IPC_SpiBusRead,
    .write = IPC_SpiBusWrite,
};

