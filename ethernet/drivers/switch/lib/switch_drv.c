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
#include <stdint.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <utils.h>
#include <atomic.h>
#include <compiler.h>
#include <eth_switch_osil.h>
#include <eth_osil.h>
#include <switch_rdb.h>
#include <cfg_rdb.h>
#include <cpu_indirect_rdb.h>
#include "eth_cntlr.h"
#ifdef __BCM8956X__
#include "p1588_rdb.h"
#endif

#define VLAN_MEM_CFG_MAC_ADDR_LIST_SZ \
    (8UL * (sizeof((ETHERSWT_VLANMemberCfgType *)0)->macAddrList))

typedef struct {
    uint64_t mac;
    uint16_t vlanID;
    uint16_t portMask;
    uint8_t  priority;
} ARLTBL_HWEntryType;

typedef struct {
    uint16_t  vlan;
    uint16_t  fwdMap;
    uint16_t  untagMap;
} VLANTBL_HWEntryType;

typedef struct {
    ETHERSWT_CfgType const *config;
    uint32_t               mirrorPortID;
    uint32_t               portLinkStatus;
#define IS_PORT_LINK_STATUS_CHANGED_MASK    (0x0000FFFFUL)
#define IS_PORT_LINK_STATUS_CHANGED_SHIFT   (0UL)
#define PORT_LINK_STATUS_MASK               (0xFFFF0000UL)
#define PORT_LINK_STATUS_SHIFT              (16UL)
    uint32_t               portLinkStateChngCnt[ETHERSWT_PORT_ID_MAX];
    uint32_t               port2TimeFifoMap[ETHERSWT_PORT_ID_MAX];
} SwitchDrv_RWDataType;

#define VLAN_ID_MASK                    (0xFFFU)
#define VLAN_TBL_CMD_READ               (0x0UL)
#define VLAN_TBL_CMD_WRITE              (0x1UL)

#define ARL_TBL_CMD_READ                (0x0UL)
#define ARL_TBL_CMD_WRITE               (0x1UL)
#define ARL_TBL_INVAL_BIN_IDX           (0xFFUL)

#define SWITCH_RDWR_TIMEOUT             (4UL * 8000UL)
#define MAC_ADDR_SIZE                   (6UL)

#define NMAC2HWMAC(macAddr)             \
    (((uint64_t)macAddr[0] << 40UL) |\
     ((uint64_t)macAddr[1] << 32UL) |\
     ((uint64_t)macAddr[2] << 24UL) |\
     ((uint64_t)macAddr[3] << 16UL) |\
     ((uint64_t)macAddr[4] << 8UL) |\
     ((uint64_t)macAddr[5] << 0UL))

#define HWMAC2NMAC(hwMac, nMac)           \
    nMac[0U] = ((uint8_t)(hwMac >> 40UL & 0xFFU));\
    nMac[1U] = ((uint8_t)(hwMac >> 32UL & 0xFFU)) ;\
    nMac[2U] = ((uint8_t)(hwMac >> 24UL & 0xFFU)) ;\
    nMac[3U] = ((uint8_t)(hwMac >> 16UL & 0xFFU)) ;\
    nMac[4U] = ((uint8_t)(hwMac >> 8UL & 0xFFU))  ;\
    nMac[5U] = ((uint8_t)(hwMac >> 0UL & 0xFFU));

#define PORTID2HWMASK(portID)           (0x1UL << (portID))

#define ERR_EXIT(fn_ret)                \
    if (((ret) = (fn_ret)) != BCM_ERR_OK) { \
        SwitchDrv_ReportError(0UL, 0xFFUL, fn_ret, 0UL, 0UL, 0UL, __LINE__);\
        goto err_exit;}
#define ERR_BREAK(fn_ret)               \
    if ((ret = (fn_ret)) != BCM_ERR_OK) { break;}

static CFG_RDBType *const CFG_REGS = (CFG_RDBType *)CFG_BASE;
#define SWITCH_INTR_CLR_ALL_PORT_LINK       (0x40FFUL)

static CPU_INDIRECT_RDBType *const CPU_INDIRECT_REGS =
                                        (CPU_INDIRECT_RDBType *)CPU_INDIRECT_BASE;

static SWITCH_RDBType *const SWITCH_REGS = (SWITCH_RDBType *)SWITCH_BASE;

static SwitchDrv_RWDataType COMP_SECTION(".data.drivers") SwitchDrv_Data = {
    .config = NULL,
    .mirrorPortID = ETHERSWT_PORT_ID_MAX,
    .portLinkStatus = 0UL, /* All port link status fail */
    .portLinkStateChngCnt[0UL] = 0UL,
    .portLinkStateChngCnt[1UL] = 0UL,
    .portLinkStateChngCnt[2UL] = 0UL,
    .portLinkStateChngCnt[3UL] = 0UL,
    .portLinkStateChngCnt[4UL] = 0UL,
    .portLinkStateChngCnt[5UL] = 0UL,
    .portLinkStateChngCnt[6UL] = 0UL,
    .portLinkStateChngCnt[7UL] = 0UL,
    .portLinkStateChngCnt[8UL] = 0UL,
    .port2TimeFifoMap[0UL] = 0x00UL,
    .port2TimeFifoMap[1UL] = 0x01UL,
    .port2TimeFifoMap[2UL] = 0x02UL,
    .port2TimeFifoMap[3UL] = 0x03UL,
    .port2TimeFifoMap[4UL] = 0x04UL,
    .port2TimeFifoMap[5UL] = 0x14UL,
    .port2TimeFifoMap[6UL] = 0x13UL,
    .port2TimeFifoMap[7UL] = 0xFFUL, /* Invalid port */
    .port2TimeFifoMap[8UL] = 0x12UL,
};

#define SWITCHDRV_TX_BUF_INFO_Q_SIZE        (128UL)
#define SWITCHDRV_RX_BUF_INFO_Q_SIZE        (32UL)

typedef uint32_t SwitchDrv_BufStateType;
#define SWITCHDRV_BUF_STATE_FREE            (0UL)
#define SWITCHDRV_BUF_STATE_ADAPTED         (1UL)
#define SWITCHDRV_BUF_STATE_MGMT_INFO_SET   (2UL)
#define SWITCHDRV_BUF_STATE_PROCESSED       (3UL)

typedef struct {
    SwitchDrv_BufStateType bufState;
    ETHERSWT_MgmtInfoType mgmtInfo;
    const uint8_t * buf;
    uint32_t isTSEnabled;
} SwitchDrv_BufInfoType;

static SwitchDrv_BufInfoType COMP_SECTION(".data.drivers")
    SwitchDrv_TxBufInfoQ[SWITCHDRV_TX_BUF_INFO_Q_SIZE] = {{0}};
static SwitchDrv_BufInfoType COMP_SECTION(".data.drivers")
    SwitchDrv_RxBufInfoQ[SWITCHDRV_RX_BUF_INFO_Q_SIZE] = {{0}};

static void SwitchDrv_ReportError(uint32_t aInstanceID, uint8_t aApiID,
        int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_SWT_ID, (uint8_t)aInstanceID, aApiID,
            aErr, 4UL, values);
}

int32_t SwitchDrv_ReadReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aAddr,
        uint64_t *const aVal)
{
    uint16_t reg;
    uint32_t count = 0UL;
    int32_t ret = BCM_ERR_OK;

    if (0UL != aAddr) {

        /* write address to ADDR_L16 and ADDR_H16 registers */
        CPU_INDIRECT_REGS->addr_cpu_l16 =
            (aAddr & CPU_INDIRECT_RDB_IND_REGS_ADDR_L16_ADDRESS_L16_MASK);
        CPU_INDIRECT_REGS->addr_cpu_h16 =
            ((aAddr >> 16UL) & CPU_INDIRECT_RDB_IND_REGS_ADDR_H16_ADDRESS_H16_MASK);

        /* set acesss width to 64-bit and commit a read */
        CPU_INDIRECT_REGS->ctrl_cpu_l16 = CPU_INDIRECT_TRANS_SZ_QWORD
                            | CPU_INDIRECT_RDB_IND_REGS_CTRL_L16_COMMIT_MASK;

        /* wait for read to complete */
        do {
            reg = CPU_INDIRECT_REGS->ctrl_cpu_l16;
            if ((reg & CPU_INDIRECT_RDB_IND_REGS_CTRL_L16_DONE_MASK) != 0U) {
                break;
            }
        } while ((count++ < SWITCH_RDWR_TIMEOUT));

        if (count < SWITCH_RDWR_TIMEOUT) {
            *aVal = ((uint64_t)CPU_INDIRECT_REGS->data_cpu_l_l16 |
                    (uint64_t)CPU_INDIRECT_REGS->data_cpu_l_h16 << 16U |
                    (uint64_t)CPU_INDIRECT_REGS->data_cpu_h_l16 << 32U |
                    (uint64_t)CPU_INDIRECT_REGS->data_cpu_h_h16 << 48U);
        } else {
            ret = BCM_ERR_TIME_OUT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t SwitchDrv_WriteReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aAddr,
        uint64_t aVal)
{
    uint16_t reg;
    uint32_t count = 0UL;
    int32_t ret = BCM_ERR_OK;

    if (0UL != aAddr) {

        /* write the value to data registers */
        CPU_INDIRECT_REGS->data_cpu_l_l16 =
            (uint16_t)(aVal & CPU_INDIRECT_RDB_IND_REGS_DATA_L_L16_L16_MASK);
        CPU_INDIRECT_REGS->data_cpu_l_h16 =
            (uint16_t)((aVal >> 16U) & CPU_INDIRECT_RDB_IND_REGS_DATA_L_H16_H16_MASK);
        CPU_INDIRECT_REGS->data_cpu_h_l16 =
            (uint16_t)((aVal >> 32U) & CPU_INDIRECT_RDB_IND_REGS_DATA_H_L16_L16_MASK);
        CPU_INDIRECT_REGS->data_cpu_h_h16 =
            (uint16_t)((aVal >> 48U) & CPU_INDIRECT_RDB_IND_REGS_DATA_H_H16_H16_MASK);

        /* write address to ADDR_L16 and ADDR_H16 registers */
        CPU_INDIRECT_REGS->addr_cpu_l16 =
            (aAddr & CPU_INDIRECT_RDB_IND_REGS_ADDR_L16_ADDRESS_L16_MASK);
        CPU_INDIRECT_REGS->addr_cpu_h16 =
            (aAddr >> 16UL) & CPU_INDIRECT_RDB_IND_REGS_ADDR_H16_ADDRESS_H16_MASK;

        /* set acesss width to 64-bit and commit a write */
        CPU_INDIRECT_REGS->ctrl_cpu_l16 = (CPU_INDIRECT_TRANS_SZ_QWORD
                | (CPU_INDIRECT_CPU_WRITE << CPU_INDIRECT_CTRL_CPU_L16_RDB_WR_SHIFT)
                | CPU_INDIRECT_RDB_IND_REGS_CTRL_L16_COMMIT_MASK);

        /* wait for write to complete */
        do {
            reg = CPU_INDIRECT_REGS->ctrl_cpu_l16;
            if ((reg & CPU_INDIRECT_RDB_IND_REGS_CTRL_L16_DONE_MASK) != 0U) {
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

int32_t SwitchDrv_SetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode)
{
    int ret = BCM_ERR_OK;
    uint64_t portMaskVal;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                (uint32_t)&SWITCH_REGS->m40_jumbo_port_mask,
                &portMaskVal));
    if (ETHXCVR_BOOLEAN_TRUE== aMode) {
        portMaskVal |= (PORTID2HWMASK(aPortID)
                        & SWITCH_P40JPM_PAGE_40_JUMBO_PORT_MASK_JUMBO_FM_PORT_MASK_MASK);
        portMaskVal |= SWITCH_P40JPM_PAGE_40_JUMBO_PORT_MASK_EN_10_100_JUMBO_MASK;
    } else {
        portMaskVal &= ~(PORTID2HWMASK(aPortID));
    }
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m40_jumbo_port_mask, portMaskVal));
err_exit:
    return ret;
}

int32_t SwitchDrv_GetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        uint32_t *const aMode)
{
    int ret = BCM_ERR_OK;
    uint64_t reg;
    uint64_t portMaskVal = 0ULL;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                (uint32_t)&SWITCH_REGS->m40_jumbo_port_mask,
                &reg));
    portMaskVal |= (PORTID2HWMASK(aPortID)
                    & SWITCH_P40JPM_PAGE_40_JUMBO_PORT_MASK_JUMBO_FM_PORT_MASK_MASK);
    portMaskVal |= SWITCH_P40JPM_PAGE_40_JUMBO_PORT_MASK_EN_10_100_JUMBO_MASK;
    if ((reg & portMaskVal) == portMaskVal) {
        *aMode = ETHXCVR_BOOLEAN_TRUE;
    } else {
        *aMode = ETHXCVR_BOOLEAN_FALSE;
    }
