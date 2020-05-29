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
#include <chip_config.h>
#include <utils.h>
#include <bcm_err.h>
#include <eth_switch.h>
#include <eth_switch_osil.h>
#include "switch_drv.h"
#ifdef ENABLE_CFP
#include "cfp_drv.h"
#endif
#include <mcu_osil.h>

/* switch state types */
typedef uint32_t ETHERSWT_StateType;
#define ETHERSWT_STATE_UNINIT       (0UL)
#define ETHERSWT_STATE_INIT         (1UL)

typedef uint32_t ETHERSWT_MgmtStateType;
#define ETHERSWT_MGMT_STATE_UNINIT       (0UL)
#define ETHERSWT_MGMT_STATE_INIT         (1UL)
typedef struct {
     ETHERSWT_StateType state;
     const ETHERSWT_CfgType *config;
     ETHERSWT_MgmtStateType mgmtState;

} ETHERSWT_RWDevType;

static ETHERSWT_RWDevType COMP_SECTION(".data.drivers") ETHERSWT_RWDev = {
    .config = NULL,
    .state = ETHERSWT_STATE_UNINIT,
    .mgmtState = ETHERSWT_MGMT_STATE_UNINIT,
};

static void ETHERSWT_ReportError(uint16_t aCompID, uint8_t aInstanceID,
        uint8_t aApiID, int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, aInstanceID, aApiID, aErr, 4UL, values);
}

static int32_t ETHERSWT_GetXcvrID(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID, uint32_t *const aXcvrID)
{
    uint32_t i;
    int ret = BCM_ERR_OK;

    for (i = 0UL; i < ETHERSWT_RWDev.config->portCfgListSz; i++) {
        if (ETHERSWT_RWDev.config->portCfgList[i].portID == aPortID) {
            *aXcvrID = ETHERSWT_RWDev.config->portCfgList[i].xcvrID;
            break;
        }
    }
    if (i == ETHERSWT_RWDev.config->portCfgListSz) {
        ret = BCM_ERR_NOT_FOUND;
    }
    return ret;
}

