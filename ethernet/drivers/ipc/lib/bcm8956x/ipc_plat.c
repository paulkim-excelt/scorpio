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
#include <bl_downloader.h>
#include <bl_ipc_downloader.h>
#include <bl_chip_config.h>
#include <ipc.h>
#include <ipc_osil.h>
#include "../common/ipc_plat.h"
#include <ipc_hwif.h>
#if defined(ENABLE_IPC_S2M_INTR)
#include <chipmisc_rdb.h>
#endif /* defined(ENABLE_IPC_S2M_INTR) */

#define IPC_MEM_BASE        (0x2003C000)

#if defined(ENABLE_IPC_S2M_INTR)
#define CFG_CPU_INTR_BIT_11_RESERVED_MASK   (0x800UL)
static CHIPMISC_RDBType *const CHIPMISC_REGS = (CHIPMISC_RDBType *)CHIPMISC_BASE;
#endif /* defined(ENABLE_IPC_S2M_INTR) */

static uint16_t IPC_CalcEvenParity(uint16_t val)
{
    uint16_t par = 0U;
    uint16_t i = sizeof(val) * 8U;

    while (i--) {
        par ^= ((val >> i) & 0x1U);
    }
    return (par & 0x1U);
}

int32_t IPC_PlatEnableIntr(IPC_ChannIDType aID)
{
#if defined(ENABLE_IPC_S2M_INTR)
    if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
        /* Configure CFG registers. MASK bit should be cleared for force to take effect.
           TODO: Shall move to cfg_rdb structure */
        *(volatile uint32_t *)CFG_CPU_INTR_MASK0 &= ~(CFG_CPU_INTR_BIT_11_RESERVED_MASK);

        /* Enable Pad to be used for signalling Master */
        CHIPMISC_REGS->cpusys_misc |= CHIPMISC_CPUSYS_MISC_INTR_OUT_EN_PAD_MASK;
    }
#endif /* defined(ENABLE_IPC_S2M_INTR) */

    return BCM_ERR_OK;
}

int32_t IPC_PlatSetIntr(IPC_ChannIDType aID)
{
    uint16_t reg;
    uint32_t busHwID = IPC_ChannCfg[aID].busInfo.hwID;
    uint8_t slaveID = IPC_ChannCfg[aID].busInfo.slaveID;
    int32_t ret = BCM_ERR_OK;

    if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
        /* write to MISC_CPUSYS_MISC register of the respective
         * slave device
         */
        ret = IPC_ChannCfg[aID].busInfo.fnTbl->read(busHwID, slaveID, (uint32_t)MISC_CPUSYS_MISC, (uint8_t *)&reg, sizeof(uint16_t),
                IPC_ACCESS_WIDTH_16);
        if (BCM_ERR_OK == ret) {
            reg |= MISC_CPUSYS_MISC_SOFT_INTR_MASK;
            ret = IPC_ChannCfg[aID].busInfo.fnTbl->write(busHwID, slaveID, (uint32_t)MISC_CPUSYS_MISC, (uint8_t *)&reg, sizeof(uint16_t),
                    IPC_ACCESS_WIDTH_16);
        }
#if defined(ENABLE_IPC_S2M_INTR)
    } else if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
        /* Toggle INTR_B Pin. TODO: Shall move to cfg_rdb structure */
        *(volatile uint32_t *)CFG_CPU_INTR_FORCE0 ^= (CFG_CPU_INTR_BIT_11_RESERVED_MASK);
#endif /* defined(ENABLE_IPC_S2M_INTR) */
    }
    return ret;
}

void IPC_PlatClearIntr(IPC_ChannIDType aID)
{
    if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[aID].mode) {
        *(volatile uint16_t *)MISC_CPUSYS_MISC &= ~(MISC_CPUSYS_MISC_SOFT_INTR_MASK);;
    }
}