err_exit:
    return ret;
}
static int32_t SwitchDrv_SetPortRole(ETHERSWT_HwIDType aSwtID,
                                    ETHERSWT_SwitchType aSwtType,
                                    ETHERSWT_PortIDType aPortID,
                                    ETHERSWT_PortType aRole)
{
    int ret = BCM_ERR_OK;
    uint64_t reg;

    if (ETHERSWT_SWITCH_STANDARD == aSwtType) {
        if (SWITCH_LIGHTSTACK_HOST_PORT == aPortID) {
            if (ETHERSWT_UP_LINK_PORT == aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            } else {
                ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            &reg));
                if (ETHERSWT_STANDARD_PORT == aRole) {
                    /* Standard Port: Enable dumb forward mode */
                    reg |= SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
                } else {
                    /* Host Port: Disable dumb forward mode */
                    reg &= ~SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
                }
                ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            reg));
            }
        } else {
            if (ETHERSWT_STANDARD_PORT != aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
    } else if (ETHERSWT_SWITCH_LIGHTSTACK_MASTER == aSwtType) {
        if (SWITCH_LIGHTSTACK_HOST_PORT == aPortID) {
            if (ETHERSWT_HOST_PORT == aRole) {
                /* Disable dumb forward mode for port 8 */
                ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            &reg));
                reg &= ~SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
                ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            reg));
            } else if (ETHERSWT_STANDARD_PORT == aRole) {
                /* Enable dumb forwarding */
                ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            &reg));
                reg |= SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
                ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            reg));
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else if (6UL == aPortID) {
            if (ETHERSWT_HOST_PORT == aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else if (5UL == aPortID) {
            if (ETHERSWT_HOST_PORT == aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else {
            if (ETHERSWT_STANDARD_PORT != aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
    } else if (ETHERSWT_SWITCH_LIGHTSTACK_SLAVE == aSwtType) {
        if (SWITCH_LIGHTSTACK_HOST_PORT == aPortID) {
            if (ETHERSWT_UP_LINK_PORT == aRole) {
                /* Enable dumb forward mode for port 8 */
                ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            &reg));
                reg |= SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
                ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                            (uint32_t)&SWITCH_REGS->m00_switch_ctrl,
                            reg));
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else if (6UL == aPortID) {
            if (ETHERSWT_HOST_PORT == aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else if (5UL == aPortID) {
            if (ETHERSWT_HOST_PORT == aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else {
            if (ETHERSWT_STANDARD_PORT != aRole) {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
    }

err_exit:
    return ret;
}

static int32_t SwitchDrv_VLANTblRdWr(ETHERSWT_HwIDType aSwtID, uint16_t aVLAN,
        uint32_t *const aUntagMap, uint32_t *const aFwdMap, uint32_t aCmd)
{
    uint64_t reg;
    uint64_t cntrl = 0ULL;
    uint32_t timeout = 0UL;
    int ret = BCM_ERR_OK;

    if (((aUntagMap == NULL) || (aFwdMap == NULL)) ||
            ((aCmd != VLAN_TBL_CMD_READ) && (aCmd != VLAN_TBL_CMD_WRITE))) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /* write the VLAN_INDX register */
    reg = aVLAN & VLAN_ID_MASK;
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_vtbl_addr, reg));

    /* if its write operation, update the VTBL entry register first */
    if (aCmd == VLAN_TBL_CMD_WRITE) {
        /* write the forward map and untag map
         * set the forward mode based on ARL flow
         * and MSTP index to 0
         */
        reg = (*aFwdMap << SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_FWD_MAP_SHIFT)
                & SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_FWD_MAP_MASK;
        reg |= (*aUntagMap << SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_UNTAG_MAP_SHIFT)
                & SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_UNTAG_MAP_MASK;
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_vtbl_entry, reg));
        cntrl = (SWITCH_ARLA_VTBL_WRITE_CMD
                << SWITCH_P05AVR_PAGE_05_ARLA_VTBL_RWCTRL_ARLA_VTBL_RW_CLR_SHIFT);
        cntrl |= (SWITCH_P05AVR_PAGE_05_ARLA_VTBL_RWCTRL_ARLA_VTBL_STDN_MASK);
    } else {
        cntrl |= (SWITCH_ARLA_VTBL_READ_CMD
                << SWITCH_P05AVR_PAGE_05_ARLA_VTBL_RWCTRL_ARLA_VTBL_RW_CLR_SHIFT);
        cntrl |= SWITCH_P05AVR_PAGE_05_ARLA_VTBL_RWCTRL_ARLA_VTBL_STDN_MASK;
    }

    /* start the table operation */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_vtbl_rwctrl, cntrl));

    while (timeout < SWITCH_RDWR_TIMEOUT) {
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID,
                        (uint32_t)&SWITCH_REGS->m05_arla_vtbl_rwctrl,
                        &cntrl));
        if (0ULL ==
            (cntrl & SWITCH_P05AVR_PAGE_05_ARLA_VTBL_RWCTRL_ARLA_VTBL_STDN_MASK)) {
            break;
        }
        timeout++;
    }

    if (timeout == SWITCH_RDWR_TIMEOUT) {
        ret = BCM_ERR_TIME_OUT;
        goto err_exit;
    }

    if (aCmd == VLAN_TBL_CMD_READ) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m05_arla_vtbl_entry,
                    &reg));

        /* in hardware bits[7:0] = ports 0-7 and bit[8] = IMP port (port 8) */
        *aFwdMap = ((reg & SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_FWD_MAP_MASK)
                >> SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_FWD_MAP_SHIFT);
        *aUntagMap = ((reg & SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_UNTAG_MAP_MASK)
                >> SWITCH_P05AVE_PAGE_05_ARLA_VTBL_ENTRY_UNTAG_MAP_SHIFT);
    }

err_exit:
    return ret;
}

static void SwitchDrv_VLANTblDump(ETHERSWT_HwIDType aSwtID)
{
#ifdef ENABLE_SWITCH_DRV_DEBUG
    int32_t ret;
    uint32_t i;
    uint32_t untagMap;
    uint32_t fwdMap;

    ULOG_INFO("***** SwitchDrv_VLANTblDump *****\n");
    ULOG_INFO("VLAN \tuntagMap \tfwdMap\t \n");
    for (i = 0; i <= 10; i++) {
        ret = SwitchDrv_VLANTblRdWr(aSwtID, i, &untagMap, &fwdMap,
                VLAN_TBL_CMD_READ);
        if (ret != BCM_ERR_OK) {
            break;
        }
        ULOG_INFO("%04x \t %02x \t %02x \t\n", i, untagMap, fwdMap);
    }
#endif
}

static int32_t SwitchDrv_ARLTblSearchByAddr(ETHERSWT_HwIDType aSwtID,
        uint64_t aMACAddr,
        uint32_t aVlanID,
        uint32_t *const aPortMask,
        uint32_t *const aBinIdx)
{
    uint64_t mac;
    uint32_t vid;
    uint64_t reg;
    uint32_t i;
    uint32_t macVidAddr;
    uint32_t fwdAddr;
    uint64_t fwdReg;
    uint32_t timeout = 0UL;

    int ret = BCM_ERR_OK;

    if ((aPortMask == NULL) || (aBinIdx == NULL)) {
        goto err_exit;
    }

    *aBinIdx = ARL_TBL_INVAL_BIN_IDX;
    *aPortMask = 0UL;

    /* write the MAC Address index register */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, SWITCH_PAGE_05_ARLA_MAC,
                aMACAddr & SWITCH_PAGE_05_ARLA_MAC_MAC_ADDR_INDX_MASK));

    /* write the VID index register */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->arla_regs_union_instance.arla_regs_struct_instance.PAGE_05_ARLA_VID,
                (uint64_t) (aVlanID & VLAN_ID_MASK)));

    reg = SWITCH_ARLA_RWCTL_ARL_READ;
    reg |= SWITCH_P05AR_PAGE_05_ARLA_RWCTL_ARL_STRTDN_MASK;
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_rwctl, reg));

    /* wait for STRDN to clear */
    while (timeout < SWITCH_RDWR_TIMEOUT) {
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_rwctl, &reg));
        if ((reg & SWITCH_P05AR_PAGE_05_ARLA_RWCTL_ARL_STRTDN_MASK) == 0ULL) {
            break;
        }
        timeout++;
    }

    if (timeout == SWITCH_RDWR_TIMEOUT) {
        ret = BCM_ERR_TIME_OUT;
        goto err_exit;
    }

    for (i = 0UL; i < SWITCH_ARL_TBL_BIN_SIZE; i++) {
        /* check if MAC is valid */
        macVidAddr = (uint32_t)&SWITCH_REGS->m05_arla_macvid_entry0 + i * 0x10;
        fwdAddr = macVidAddr + 0x8UL;
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, fwdAddr,
                    &fwdReg));
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, macVidAddr,
                    &reg));
        if ((fwdReg & SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_ARL_VALID_MASK) != 0UL) {
            mac = reg & SWITCH_P05AME0_PAGE_05_ARLA_MACVID_ENTRY0_ARL_MACADDR_MASK;
            vid = ((reg & SWITCH_P05AME0_PAGE_05_ARLA_MACVID_ENTRY0_VID_MASK)
                    >> SWITCH_P05AME0_PAGE_05_ARLA_MACVID_ENTRY0_VID_SHIFT);
            if ((mac == aMACAddr) && (vid == aVlanID)) {
                *aPortMask = ((fwdReg
                            & SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_PORTID_MASK)
                            >> SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_PORTID_SHIFT);
                *aBinIdx = i;
                break;
            }
        } else {
            if (*aBinIdx == ARL_TBL_INVAL_BIN_IDX) {
                *aBinIdx = i;
            }
        }
    }
    if (*aBinIdx == ARL_TBL_INVAL_BIN_IDX) {
        ret = BCM_ERR_NOT_FOUND;
    }

err_exit:
    return ret;
}

#if 0
static int32_t SwitchDrv_ARLTblSearchLinear(ETHERSWT_HwIDType aSwtID,
        ARLTBL_HWEntryType *const entry)
{
    uint64_t reg;
    uint64_t ent0;
    uint64_t ent1;
    uint64_t result0;
    uint64_t result1;
    uint64_t macAddr;
    uint32_t found = FALSE;
    int32_t ret = BCM_ERR_OK;

    if (entry == NULL)  {
        goto err_exit;
    }

    /* start the search operation by setting STDN bit */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_ctl,
            SWITCH_P05ASC_PAGE_05_ARLA_SRCH_CTL_ARLA_SRCH_STDN_MASK));
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_ctl, &reg));

    /* read the search result until valid */
    while ((reg & SWITCH_P05ASC_PAGE_05_ARLA_SRCH_CTL_ARLA_SRCH_STDN_MASK) != 0UL) {
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_0_macvid,
                    &ent0));
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_0,
                    &result0));
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_1_macvid,
                    &ent1));
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_1,
                    &result1));

        result0 |= SWITCH_P05ASR0_P05ASR0ASRV0_MASK;
        if ((result0 & SWITCH_P05ASR0_P05ASR0ASRV0_MASK) != 0ULL) {
            macAddr = ent0 & SWITCH_P05ASR0M_P05ASR0MASM0_MASK;
            if (entry->mac == macAddr) {
                entry->vlanID = (ent0 & SWITCH_P05ASR0M_P05ASR0MASRV0_MASK) >>
                    SWITCH_P05ASR0M_P05ASR0MASRV0_SHIFT;
                /* TODO: check if multicast addressing scheme is enabled or
                 * not if not bits[3:0] gives the unique port ID where this
                 * station is connected to
                 */
                entry->portMask = (result0 & SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_MASK) >>
                    SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_SHIFT;
                found = TRUE;
            }
        }

        result1 |= SWITCH_P05ASR0_P05ASR0ASRV0_MASK;
        if ((result1 & SWITCH_P05ASR0_P05ASR0ASRV0_MASK) != 0ULL) {
            macAddr = ent1 & SWITCH_P05ASR0M_P05ASR0MASM0_MASK;
            if (entry->mac == macAddr) {
                entry->vlanID = (ent1 & SWITCH_P05ASR0M_P05ASR0MASRV0_MASK) >>
                    SWITCH_P05ASR0M_P05ASR0MASRV0_SHIFT;
                entry->portMask = (result1 & SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_MASK) >>
                    SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_SHIFT;
                found = TRUE;
            }
        }
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_ctl, &reg));
    }

    if (found == FALSE) {
        ret = BCM_ERR_NOT_FOUND;
    }

err_exit:
    return ret;
}
#endif


static int32_t SwitchDrv_ARLTblAddEntry(ETHERSWT_HwIDType aSwtID,
        ARLTBL_HWEntryType *const aEntry)
{
    uint64_t macVidReg;
    uint64_t fwdReg;
    uint64_t cntrlReg;
    uint32_t macVidAddr;
    uint32_t fwdAddr;
    uint32_t portMask = 0UL;
    uint32_t binIdx = 0UL;
    uint32_t timeout = 0UL;
    int ret = BCM_ERR_OK;


    /* find if the entry already exits in the ARL table
     * if yes, replace that entry
     */
    ret = SwitchDrv_ARLTblSearchByAddr(aSwtID, aEntry->mac, aEntry->vlanID,
            &portMask, &binIdx);
    if (ret != BCM_ERR_OK) {
        if (ret == BCM_ERR_NOT_FOUND) {
            binIdx = 0UL;
            portMask = 0UL;
        }
    }

    macVidAddr = (uint32_t)&SWITCH_REGS->m05_arla_macvid_entry0 + binIdx * 0x10UL;
    fwdAddr = macVidAddr + 0x8UL;

    macVidReg = (aEntry->mac)
                & SWITCH_P05AME0_PAGE_05_ARLA_MACVID_ENTRY0_ARL_MACADDR_MASK;
    macVidReg |= (uint64_t)aEntry->vlanID
                << SWITCH_P05AME0_PAGE_05_ARLA_MACVID_ENTRY0_VID_SHIFT;

    fwdReg = aEntry->portMask
                & SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_PORTID_MASK;
    fwdReg |= SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_ARL_VALID_MASK;
    fwdReg |= SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_ARL_STATIC_MASK;
    fwdReg |= SWITCH_ARLA_FWD_ENTRY_ARL_MODE_FWD_MAP
                << SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_PORTID_SHIFT;

    /* Write the MACVID entry register */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, macVidAddr,
                macVidReg));
    /* write the FWD register */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, fwdAddr,
                fwdReg));

    /* start the write operation and wait for completion */
    cntrlReg = SWITCH_ARLA_RWCTL_ARL_WRITE;
    cntrlReg |= SWITCH_P05AR_PAGE_05_ARLA_RWCTL_ARL_STRTDN_MASK;
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_rwctl,
                cntrlReg));

    /* wait for STRDN to clear */
    while (timeout < SWITCH_RDWR_TIMEOUT) {
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_rwctl,
                    &cntrlReg));
        if ((cntrlReg
                & SWITCH_P05AR_PAGE_05_ARLA_RWCTL_ARL_STRTDN_MASK) == 0ULL) {
            break;
        }
        timeout++;
    }

    if (timeout == SWITCH_RDWR_TIMEOUT) {
        ret = BCM_ERR_TIME_OUT;
    }

err_exit:
    return ret;
}

static int32_t SwitchDrv_ARLTblDeleteEntry(ETHERSWT_HwIDType aSwtID,
        ARLTBL_HWEntryType *const aEntry)
{
    uint64_t fwdReg;
    uint64_t cntrlReg;
    uint32_t macVidAddr;
    uint32_t fwdAddr;
    uint32_t portMask = 0UL;
    uint32_t binIdx = 0UL;
    uint32_t timeout = 0UL;
    int ret = BCM_ERR_OK;


    /* find if the entry already exits in the ARL table
     * if yes, replace that entry
     */
    ret = SwitchDrv_ARLTblSearchByAddr(aSwtID, aEntry->mac, aEntry->vlanID,
            &portMask, &binIdx);
    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }

    macVidAddr = (uint32_t)&SWITCH_REGS->m05_arla_macvid_entry0 + binIdx * 0x10UL;
    fwdAddr = macVidAddr + 0x8UL;

    /* read the FWD register */
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, fwdAddr,
                &fwdReg));
    fwdReg &= ~SWITCH_P05AFE0_PAGE_05_ARLA_FWD_ENTRY0_ARL_VALID_MASK;
    /* write the FWD register */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, fwdAddr,
                fwdReg));

    /* start the write operation and wait for completion */
    cntrlReg = SWITCH_ARLA_RWCTL_ARL_WRITE;
    cntrlReg |= SWITCH_P05AR_PAGE_05_ARLA_RWCTL_ARL_STRTDN_MASK;
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_rwctl,
                cntrlReg));

    /* wait for STRDN to clear */
    while (timeout < SWITCH_RDWR_TIMEOUT) {
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_rwctl,
                    &cntrlReg));
        if ((cntrlReg
                & SWITCH_P05AR_PAGE_05_ARLA_RWCTL_ARL_STRTDN_MASK) == 0ULL) {
            break;
        }
        timeout++;
    }

    if (timeout == SWITCH_RDWR_TIMEOUT) {
        ret = BCM_ERR_TIME_OUT;
    }

err_exit:
    return ret;
}