int32_t ETHERSWT_GetPortMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_ModeType *const aMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aMode != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetMode(xcvrID, aMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_SetPortMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_ModeType aMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_SetMode(xcvrID, aMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortMasterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aMode != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetMasterMode(xcvrID, aMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_SetPortMasterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_SetMasterMode(xcvrID, aMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortLoopbackMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aMode != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetLoopbackMode(xcvrID, aMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_SetPortLoopbackMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_SetLoopbackMode(xcvrID, aMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMode)
{
    int32_t ret;
    EthSwtIO swtIO;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aMode != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (BCM_ERR_OK == ret) {
                ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_JUMBO_MODE, &swtIO);
                if (ret == BCM_ERR_OK) {
                    ret = ETHXCVR_GetJumboMode(xcvrID, aMode);
                    if (ret == BCM_ERR_OK ) {
                        if (!(swtIO.jumbo == *aMode)) {
                            ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                                    BRCM_SWARCH_ETHERSWT_GET_PORT_JUMBOMODE_PROC, ret, aPortID, swtIO.mode, *aMode,
                                    __LINE__);
                            ret = BCM_ERR_UNKNOWN;
                        }
                    }
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_SetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode)
{
    int32_t ret;
    EthSwtIO swtIO;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.jumbo = aMode;
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (BCM_ERR_OK == ret) {
                ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_JUMBO_MODE, &swtIO);
                if (ret == BCM_ERR_OK) {
                    ret = ETHXCVR_SetJumboMode(xcvrID, aMode);
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType *const aMode)

{
    int32_t ret;
    EthSwtIO swtIO;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aMode != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_DUMBFWD_MODE, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aMode = swtIO.mode;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_SetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType aMode)
{
    int32_t ret;
    EthSwtIO swtIO;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.mode = aMode;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_DUMBFWD_MODE, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortLinkState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType *const aLinkState)
{
    int32_t ret;
    EthSwtIO swtIO;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aLinkState != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_LINKSTATE, &swtIO);
            if (ret == BCM_ERR_OK) {
                *aLinkState = swtIO.linkState;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetPortSpeed(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_SpeedType aSpeed)
{
    int32_t ret;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_SetSpeed(xcvrID, aSpeed);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortSpeed(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_SpeedType *const aSpeed)
{
    int32_t ret;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aSpeed != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetSpeed(xcvrID, aSpeed);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_EnablePortAutoNeg(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID)
{
    int32_t ret;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_SetAutoNegMode(xcvrID, ETHXCVR_BOOLEAN_TRUE);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortAutoNegStatus(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_AutoNegStatusType *const aAutoNegStatus)
{
    int32_t ret;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aAutoNegStatus != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetAutoNegStatus(xcvrID, aAutoNegStatus);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetPortDuplexMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t ret;
    uint32_t xcvrID;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aDuplexMode != NULL)){
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetDuplexMode(xcvrID, aDuplexMode);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}
int32_t ETHERSWT_GetRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHER_RxStatsType *const aRxStat)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aRxStat != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.rxStat = aRxStat;
            swtIO.portHwID = aPortID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_RX_STAT, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_GET_RX_STAT_PROC, ret, 0UL, 0UL, 0UL,
                        __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_ClearRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CLEAR_RX_STAT, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetTxStat(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHER_TxStatsType *const aTxStat)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aTxStat != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.txStat = aTxStat;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_TX_STAT, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_GET_TX_STAT_PROC, ret, 0UL, 0UL, 0UL,
                        __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_ClearTxStat(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CLEAR_TX_STAT, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_GetXcvrStats(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortStatsType *const aStats)
{
    int32_t ret;
    EthSwtIO swtIO;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX){
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.portStats = aStats;
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_XCVR_STATS,
                        &swtIO);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetSQIValue(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        uint32_t *const aSQIValue)
{
    int32_t ret;
    uint32_t xcvrID;

    if (aSwtID < ETHERSWT_HW_ID_MAX){
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret = ETHXCVR_GetSQIValue(aPortID, aSQIValue);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode aMode)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.learningMode = &aMode;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_MACLEARNING_MODE, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_SET_MAC_LEARN_MODE_PROC, ret, 0UL, 0UL,
                        0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode *const aMode)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.learningMode = aMode;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_MACLEARNING_MODE, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_GET_MAC_LEARN_MODE_PROC, ret, 0UL, 0UL,
                        0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        ETHERSWT_PortMirrorCfgType *const aConfig)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.direction = aDirection;
            swtIO.portMirrorCfg = aConfig;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_PORT_MIRROR_CFG, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_SET_PORT_MIRROR_CFG_PROC, ret, 0UL, 0UL,
                        0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        ETHERSWT_PortMirrorCfgType *const aConfig)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.direction = aDirection;
            swtIO.portMirrorCfg = aConfig;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_PORT_MIRROR_CFG, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_GET_PORT_MIRROR_CFG_PROC, ret, 0UL, 0UL,
                        0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType aState)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.portMirrorState = &aState;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_PORT_MIRROR_MODE, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_SET_PORT_MIRROR_STATE_PROC, ret, 0UL, 0UL,
                        0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType *const aState)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.portMirrorState = aState;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_PORT_MIRROR_MODE, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetMirrorCapturePort(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_MIRROR_CAPTURE_PORT, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aPortID = swtIO.portHwID;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_ReadReg(ETHERSWT_HwIDType aSwtID, uint32_t aAddr,
        uint64_t *const aVal)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (aVal != NULL)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.regAddr = aAddr;
            swtIO.regVal = aVal;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_READ_REG, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_READ_REG_PROC, ret, 0UL, 0UL, 0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_WriteReg(ETHERSWT_HwIDType aSwtID, uint32_t aAddr,
        uint64_t aVal)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.swtHwID = aSwtID;
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.regAddr = aAddr;
            swtIO.regVal = &aVal;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_WRITE_REG, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_WRITE_REG_PROC, ret, 0UL, 0UL, 0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_EnableVLAN(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aEnable)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.vlanID = aVlanID;
            swtIO.enable = aEnable;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_ENABLE_VLAN, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_ENABLE_VLAN_PROC, ret, 0UL, 0UL, 0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetARLTable(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLTbl,
        uint16_t *const aTblSize)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.ARLTbl = aARLTbl;
            swtIO.ARLTblSz = aTblSize;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_ARL_TABLE, &swtIO);
            if (BCM_ERR_OK != ret) {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_GET_ARL_TBL_PROC, ret, 0UL, 0UL, 0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetPortMacAddr(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID,
        const uint8_t *const aMacAddr,
        ETHERSWT_VLANIDType aVlanID)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (NULL != aPortID)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.macAddr = aMacAddr;
            swtIO.vlanID = aVlanID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_PORT_MAC_ADDR, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aPortID = swtIO.portHwID;
            } else {
                ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER),
                        (uint8_t)aSwtID,
                        BRCM_SWARCH_ETHERSWT_GET_PORT_MAC_ADDR_PROC, ret,
                        0UL, 0UL, 0UL, __LINE__);
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetLedState(ETHERSWT_HwIDType aSwtID,
                                ETHERSWT_PortIDType aPortID,
                                ETHERSWT_LedType aLedType,
                                uint32_t aTurnOn)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.ledType = aLedType;
            swtIO.enable = aTurnOn;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_LED_STATE, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetLedState(ETHERSWT_HwIDType aSwtID,
                                ETHERSWT_PortIDType aPortID,
                                ETHERSWT_LedType aLedType,
                                uint32_t *const aIsStateOn)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (NULL != aIsStateOn)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.ledType = aLedType;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_LED_STATE, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aIsStateOn = swtIO.enable;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetVLANPorts(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t *const aPortMask,
        uint32_t *const aTagMask,
        uint32_t *const aStaticPortMask)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (NULL != aPortMask)
            && (NULL != aTagMask) && (NULL != aStaticPortMask)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.vlanID = aVlanID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_VLAN_PORTS, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aPortMask = swtIO.portMask;
                *aTagMask = swtIO.tagMask;
                *aStaticPortMask = swtIO.staticPortMask;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_AddVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aTaggedMask)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portMask = aPortMask;
            swtIO.vlanID = aVlanID;
            swtIO.tagMask = aTaggedMask;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_ADD_VLAN_PORT, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_RemoveVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portMask = aPortMask;
            swtIO.vlanID = aVlanID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_REMOVE_VLAN_PORT, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        ETHER_PrioType aPrio)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.vlanID = aVlanID;
            swtIO.prio = aPrio;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_PORT_DEFAULT_VLAN, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType *const aVlanID,
        ETHER_PrioType *const aPrio)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (NULL != aVlanID) && (NULL != aPrio)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_PORT_DEFAULT_VLAN, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aVlanID = swtIO.vlanID;
                *aPrio = swtIO.prio;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType aMode)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.mode = aMode;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_VLAN_IFILTER_MODE, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_VLANIngressFilterModeType *const aMode)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (NULL != aMode)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_VLAN_IFILTER_MODE, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aMode = swtIO.mode;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_SetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t aAge)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.age = aAge;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_AGE, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t *const aAge)
{
    EthSwtIO swtIO;
    int32_t ret;

    if ((aSwtID < ETHERSWT_HW_ID_MAX) && (NULL != aAge)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_GET_AGE, &swtIO);
            if (BCM_ERR_OK == ret) {
                *aAge = swtIO.age;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_AddARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.arlEntry = aARLEntry;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_ADD_ARL_ENTRY, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_DeleteARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.arlEntry = aARLEntry;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_DELETE_ARL_ENTRY, &swtIO);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

int32_t ETHERSWT_GetPortType(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BusModeType *const aBusMode,
        ETHXCVR_PhyMediaType *const aPhyMedia)
{
    uint32_t xcvrID;
    int32_t ret;
    ETHXCVR_PortConfigType portConfig;

    if ((aSwtID < ETHERSWT_HW_ID_MAX)
            && (aPortID < ETHERSWT_PORT_ID_MAX)
            && (NULL != aBusMode) && (NULL != aPhyMedia)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            ret = ETHERSWT_GetXcvrID(aSwtID, aPortID, &xcvrID);
            if (ret == BCM_ERR_OK) {
                ret  = ETHXCVR_GetPortConfig(xcvrID, &portConfig);
                if (BCM_ERR_OK == ret) {
                    *aBusMode = portConfig.busMode;
                    *aPhyMedia = portConfig.phy.phyMedia;
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}
void ETHERSWT_Init(ETHERSWT_HwIDType aSwtID,
        const ETHERSWT_CfgType *const aConfig)
{
    EthSwtIO swtIO;
    int32_t ret = BCM_ERR_OK;
    uint32_t port2TimeFifoMap[ETHERSWT_PORT_ID_MAX];

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = MCU_GetSwitchPort2TimeFifoMap(port2TimeFifoMap);
            if (BCM_ERR_OK == ret) {
                swtIO.retVal = BCM_ERR_UNKNOWN;
                swtIO.swtHwID = aSwtID;
                swtIO.cfg = aConfig;
                swtIO.port2TimeFifoMap = &port2TimeFifoMap[0UL];
                ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_INIT, &swtIO);
            }
        } else {
            ret = BCM_ERR_INVAL_STATE;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        ETHERSWT_ReportError((BCM_SWT_ID & BCM_LOGMASK_USER), (uint8_t)aSwtID,
                BRCM_SWARCH_ETHERSWT_INIT_PROC, ret, 0UL, 0UL, 0UL, __LINE__);
    }
}

int32_t ETHERSWT_MgmtInit(ETHERSWT_HwIDType aSwtID)
{
    EthSwtIO swtIO;
    int32_t ret = BCM_ERR_OK;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            if (ETHERSWT_RWDev.mgmtState != ETHERSWT_MGMT_STATE_UNINIT) {
                ret = BCM_ERR_INVAL_STATE;
            } else {
                swtIO.retVal = BCM_ERR_UNKNOWN;
                swtIO.swtHwID = aSwtID;
                ret = MCU_EnableSwitchCPUPort();
                if (BCM_ERR_OK == ret) {
                    ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_MGMT_INIT, &swtIO);
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_MgmtDeInit(ETHERSWT_HwIDType aSwtID)
{
    EthSwtIO swtIO;
    int32_t ret = BCM_ERR_OK;

    if (aSwtID < ETHERSWT_HW_ID_MAX) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            ret = BCM_ERR_UNINIT;
        } else {
            if (ETHERSWT_RWDev.mgmtState != ETHERSWT_MGMT_STATE_INIT) {
                ret = BCM_ERR_INVAL_STATE;
            } else {
                swtIO.retVal = BCM_ERR_UNKNOWN;
                swtIO.swtHwID = aSwtID;
                ret = MCU_DisableSwitchCPUPort();
                if (BCM_ERR_OK == ret) {
                    ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_MGMT_DEINIT, &swtIO);
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t ETHERSWT_SetMgmtInfo(ETHER_HwIDType aCntrlID,
                            uint32_t aBufIdx,
                            ETHERSWT_MgmtInfoType * const aMgmtInfo)
{
    EthSwtIO swtIO;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        swtIO.retVal = BCM_ERR_UNKNOWN;
        swtIO.ctrlHwID = aCntrlID;
        swtIO.bufIdx = aBufIdx;
        swtIO.mgmtInfo = aMgmtInfo;

        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_SET_MGMT_INFO, &swtIO);
    }

    return ret;
}

int32_t ETHERSWT_EnableTxTimestamp(ETHER_HwIDType aCntrlID,
                            uint32_t aBufIdx,
                            ETHERSWT_MgmtInfoType * const aMgmtInfo)
{
    EthSwtIO swtIO;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        ret = SwitchDrv_EnableTxTimestamp(aCntrlID, aBufIdx, aMgmtInfo);

        if (BCM_ERR_OK == ret) {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.ctrlHwID = aCntrlID;
            swtIO.bufIdx = aBufIdx;

            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_ENABLE_TX_TIMESTAMP, &swtIO);
        }
    }

    return ret;
}

int32_t ETHERSWT_TxAdaptBuffer(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        swtIO.retVal = BCM_ERR_UNKNOWN;
        swtIO.ctrlHwID = aCntrlID;
        swtIO.bufIdx = aBufIdx;
        swtIO.data = aDataInOut;
        swtIO.len = aLenInOut;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_TX_ADAPT_BUFFER, &swtIO);
    }

    return ret;
}