int32_t IPC_PlatSetBuffInfo(IPC_ChannIDType aID, uint32_t aSize, uint32_t aCount)
{
    int32_t ret = BCM_ERR_OK;
    uint16_t regVal16;
    /* Setup spare registers */
    regVal16 = (uint16_t)((IPC_MEM_BASE >> IPC_BUFF_INFO_BASE_ALIGN_SHIFT)
            << IPC_BUFF_INFO_BASE_SHIFT);

    regVal16 |= ((aSize << IPC_BUFF_INFO_SZ_SHIFT)
            & IPC_BUFF_INFO_SZ_MASK);
    regVal16 |= ((aCount << IPC_BUFF_INFO_CNT_SHIFT)
            & IPC_BUFF_INFO_CNT_MASK);
    regVal16 |= ((IPC_CalcEvenParity(regVal16) << IPC_BUFF_INFO_PAR_SHIFT)
            & IPC_BUFF_INFO_PAR_MASK);
    *IPC_BUFF_INFO_REG = regVal16;

    /* IPC memory address bit [19-31] are extracted and stored */
    regVal16 = (uint16_t)((IPC_MEM_BASE >> IPC_BUFF_INFO2_BASE_ALIGN_SHIFT)
            << IPC_BUFF_INFO2_BASE_SHIFT);
    regVal16 |= ((IPC_CalcEvenParity(regVal16) << IPC_BUFF_INFO2_PAR_SHIFT)
            & IPC_BUFF_INFO2_PAR_MASK);
    *IPC_BUFF_INFO2_REG = regVal16;

    return ret;
}

int32_t IPC_PlatGetBuffInfo(IPC_ChannIDType aID, uint8_t **const aBuff, uint16_t *const aCnt,
        uint8_t *const aCntRollOverMask, uint16_t *const aSize)
{
    uint16_t reg1Val16;
    uint16_t reg2Val16;
    uint8_t busHwID = IPC_ChannCfg[aID].busInfo.hwID;
    uint16_t slaveID = IPC_ChannCfg[aID].busInfo.slaveID;
    int32_t ret = BCM_ERR_OK;

    if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
        ret = IPC_ChannCfg[aID].busInfo.fnTbl->read(busHwID, slaveID, (uint32_t)IPC_BUFF_INFO_REG,
                (uint8_t *)&reg1Val16, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);
        if (BCM_ERR_OK != ret) {
            goto err;
        }

        ret = IPC_ChannCfg[aID].busInfo.fnTbl->read(busHwID, slaveID, (uint32_t)IPC_BUFF_INFO2_REG,
                (uint8_t *)&reg2Val16, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);
        if (BCM_ERR_OK != ret) {
            goto err;
        }
    } else {
        reg1Val16 = *IPC_BUFF_INFO_REG;
        reg2Val16 = *IPC_BUFF_INFO2_REG;
    }

    if ((reg1Val16 != 0xFFFF) && (reg1Val16 != 0)
            && (IPC_CalcEvenParity(reg1Val16) == 0U)
            && (IPC_CalcEvenParity(reg2Val16) == 0U)) {
        *aCnt = 1 << ((reg1Val16 & IPC_BUFF_INFO_CNT_MASK) >> IPC_BUFF_INFO_CNT_SHIFT);
        *aSize = 1 << ((reg1Val16 & IPC_BUFF_INFO_SZ_MASK) >> IPC_BUFF_INFO_SZ_SHIFT);

        *aBuff = (uint8_t *)(intptr_t)((((reg1Val16 & IPC_BUFF_INFO_BASE_MASK)
                        >> IPC_BUFF_INFO_BASE_SHIFT)
                    << IPC_BUFF_INFO_BASE_ALIGN_SHIFT)
                | (((reg2Val16 & IPC_BUFF_INFO2_BASE_MASK)
                        >> IPC_BUFF_INFO2_BASE_SHIFT)
                    << IPC_BUFF_INFO2_BASE_ALIGN_SHIFT));

        if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[aID].mode) {
            ret = IPC_ChannCfg[aID].busInfo.fnTbl->read(busHwID, slaveID,
                    (uint32_t)BL_DWNLD_TARGET_SPARE_REG, (uint8_t *)&reg1Val16, sizeof(uint16_t), IPC_ACCESS_WIDTH_16);

            if ((1U == *aCnt)    /* BROM and BL */
                    && (BL_IPC_DWNLD_BL_READY_MASK != (reg1Val16 & BL_IPC_DWNLD_BL_READY_MASK))) {
                /* BootROM only */
                *aCntRollOverMask = 0x1U; /* 0-1 */
            } else {
                *aCntRollOverMask = 0xFU; /* 0-15 */
            }
        } else {
            *aCntRollOverMask = 0xFU; /* 0-15 */
        }
    } else {
        *aCnt = 0UL;
        *aSize = 0UL;
        *aBuff = NULL;
        ret = BCM_ERR_UNINIT;
    }

err:
    return ret;
}