static int32_t SwitchDrv_ARLTblConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType * const aConfig)
{
    uint32_t i, j, k;
    uint64_t mac;
    uint8_t *temp;
    int32_t ret = BCM_ERR_OK;
    ARLTBL_HWEntryType hwEntry;
    const ETHERSWT_PortCfgType *portCfg;
    const ETHERSWT_VLANMemberCfgType * vlanMemCfg;
    uint32_t isMacAddrFound[ETHERSWT_PORT_MAC_ENTY_MAX];

    for (i = 0UL; i < aConfig->portCfgListSz; i++) {
        memset(isMacAddrFound, 0x0, sizeof(isMacAddrFound));
        portCfg = &aConfig->portCfgList[i];
        /* Add ARL table entry for each VLAN member and MAC addresss */
        for (j = 0UL; j < portCfg->vlanMemListSz; j++) {
            vlanMemCfg = &portCfg->vlanMemList[j];
            hwEntry.vlanID = vlanMemCfg->vlanID;
            hwEntry.priority = vlanMemCfg->defaultPri;
            for (k = 0UL; k < VLAN_MEM_CFG_MAC_ADDR_LIST_SZ; k++) {
                if ((vlanMemCfg->macAddrList & (0x1U << k)) != 0U) {
                    isMacAddrFound[k] = 1UL;
                    temp = (uint8_t *)(&portCfg->fixedMacAddrList[k][0]);
                    mac = NMAC2HWMAC(temp);
                    hwEntry.mac = mac;
                    /* H/W field expects port number for unicast entry and a portmask */
                    /* for multicast entry                                            */
                    hwEntry.portMask = portCfg->portID;
                    if (0x1U == (temp[0U] & 0x1U)) {
                        /* Mcast entry. Read from the ARL table first  */
                        uint32_t portMask = 0UL;
                        uint32_t binIdx = 0UL;

                        ret = SwitchDrv_ARLTblSearchByAddr(aID,
                                mac, vlanMemCfg->vlanID, &portMask, &binIdx);
                        if ((ret != BCM_ERR_OK) && (ret != BCM_ERR_NOT_FOUND)) {
                            break;
                        }

                        hwEntry.portMask = portMask | ( 1UL << portCfg->portID);;
                    }
                    ret = SwitchDrv_ARLTblAddEntry(aID, &hwEntry);
                    if (ret != BCM_ERR_OK) {
                        break;
                    }
                }
            }
            if (ret != BCM_ERR_OK) {
                break;
            }
        }
        if (ret != BCM_ERR_OK) {
            break;
        }
        /* Add ARL entry for MAC address which are not part of any VLAN */
        for (j = 0UL; j < portCfg->macAddrListSz; j++) {
            if (0UL == isMacAddrFound[j]) {
                temp = (uint8_t *)(&portCfg->fixedMacAddrList[j][0]);
                mac = NMAC2HWMAC(temp);
                hwEntry.mac = mac;
                hwEntry.vlanID = 0UL;
                hwEntry.priority = 0UL;
                /* H/W field expects port number for unicast entry and a portmask */
                /* for multicast entry                                            */
                hwEntry.portMask = portCfg->portID;
                if (0x1U == (temp[0U] & 0x1U)) {
                    /* Mcast entry. Read from the ARL table first  */
                    uint32_t portMask = 0UL;
                    uint32_t binIdx = 0UL;

                    ret = SwitchDrv_ARLTblSearchByAddr(aID, mac, 0, &portMask, &binIdx);
                    if ((ret != BCM_ERR_OK) && (ret != BCM_ERR_NOT_FOUND)) {
                        break;
                    }

                    hwEntry.portMask = portMask | ( 1UL << portCfg->portID);;
                }
                ret = SwitchDrv_ARLTblAddEntry(aID, &hwEntry);
                if (ret != BCM_ERR_OK) {
                    break;
                }
            }
        }
        if (ret != BCM_ERR_OK) {
            break;
        }
    }

    return ret;
}

static int32_t SwitchDrv_IsARLEntryInitConfig(ETHERSWT_HwIDType aSwtID,
                                        ARLTBL_HWEntryType *const aHwEntry,
                                        uint32_t *const aIsInitARLEntry)
{
    uint32_t i, j, k;
    uint64_t mac;
    uint8_t *temp;
    int32_t ret = BCM_ERR_OK;
    ARLTBL_HWEntryType hwEntry;
    const ETHERSWT_PortCfgType *portCfg;
    const ETHERSWT_VLANMemberCfgType * vlanMemCfg;
    uint8_t isMacAddrFound[ETHERSWT_PORT_MAC_ENTY_MAX];
    const ETHERSWT_CfgType *swtConfig = SwitchDrv_Data.config;

    *aIsInitARLEntry = FALSE;

    for (i = 0UL; i < swtConfig->portCfgListSz; i++) {
        memset(isMacAddrFound, 0x0, sizeof(isMacAddrFound));
        portCfg = &swtConfig->portCfgList[i];
        for (j = 0UL; j < portCfg->vlanMemListSz; j++) {
            vlanMemCfg = &portCfg->vlanMemList[j];
            hwEntry.vlanID = vlanMemCfg->vlanID;
            hwEntry.portMask = portCfg->portID;
            hwEntry.priority = vlanMemCfg->defaultPri;
            for (k = 0UL; k < VLAN_MEM_CFG_MAC_ADDR_LIST_SZ; k++) {
                if ((vlanMemCfg->macAddrList & (0x1U << k)) != 0U) {
                    isMacAddrFound[k] = (uint8_t)1;
                    temp = (uint8_t *)(&portCfg->fixedMacAddrList[k][0]);
                    mac = NMAC2HWMAC(temp);
                    hwEntry.mac = mac;

                    /* Compare ARL hwEntry in config */
                    if ((aHwEntry->vlanID == hwEntry.vlanID)
                            && (aHwEntry->mac == hwEntry.mac)) {
                            *aIsInitARLEntry = TRUE;
                            break;
                    }
                }
            }
            if (TRUE == *aIsInitARLEntry) {
                break;
            }
        }
        if (TRUE == *aIsInitARLEntry) {
            break;
        }

        /* Search for ARL entry configuration of a MAC address, which are
         * not participating in any VLAN */
        for (j = 0UL; j < portCfg->macAddrListSz; j++) {
            if (((uint8_t)0) == isMacAddrFound[j]) {
                temp = (uint8_t *)(&portCfg->fixedMacAddrList[j][0]);
                mac = NMAC2HWMAC(temp);
                hwEntry.mac = mac;
                hwEntry.vlanID = 0UL;
                hwEntry.portMask = portCfg->portID;
                hwEntry.priority = 0UL;
                /* Compare ARL hwEntry in config */
                if (aHwEntry->mac == hwEntry.mac) {
                    *aIsInitARLEntry = TRUE;
                    break;
                }
            }
        }
        if (TRUE == *aIsInitARLEntry) {
            break;
        }
    }

    return ret;
}

static int32_t SwitchDrv_UpdateVLANTblEntry(ETHERSWT_HwIDType aSwtID,
                                        ETHERSWT_PortIDType aPortID,
                                        ETHERSWT_VLANIDType aVlanID,
                                        ETHERSWT_VLANFwrdType aForward)
{
    uint32_t untagMap = 0UL;
    uint32_t fwdMap = 0UL;
    int32_t ret = BCM_ERR_OK;

    if (aVlanID > ETHERSWT_VLANID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }
    /* read the current port map configured in the HW */
    ret = SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
            VLAN_TBL_CMD_READ);
    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }
    switch (aForward) {
        case ETHERSWT_VLAN_FRWRD_DONT_SEND:
            /* clear this port from forward mask */
            fwdMap &= ~PORTID2HWMASK(aPortID);
            untagMap &= ~PORTID2HWMASK(aPortID);
            break;
        case ETHERSWT_VLAN_FRWRD_TAGGED:
            fwdMap |= PORTID2HWMASK(aPortID);
            untagMap &= ~PORTID2HWMASK(aPortID);
            break;
        case ETHERSWT_VLAN_FRWRD_UNTAGGED:
            fwdMap |= PORTID2HWMASK(aPortID);
            untagMap |= PORTID2HWMASK(aPortID);
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }
    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }

    /* write the configuration to VLAN table */
    ret = SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
            VLAN_TBL_CMD_WRITE);

err_exit:
    return ret;
}

static int32_t SwitchDrv_VLANTblConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType * const aConfig)
{
    uint32_t i;
    uint32_t j;
    uint16_t vlanID;
    ETHERSWT_VLANFwrdType forward;
    const ETHERSWT_PortCfgType *portCfg;
    int32_t ret = BCM_ERR_OK;

    for (i = 0UL; i < aConfig->portCfgListSz; i++) {
        portCfg = &aConfig->portCfgList[i];
        for (j = 0UL; j < portCfg->vlanMemListSz; j++) {
            vlanID = portCfg->vlanMemList[j].vlanID;
            forward = portCfg->vlanMemList[j].forward;
            if (vlanID > ETHERSWT_VLANID_MAX) {
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            ret = SwitchDrv_UpdateVLANTblEntry(aID, portCfg->portID,
                    vlanID, forward);
            if (ret != BCM_ERR_OK) {
                break;
            }
        }
        if (ret != BCM_ERR_OK) {
            break;
        }
    }

    SwitchDrv_VLANTblDump(aID);

    return ret;
}

static int32_t SwitchDrv_GetVLANInitPort(ETHERSWT_HwIDType aSwtID,
        uint16_t aVlanID, uint32_t *const aPortMask)
{
    uint32_t i;
    uint32_t j;
    uint16_t vlanID;
    const ETHERSWT_PortCfgType *portCfg;
    int32_t ret = BCM_ERR_NOT_FOUND;
    const ETHERSWT_CfgType *swtConfig = SwitchDrv_Data.config;

    *aPortMask = 0UL;

    for (i = 0UL; i < swtConfig->portCfgListSz; i++) {
        portCfg = &swtConfig->portCfgList[i];
        for (j = 0UL; j < portCfg->vlanMemListSz; j++) {
            vlanID = portCfg->vlanMemList[j].vlanID;
            if (vlanID == aVlanID) {
                *aPortMask |= (0x1UL << portCfg->portID);
                ret = BCM_ERR_OK;
            }
        }
    }

    return ret;
}

static int32_t SwitchDrv_PortConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType * const aConfig)
{
    uint32_t i;
    const ETHERSWT_PortCfgType *portCfg;
    int32_t ret = BCM_ERR_OK;
    uint64_t reg;
    uint32_t hostPortCnt = 0UL;
    uint32_t uplinkPortCnt = 0UL;
    uint32_t uplinkPortMap[ETHERSWT_PORT_ID_MAX] = {0};

    for (i = 0UL; i < aConfig->portCfgListSz; i++) {
        portCfg = &aConfig->portCfgList[i];
        ret = SwitchDrv_SetPortJumboMode(aID, portCfg->portID,
                (portCfg->enableJumboFrm == TRUE) ?
                ETHXCVR_BOOLEAN_TRUE: ETHXCVR_BOOLEAN_FALSE);
        if (ret != BCM_ERR_OK) {
            break;
        }

        if (ETHERSWT_HOST_PORT == portCfg->role) {
            hostPortCnt++;
        } else {
            if (ETHERSWT_UP_LINK_PORT == portCfg->role) {
                if (SWITCH_LIGHTSTACK_SLAVES_PORT_TO_MASTER
                        != portCfg->portID) {
                    uplinkPortMap[uplinkPortCnt] = portCfg->portID;
                    uplinkPortCnt++;
                }
            }
        }

        ret = SwitchDrv_SetPortRole(aID, aConfig->switchType,
                                    portCfg->portID, portCfg->role);
        if (ret != BCM_ERR_OK) {
            break;
        }
    }

    if (BCM_ERR_OK != ret) {
        goto err_exit;
    }

    /* Verify Port Role */
    if (ETHERSWT_SWITCH_LIGHTSTACK_MASTER == aConfig->switchType) {
        if ((hostPortCnt > 1UL)
                || (0UL == uplinkPortCnt)
                || (uplinkPortCnt > SWITCH_LIGHTSTACK_UPLINK_PORT_CNT)) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto err_exit;
        }
    } else if (ETHERSWT_SWITCH_LIGHTSTACK_SLAVE == aConfig->switchType) {
        if (uplinkPortCnt >= (SWITCH_LIGHTSTACK_UPLINK_PORT_CNT)
                    || (0UL != hostPortCnt)) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto err_exit;
        }
    } else {
        if ((hostPortCnt > 1UL) || (0UL != uplinkPortCnt)) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto err_exit;
        }
    }

    if ((ETHERSWT_SWITCH_LIGHTSTACK_MASTER == aConfig->switchType)
            || (ETHERSWT_SWITCH_LIGHTSTACK_SLAVE == aConfig->switchType)) {
        /* Enable light stacking support */
        ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m00_lightstack_ctrl, &reg));
        if (ETHERSWT_SWITCH_LIGHTSTACK_MASTER == aConfig->switchType) {
            /* Enable light stack master */
            reg |= SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_MASTER_MASK;
            if (uplinkPortCnt >= 1UL) {
                /* Configure light stack port 0 */
                reg &= ~SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT0_MASK;
                reg |= ((uplinkPortMap[0UL]
                        << SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT0_SHIFT)
                        & SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT0_MASK);
                /* clear Port 1 */
                reg &= ~SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_EN_MASK;
            }
            if (uplinkPortCnt == 2UL) {
                /* Configure light stack port 1 */
                reg &= ~SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_MASK;
                reg |= ((uplinkPortMap[1UL]
                        << SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_SHIFT)
                        & SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_MASK);
                reg |= SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_EN_MASK;
            }
        } else {
            /* Configure light stack slave */
            reg &= ~SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_MASTER_MASK;
            if (uplinkPortCnt == 1UL) {
                /* Configure light stack port 1 */
                reg &= ~SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_MASK;
                reg |= ((uplinkPortMap[0UL]
                        << SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_SHIFT)
                        & SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_PORT1_MASK);
            }
        }

        reg |= SWITCH_P00LC_PAGE_00_LIGHTSTACK_CTRL_LIGHTSTACK_EN_MASK;
        ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m00_lightstack_ctrl, reg));
    }

err_exit:
    return ret;
}