int32_t ETHERSWT_TxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        swtIO.retVal = BCM_ERR_UNKNOWN;
        swtIO.ctrlHwID = aCntrlID;
        swtIO.bufIdx = aBufIdx;
        swtIO.data = aDataInOut;
        swtIO.len = aLenInOut;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_TX_PROCESS_FRAME, &swtIO);
    }

    return ret;
}

int32_t ETHERSWT_TxDoneInd(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx)
{
    EthSwtIO swtIO;
    int32_t ret;
    ETHER_TimestampType ts;
    ETHER_TimestampQualType tsQual;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        swtIO.retVal = BCM_ERR_UNKNOWN;
        swtIO.ctrlHwID = aCntrlID;
        swtIO.bufIdx = aBufIdx;
        swtIO.mgmtInfo = NULL;
        swtIO.ts = &ts;
        swtIO.tsQual = &tsQual;

        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_TX_DONE_IND, &swtIO);
        if ((BCM_ERR_OK == ret) && (NULL != swtIO.mgmtInfo)) {
            ETHERSWT_TxTSInd(aCntrlID, aBufIdx, swtIO.mgmtInfo, &ts, &tsQual);
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_TX_DONE_IND_COMPLETE, &swtIO);
        }
    }

    return ret;
}

int32_t ETHERSWT_RxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut,
        uint32_t *const aIsMgmtFrameOnly)
{
    EthSwtIO swtIO;
    int32_t ret;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        swtIO.retVal = BCM_ERR_UNKNOWN;
        swtIO.ctrlHwID = aCntrlID;
        swtIO.bufIdx = aBufIdx;
        swtIO.data = aDataInOut;
        swtIO.len = aLenInOut;
        swtIO.isMgmtFrameOnly = aIsMgmtFrameOnly;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_RX_PROCESS_FRAME, &swtIO);
    }

    return ret;
}

