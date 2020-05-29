/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "bus_xfer.h"
#include "spi.h"
#include "spi_xfer.h"
#include <bcm_err.h>
#include "bl_downloader.h"
#include "bl_ipc_downloader.h"
#include "bl_chip_config.h"
#include "mcu.h"
#include "utils.h"
#include <hlog.h>
#include <cpu_indirect_rdb.h>

/* Macros for SPI opcode generation */
#define MAX_BUF_SZ                      (16)
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

#define DEFAULT_DEVICE_ID   (0)

static const CPU_INDIRECT_RDBType *CPU_INDIRECT_RegBase = (CPU_INDIRECT_RDBType *)CPU_INDIRECT_BASE;

/* SPI protocol for register access */
/* Direct Accesses */
static int32_t SPI_Read16(uint32_t slaveId, uint32_t addr, uint16_t *rd_val)
{
    uint8_t buf[MAX_BUF_SZ];
    int32_t len = 0;
    int i, retVal = BCM_ERR_OK;
    uint8_t val[2];

    //opcode
    buf[len++] = SPI_OPCODE_PHYADDR(slaveId) | SPI_OPCODE_RD | SPI_OPCODE_NO_INC | SPI_OPCODE_RD_WAIT_6 | SPI_OPCODE_TX_SZ_16;
    buf[len++] = (addr >> 24) & 0xff;
    buf[len++] = (addr >> 16) & 0xff;
    buf[len++] = (addr >> 8) & 0xff;
    buf[len++] = (addr & 0xff);
    /* wait states as per opcode */
    for (i =0; i < ((buf[0] & SPI_OPCODE_RD_WAIT_MASK) >>
        SPI_OPCODE_RD_WAIT_SHIFT) * 2; i++) {
        buf[len++] = 0x0;
    }
    retVal = SPI_Read(DEFAULT_DEVICE_ID, &buf[0], len, &val[0], sizeof(val));
    if (retVal != BCM_ERR_OK) {
        HOST_Log("SPI_Read failed retVal=%d\n", retVal);
        goto done;
    }
    /* Based on the protocol, MSB will be received first - Big Endian format */
    *rd_val = ((uint16_t)val[0] << 8UL) | val[1];

done:
    return retVal;
}

static int32_t SPI_Write16(uint32_t slaveId, uint32_t addr, uint16_t data)
{
    uint8_t buf[MAX_BUF_SZ];
    int32_t len = 0;

    //opcode
    buf[len++] = SPI_OPCODE_PHYADDR(slaveId) | SPI_OPCODE_WR | SPI_OPCODE_NO_INC | SPI_OPCODE_RD_WAIT_0 | SPI_OPCODE_TX_SZ_16;
    buf[len++] = (addr >> 24) & 0xff;
    buf[len++] = (addr >> 16) & 0xff;
    buf[len++] = (addr >> 8) & 0xff;
    buf[len++] = (addr & 0xff);
    buf[len++] = (data >> 8) & 0xff;
    buf[len++] = (data & 0xff);
    SPI_Write(DEFAULT_DEVICE_ID, &buf[0], len);

    return 0;
}

static int32_t SPI_Read32(uint32_t slaveId, uint32_t addr, uint32_t *rd_val)
{
    uint8_t buf[MAX_BUF_SZ];
    int32_t len = 0;
    int i, retVal;
    uint8_t val[4];

    //opcode
    buf[len++] = SPI_OPCODE_PHYADDR(slaveId) | SPI_OPCODE_RD | SPI_OPCODE_NO_INC | SPI_OPCODE_RD_WAIT_6 | SPI_OPCODE_TX_SZ_32;
    buf[len++] = (addr >> 24) & 0xff;
    buf[len++] = (addr >> 16) & 0xff;
    buf[len++] = (addr >> 8) & 0xff;
    buf[len++] = (addr & 0xff);
    /* wait states as per opcode */
    for (i =0; i < ((buf[0] & SPI_OPCODE_RD_WAIT_MASK) >>
        SPI_OPCODE_RD_WAIT_SHIFT) * 2; i++) {
        buf[len++] = 0x0;
    }
    retVal = SPI_Read(DEFAULT_DEVICE_ID, &buf[0], len, &val[0], sizeof(val));
    if (retVal) {
        HOST_Log("SPI_Read failed retVal=%d\n", retVal);
        goto done;
    }

    /* Based on the protocol, MSB will be received first - Big Endian format */
    *rd_val = ((uint32_t)val[0] << 24UL)
                | ((uint32_t)val[1] << 16UL)
                | ((uint32_t)val[2] << 8UL)
                | val[3];

done:
    return retVal;
}

static int32_t SPI_Write32(uint32_t slaveId, uint32_t addr, uint32_t data)
{
    uint8_t buf[MAX_BUF_SZ];
    int32_t len = 0;

    //opcode
    buf[len++] = SPI_OPCODE_PHYADDR(slaveId) | SPI_OPCODE_WR | SPI_OPCODE_NO_INC | SPI_OPCODE_RD_WAIT_0 | SPI_OPCODE_TX_SZ_32;
    buf[len++] = (addr >> 24) & 0xff;
    buf[len++] = (addr >> 16) & 0xff;
    buf[len++] = (addr >> 8) & 0xff;
    buf[len++] = (addr & 0xff);
    buf[len++] = (data >> 24) & 0xff;
    buf[len++] = (data >> 16) & 0xff;
    buf[len++] = (data >> 8) & 0xff;
    buf[len++] = (data & 0xff);
    SPI_Write(DEFAULT_DEVICE_ID, &buf[0], len);

    return 0;
}