static int32_t SwitchDrv_QoSConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_PortCfgType *const aPortCfg)
{
    uint32_t i;
    uint64_t reg;
    uint64_t tcSel;
    uint32_t tcSelRegAddr;
    uint32_t pcp2tcMapRegAddr;
    uint32_t tc2cosMapRegAddr;
    int32_t ret = BCM_ERR_OK;
    const ETHERSWT_PortIngressCfgType *ingress = &aPortCfg->ingressCfg;
    const ETHERSWT_PortEgressCfgType  *egress = &aPortCfg->egressCfg;

    /* enable 802.1P for this port */
    ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m30_qos_1p_en, &reg));
    reg |= PORTID2HWMASK(aPortCfg->portID);
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m30_qos_1p_en, reg));

    /* QoS configuration Steps:
     * 1. Select the Port TC selection (PID2TC, PCP2TC, DSC2TC etc)
     * 2. Configure PCP to TC in PCP To TC DEI 0 selection register
     * 3. Configure TC to COS Queue in Port TC to COS mapping register
     */

    /* port based traffic class selection
     * if "tc" is valid range, select PID2TC
     * select table for this port in hardware
     * otherwise QoS is based on PCP2TC map
     */
    if ((ingress->tc >= ETHERSWT_TC_0) &&
            (ingress->tc <= ETHERSWT_TC_7)) {
        ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m30_pid2tc, &reg));
        reg &= ~(SWITCH_PID2TC_PORT_MASK << (aPortCfg->portID * SWITCH_PID2TC_PORT_SHIFT));
        reg |= ((ingress->tc & SWITCH_PID2TC_PORT_MASK) <<
                (aPortCfg->portID * SWITCH_PID2TC_PORT_SHIFT));
        ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m30_pid2tc, reg));
        /* select PID2TC bits in TC select table register */
        tcSel = ((SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_0_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_1_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_2_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_3_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_4_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_5_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_6_0_SHIFT) |
                (SWITCH_TC_SEL_PID2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_7_0_SHIFT));
    } else {
        /* configure P2P2TC port map registers and choose PCP2TC map
         * for QoS
         */
        pcp2tcMapRegAddr = SWITCH_M30_PN_PCP2TC_DEI0_PORT0
                            + aPortCfg->portID * 4UL;
        reg = 0UL;
        for (i = 0UL; i < 8UL; i++) {
            reg |= ((ingress->pcp2tcMap[i] & SWITCH_PN_PCP2TC_DEI0_TAG_PRI_MAP_MASK) <<
                    i * SWITCH_PN_PCP2TC_DEI0_TAG_PRI_MAP_SHIFT);
        }
        ERR_EXIT(SwitchDrv_WriteReg(aID, pcp2tcMapRegAddr, reg));
        /* select PCP2TC bits in TC select table register */
        tcSel = ((SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_0_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_1_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_2_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_3_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_4_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_5_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_6_0_SHIFT) |
                (SWITCH_TC_SEL_PCP2TC_VAL
                    << SWITCH_TC_SEL_TABLE_TC_SEL_7_0_SHIFT));
    }

    tcSelRegAddr = SWITCH_M30_TC_SEL_TABLE_PORT0 + aPortCfg->portID * 2UL;
    /* set the TC selection bits in TC select table register */
    ERR_EXIT(SwitchDrv_WriteReg(aID, tcSelRegAddr, tcSel));

    /* configure the TC2QOS mapping */
    tc2cosMapRegAddr = SWITCH_M30_PN_TC2COS_MAP_PORT0 + aPortCfg->portID * 4UL;
    ERR_EXIT(SwitchDrv_ReadReg(aID, tc2cosMapRegAddr, &reg));
    for (i = 0UL; i < egress->fifoListSz; i++) {
        reg &= ~((SWITCH_TC2COS_MAP_PRT_TO_QID_MASK <<
                    (egress->fifoList[i].tc * SWITCH_TC2COS_MAP_PRT_TO_QID_SHIFT)));
        reg |= (egress->fifoList[i].id <<
                (egress->fifoList[i].tc * SWITCH_TC2COS_MAP_PRT_TO_QID_SHIFT));
    }
    ERR_EXIT(SwitchDrv_WriteReg(aID, tc2cosMapRegAddr, reg));

    /* Configure the PCP remarking */
    if (TRUE == egress->pcpRemarkEn) {
        /* Enable egress PCP remarking on the port */
        ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m91_trreg_ctrl0, &reg));
        reg |= 1UL << (SWITCH_PAGE_91_TRREG_CTRL0_PCP_RMK_EN_SHIFT
                        + aPortCfg->portID);
        ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m91_trreg_ctrl0, reg));

        reg = 0ULL;
        /* Configure the TC2PCP mapping */
        for (i = 0UL; i < 8UL; i++) {
            reg |= ((egress->tc2pcpMap[i] & SWITCH_TC2PCP_MAP_MASK) <<
                             (i * SWITCH_TC2PCP_MAP_SHIFT));
            /* Configuring the same mapping for both packets with and without */
            /* rate violations                                                */
            reg |= ((egress->tc2pcpMap[i] & SWITCH_TC2PCP_MAP_MASK) <<
                             (32UL + (i * SWITCH_TC2PCP_MAP_SHIFT)));
        }
        ERR_EXIT(SwitchDrv_WriteReg(aID,
                    SWITCH_M91_PN_EGRESS_PKT_TC2PCP_MAP_PORT0 + (aPortCfg->portID * 8UL),
                    reg));
    }

err_exit:
    return ret;
}

static int32_t SwitchDrv_PortSchConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_PortCfgType *const aPortCfg)
{
    uint32_t i;
    uint64_t schSel;
    uint64_t reg;
    uint32_t portRegAddr;
    int32_t ret = BCM_ERR_OK;

    /* fetch the last SP index */
    for (i = 0; i < SWITCH_COS_QUEUE_MAX + 1; i++) {
        if (aPortCfg->egressCfg.scheduler.algo[i] == ETHERSWT_SCHED_ALGO_SP) {
            break;
        }
    }

    if (i == 0UL) {
        schSel = 0x0UL;
    } else if ((i > 3UL) && (i <= SWITCH_COS_QUEUE_MAX)) {
        schSel = SWITCH_COS_QUEUE_MAX - i + 1UL;
    } else {
        schSel = 0x5UL;
    }
    portRegAddr = SWITCH_M46_PN_QOS_PRI_CTL_PORT0 + aPortCfg->portID * 1UL;

    ERR_EXIT(SwitchDrv_ReadReg(aID, portRegAddr, &reg));
    reg &= ~SWITCH_SCHED_PN_QOS_PRI_CTL_SCHEDULER_SELECT_MASK;
    reg |= schSel;
    if (i > 0UL) {
        if (aPortCfg->egressCfg.scheduler.algo[i - 1] == ETHERSWT_SCHED_ALGO_WRR) {
            reg |= SWITCH_SCHED_PN_QOS_PRI_CTL_WDRR_GRANULARITY_MASK;
        } else {
            reg &= ~SWITCH_SCHED_PN_QOS_PRI_CTL_WDRR_GRANULARITY_MASK;
        }
    }
    ERR_EXIT(SwitchDrv_WriteReg(aID, portRegAddr, reg));

err_exit:
    return ret;
}

static int32_t SwitchDrv_QueueShaperConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_PortCfgType *const aPortCfg)
{
    uint32_t i;
    uint64_t reg;
    uint32_t regAddr;
    const ETHERSWT_FifoCfgType *fifo;
    const ETHERSWT_PortEgressCfgType *egress = &aPortCfg->egressCfg;
    int32_t ret = BCM_ERR_OK;

    for (i = 0UL; i < egress->fifoListSz; i++) {
        fifo = &egress->fifoList[i];

        if (fifo->shaper.rateBps != 0UL) {
            /* configure MAX_REFRESH value in shaping rate control register */
            regAddr = SWITCH_SHAPER_PN_QUEUE0_MAX_REFRESH_PORT0 + fifo->id * 256UL
                        + aPortCfg->portID * 4UL;
            reg = fifo->shaper.rateBps / SWITCH_QUEUE_SHAPER_BIT_RATE_PER_TOCKEN;
            ERR_BREAK(SwitchDrv_WriteReg(aID, regAddr, reg));

            /* configure MAX_THD_SEL value in burst size control register */
            regAddr = SWITCH_SHAPER_PN_QUEUE0_MAX_THD_SEL_PORT0 + fifo->id * 256UL
                        + aPortCfg->portID * 4UL;
            reg = fifo->shaper.burstBytes / SWITCH_QUEUE_SHAPER_BUCKET_UNIT;
            ERR_BREAK(SwitchDrv_WriteReg(aID, regAddr, reg));

            /* select Bit based shaping */
            ERR_BREAK(SwitchDrv_ReadReg(aID,
                        SWITCH_SHAPER_QUEUE_SHAPER_BUCKET_COUNT_SELECT + fifo->id * 256UL,
                        &reg));
            reg &= ~PORTID2HWMASK(aPortCfg->portID);
            ERR_BREAK(SwitchDrv_WriteReg(aID,
                        SWITCH_SHAPER_QUEUE_SHAPER_BUCKET_COUNT_SELECT + fifo->id * 256UL,
                        reg));
            /* enable the port shaper and if avbShaping mode is enabled
             * enable AVB shaper */
            if (fifo->shaper.avbShapingModeEn == TRUE) {
                ERR_BREAK(SwitchDrv_ReadReg(aID,
                            SWITCH_SHAPER_QUEUE_AVB_SHAPING_MODE + fifo->id * 256UL,
                            &reg));
                reg |= PORTID2HWMASK(aPortCfg->portID);
                ERR_BREAK(SwitchDrv_WriteReg(aID,
                            SWITCH_SHAPER_QUEUE_AVB_SHAPING_MODE + fifo->id * 256UL,
                            reg));
            }
            ERR_BREAK(SwitchDrv_ReadReg(aID,
                        SWITCH_SHAPER_QUEUE_SHAPER_ENABLE + fifo->id * 256UL,
                        &reg));
            reg |= PORTID2HWMASK(aPortCfg->portID);
            ERR_BREAK(SwitchDrv_WriteReg(aID,
                        SWITCH_SHAPER_QUEUE_SHAPER_ENABLE + fifo->id * 256UL,
                        reg));
        } else {
            /* disable the Queue shaper for this Queue */
            ERR_BREAK(SwitchDrv_ReadReg(aID,
                        SWITCH_SHAPER_QUEUE_SHAPER_ENABLE + fifo->id * 256UL,
                        &reg));
            reg &= ~PORTID2HWMASK(aPortCfg->portID);
            ERR_BREAK(SwitchDrv_WriteReg(aID,
                        SWITCH_SHAPER_QUEUE_SHAPER_ENABLE + fifo->id * 256UL,
                        reg));
        }
        if (ret != BCM_ERR_OK) {
            break;
        }
    }

    return ret;
}

static int32_t SwitchDrv_PortFlowCntrlConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_PortCfgType * const aPortCfg)
{
    uint32_t i;
    uint32_t portFCMode = FALSE;
    uint32_t fifoThdRegAddr;
    int32_t ret = BCM_ERR_OK;
    const ETHERSWT_PortEgressCfgType *egress = &aPortCfg->egressCfg;
    const ETHERSWT_FifoCfgType *fifo;

      /* configure the reserved threshold for each COS Queue */
    for (i = 0UL; i < egress->fifoListSz; i++) {
        fifo = &egress->fifoList[i];
        fifoThdRegAddr = (uint32_t)&SWITCH_REGS->m0b_fc_lan_txq_thd_rsv_q0 + fifo->id * 2UL;
        if (fifo->minLen != 0UL) {
            if (portFCMode == FALSE) {
                /* select the flow control mode as per port basis */
                ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m0a_fc_ctrl_mode,
                            SWITCH_FC_MODE_PORT));
                /* select this port flow control port selection register */
                ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m0a_fc_ctrl_port,
                            aPortCfg->portID));
                portFCMode = TRUE;
            }
            ERR_BREAK(SwitchDrv_WriteReg(aID, fifoThdRegAddr,
                        fifo->minLen / SWITCH_QUEUE_FLOW_CTRL_THRSLD_UNIT));
        }
        if (ret != BCM_ERR_OK) {
            break;
        }
    }
err_exit:
    return ret;
}

static int32_t SwitchDrv_EgressConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType * const aConfig)
{
    uint32_t i;
    const ETHERSWT_PortCfgType *portCfg;
    int32_t ret = BCM_ERR_OK;

    for (i = 0UL; i < aConfig->portCfgListSz; i++) {
        portCfg = &aConfig->portCfgList[i];
        /* configure the scheduler */
        ERR_BREAK(SwitchDrv_PortSchConfig(aID, portCfg));
        /* configure the shaper */
        ERR_BREAK(SwitchDrv_QueueShaperConfig(aID, portCfg));
        /* configure the reserve threshold register */
        ERR_BREAK(SwitchDrv_PortFlowCntrlConfig(aID, portCfg));
    }
    return ret;
}

static int32_t SwitchDrv_IngressConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType * const aConfig)
{
    uint32_t i;
    const ETHERSWT_PortCfgType *portCfg;
    const ETHERSWT_PortIngressCfgType *ingress;
    uint64_t reg;
    uint32_t portRegAddr;
    int32_t ret = BCM_ERR_OK;

    /* Drop packets with VLAN voilation for all the ports. */
    ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl4,
                &reg));
    reg &= ~SWITCH_P34VC4_PAGE_34_VLAN_CTRL4_INGR_VID_CHK_MASK ;
    reg |= (SWITCH_VLAN_CTRL4_INGR_VID_CHK_VID_VIO
            << SWITCH_P34VC4_PAGE_34_VLAN_CTRL4_INGR_VID_CHK_SHIFT);
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl4,
                reg));

    for (i = 0UL; i < aConfig->portCfgListSz; i++) {
        portCfg = &aConfig->portCfgList[i];
        ingress = &portCfg->ingressCfg;

        /* if configured drop all untagged packet on this port */
        if (ingress->dropUntagged == TRUE) {
            ERR_BREAK(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl3,
                        &reg));
            reg |= PORTID2HWMASK(portCfg->portID);
            ERR_BREAK(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl3,
                        reg));
        } else {
            portRegAddr = SWITCH_M34_DEFAULT_1Q_TAG_PORT0 +
                portCfg->portID * 2UL;
            reg = (ingress->defaultVLAN << SWITCH_DEFAULT_1Q_TAG_VID_0_SHIFT) &
                SWITCH_DEFAULT_1Q_TAG_VID_0_MASK;
            reg |= (ingress->defaultPrio << SWITCH_DEFAULT_1Q_TAG_PRI_0_SHIFT) &
                SWITCH_DEFAULT_1Q_TAG_PRI_0_MASK;
            ERR_BREAK(SwitchDrv_WriteReg(aID, portRegAddr, reg));
        }

        /* configure the per port QoS paramters */
        ret = SwitchDrv_QoSConfig(aID, portCfg);
        if (ret != BCM_ERR_OK) {
            break;
        }
    }
err_exit:
    return ret;
}

static int32_t SwitchDrv_CPUPortConfig(ETHERSWT_HwIDType aID)
{
    uint64_t reg;
    int32_t  ret = BCM_ERR_OK;

    /* enable BRCM TAG on Port 7 (CPU port) */
    ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m02_brcm_hdr_ctrl,
                &reg));
    reg |= SWITCH_BRCM_HDR_CTRL_P7_EN_MASK;
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m02_brcm_hdr_ctrl,
                reg));

    /* Configure the TC2COS mapping */
    /* TC 0 - COS 0 */
    /* TC 1 - COS 1 */
    /* TC 2 - COS 2 */
    /* TC 3 - COS 3 */
    /* TC 4 - COS 4 */
    /* TC 5 - COS 5 */
    /* TC 6 - COS 6 */
    /* TC 7 - COS 7 */
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m30_p7_tc2cos_map,
                                SWITCH_TC2COS_MAP_PORT7_VAL));
err_exit:
    return ret;
}

static int32_t SwitchDrv_VerifyEgressConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_PortCfgType * const aPortCfg)
{
    uint32_t i;
    const ETHERSWT_FifoCfgType *fifo;
    int32_t ret = BCM_ERR_OK;
    const ETHERSWT_PortEgressCfgType *const egress = &aPortCfg->egressCfg;

    for (i = 0UL; i < ETHERSWT_PORT_FIFO_MAX; i++) {
        if (egress->scheduler.algo[i] > ETHERSWT_SCHED_ALGO_DRR) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }

    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }

    for (i = 0UL; i < egress->fifoListSz; i++) {
        fifo = &egress->fifoList[i];
        if (fifo->id > SWITCH_COS_QUEUE_MAX) {
            break;
        }
        if ((fifo->minLen % SWITCH_QUEUE_FLOW_CTRL_THRSLD_UNIT) != 0UL) {
            break;
        }
        if (fifo->tc > ETHERSWT_TC_7) {
            break;
        }
        if ((fifo->shaper.rateBps % SWITCH_QUEUE_SHAPER_BIT_RATE_PER_TOCKEN) != 0UL) {
            break;
        }
        if ((fifo->shaper.rateBps != 0UL) && (fifo->shaper.burstBytes == 0UL)) {
            break;
        }
    }
    if ((i > 0UL) && (i < egress->fifoListSz)) {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

static int32_t SwitchDrv_VerifyIngressConfig(ETHERSWT_HwIDType aID,
        const ETHERSWT_PortCfgType * const aPortCfg)
{
    uint32_t defaultVlanEn = FALSE;
    int32_t ret = BCM_ERR_OK;
    uint32_t i;

    const ETHERSWT_PortIngressCfgType *ingress = &aPortCfg->ingressCfg;

    if ((ingress->defaultVLAN >= ETHERSWT_VLANID_MIN) &&
            (ingress->defaultVLAN <= ETHERSWT_VLANID_MAX)) {
        defaultVlanEn = TRUE;
    }

    /* The truth table looks as follows:            */
    /*  ------------------------------------------- */
    /*  | DropUntagged | defaultVlanEn  |  Valid? | */
    /*  ------------------------------------------- */
    /*  |     1        |     1          |    0    | */
    /*  |     1        |     0          |    1    | */
    /*  |     0        |     1          |    1    | */
    /*  |     0        |     0          |    0    | */
    /*  ------------------------------------------- */
    if (ingress->dropUntagged == defaultVlanEn) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if (ingress->tc > ETHERSWT_TC_7) {
        for (i = 0UL; i < 8UL; i++) {
            if (ingress->pcp2tcMap[i] > ETHERSWT_TC_7) {
                break;
            }
        }
        if (i < 8UL) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto err_exit;
        }
    }
err_exit:
    return ret;
}