int32_t ETHERSWT_RxDoneInd(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx)
{
    EthSwtIO swtIO;
    int32_t ret;
    ETHER_TimestampType ts;
    ETHER_TimestampQualType tsQual;

    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        ret = BCM_ERR_UNINIT;
    } else if (ETHERSWT_RWDev.mgmtState == ETHERSWT_MGMT_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        swtIO.retVal = BCM_ERR_UNKNOWN;
        swtIO.ctrlHwID = aCntrlID;
        swtIO.bufIdx = aBufIdx;
        swtIO.ts = &ts;
        swtIO.tsQual = &tsQual;

        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_RX_DONE_IND, &swtIO);
        if (BCM_ERR_OK == ret) {
            ETHERSWT_MgmtInfoInd(aCntrlID, swtIO.buf, swtIO.mgmtInfo);
            if (TRUE == swtIO.tsAvailable) {
                ETHERSWT_RxTSInd(aCntrlID, swtIO.buf, swtIO.mgmtInfo,
                        swtIO.ts, swtIO.tsQual);
            }
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_RX_DONE_IND_COMPLETE, &swtIO);
        }
    }

    return ret;
}

void ETHERSWT_LinkIRQHandler(ETHERSWT_HwIDType aSwtID,
                                    ETHERSWT_PortIDType aPortID)
{
    int32_t ret;
    if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
        /* If this interrupt request is raised in switch driver
         * uninitialised state. It is an erraneous condition.
         * But it is required to service the interrupt, so that
         * interrupt status could be cleared and firmware could
         * continue with the usual processing. Also, it is
         * required to log error in this condition.
         */
        ret = SwitchDrv_LinkIRQHandler(aSwtID, aPortID);
        if (BCM_ERR_OK != ret) {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = SwitchDrv_LinkIRQHandler(aSwtID, aPortID);
    }

    if (BCM_ERR_OK != ret) {
        ETHERSWT_ReportError(BCM_SWT_ID, (uint8_t)aSwtID,
                BRCM_SWARCH_ETHERSWT_IL_LINK_IRQ_HANDLER_PROC, ret,
                aPortID, 0UL, 0UL, __LINE__);
    }
}