/* Indirect Accesses */
static int32_t SPI_Read8Indirect(uint32_t slaveId, uint32_t addr, uint8_t *data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
	int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_RD | IND_ACC_TX_SZ_8));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_TIME_OUT;
		goto done;
    }
    SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, &rd_val);
    *data = (uint8_t)(rd_val & 0xff);

done:
    return retVal;
}

static int32_t SPI_Write8Indirect(uint32_t slaveId, uint32_t addr, uint8_t data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
	int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, (uint32_t)(data));
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_WR | IND_ACC_TX_SZ_8));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}


static int32_t SPI_Read16Indirect(uint32_t slaveId, uint32_t addr, uint16_t *data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
    int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_RD | IND_ACC_TX_SZ_16));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }
    SPI_Read16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, data);

done:
    return retVal;
}

static int32_t SPI_Write16Indirect(uint32_t slaveId, uint32_t addr, uint16_t data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
    int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, (uint32_t)data);
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_WR | IND_ACC_TX_SZ_16));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}

static int32_t SPI_Read32Indirect(uint32_t slaveId, uint32_t addr, uint32_t *data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
    int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_RD | IND_ACC_TX_SZ_32));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }
    SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, data);

done:
    return retVal;
}

static int32_t SPI_Write32Indirect(uint32_t slaveId, uint32_t addr, uint32_t data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
    int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, data);
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_WR | IND_ACC_TX_SZ_32));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}

static int32_t SPI_Read64Indirect(uint32_t slaveId, uint32_t addr, uint64_t *data)
{
    uint32_t rd_val[2];
    uint32_t wait = IND_ACC_RETRY;
    int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_RD | IND_ACC_TX_SZ_64));

    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val[0]);
    } while (--wait && !(rd_val[0] & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val[0]);
        retVal = BCM_ERR_TIME_OUT;
        goto done;
    }

    SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, &rd_val[1]);
    SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_h_l16, &rd_val[0]);

    *data = ((uint64_t)rd_val[0] << 32) | rd_val[1];

done:
    return retVal;
}

static int32_t SPI_Write64Indirect(uint32_t slaveId, uint32_t addr, uint64_t data)
{
    uint32_t rd_val;
    uint32_t wait = IND_ACC_RETRY;
    int32_t retVal = BCM_ERR_OK;

    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->addr_ser_l16, addr);
    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_l_l16, (uint32_t)(data & 0xFfFfFfFf));
    SPI_Write32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->data_ser_h_l16, (uint32_t)(data >> 32));
    SPI_Write16(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, (IND_ACC_COMMIT | IND_ACC_NO_AUTO_INC | IND_ACC_WR | IND_ACC_TX_SZ_64));
    do {
        SPI_Read32(slaveId, (uint32_t)(intptr_t)&CPU_INDIRECT_RegBase->ctrl_ser_l16, &rd_val);
    } while (--wait && !(rd_val & IND_ACC_DONE));
    if (!wait) {
        HOST_Log("Ind. Wr. timed out ctrl 0x%x\n", rd_val);
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}

int32_t SPI_XferRead(uint32_t slaveId, HIPC_BusAccessType access, uint32_t addr,
                        uint8_t *data, uint32_t width)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    switch (width) {
    case 8UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Read8Indirect(slaveId, addr, data);
        }
        break;
    case 16UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Read16Indirect(slaveId, addr, (uint16_t *)data);
        } else if (HIPC_BUS_ACCESS_DIRECT == access) {
            retVal = SPI_Read16(slaveId, addr, (uint16_t *)data);
        }
        break;
    case 32UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Read32Indirect(slaveId, addr, (uint32_t *)data);
        } else if (HIPC_BUS_ACCESS_DIRECT == access) {
            retVal = SPI_Read32(slaveId, addr, (uint32_t *)data);
        }
        break;
    case 64UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Read64Indirect(slaveId, addr, (uint64_t *)data);
        }
        break;
    default:
        break;
    }
    return retVal;
}

int32_t SPI_XferWrite(uint32_t slaveId, HIPC_BusAccessType access, uint32_t addr,
                        uint8_t *data, uint32_t width)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    switch (width) {
    case 8UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Write8Indirect(slaveId, addr, *data);
        }
        break;
    case 16UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Write16Indirect(slaveId, addr, *(uint16_t *)data);
        } else if (HIPC_BUS_ACCESS_DIRECT == access) {
            retVal = SPI_Write16(slaveId, addr, *(uint16_t *)data);
        }
        break;
    case 32UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Write32Indirect(slaveId, addr, *(uint32_t *)data);
        } else if (HIPC_BUS_ACCESS_DIRECT == access) {
            retVal = SPI_Write32(slaveId, addr, *(uint32_t *)data);
        }
        break;
    case 64UL:
        if (HIPC_BUS_ACCESS_INDIRECT == access) {
            retVal = SPI_Write64Indirect(slaveId, addr, *(uint64_t *)data);
        }
        break;
    default:
        break;
    }
    return retVal;
}