int32_t SwitchDrv_SetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode aMode)
{
    uint64_t disReg;
    uint64_t swLearnCtrlReg;
    int ret = BCM_ERR_OK;


    if ((aSwtID < SWITCH_MAX_HW_ID) && (aPortID < ETHERSWT_PORT_ID_MAX)) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_sft_lrn_ctl,
                    &swLearnCtrlReg));
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_dis_learn,
                    &disReg));

        switch (aMode) {
            case ETHERSWT_MACLEARNING_DISABLED:
                disReg |= PORTID2HWMASK(aPortID);
                break;
            case ETHERSWT_MACLEARNING_HW_ENABLED:
                disReg &= ~(PORTID2HWMASK(aPortID));
                swLearnCtrlReg &= ~(PORTID2HWMASK(aPortID));
                break;
            case ETHERSWT_MACLEARNING_SW_ENABLED:
                disReg &= ~(PORTID2HWMASK(aPortID));
                swLearnCtrlReg |= PORTID2HWMASK(aPortID);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
        }
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_sft_lrn_ctl,
                    swLearnCtrlReg));
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_dis_learn,
                    disReg));
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode *const aMode)
{
    uint64_t disReg;
    uint64_t swLearnCtrlReg;
    int ret = BCM_ERR_OK;

    if ((aSwtID < SWITCH_MAX_HW_ID) && (aPortID < ETHERSWT_PORT_ID_MAX) &&
            (aMode != NULL)) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_sft_lrn_ctl,
                    &swLearnCtrlReg));
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_dis_learn,
                    &disReg));
        if ((disReg & PORTID2HWMASK(aPortID)) !=  0UL) {
            *aMode = ETHERSWT_MACLEARNING_DISABLED;
        } else {
            if ((swLearnCtrlReg & PORTID2HWMASK(aPortID)) != 0UL) {
                *aMode = ETHERSWT_MACLEARNING_SW_ENABLED;
            } else {
                *aMode = ETHERSWT_MACLEARNING_HW_ENABLED;
            }
        }

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetARLTable(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLTbl,
        uint16_t *const aTblSize)
{
    int32_t ret = BCM_ERR_OK;
    uint64_t reg;
    uint64_t ent0;
    uint64_t ent1;
    uint64_t result0;
    uint64_t result1;
    uint64_t macAddr;
    ETHERSWT_ARLEntryType *temp;
    uint32_t inSize;
    uint32_t outSz = 0UL;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (NULL == aARLTbl) ||
            (NULL == aTblSize)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    temp = aARLTbl;
    inSize = *aTblSize;

    /* start the search operation by setting STDN bit */
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_ctl,
            SWITCH_P05ASC_PAGE_05_ARLA_SRCH_CTL_ARLA_SRCH_STDN_MASK));

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_ctl, &reg));

    /* read the search result until valid */
    while (((reg & SWITCH_P05ASC_PAGE_05_ARLA_SRCH_CTL_ARLA_SRCH_STDN_MASK) != 0UL) &&
            (outSz < inSize)) {
        if ((reg & SWITCH_P05ASC_PAGE_05_ARLA_SRCH_CTL_ARLA_SRCH_VLID_MASK) != 0UL) {
            /* read MACVID0/1 entry registers */
            ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_0_macvid,
                        &ent0));
            ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_1_macvid,
                        &ent1));
            ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_0,
                        &result0));

            if ((result0 & SWITCH_P05ASR0_P05ASR0ASRV0_MASK) != 0ULL) {
                macAddr = ent0 & SWITCH_P05ASR0M_P05ASR0MASM0_MASK;
                HWMAC2NMAC(macAddr, temp[outSz].macAddr);
                temp[outSz].vlanID = (ent0 & SWITCH_P05ASR0M_P05ASR0MASRV0_MASK) >>
                    SWITCH_P05ASR0M_P05ASR0MASRV0_SHIFT;
                temp[outSz].portMask = ((result0
                                        & SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_MASK)
                                        >> SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_SHIFT);

                /* The interface expects a portmask. The register value will be      */
                /* the port index for a unicast entry and a portmask for a multicast */
                /* entry. So in case the entry is unicast, convert it into a portmask*/
                if (0x1U != (temp[outSz].macAddr[0U] & 0x1U)) {
                    temp[outSz].portMask = 0x1UL << temp[outSz].portMask;
                }

                temp[outSz].reserved = 0U;
                outSz++;
            }

            /* reading the result1 register will make hardware continue with
             * search */
            ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_rslt_1,
                        &result1));
            if ((result1 & SWITCH_P05ASR0_P05ASR0ASRV0_MASK) != 0ULL) {
                if (outSz < inSize) {
                    macAddr = ent1 & SWITCH_P05ASR0M_P05ASR0MASM0_MASK;
                    HWMAC2NMAC(macAddr, temp[outSz].macAddr);
                    temp[outSz].vlanID = (ent1 & SWITCH_P05ASR0M_P05ASR0MASRV0_MASK) >>
                        SWITCH_P05ASR0M_P05ASR0MASRV0_SHIFT;
                    temp[outSz].portMask = (result1 & SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_MASK) >>
                        SWITCH_P05ASR0_PAGE_05_ARLA_SRCH_RSLT_0_PORTID_0_SHIFT;

                    /* The interface expects a portmask. The register value will be      */
                    /* the port index for a unicast entry and a portmask for a multicast */
                    /* entry. So in case the entry is unicast, convert it into a portmask*/
                    if (0x1U != (temp[outSz].macAddr[0U] & 0x1U)) {
                        temp[outSz].portMask = 0x1UL << temp[outSz].portMask;
                    }

                    temp[outSz].reserved = 0U;
                    outSz++;
                }
            }
        }
        ERR_BREAK(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m05_arla_srch_ctl, &reg));
    }

    *aTblSize = outSz;

err_exit:
    return ret;
}


int32_t SwitchDrv_GetPortMacAddr(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID,
        const uint8_t *const aMacAddr,
        ETHERSWT_VLANIDType aVlanID)
{
    uint32_t binIdx;
    uint64_t mac;
    int ret = BCM_ERR_OK;

    if ((aMacAddr[0] & 0x1U) != 0UL) {
        /* this is multicast mac address */
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    mac = NMAC2HWMAC(aMacAddr);

    /* Hw hash based searching needs both the VLAN ID
     * and MAC Address to perfom the hash
     * indexing into the ARL table
     * Since this API does not provide the VLAN ID,
     * use the hardware linear search method
     */
    ret = SwitchDrv_ARLTblSearchByAddr(aSwtID, mac, aVlanID, aPortID, &binIdx);

err_exit:
    return ret;
}
int32_t SwitchDrv_EnableVLAN(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aEnable)
{
    uint32_t i;
    uint32_t j;
    const ETHERSWT_PortCfgType *portCfg;
    uint32_t untagMap = 0UL;
    uint32_t fwdMap = 0UL;
    int32_t ret = BCM_ERR_OK;
    uint32_t found = FALSE;

    if ((aSwtID >= SWITCH_MAX_HW_ID) && (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if (SwitchDrv_Data.config == NULL) {
        ret = BCM_ERR_UNKNOWN;
        goto err_exit;
    }

    /* if aVlanID is not configured for this port return error */
    for (i = 0UL; i < SwitchDrv_Data.config->portCfgListSz; i++) {
        portCfg = &SwitchDrv_Data.config->portCfgList[i];
        for (j = 0UL; j < portCfg->vlanMemListSz; j++) {
            if (portCfg->vlanMemList[j].vlanID == aVlanID) {
                found = TRUE;
                break;
            }
        }
        if (found == TRUE) {
            break;
        }
    }

    if (i == SwitchDrv_Data.config->portCfgListSz) {
        ret = BCM_ERR_NOT_FOUND;
        goto err_exit;
    }

    /* read the current port map configured in the HW */
    ERR_EXIT(SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
            VLAN_TBL_CMD_READ));

    if (aEnable == FALSE) {
        /* clear the forward port map */
        fwdMap &= ~PORTID2HWMASK(portCfg->portID);
    } else {
        fwdMap |= PORTID2HWMASK(portCfg->portID);
    }

    /* write the configuration to VLAN table */
    ret = SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
            VLAN_TBL_CMD_WRITE);

    SwitchDrv_VLANTblDump(aSwtID);

err_exit:
    return ret;
}

int32_t SwitchDrv_SetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        const ETHERSWT_PortMirrorCfgType *const aConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint64_t srcMacAddr;
    uint64_t destMacAddr;
    uint64_t macAddr;
    uint64_t mirrorCntrReg;
    uint64_t reg;
    uint64_t packetDivider = 0ULL;

    if (aConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }
    if ((aDirection != ETHERSWT_TRAFFICDIR_INGRESS)
            && (aDirection != ETHERSWT_TRAFFICDIR_EGRESS)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if ((aConfig->portMask == 0UL)
            && (SwitchDrv_Data.config->switchType == ETHERSWT_SWITCH_STANDARD)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    srcMacAddr = NMAC2HWMAC(aConfig->srcMacAddrFilter);
    destMacAddr = NMAC2HWMAC(aConfig->destMacAddrFilter);

    if (aConfig->packetDivider > SWITCH_PAGE_02_IGMIRDIV_IN_MIR_DIV_MASK) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if (srcMacAddr != 0ULL) {
        macAddr = srcMacAddr;
        mirrorCntrReg = SWITCH_MIR_FLTR_SA_MATCH
                        << SWITCH_PAGE_02_IGMIRCTL_IN_MIR_FLTR_SHIFT;
    } else if (destMacAddr != 0ULL) {
        macAddr = destMacAddr;
        mirrorCntrReg = SWITCH_MIR_FLTR_DA_MATCH
                        << SWITCH_PAGE_02_IGMIRCTL_IN_MIR_FLTR_SHIFT;
    } else {
        macAddr = 0ULL;
        mirrorCntrReg = SWITCH_MIR_FLTR_ALL_PACKETS
                        << SWITCH_PAGE_02_IGMIRCTL_IN_MIR_FLTR_SHIFT;
    }

    if (aConfig->packetDivider != 0UL) {
        mirrorCntrReg |= SWITCH_PAGE_02_IGMIRCTL_IN_DIV_EN_MASK;
        packetDivider = aConfig->packetDivider - 1UL;
    }

    mirrorCntrReg |= aConfig->portMask
                            & SWITCH_PAGE_02_IGMIRCTL_IN_MIR_MSK_MASK;

    /* first ensure that port mirroring is disabled */
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, &reg));
    reg &= ~SWITCH_PAGE_02_MIRCAPCTL_MIR_EN_MASK;
    reg &= ~SWITCH_PAGE_02_MIRCAPCTL_SMIR_CAP_PORT_MASK;
    reg |= aPortID & SWITCH_PAGE_02_MIRCAPCTL_SMIR_CAP_PORT_MASK;
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, reg));

    if (aDirection == ETHERSWT_TRAFFICDIR_INGRESS) {
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, SWITCH_PAGE_02_IGMIRMAC,
                    macAddr & SWITCH_PAGE_02_IGMIRMAC_IN_MIR_MAC_MASK));
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_igmirdiv, packetDivider));
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_igmirctl, mirrorCntrReg));
    }

    if (aDirection == ETHERSWT_TRAFFICDIR_EGRESS) {
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, SWITCH_PAGE_02_EGMIRMAC,
                    macAddr & SWITCH_PAGE_02_EGMIRMAC_OUT_MIR_MAC_MASK));
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_egmirdiv, packetDivider));
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->mirregs_union_instance.mirregs_struct_instance.PAGE_02_EGMIRCTL, mirrorCntrReg));
    }

    SwitchDrv_Data.mirrorPortID = aPortID;

err_exit:
    return ret;
}

int32_t SwitchDrv_GetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        ETHERSWT_PortMirrorCfgType *const aConfig)
{
    uint64_t reg;
    uint64_t macAddr;
    uint64_t packetDivider;
    uint64_t mirrorCntrReg;

    int32_t ret = BCM_ERR_OK;

    if (aConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if (SwitchDrv_Data.mirrorPortID != aPortID) {
        ret = BCM_ERR_INVAL_STATE;
        goto err_exit;
    }

    BCM_MemSet(aConfig, 0x0, sizeof(ETHERSWT_PortMirrorCfgType));

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, &reg));

    if ((reg & SWITCH_PAGE_02_MIRCAPCTL_SMIR_CAP_PORT_MASK) != aPortID) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if (aDirection == ETHERSWT_TRAFFICDIR_INGRESS) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_igmirctl, &mirrorCntrReg));
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, SWITCH_PAGE_02_IGMIRMAC,
                    &macAddr));
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_igmirdiv, &packetDivider));

    }

    if (aDirection == ETHERSWT_TRAFFICDIR_EGRESS) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->mirregs_union_instance.mirregs_struct_instance.PAGE_02_EGMIRCTL, &mirrorCntrReg));
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, SWITCH_PAGE_02_EGMIRMAC,
                    &macAddr));
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_egmirdiv, &packetDivider));
    }

    aConfig->portMask = mirrorCntrReg & SWITCH_PAGE_02_IGMIRCTL_IN_MIR_MSK_MASK;

    if ((mirrorCntrReg & (SWITCH_MIR_FLTR_SA_MATCH
            << SWITCH_PAGE_02_IGMIRCTL_IN_MIR_FLTR_SHIFT)) != 0UL) {
        HWMAC2NMAC(macAddr, aConfig->srcMacAddrFilter);
    } else if ((mirrorCntrReg & (SWITCH_MIR_FLTR_DA_MATCH << SWITCH_PAGE_02_EGMIRCTL_OUT_MIR_FLTR_SHIFT)) != 0UL) {
        HWMAC2NMAC(macAddr, aConfig->destMacAddrFilter);
    }

    aConfig->packetDivider = packetDivider;

err_exit:
    return ret;
}

int32_t SwitchDrv_SetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType aState)
{
    uint64_t reg;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, &reg));

    if ((reg & SWITCH_PAGE_02_MIRCAPCTL_SMIR_CAP_PORT_MASK) != aPortID) {
        ret = BCM_ERR_INVAL_STATE;
        goto err_exit;
    }

    if (aState == ETHERSWT_PORT_MIRROR_STATE_DISABLED) {
        reg &= ~SWITCH_PAGE_02_MIRCAPCTL_MIR_EN_MASK;
    } else if (aState == ETHERSWT_PORT_MIRROR_STATE_ENABLED) {
        reg |= SWITCH_PAGE_02_MIRCAPCTL_MIR_EN_MASK;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, reg));