void ETHERSWT_LinkStatChgIndHandler(ETHERSWT_HwIDType aSwtID,
                                    ETHERSWT_PortIDType aPortID)
{
    ETHXCVR_LinkStateType linkState;
    uint32_t isLinkStateChanged;
    int32_t retVal;
    EthSwtIO swtIO;

    if ((aSwtID < ETHERSWT_HW_ID_MAX)
            && (aPortID < ETHERSWT_PORT_ID_MAX)) {
        if (ETHERSWT_RWDev.state == ETHERSWT_STATE_UNINIT) {
            retVal = BCM_ERR_UNINIT;
        } else {
            swtIO.retVal = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aSwtID;
            swtIO.portHwID = aPortID;
            swtIO.linkStatePtr = &linkState;
            swtIO.isLinkStateChangedPtr = &isLinkStateChanged;
            retVal = ETHERSWT_SysCmdReq(SWT_IO_CMD_LINK_STATE_CHANGE_HDLR, &swtIO);

            if ((BCM_ERR_OK == retVal) && (TRUE == isLinkStateChanged)) {
                ETHERSWT_IntgLinkStateChangeInd(aSwtID, aPortID, linkState);
            }
        }
    }
}

int32_t ETHERSWT_CmdHandler(ETHERSWT_CmdType aCmd, EthSwtIO *const aIO)
{
    int32_t ret;
    if (NULL != aIO) {
        switch (aCmd) {
        case SWT_IO_CMD_INIT:
            ret = SwitchDrv_Init(aIO->swtHwID, aIO->cfg, aIO->port2TimeFifoMap);
            if (ret == BCM_ERR_OK) {
                ETHERSWT_RWDev.state = ETHERSWT_STATE_INIT;
                ETHERSWT_RWDev.config = aIO->cfg;
            }
            break;
        case SWT_IO_CMD_GET_ARL_TABLE:
            ret = SwitchDrv_GetARLTable(aIO->swtHwID, aIO->ARLTbl,
                    aIO->ARLTblSz);
            break;
        case SWT_IO_CMD_READ_REG:
            ret = SwitchDrv_ReadReg(aIO->swtHwID, aIO->regAddr,
                    aIO->regVal);
            break;
        case SWT_IO_CMD_WRITE_REG:
            ret = SwitchDrv_WriteReg(aIO->swtHwID, aIO->regAddr,
                    *(aIO->regVal));
            break;
        case SWT_IO_CMD_GET_RX_STAT:
            ret = SwitchDrv_GetRxStat(aIO->swtHwID, aIO->portHwID, aIO->rxStat);
            break;
        case SWT_IO_CMD_GET_TX_STAT:
            ret = SwitchDrv_GetTxStat(aIO->swtHwID, aIO->portHwID, aIO->txStat);
            break;
        case SWT_IO_CMD_CLEAR_RX_STAT:
            ret = SwitchDrv_ClearRxStat(aIO->swtHwID, aIO->portHwID);
            break;
        case SWT_IO_CMD_CLEAR_TX_STAT:
            ret = SwitchDrv_ClearTxStat(aIO->swtHwID, aIO->portHwID);
            break;
        case SWT_IO_CMD_GET_PORT_MAC_ADDR:
            ret = SwitchDrv_GetPortMacAddr(aIO->swtHwID, &(aIO->portHwID),
                    aIO->macAddr, aIO->vlanID);
            break;
        case SWT_IO_CMD_SET_MACLEARNING_MODE:
            ret =SwitchDrv_SetMACLearningMode(aIO->swtHwID, aIO->portHwID,
                    *(aIO->learningMode));
            break;
        case SWT_IO_CMD_GET_MACLEARNING_MODE:
            ret = SwitchDrv_GetMACLearningMode(aIO->swtHwID, aIO->portHwID,
                    aIO->learningMode);
            break;
        case SWT_IO_CMD_ENABLE_VLAN:
            ret = SwitchDrv_EnableVLAN(aIO->swtHwID, aIO->portHwID,
                    aIO->vlanID, aIO->enable);
            break;
        case SWT_IO_CMD_SET_PORT_MIRROR_CFG:
            ret = SwitchDrv_SetPortMirrorConfig(aIO->swtHwID, aIO->portHwID,
                    aIO->direction, aIO->portMirrorCfg);
            break;
        case SWT_IO_CMD_GET_PORT_MIRROR_CFG:
            ret = SwitchDrv_GetPortMirrorConfig(aIO->swtHwID, aIO->portHwID,
                    aIO->direction, aIO->portMirrorCfg);
            break;
        case SWT_IO_CMD_SET_PORT_MIRROR_MODE:
            ret = SwitchDrv_SetPortMirrorState(aIO->swtHwID, aIO->portHwID,
                    *aIO->portMirrorState);
            break;
        case SWT_IO_CMD_GET_PORT_MIRROR_MODE:
            ret = SwitchDrv_GetPortMirrorState(aIO->swtHwID, aIO->portHwID,
                    aIO->portMirrorState);
            break;
        case SWT_IO_CMD_GET_MIRROR_CAPTURE_PORT:
            ret = SwitchDrv_GetMirrorCapturePort(aIO->swtHwID, &(aIO->portHwID));
            break;
        case SWT_IO_CMD_MGMT_INIT:
            ETHERSWT_RWDev.mgmtState = ETHERSWT_MGMT_STATE_INIT;
            ret = BCM_ERR_OK;
            break;
        case SWT_IO_CMD_MGMT_DEINIT:
            ETHERSWT_RWDev.mgmtState = ETHERSWT_MGMT_STATE_UNINIT;
            ret = BCM_ERR_OK;
            break;
        case SWT_IO_CMD_ENABLE_TX_TIMESTAMP:
            ret = SwitchDrv_SetTSEnabled(aIO->ctrlHwID, aIO->bufIdx);
            break;
        case SWT_IO_CMD_SET_LED_STATE:
            ret = SwitchDrv_SetLedState(aIO->swtHwID, aIO->portHwID,
                    aIO->ledType, aIO->enable);
            break;
        case SWT_IO_CMD_GET_LED_STATE:
            ret = SwitchDrv_GetLedState(aIO->swtHwID, aIO->portHwID,
                    aIO->ledType, &(aIO->enable));
            break;
        case SWT_IO_CMD_SET_JUMBO_MODE:
            ret = SwitchDrv_SetPortJumboMode(aIO->swtHwID, aIO->portHwID, aIO->jumbo);
            break;
        case SWT_IO_CMD_GET_JUMBO_MODE:
            ret = SwitchDrv_GetPortJumboMode(aIO->swtHwID, aIO->portHwID, &(aIO->jumbo));
            break;
        case SWT_IO_CMD_SET_DUMBFWD_MODE:
            ret = SwitchDrv_SetDumbFwdMode(aIO->swtHwID, aIO->mode);
            break;
        case SWT_IO_CMD_GET_DUMBFWD_MODE:
            ret = SwitchDrv_GetDumbFwdMode(aIO->swtHwID, &(aIO->mode));
            break;
        case SWT_IO_CMD_GET_XCVR_STATS:
            ret = SwitchDrv_GetXcvrStats(aIO->swtHwID, aIO->portHwID, aIO->portStats);
            break;
        case SWT_IO_CMD_GET_LINKSTATE:
            ret = SwitchDrv_GetPortLinkState(aIO->swtHwID, aIO->portHwID, &(aIO->linkState));
            break;
        case SWT_IO_CMD_GET_VLAN_PORTS:
            ret = SwitchDrv_GetVLANPorts(aIO->swtHwID, aIO->vlanID,
                    &(aIO->portMask), &(aIO->tagMask), &(aIO->staticPortMask));
            break;
        case SWT_IO_CMD_ADD_VLAN_PORT:
            ret = SwitchDrv_AddVLANPorts(aIO->swtHwID, aIO->portMask,
                    aIO->vlanID, aIO->tagMask);
            break;
        case SWT_IO_CMD_REMOVE_VLAN_PORT:
            ret = SwitchDrv_RemoveVLANPorts(aIO->swtHwID, aIO->portMask,
                    aIO->vlanID);
            break;
        case SWT_IO_CMD_SET_PORT_DEFAULT_VLAN:
            ret = SwitchDrv_SetPortDefaultVlan(aIO->swtHwID, aIO->portHwID,
                    aIO->vlanID, aIO->prio);
            break;
        case SWT_IO_CMD_GET_PORT_DEFAULT_VLAN:
            ret = SwitchDrv_GetPortDefaultVlan(aIO->swtHwID, aIO->portHwID,
                    &(aIO->vlanID), &(aIO->prio));
            break;
        case SWT_IO_CMD_SET_VLAN_IFILTER_MODE:
            ret = SwitchDrv_SetVLANIngressFilterMode(aIO->swtHwID, aIO->mode);
            break;
        case SWT_IO_CMD_GET_VLAN_IFILTER_MODE:
            ret = SwitchDrv_GetVLANIngressFilterMode(aIO->swtHwID,
                    &(aIO->mode));
            break;
        case SWT_IO_CMD_SET_AGE:
            ret = SwitchDrv_SetAge(aIO->swtHwID, aIO->age);
            break;
        case SWT_IO_CMD_GET_AGE:
            ret = SwitchDrv_GetAge(aIO->swtHwID, &(aIO->age));
            break;
        case SWT_IO_CMD_ADD_ARL_ENTRY:
            ret = SwitchDrv_AddARLEntry(aIO->swtHwID, aIO->arlEntry);
            break;
        case SWT_IO_CMD_DELETE_ARL_ENTRY:
            ret = SwitchDrv_DeleteARLEntry(aIO->swtHwID, aIO->arlEntry);
            break;
        case SWT_IO_CMD_TX_ADAPT_BUFFER:
            ret = SwitchDrv_TxAdaptBuffer(aIO->ctrlHwID, aIO->bufIdx, aIO->data, aIO->len);
            break;
        case SWT_IO_CMD_TX_PROCESS_FRAME:
            ret = SwitchDrv_TxProcessFrame(aIO->ctrlHwID, aIO->bufIdx, aIO->data, aIO->len);
            break;
        case SWT_IO_CMD_TX_DONE_IND:
            ret = SwitchDrv_TxDoneInd(aIO->ctrlHwID, aIO->bufIdx, &aIO->mgmtInfo,
                  aIO->ts, aIO->tsQual);
            break;
        case SWT_IO_CMD_TX_DONE_IND_COMPLETE:
            ret = SwitchDrv_TxDoneIndComplete(aIO->bufIdx);
            break;
        case SWT_IO_CMD_RX_PROCESS_FRAME:
            ret = SwitchDrv_RxProcessFrame(aIO->ctrlHwID, aIO->bufIdx, aIO->data,
                  aIO->len, aIO->isMgmtFrameOnly);
            break;
        case SWT_IO_CMD_RX_DONE_IND:
            ret = SwitchDrv_RxDoneInd(aIO->ctrlHwID, aIO->bufIdx, &aIO->buf,
            &aIO->mgmtInfo, aIO->ts, aIO->tsQual, &aIO->tsAvailable);
            break;
        case SWT_IO_CMD_RX_DONE_IND_COMPLETE:
            ret = SwitchDrv_RxDoneIndComplete(aIO->bufIdx);
            break;
        case SWT_IO_CMD_SET_MGMT_INFO:
            ret = SwitchDrv_SetMgmtInfo(aIO->ctrlHwID, aIO->bufIdx, aIO->mgmtInfo);
            break;
        case SWT_IO_CMD_LINK_STATE_CHANGE_HDLR:
            ret = SwitchDrv_LinkStatChgIndHandler(aIO->swtHwID, aIO->portHwID,
                    aIO->linkStatePtr, aIO->isLinkStateChangedPtr);
            break;
        default:
#ifdef ENABLE_CFP
            ret = CFP_CmdHandler(aCmd, aIO);
#else
            ret = BCM_ERR_NOSUPPORT;
#endif
            if (BCM_ERR_NOSUPPORT == ret) {
                ETHERSWT_ReportError(BCM_SWT_ID, (uint8_t)aIO->swtHwID,
                        BRCM_SWARCH_ETHERSWT_IL_CMD_HANDLER_PROC, ret, aCmd, 0UL, 0UL, __LINE__);
            }
            break;
        }
    } else {
        ret = BCM_ERR_UNKNOWN;
    }
    return ret;
}
