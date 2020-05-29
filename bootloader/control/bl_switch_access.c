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
#include <bl_utils.h>
#include <eth_switch.h>
#include <chip_config.h>
#include <bcm8953x_switch.h>
#include <etherswt_ipc.h>
#include <switch_cfg.h>

#define SWITCH_RDWR_TIMEOUT             (4UL * 8000UL)

static IND_ACC_RegsType *const IND_ACC_REGS = (IND_ACC_RegsType *)IND_ACC_BASE;

static int32_t BL_Switch_ReadReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aPage,
        uint32_t aOffset,
        uint64_t *const aVal)
{
    uint16_t reg;
    uint32_t addr;
    uint32_t count = 0UL;
    int32_t ret = BCM_ERR_OK;

    if ((aPage < ETH_SWT_REGS_MAX_PAGES) &&
            (aOffset < ETH_SWT_REGS_PAGE_SIZE)) {
        addr = ETH_SWT_BASE + (aPage * ETH_SWT_REGS_PAGE_SIZE) + aOffset;

        /* write address to ADDR_L16 and ADDR_H16 registers */
        IND_ACC_REGS->ADDR_CPU_L16 = (addr & ADDR_CPU_ADDRESS_MASK);
        IND_ACC_REGS->ADDR_CPU_H16 = (addr >> 16UL) & ADDR_CPU_ADDRESS_MASK;

        /* set acesss width to 64-bit and commit a read */
        IND_ACC_REGS->CTRL_CPU_L16 = TRANS_SZ_QWORD | CTRL_CPU_L16_COMMIT_MASK;

        /* wait for read to complete */
        do {
            reg = IND_ACC_REGS->CTRL_CPU_L16;
            if ((reg & CTRL_CPU_L16_DONE_MASK) != 0U) {
                break;
            }
        } while ((count++ < SWITCH_RDWR_TIMEOUT));

        if (count < SWITCH_RDWR_TIMEOUT) {
            *aVal = ((uint64_t)IND_ACC_REGS->DATA_CPU_L_L16 |
                    (uint64_t)IND_ACC_REGS->DATA_CPU_L_H16 << 16U |
                    (uint64_t)IND_ACC_REGS->DATA_CPU_H_L16 << 32U |
                    (uint64_t)IND_ACC_REGS->DATA_CPU_H_H16 << 48U);
        } else {
            ret = BCM_ERR_TIME_OUT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

static int32_t BL_Switch_WriteReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aPage,
        uint32_t aOffset,
        uint64_t aVal)
{
    uint32_t addr;
    uint16_t reg;
    uint32_t count = 0UL;
    int32_t ret = BCM_ERR_OK;

    if ((aPage < ETH_SWT_REGS_MAX_PAGES) &&
            (aOffset < ETH_SWT_REGS_PAGE_SIZE)) {
        addr = ETH_SWT_BASE + (aPage * ETH_SWT_REGS_PAGE_SIZE) + aOffset;

        /* write the value to data registers */
        IND_ACC_REGS->DATA_CPU_L_L16 = (uint16_t)(aVal & DATA_CPU_DATA_MASK);
        IND_ACC_REGS->DATA_CPU_L_H16 = (uint16_t)((aVal >> 16U) & DATA_CPU_DATA_MASK);
        IND_ACC_REGS->DATA_CPU_H_L16 = (uint16_t)((aVal >> 32U) & DATA_CPU_DATA_MASK);
        IND_ACC_REGS->DATA_CPU_H_H16 = (uint16_t)((aVal >> 48U) & DATA_CPU_DATA_MASK);

        /* write address to ADDR_L16 and ADDR_H16 registers */
        IND_ACC_REGS->ADDR_CPU_L16 = (addr & ADDR_CPU_ADDRESS_MASK);
        IND_ACC_REGS->ADDR_CPU_H16 = (addr >> 16UL) & ADDR_CPU_ADDRESS_MASK;

        /* set acesss width to 64-bit and commit a write */
        IND_ACC_REGS->CTRL_CPU_L16 = (TRANS_SZ_QWORD | CPU_WRITE |
                CTRL_CPU_L16_COMMIT_MASK);

        /* wait for write to complete */
        do {
            reg = IND_ACC_REGS->CTRL_CPU_L16;
            if ((reg & CTRL_CPU_L16_DONE_MASK) != 0U) {
                break;
            }
        } while ((count++ < SWITCH_RDWR_TIMEOUT));

        if (count >= SWITCH_RDWR_TIMEOUT) {
            ret = BCM_ERR_TIME_OUT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t BL_IPC_EthSwtRead(ETHERSWT_RegAccessType* aEthSwtAccess, uint32_t * aLen)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t idx = 0UL;
    uint32_t pageID = 0UL;
    uint32_t offsetID = 0UL;

    /* Command payload size check */
    if(sizeof(ETHERSWT_RegAccessType) != *aLen){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Clear the memory region where data needs to be updated */
        BL_BCM_MemSet(aEthSwtAccess->data, 0U,
                   sizeof(aEthSwtAccess->data));

        /* Fetch the page and offset value from input address */
        pageID = (aEthSwtAccess->addr >> 8UL) & 0xFFUL;
        offsetID = aEthSwtAccess->addr & 0xFFUL;

        /* For the specified length, read the data */
        for(idx = 0UL; idx < aEthSwtAccess->len; idx++){
            uint64_t regVal;
            retVal = BL_Switch_ReadReg(ETH_SWITCH_HW_ID, pageID, offsetID,
                                      &regVal);
            if(BCM_ERR_OK != retVal){
                break;
            }

            BL_BCM_MemCpy(&aEthSwtAccess->data[idx*8UL],&regVal,
                       sizeof(uint64_t));

            /* Increment offset ID value for 64 bit register */
            offsetID = offsetID + sizeof(uint64_t);
        }
    }
    return retVal;
}

int32_t BL_IPC_EthSwtWrite(ETHERSWT_RegAccessType* aEthSwtAccess, uint32_t * aLen)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t idx = 0UL;
    uint32_t pageID = 0UL;
    uint32_t offsetID = 0UL;

    /* Command payload size check */
    if(sizeof(ETHERSWT_RegAccessType) != *aLen){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Fetch the page and offset value from specified address */
        pageID = (aEthSwtAccess->addr >> 8UL) & 0xFFUL;
        offsetID = aEthSwtAccess->addr & 0xFFUL;

        /* For the specified length, write the data */
        for(idx = 0UL; idx < aEthSwtAccess->len; idx++){
            uint64_t regVal;

            BCM_MemCpy(&regVal, &aEthSwtAccess->data[idx*8UL],
                       sizeof(uint64_t));

            retVal = BL_Switch_WriteReg(ETH_SWITCH_HW_ID, pageID, offsetID, regVal);
            if(BCM_ERR_OK != retVal){
                break;
            }
            /* Increment offset ID value for 64 bit register */
            offsetID = offsetID + sizeof(uint64_t);
        }
    }
    return retVal;
}