err_exit:
    return ret;
}

int32_t SwitchDrv_GetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType *const aState)
{
    uint64_t reg;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX) ||
            (aState == NULL)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, &reg));

    if ((reg & SWITCH_PAGE_02_MIRCAPCTL_SMIR_CAP_PORT_MASK) != aPortID) {
        ret = BCM_ERR_INVAL_STATE;
        goto err_exit;
    }

    if ((reg & SWITCH_PAGE_02_MIRCAPCTL_MIR_EN_MASK) == 0UL) {
        *aState = ETHERSWT_PORT_MIRROR_STATE_DISABLED;
    } else {
        *aState = ETHERSWT_PORT_MIRROR_STATE_ENABLED;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetMirrorCapturePort(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID)
{
    uint64_t reg;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (NULL == aPortID)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_mircapctl, &reg));

    if ((reg & SWITCH_PAGE_02_MIRCAPCTL_MIR_EN_MASK) == 0UL) {
        ret = BCM_ERR_NOT_FOUND;
    } else {
        *aPortID = reg & SWITCH_PAGE_02_MIRCAPCTL_SMIR_CAP_PORT_MASK;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHER_RxStatsType *const aRxStat)
{
    uint64_t val;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX) ||
            (NULL == aRxStat)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    memset(aRxStat, 0xFF, sizeof(ETHER_RxStatsType));

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxgoodoctets + aPortID*256UL,
                    &val));
    aRxStat->gdPkts = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxoctets + aPortID*256UL,
                    &val));
    aRxStat->octetsLow = (uint32_t)val;
    aRxStat->octetsHigh = (uint32_t)(val >> 32UL);
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxbroadcastpkts + aPortID*256UL,
                    &val));
    aRxStat->brdCast = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxmulticastpkts + aPortID*256UL,
                    &val));
    aRxStat->mutCast = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxunicastpkts + aPortID*256UL,
                    &val));
    aRxStat->uniCast = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts64octets + aPortID*256UL,
                    &val));
    aRxStat->pkts64 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts65to127octets + aPortID*256UL,
                    &val));
    aRxStat->pkts65_127 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts128to255octets + aPortID*256UL,
                    &val));
    aRxStat->pkts128_255 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts256to511octets + aPortID*256UL,
                    &val));
    aRxStat->pkts256_511 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts512to1023octets + aPortID*256UL,
                    &val));
    aRxStat->pkts512_1023 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts1024tomaxpktoctets + aPortID*256UL,
                    &val));
    aRxStat->pkts1024_MAX = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxjumbopkt + aPortID*256UL,
                    &val));
    aRxStat->pkts8192_MAX = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxjabbers + aPortID*256UL,
                    &val));
    aRxStat->pktsJabber = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxoversizepkts + aPortID*256UL,
                    &val));
    aRxStat->pktsOvrSz = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxfragments + aPortID*256UL,
                    &val));
    aRxStat->pktsFrag = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxdroppkts + aPortID*256UL,
                    &val));
    aRxStat->pktsRxDrop = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxalignmenterrors + aPortID*256UL,
                    &val));
    aRxStat->pktsCrcAlignErr = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxundersizepkts + aPortID*256UL,
                    &val));
    aRxStat->pktsUndSz = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxfcserrors + aPortID*256UL,
                    &val));
    aRxStat->pktsCrcErr = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxdiscard + aPortID*256UL,
                    &val));
    aRxStat->pktsRxDiscard = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpausepkts + aPortID*256UL,
                    &val));
    aRxStat->rxPause = (uint32_t)val;
err_exit:
    return ret;
}

int32_t SwitchDrv_ClearRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID)
{
    uint64_t val;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }


    val = 0ULL;

    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxgoodoctets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxoctets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxbroadcastpkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxmulticastpkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxunicastpkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts64octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts65to127octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts128to255octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts256to511octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts512to1023octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpkts1024tomaxpktoctets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxjumbopkt + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxjabbers + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxoversizepkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxfragments + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxdroppkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxalignmenterrors + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxundersizepkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxfcserrors + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxdiscard + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_rxpausepkts + aPortID*256UL,
                    val));
err_exit:
    return ret;
}

int32_t SwitchDrv_GetTxStat(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHER_TxStatsType *const aTxStat)
{
    uint64_t val;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX) ||
            (NULL == aTxStat)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /* first set all the counters to invalid */
    memset(aTxStat, 0xFF, sizeof(ETHER_TxStatsType));
    /* read the counters and update */

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txoctets + aPortID*256UL,
                    &val));
    aTxStat->octets = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txbroadcastpkts + aPortID*256UL,
                    &val));
    aTxStat->brdCast = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txmulticastpkts + aPortID*256UL,
                    &val));
    aTxStat->mutCast = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txunicastpkts + aPortID*256UL,
                    &val));
    aTxStat->uniCast = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txdroppkts + aPortID*256UL,
                    &val));
    aTxStat->txDropped = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txcollisions + aPortID*256UL,
                    &val));
    aTxStat->txCollision = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txsinglecollision + aPortID*256UL,
                    &val));
    aTxStat->txCollisionSingle = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txmultiplecollision + aPortID*256UL,
                    &val));
    aTxStat->txCollisionMulti = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txmultiplecollision + aPortID*256UL,
                    &val));
    aTxStat->txCollisionMulti = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txlatecollision + aPortID*256UL,
                    &val));
    aTxStat->txLateCollision = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txexcessivecollision + aPortID*256UL,
                    &val));
    aTxStat->txExcessiveCollision = (uint32_t)val;
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txdeferredtransmit + aPortID*256UL,
                    &val));
    aTxStat->txDeferredTransmit = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txframeindisc + aPortID*256UL,
                    &val));
    aTxStat->txFrameInDiscard = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpausepkts + aPortID*256UL,
                    &val));
    aTxStat->txPause = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq0 + aPortID*256UL,
                    &val));
    aTxStat->txQ0 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq1 + aPortID*256UL,
                    &val));
    aTxStat->txQ1 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq2 + aPortID*256UL,
                    &val));
    aTxStat->txQ2 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq3 + aPortID*256UL,
                    &val));
    aTxStat->txQ3 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq4 + aPortID*256UL,
                    &val));
    aTxStat->txQ4 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq5 + aPortID*256UL,
                    &val));
    aTxStat->txQ5 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq6 + aPortID*256UL,
                    &val));
    aTxStat->txQ6 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq7 + aPortID*256UL,
                    &val));
    aTxStat->txQ7 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts64octets + aPortID*256UL,
                    &val));
    aTxStat->pkts64 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts65to127octets + aPortID*256UL,
                    &val));
    aTxStat->pkts65_127 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts128to255octets + aPortID*256UL,
                    &val));
    aTxStat->pkts128_255 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts256to511octets + aPortID*256UL,
                    &val));
    aTxStat->pkts256_511 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts512to1023octets + aPortID*256UL,
                    &val));
    aTxStat->pkts512_1023 = (uint32_t)val;

    ERR_EXIT(SwitchDrv_ReadReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts1024tomaxpktoctets + aPortID*256UL,
                    &val));
    aTxStat->pkts1024_MAX = (uint32_t)val;
err_exit:
    return ret;
}

int32_t SwitchDrv_ClearTxStat(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID)
{
    uint64_t val;
    int32_t ret = BCM_ERR_OK;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /* clear the counter */
    val = 0ULL;

    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txoctets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txbroadcastpkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txmulticastpkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txunicastpkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txdroppkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txcollisions + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txsinglecollision + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txmultiplecollision + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txmultiplecollision + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txlatecollision + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txexcessivecollision + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txdeferredtransmit + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txframeindisc + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpausepkts + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq0 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq1 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq2 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq3 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq4 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq5 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq6 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txqpktq7 + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts64octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts65to127octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts128to255octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts256to511octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts512to1023octets + aPortID*256UL,
                    val));
    ERR_EXIT(SwitchDrv_WriteReg(aSwtID,
                    (uint32_t)&SWITCH_REGS->m20_txpkts1024tomaxpktoctets + aPortID*256UL,
                    val));
err_exit:
    return ret;
}

int SwitchDrv_Init (ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType *const aConfig,
        uint32_t *const aPort2TimeFifoMap)
{
    uint32_t i;
    uint32_t j;
    uint64_t reg;
    int32_t ret = BCM_ERR_OK;
    const ETHERSWT_PortCfgType *portCfg;
    uint64_t portLedEnMask = 0ULL;

    if ((aID >= SWITCH_MAX_HW_ID) || (aConfig == NULL)
            || (NULL == aPort2TimeFifoMap)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if ((ETHERSWT_SWITCH_STANDARD != aConfig->switchType)
            && (ETHERSWT_SWITCH_LIGHTSTACK_MASTER != aConfig->switchType)
            && (ETHERSWT_SWITCH_LIGHTSTACK_SLAVE != aConfig->switchType)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /* Disable Link status interrupt for all the ports */
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m03_link_sts_int_en, 0x0ULL));

    /* verify the configuration first */
    for (i = 0UL; i < aConfig->portCfgListSz; i++) {
        ret = BCM_ERR_INVAL_PARAMS;
        portCfg = &aConfig->portCfgList[i];
        if (portCfg->portID >= ETHERSWT_PORT_ID_MAX) {
            break;
        }
        if ((portCfg->role != ETHERSWT_STANDARD_PORT)
                && (portCfg->role != ETHERSWT_HOST_PORT)
                && (portCfg->role != ETHERSWT_UP_LINK_PORT)) {
            break;
        }
        if (portCfg->macAddrListSz > ETHERSWT_PORT_MAC_ENTY_MAX) {
            break;
        }
        if (portCfg->vlanMemListSz > ETHERSWT_PORT_VLAN_ENTY_MAX) {
            break;
        }
        for (j = 0UL; j < portCfg->vlanMemListSz; j++) {
            if ((portCfg->vlanMemList[j].vlanID < ETHERSWT_VLANID_MIN) ||
                    (portCfg->vlanMemList[j].vlanID > ETHERSWT_VLANID_MAX)) {
                break;
            }
            if (portCfg->vlanMemList[j].defaultPri > ETHERSWT_PCP_7) {
                break;
            }
            if ((portCfg->vlanMemList[j].forward < ETHERSWT_VLAN_FRWRD_DONT_SEND)
                    || (portCfg->vlanMemList[j].forward > ETHERSWT_VLAN_FRWRD_UNTAGGED)) {
                break;
            }
        }
        if ((portCfg->vlanMemListSz > 0UL) && (j < portCfg->vlanMemListSz)) {
            break;
        }
        ERR_BREAK(SwitchDrv_VerifyIngressConfig(aID, portCfg));
        ERR_BREAK(SwitchDrv_VerifyEgressConfig(aID, portCfg));
        portLedEnMask |= (0x1ULL << portCfg->portID);
        ret = BCM_ERR_OK;
    }

    if (ret == BCM_ERR_INVAL_PARAMS) {
        goto err_exit;
    }

    /* enable SVL mode and 802.1Q VLAN */
    ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl0, &reg));
    reg |= SWITCH_P34VC0_PAGE_34_VLAN_CTRL0_VLAN_EN_MASK;
    reg &= ~(SWITCH_P34VC0_PAGE_34_VLAN_CTRL0_VLAN_LEARN_MODE_MASK);
    reg |= SWITCH_VLAN_LEARN_MODE_IVL
            << SWITCH_P34VC0_PAGE_34_VLAN_CTRL0_VLAN_LEARN_MODE_SHIFT;
    /* TODO: Check if it is required to change inner VID and outer VID in case
     * of double tagged packets
     */
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl0, reg));

    /* configure ARL table and VLAN tables */
    ERR_EXIT(SwitchDrv_ARLTblConfig(aID, aConfig));
    ERR_EXIT(SwitchDrv_VLANTblConfig(aID, aConfig));
    ERR_EXIT(SwitchDrv_IngressConfig(aID, aConfig));
    ERR_EXIT(SwitchDrv_EgressConfig(aID, aConfig));
    ERR_EXIT(SwitchDrv_PortConfig(aID, aConfig));
    ERR_EXIT(SwitchDrv_CPUPortConfig(aID));

    SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m00_swmode, 0x6); /* Check if it is required? */
    SwitchDrv_Data.config = aConfig;
    BCM_MemCpy(SwitchDrv_Data.port2TimeFifoMap, aPort2TimeFifoMap,
            sizeof(uint32_t) * ETHERSWT_PORT_ID_MAX);

    reg = (SWITCH_CTRL_LED_FUNC1_AVB_LINK_MASK
            | SWITCH_CTRL_LED_FUNC1_SPD1G_MASK
            | SWITCH_CTRL_LED_FUNC1_SPD10M_MASK
            | SWITCH_CTRL_LED_FUNC1_LNK_ACT_MASK);
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m00_led_func1_ctl, reg));

    ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m00_led_en_map, &reg));
    reg &= ~SWITCH_CTRL_LED_EN_MAP_MASK;
    reg |= (portLedEnMask & SWITCH_CTRL_LED_EN_MAP_MASK);
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m00_led_en_map, reg));

    /* Enable Link status interrupt for all the ports except port7 */
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m03_link_sts_int_en,
                                    SWITCH_LNK_STS_INT_EN_PORT_XLD7_MASK));

    /* Configure standard max frame size equal to 1518 */
    reg = SWITCH_JUMBO_MIB_GD_FM_MAX_SIZE;
    ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m40_mib_gd_fm_max_size, reg));

#ifdef __BCM8956X__
    /* To enable timestamping on stacking link (presence of egress Broadcom header) */
    if ((ETHERSWT_SWITCH_LIGHTSTACK_MASTER == aConfig->switchType) ||
            (ETHERSWT_SWITCH_LIGHTSTACK_SLAVE == aConfig->switchType)) {
        P1588_RDBType * const ETHER_P1588REGS = (P1588_RDBType *const)P1588_0_BASE;
        ETHER_P1588REGS->mpls_label9_lsb_value = 0xC0C0U;
    }
#endif
err_exit:
    return ret;
}

int32_t SwitchDrv_SetLedState(ETHERSWT_HwIDType aID,
                            ETHERSWT_PortIDType aPortID,
                            ETHERSWT_LedType aLedType,
                            uint32_t aTurnOn)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint64_t mask, reg;

    if ((aID < SWITCH_MAX_HW_ID)
            && (aPortID < ETHERSWT_PORT_ID_MAX)
            && ((TRUE == aTurnOn) || (FALSE == aTurnOn))
            && (ETHERSWT_LED_AVB == aLedType)) {
        mask = ((0x1ULL << aPortID) & SWITCH_P90ELS_PAGE_90_EAV_LNK_STATUS_PT_EAV_LNK_STATUS_MASK);
        if (0UL != mask) {
            ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m90_eav_lnk_status, &reg));
            if (TRUE == aTurnOn) {
                reg |= mask;
            } else {
                reg &= ~mask;
            }
            ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->m90_eav_lnk_status, reg));
        } else {
            /* Ignore this function call, HW doesn't support these
             * LED ports */
            ret = BCM_ERR_OK;
        }
    }
err_exit:
    return ret;
}

int32_t SwitchDrv_GetLedState(ETHERSWT_HwIDType aID,
                            ETHERSWT_PortIDType aPortID,
                            ETHERSWT_LedType aLedType,
                            uint32_t *const aIsStateOn)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint64_t mask, reg;

    if ((aID < SWITCH_MAX_HW_ID)
            && (aPortID < ETHERSWT_PORT_ID_MAX)
            && (NULL != aIsStateOn)
            && (ETHERSWT_LED_AVB == aLedType)) {
        mask = ((0x1ULL << aPortID) & SWITCH_P90ELS_PAGE_90_EAV_LNK_STATUS_PT_EAV_LNK_STATUS_MASK);
        if (0UL != mask) {
            ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->m90_eav_lnk_status, &reg));
            if ((reg & mask) == mask) {
                *aIsStateOn = TRUE;
            } else {
                *aIsStateOn = FALSE;
            }

        } else {
            *aIsStateOn = FALSE;
            ret = BCM_ERR_OK;
        }
    }
err_exit:
    return ret;
}

int32_t SwitchDrv_SetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType aMode)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint64_t reg;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && ((ETHERSWT_DUMBFWD_ENABLE == aMode)
                || (ETHERSWT_DUMBFWD_DISABLE == aMode))) {

        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_switch_ctrl, &reg));
        if (ETHERSWT_DUMBFWD_ENABLE == aMode) {
            /* Standard Port: Enable dumb forward mode */
            reg |= SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
        } else {
            /* Host Port: Disable dumb forward mode */
            reg &= ~SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
        }
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_switch_ctrl, reg));
    }
err_exit:
    return ret;
}

int32_t SwitchDrv_GetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType *const aMode)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint64_t reg;
    uint64_t mask = 0ULL;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (NULL != aMode)) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m00_switch_ctrl, &reg));
        mask |= SWITCH_P00SC_PAGE_00_CTRL_MII_DUMB_FWDG_EN_MASK;
        if ((reg & mask) == mask) {
            *aMode = ETHERSWT_DUMBFWD_ENABLE;
        } else {
            *aMode = ETHERSWT_DUMBFWD_DISABLE;
        }
    }
err_exit:
    return ret;
}

/* Function to do switch port to timer hardware port mapping
 * TimeHW0: ports - 0x00, 0x01, ..., 0x07
 * TimeHW1: ports - 0x10, 0x11, ..., 0x17
 * Switch port 0, 1, ..., 8 (excluding port 7)
 * */
static int32_t SwitchDrv_GetTimeHwPort(ETHERSWT_PortIDType aPort,
                            uint32_t *const aTimeHwPort)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((aPort <= 8UL) && (aPort != 7UL)) {
        *aTimeHwPort = SwitchDrv_Data.port2TimeFifoMap[aPort];
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

int32_t SwitchDrv_TxAdaptBuffer(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;

    if ((SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx)
            && (NULL != aDataInOut)
            && (NULL != (*aDataInOut))
            && (NULL != aLenInOut)
            && (SWITCH_INGRESS_MGMT_INFO_SIZE < (*aLenInOut))) {
        bufInfo = &SwitchDrv_TxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_FREE == bufInfo->bufState) {
            *aLenInOut -= SWITCH_INGRESS_MGMT_INFO_SIZE;
            *aDataInOut = &((*aDataInOut)[SWITCH_INGRESS_MGMT_INFO_SIZE]);
            bufInfo->bufState = SWITCHDRV_BUF_STATE_ADAPTED;
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

int32_t SwitchDrv_SetMgmtInfo(ETHER_HwIDType aCntrlID,
                            uint32_t aBufIdx,
                            const ETHERSWT_MgmtInfoType * const aMgmtInfo)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;

    if ((SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx) && (NULL != aMgmtInfo)) {
        bufInfo = &SwitchDrv_TxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_ADAPTED == bufInfo->bufState) {
            bufInfo->mgmtInfo = *aMgmtInfo;
            bufInfo->bufState = SWITCHDRV_BUF_STATE_MGMT_INFO_SET;
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

static int32_t SwitchDrv_EnableTxTS(ETHER_HwIDType aID,
                                uint32_t aBufIdx,
                                const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t timeHwPort;
    ETHER_TimeIOType ethTimeIO;

    retVal = SwitchDrv_GetTimeHwPort(aMgmtInfo->PortIdx, &timeHwPort);
    if (BCM_ERR_OK == retVal) {
        ethTimeIO.retVal = BCM_ERR_UNKNOWN;
        ethTimeIO.hwID = aID;
        ethTimeIO.buffIdxInOut = &aBufIdx;
        ethTimeIO.portIdx = timeHwPort;

        retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_ENABLE_EGRESS_TIMESTAMP,
                &ethTimeIO);
    }
    return retVal;
}

int32_t SwitchDrv_EnableTxTimestamp(ETHER_HwIDType aSwtID,
                            uint32_t aBufIdx,
                            const ETHERSWT_MgmtInfoType * const aMgmtInfo)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;
    const ETHERSWT_PortCfgType *portCfg;
    uint32_t i, isPortTSEn = FALSE;
    ETHER_IOType ethIO;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx)
            && (NULL != aMgmtInfo)) {
        for (i = 0UL; i < SwitchDrv_Data.config->portCfgListSz; i++) {
            portCfg = &SwitchDrv_Data.config->portCfgList[i];
            if (aMgmtInfo->PortIdx == portCfg->portID) {
                if (TRUE == portCfg->enableTimeStamp) {
                    isPortTSEn = TRUE;
                }
                break;
            }
        }
        if (TRUE != isPortTSEn) {
            goto exit;
        }

        bufInfo = &SwitchDrv_TxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_MGMT_INFO_SET == bufInfo->bufState) {
            ethIO.retVal = BCM_ERR_UNKNOWN;
            ethIO.hwID = 0x0UL;
            ethIO.buffIdxInOut = &aBufIdx;

            ret = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_TX_MARK_TS_PKT, &ethIO);
            if (BCM_ERR_OK == ret) {
                ret = SwitchDrv_EnableTxTS(ETHER_HW_ID_0, aBufIdx, aMgmtInfo);
            }
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }
exit:
    return ret;
}

int32_t SwitchDrv_SetTSEnabled(ETHER_HwIDType aSwtID,
                            uint32_t aBufIdx)
{
    int32_t ret;
    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx)) {
        SwitchDrv_TxBufInfoQ[aBufIdx].isTSEnabled = TRUE;
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t SwitchDrv_TxProcessFrame(ETHER_HwIDType aCntrlID,
                                uint32_t aBufIdx,
                                uint8_t ** const aDataInOut,
                                uint32_t * const aLenInOut)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;
    uint32_t i;
    uint32_t * mgmtInfo;
    uint32_t mask = 0UL;

    if (SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx) {
        if ((NULL != aDataInOut)
                && (NULL != (*aDataInOut))
                && (NULL != aLenInOut)
                && (0UL != (*aLenInOut))) {
            bufInfo = &SwitchDrv_TxBufInfoQ[aBufIdx];
            if (SWITCHDRV_BUF_STATE_MGMT_INFO_SET == bufInfo->bufState) {
                *aLenInOut += SWITCH_INGRESS_MGMT_INFO_SIZE;
                mgmtInfo = (uint32_t *)(*aDataInOut);
                /* Insert management information */
#ifdef __BCM8956X__
                if (SwitchDrv_Data.config->switchType == ETHERSWT_SWITCH_LIGHTSTACK_SLAVE) {
                    mask |= SWITCH_INGRESS_MGMT_INFO_OPCODE3;
                } else {
                    mask |= SWITCH_INGRESS_MGMT_INFO_OPCODE1;
                }
#else
                mask |= SWITCH_INGRESS_MGMT_INFO_OPCODE1;
#endif
                mask |= (0x4UL << SWITCH_INGRESS_MGMT_INFO_TC_SHIFT);
                mask |= (0x1UL << SWITCH_INGRESS_MGMT_INFO_TE_SHIFT);
                mask |= (0x1UL << bufInfo->mgmtInfo.PortIdx);
                *mgmtInfo = Host2BE32(mask);
                bufInfo->bufState = SWITCHDRV_BUF_STATE_PROCESSED;
                ret = BCM_ERR_OK;
            } else if (SWITCHDRV_BUF_STATE_ADAPTED == bufInfo->bufState) {
                for (i = 0UL; i < (*aLenInOut); i++) {
                    (*aDataInOut)[i] = (*aDataInOut)[i + SWITCH_INGRESS_MGMT_INFO_SIZE];
                }
                bufInfo->bufState = SWITCHDRV_BUF_STATE_PROCESSED;
                ret = BCM_ERR_OK;
            } else {
                ret = BCM_ERR_INVAL_BUF_STATE;
            }
        } else if ((NULL == aDataInOut)
                && (NULL != aLenInOut)
                && (0UL == (*aLenInOut))) {
            /* Change switch buffer info state to Free */
            SwitchDrv_TxBufInfoQ[aBufIdx].bufState = SWITCHDRV_BUF_STATE_FREE;
            ret = BCM_ERR_OK;
        }
    }

    return ret;
}

static int32_t SwitchDrv_GetTxTimestamp(ETHER_HwIDType aID,
                                uint32_t aBufIdx,
                                ETHER_TimestampType *const aTS,
                                ETHER_TimestampQualType *const aTSQual,
                                ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t timeHwPort;

    retVal = SwitchDrv_GetTimeHwPort(aMgmtInfo->PortIdx, &timeHwPort);
    if (BCM_ERR_OK == retVal) {
        retVal = ETHER_TimeGetEgressTS(aID, timeHwPort, aBufIdx, aTS, aTSQual);
    }
    return retVal;
}

int32_t SwitchDrv_TxDoneInd (ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx, ETHERSWT_MgmtInfoType **aMgmtInfo,
        ETHER_TimestampType *aTs, ETHER_TimestampQualType *aTsQual)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;

    *aMgmtInfo = NULL;

    if (SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx) {
        bufInfo = &SwitchDrv_TxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_PROCESSED == bufInfo->bufState) {
            if (TRUE == bufInfo->isTSEnabled) {
                ret = SwitchDrv_GetTxTimestamp(aCntrlID, aBufIdx, aTs, aTsQual,
                        &(bufInfo->mgmtInfo));
                if (BCM_ERR_OK == ret) {
                    *aMgmtInfo = &(bufInfo->mgmtInfo);
                }
            } else {
                ret = BCM_ERR_OK;
                bufInfo->bufState = SWITCHDRV_BUF_STATE_FREE;
            }
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

int32_t SwitchDrv_TxDoneIndComplete (uint32_t aBufIdx)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;

    if (SWITCHDRV_TX_BUF_INFO_Q_SIZE > aBufIdx) {
        bufInfo = &SwitchDrv_TxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_PROCESSED == bufInfo->bufState) {
            if (TRUE == bufInfo->isTSEnabled) {
                bufInfo->isTSEnabled = FALSE;
                bufInfo->bufState = SWITCHDRV_BUF_STATE_FREE;
            }
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

int32_t SwitchDrv_RxProcessFrame(ETHER_HwIDType aCntrlID,
                                uint32_t aBufIdx,
                                uint8_t **const aDataInOut,
                                uint32_t * const aLenInOut,
                                uint32_t *const aIsMgmtFrameOnly)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;

    /* TODO: Check what to do with aIsMgmtFrameOnly */

    if ((SWITCHDRV_RX_BUF_INFO_Q_SIZE > aBufIdx)
            && (NULL != aDataInOut)
            && (NULL != (*aDataInOut))
            && (NULL != aLenInOut)
            && (0UL != (*aLenInOut))) {
        bufInfo = &SwitchDrv_RxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_FREE == bufInfo->bufState) {
            /* Assuming Broadcom header is present */
            bufInfo->mgmtInfo.PortIdx = ((*aDataInOut)[3])
                                        & SWITCH_EGRESS_MGMT_INFO_SRC_PID_MASK;
            *aLenInOut -= SWITCH_EGRESS_MGMT_INFO_SIZE;
            *aDataInOut = &((*aDataInOut)[SWITCH_EGRESS_MGMT_INFO_SIZE]);
            bufInfo->buf = *aDataInOut;
            bufInfo->bufState = SWITCHDRV_BUF_STATE_PROCESSED;
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

static int32_t SwitchDrv_GetRxTimestamp(ETHER_HwIDType aID,
                                const uint8_t* const aPktBuf,
                                ETHER_TimestampType *const aTS,
                                ETHER_TimestampQualType *const aTSQual,
                                ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t timeHwPort;

    retVal = SwitchDrv_GetTimeHwPort(aMgmtInfo->PortIdx, &timeHwPort);
    if (BCM_ERR_OK == retVal) {
        retVal = ETHER_TimeGetIngressTS(aID, timeHwPort, aPktBuf, aTS, aTSQual);
    }

    return retVal;
}

int32_t SwitchDrv_RxDoneInd (ETHER_HwIDType aCntrlID, uint32_t aBufIdx,
        const uint8_t **aBuf, ETHERSWT_MgmtInfoType **aMgmtInfo,
        ETHER_TimestampType *aTs, ETHER_TimestampQualType *aTsQual,
        uint32_t *aTSAvailable)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;
    uint32_t i, isPortTSEn = FALSE;
    const ETHERSWT_PortCfgType *portCfg;

    *aBuf = NULL;
    *aMgmtInfo = NULL;
    *aTSAvailable = FALSE;

    if (SWITCHDRV_RX_BUF_INFO_Q_SIZE > aBufIdx) {
        bufInfo = &SwitchDrv_RxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_PROCESSED == bufInfo->bufState) {
            *aBuf = bufInfo->buf;
            *aMgmtInfo = &(bufInfo->mgmtInfo);
            for (i = 0UL; i < SwitchDrv_Data.config->portCfgListSz; i++) {
                portCfg = &SwitchDrv_Data.config->portCfgList[i];
                if (bufInfo->mgmtInfo.PortIdx == portCfg->portID) {
                    if (TRUE == portCfg->enableTimeStamp) {
                        isPortTSEn = TRUE;
                    }
                    break;
                }
            }
            if (TRUE == isPortTSEn) {
                ret = SwitchDrv_GetRxTimestamp(aCntrlID, bufInfo->buf,
                        aTs, aTsQual, &(bufInfo->mgmtInfo));
                if (BCM_ERR_OK == ret) {
                    *aTSAvailable = TRUE;
                }
            }
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

int32_t SwitchDrv_RxDoneIndComplete (uint32_t aBufIdx)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    SwitchDrv_BufInfoType *bufInfo;

    if (SWITCHDRV_RX_BUF_INFO_Q_SIZE > aBufIdx) {
        bufInfo = &SwitchDrv_RxBufInfoQ[aBufIdx];
        if (SWITCHDRV_BUF_STATE_PROCESSED == bufInfo->bufState) {
            bufInfo->bufState = SWITCHDRV_BUF_STATE_FREE;
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_INVAL_BUF_STATE;
        }
    }

    return ret;
}

int32_t SwitchDrv_LinkIRQHandler(ETHERSWT_HwIDType aSwtID,
                                        ETHERSWT_PortIDType aPortID)
{
    uint64_t reg;
    int32_t ret = BCM_ERR_OK;
    uint16_t i = 0U;
    uint16_t linkStatus;

    if ((aSwtID >= SWITCH_MAX_HW_ID) || (aPortID >= ETHERSWT_PORT_ID_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }
    CFG_REGS->sw_intr_clr = SWITCH_INTR_CLR_ALL_PORT_LINK;

    /* Read link status change register */
    ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m01_lnkstschg, &reg));
    reg &= (uint64_t)SWITCH_LNK_STS_CHG_PORT_XLD7_MASK;
    SwitchDrv_Data.portLinkStatus |=
        (((uint32_t)reg << IS_PORT_LINK_STATUS_CHANGED_SHIFT)
        & IS_PORT_LINK_STATUS_CHANGED_MASK);
    if (0UL != reg) {
        /* Read link status register */
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m01_lnksts, &reg));
        reg &= (uint64_t)SWITCH_LNK_STS_PORT_XLD7_MASK;
        SwitchDrv_Data.portLinkStatus &= ~PORT_LINK_STATUS_MASK;
        SwitchDrv_Data.portLinkStatus |=
            (((uint32_t)reg << PORT_LINK_STATUS_SHIFT)
             & PORT_LINK_STATUS_MASK);
        linkStatus = SwitchDrv_Data.portLinkStatus;
        for (i = 0U; i < ETHERSWT_PORT_ID_MAX; i++) {
            if ((linkStatus & (0x1U << i)) != 0U) {
                (SwitchDrv_Data.portLinkStateChngCnt[i])++;
            }
        }
    }
err_exit:
    return ret;
}

int32_t SwitchDrv_LinkStatChgIndHandler(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType *const aLinkState,
        uint32_t *const aIsLinkStateChanged)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t portLinkStatus = SwitchDrv_Data.portLinkStatus;
    uint32_t isPortLinkStatChg =
        (portLinkStatus & IS_PORT_LINK_STATUS_CHANGED_MASK);
    SwitchDrv_Data.portLinkStatus &= ~(isPortLinkStatChg & (0x1UL << aPortID));
    isPortLinkStatChg >>= IS_PORT_LINK_STATUS_CHANGED_SHIFT;
    isPortLinkStatChg &= (0x1UL << aPortID);
    isPortLinkStatChg >>= aPortID;

    uint32_t portLinkStat = (portLinkStatus & PORT_LINK_STATUS_MASK);
    SwitchDrv_Data.portLinkStatus &= ~(portLinkStat & (0x1UL << aPortID));
    portLinkStat >>= PORT_LINK_STATUS_SHIFT;
    portLinkStat &= (0x1UL << aPortID);
    portLinkStat >>= aPortID;

    if (0UL != isPortLinkStatChg) {
        if (1UL == portLinkStat) {
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
        } else {
            *aLinkState = ETHXCVR_LINKSTATE_DOWN;
        }
        *aIsLinkStateChanged = TRUE;
    } else {
        *aIsLinkStateChanged = FALSE;
    }

    return retVal;
}

int32_t SwitchDrv_GetXcvrStats(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortStatsType *const aStats)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (aPortID < ETHERSWT_PORT_ID_MAX)
            && (NULL != aStats)) {
        aStats->linkStateChangeCount =
            SwitchDrv_Data.portLinkStateChngCnt[aPortID];
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

int32_t SwitchDrv_GetPortLinkState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType *const aLinkState)
{
    uint64_t reg;
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t portLinkStatus;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (aPortID < ETHERSWT_PORT_ID_MAX)
            && (NULL != aLinkState)) {

        /* Read link status register */
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m01_lnksts, &reg));
        reg &= (uint64_t)SWITCH_LNK_STS_PORT_XLD7_MASK;
        portLinkStatus = reg;
        portLinkStatus &= ~PORT_LINK_STATUS_MASK;
        portLinkStatus |= (((uint32_t)reg << PORT_LINK_STATUS_SHIFT)
                            & PORT_LINK_STATUS_MASK);
        if ((portLinkStatus & (0x1U << aPortID)) != 0U) {
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
        } else {
            *aLinkState = ETHXCVR_LINKSTATE_DOWN;
        }
        ret = BCM_ERR_OK;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetVLANPorts(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t *const aPortMask,
        uint32_t *const aTagMask,
        uint32_t *const aStaticPortMask)
{
    int ret = BCM_ERR_OK;
    uint32_t untagMap = 0UL;
    uint32_t fwdMap = 0UL;

    if ((aSwtID < SWITCH_MAX_HW_ID) && ((aVlanID & (~VLAN_ID_MASK)) == 0U)
            && (NULL != aPortMask) && (NULL != aTagMask)
            && (NULL != aStaticPortMask)) {
        /* Search through the VLAN table and find which all ports
         * are participating this particular VLAN */

        /* read the current port map configured in the HW */
        ERR_EXIT(SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
                VLAN_TBL_CMD_READ));

        *aPortMask = fwdMap;
        *aTagMask = (~untagMap) & fwdMap;
        *aStaticPortMask = 0UL;
        if (fwdMap != 0UL) {
            /* Search static ports for this VLAN */
            ret = SwitchDrv_GetVLANInitPort(aSwtID, aVlanID, aStaticPortMask);
            if (BCM_ERR_NOT_FOUND == ret) {
                *aStaticPortMask = 0UL;
                ret = BCM_ERR_OK;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }


err_exit:
    return ret;

}

int32_t SwitchDrv_AddVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aTaggedMask)
{
    int ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t portMask = (aPortMask & ((0x1UL << ETHERSWT_PORT_ID_MAX) - 1UL));
    ETHERSWT_VLANFwrdType forward;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (portMask != 0UL)
            && ((aVlanID & (~VLAN_ID_MASK)) == 0U)
            && (aVlanID <= ETHERSWT_VLANID_MAX)) {
        for (i = 0UL; i < ETHERSWT_PORT_ID_MAX; i++) {
            if ((portMask & (0x1UL << i)) != 0UL) {
                if ((aTaggedMask & (0x1UL << i)) != 0UL) {
                    forward =  ETHERSWT_VLAN_FRWRD_TAGGED;
                } else {
                    forward =  ETHERSWT_VLAN_FRWRD_UNTAGGED;
                }

                ret = SwitchDrv_UpdateVLANTblEntry(aSwtID, i, aVlanID, forward);
                if (BCM_ERR_OK != ret) {
                    break;
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t SwitchDrv_RemoveVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID)
{
    int ret = BCM_ERR_OK;
    uint32_t portMask = (aPortMask & ((0x1UL << ETHERSWT_PORT_ID_MAX) - 1UL));
    uint32_t initPortMask = 0x0UL;
    uint32_t untagMap = 0UL;
    uint32_t fwdMap = 0UL;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (portMask != 0UL)
            && ((aVlanID & (~VLAN_ID_MASK)) == 0U)
            && (aVlanID <= ETHERSWT_VLANID_MAX)) {
        /* Get VLAN ID's init config Port mask.
         */
        ret = SwitchDrv_GetVLANInitPort(aSwtID, aVlanID, &initPortMask);
        /* Static ports in a static VLAN are not allowed to be deleted */
        if (((portMask & initPortMask) != 0UL) && (BCM_ERR_OK == ret)) {
            ret = BCM_ERR_NOPERM;
            goto err_exit;
        }
        if ((BCM_ERR_NOT_FOUND == ret) || (BCM_ERR_OK == ret)) {
            /* read the current port map configured in the HW */
            ret = SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
                VLAN_TBL_CMD_READ);
            if (ret != BCM_ERR_OK) {
                goto err_exit;
            }
            /* check the portMask is a part of given vlan or not */
            if (0UL != ((~fwdMap) & portMask)){
                ret = BCM_ERR_INVAL_PARAMS;
                goto err_exit;
            }
            untagMap &= (~portMask);
            fwdMap   &= (~portMask);
            /* Update the VLAN table entry with updated map */
            ret = SwitchDrv_VLANTblRdWr(aSwtID, aVlanID, &untagMap, &fwdMap,
                VLAN_TBL_CMD_WRITE);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_SetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        ETHER_PrioType aPrio)
{
    int ret = BCM_ERR_OK;
    uint64_t reg;
    uint32_t portRegAddr;

    if ((aSwtID < SWITCH_MAX_HW_ID) && (aPortID < ETHERSWT_PORT_ID_MAX)
            && ((aVlanID & (~VLAN_ID_MASK)) == 0U)
            && (aPrio <= ETHER_PRIO_MAX)) {

        portRegAddr = SWITCH_M34_DEFAULT_1Q_TAG_PORT0 + aPortID * 2UL;
        reg = (aVlanID << SWITCH_DEFAULT_1Q_TAG_VID_0_SHIFT)
            & SWITCH_DEFAULT_1Q_TAG_VID_0_MASK;
        reg |= (aPrio << SWITCH_DEFAULT_1Q_TAG_PRI_0_SHIFT)
            & SWITCH_DEFAULT_1Q_TAG_PRI_0_MASK;
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, portRegAddr, reg));

        /* Disable dropping of all untagged packet on this port */
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl3,
                    &reg));
        reg &= ~PORTID2HWMASK(aPortID);
        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl3,
                    reg));
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType *const aVlanID,
        ETHER_PrioType *const aPrio)
{
    uint64_t reg;
    uint32_t portRegAddr;
    int ret = BCM_ERR_OK;

    if ((aSwtID < SWITCH_MAX_HW_ID) && (aPortID < ETHERSWT_PORT_ID_MAX)
            && (NULL != aVlanID) && (NULL != aPrio)) {

        portRegAddr = SWITCH_M34_DEFAULT_1Q_TAG_PORT0 + aPortID * 2UL;
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, portRegAddr, &reg));

        *aVlanID = ((reg & SWITCH_DEFAULT_1Q_TAG_VID_0_MASK)
                        >> SWITCH_DEFAULT_1Q_TAG_VID_0_SHIFT);
        *aPrio = ((reg & SWITCH_DEFAULT_1Q_TAG_PRI_0_MASK)
                        >> SWITCH_DEFAULT_1Q_TAG_PRI_0_SHIFT);

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_SetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType aMode)
{
    uint64_t reg;
    int ret = BCM_ERR_OK;

    /* Untagged packets would not be dropped */

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && ((ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED == aMode)
                || (ETHERSWT_VLAN_INGRESS_FILTER_MODE_DISABLED == aMode))) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl4,
                    &reg));
        reg &= ~(SWITCH_P34VC4_PAGE_34_VLAN_CTRL4_INGR_VID_CHK_MASK); /* Clear the bits */
        if (ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED == aMode) {
            /* Enable filtering - drop frame if VID violation */
            reg |= (SWITCH_VLAN_CTRL4_INGR_VID_CHK_VID_VIO
                    << SWITCH_P34VC4_PAGE_34_VLAN_CTRL4_INGR_VID_CHK_SHIFT);
        }

        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl4,
                    reg));

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType *const aMode)
{
    uint64_t reg;
    int ret = BCM_ERR_OK;

    if ((aSwtID < SWITCH_MAX_HW_ID) && (NULL != aMode)) {

        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m34_vlan_ctrl4,
                    &reg));
        reg &= (SWITCH_P34VC4_PAGE_34_VLAN_CTRL4_INGR_VID_CHK_MASK);
        reg >>= SWITCH_P34VC4_PAGE_34_VLAN_CTRL4_INGR_VID_CHK_SHIFT;
        if (SWITCH_VLAN_CTRL4_INGR_VID_CHK_VID_VIO == reg) {
            *aMode = ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED;
        } else {
            *aMode = ETHERSWT_VLAN_INGRESS_FILTER_MODE_DISABLED;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}


int32_t SwitchDrv_SetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t aAge)
{
    uint64_t reg;
    int ret = BCM_ERR_OK;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (aAge < SWITCH_BCM895XX_AGE_TIMER_MAX)) {

        /* Update the aging timer control register */
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_sptagt, &reg));
        reg &= ~SWITCH_PAGE_02_SPTAGT_AGE_TIME_MASK;
        reg |= SWITCH_PAGE_02_SPTAGT_AGE_CHANGE_EN_MASK;
        reg |= (aAge & SWITCH_PAGE_02_SPTAGT_AGE_TIME_MASK);

        ERR_EXIT(SwitchDrv_WriteReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_sptagt, reg));

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_GetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t *const aAge)
{
    uint64_t reg;
    int ret = BCM_ERR_OK;

    if ((aSwtID < SWITCH_MAX_HW_ID) && (NULL != aAge)) {
        ERR_EXIT(SwitchDrv_ReadReg(aSwtID, (uint32_t)&SWITCH_REGS->m02_sptagt, &reg));
        *aAge = (reg & SWITCH_PAGE_02_SPTAGT_AGE_TIME_MASK);

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_AddARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry)
{
    uint64_t mac;
    uint8_t *temp;
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    ARLTBL_HWEntryType hwEntry;
    uint32_t portMask;
    uint32_t untagMap = 0UL;
    uint32_t fwdMap = 0UL;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (aARLEntry != NULL)
            && ((aARLEntry->portMask & ((0x1UL << ETHERSWT_PORT_ID_MAX) - 1UL)) != 0UL)
            && (((aARLEntry->vlanID) & (~VLAN_ID_MASK)) == 0U)) {
        portMask = (aARLEntry->portMask
                & ((0x1UL << ETHERSWT_PORT_ID_MAX) - 1UL));

        /* If the entry is unicast, ensure that only a single port */
        /* is provided in the destination map                      */
        if (0x1U != (aARLEntry->macAddr[0U] & 0x1U)) {
            if (0UL != (aARLEntry->portMask & (aARLEntry->portMask - 1UL))) {
                ret = BCM_ERR_INVAL_PARAMS;
                goto err_exit;
            } else {
                /* Convert portmask to port number for unicast entry */
                for (i = 0UL; i < ETHERSWT_PORT_ID_MAX; ++i) {
                    if (0UL != ((1UL << i) & aARLEntry->portMask)) {
                        portMask = i;
                        break;
                    }
                }
            }
        }
        /* read the current port map configured in the HW */
        ret = SwitchDrv_VLANTblRdWr(aSwtID, aARLEntry->vlanID, &untagMap, &fwdMap,
                VLAN_TBL_CMD_READ);
        if (ret != BCM_ERR_OK) {
            goto err_exit;
        }
        /* check the aARLEntry->portMask is a part of given vlan or not  */
        if (0UL != ((~fwdMap) & aARLEntry->portMask)){
            ret = BCM_ERR_INVAL_PARAMS;
            goto err_exit;
        }

        hwEntry.vlanID = aARLEntry->vlanID;
        hwEntry.portMask = portMask;
        temp = aARLEntry->macAddr;
        mac = NMAC2HWMAC(temp);
        hwEntry.mac = mac;
        ret = SwitchDrv_ARLTblAddEntry(aSwtID, &hwEntry);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

err_exit:
    return ret;
}

int32_t SwitchDrv_DeleteARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry)
{
    uint64_t mac;
    uint8_t *temp;
    int32_t ret = BCM_ERR_OK;
    ARLTBL_HWEntryType hwEntry;
    uint32_t isInitARLEntry = FALSE;

    if ((aSwtID < SWITCH_MAX_HW_ID)
            && (aARLEntry != NULL)
            && (((aARLEntry->vlanID) & (~VLAN_ID_MASK)) == 0U)) {

        /* find if the entry already exits in the ARL table
         * if yes, delete the entry
         */
        hwEntry.vlanID = aARLEntry->vlanID;
        hwEntry.portMask = 0UL;
        temp = aARLEntry->macAddr;
        mac = NMAC2HWMAC(temp);
        hwEntry.mac = mac;
        /* Search if this ARL entry is present in switch
         * config, if so then this ARL entry should not be
         * deleted */
        ret = SwitchDrv_IsARLEntryInitConfig(aSwtID, &hwEntry, &isInitARLEntry);
        if (BCM_ERR_OK == ret) {
            if (TRUE == isInitARLEntry) {
                ret = BCM_ERR_NOPERM;
            } else {
                ret = SwitchDrv_ARLTblDeleteEntry(aSwtID, &hwEntry);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

