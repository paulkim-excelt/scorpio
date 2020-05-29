/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
/******************************************************************************
 File Name: sgmii.c
 Descritpion: This file implements SGMII driver.
******************************************************************************/
#include <string.h>
#include <compiler.h>
#include <chip_config.h>
#include <utils.h>
#include <bcm_err.h>
#include <eth_xcvr.h>
#include <ethxcvr_osil.h>
#include "sgmii.h"

#define SGMII_RESET_TIMEOUT_LOOPCNT         (2000UL)
#define SGMII_POWER_DOWN_BIT_TIMEOUT_CNT    (1000UL)

/* Global variables */
static SGMII_CL22_IEEE_RegType * const SGMII_CL22_IEEE_REGS[SGMII_MAX_HW_ID] =
{
#if (SGMII_MAX_HW_ID == 0UL)
#error "SGMII_MAX_HW_ID == 0UL"
#endif
    (SGMII_CL22_IEEE_RegType *const)SGMII0_CL22_IEEE_BASE,
#if (SGMII_MAX_HW_ID > 1UL)
    (SGMII_CL22_IEEE_RegType *const)SGMII1_CL22_IEEE_BASE,
#endif
#if (SGMII_MAX_HW_ID > 2UL)
    (SGMII_CL22_IEEE_RegType *const)SGMII2_CL22_IEEE_BASE,
#endif
#if (SGMII_MAX_HW_ID > 3UL)
    (SGMII_CL22_IEEE_RegType *const)SGMII3_CL22_IEEE_BASE,
#endif
#if (SGMII_MAX_HW_ID > 4UL)
    (SGMII_CL22_IEEE_RegType *const)SGMII4_CL22_IEEE_BASE,
#endif
#if (SGMII_MAX_HW_ID > 5UL)
#error "SGMII_MAX_HW_ID > 5UL is not supported"
#endif
};

typedef struct {
    ETHXCVR_StateType       state;
    uint32_t                isAutoNegStarted;
    uint32_t                linkStateChangeCount;
    ETHXCVR_BooleanType   jumboMode;
} SGMII_RWDataType;

static SGMII_RWDataType COMP_SECTION(".data.drivers")
    SGMII_RWData[SGMII_MAX_HW_ID] =
        {
        #if (SGMII_MAX_HW_ID == 0UL)
        #error "SGMII_MAX_HW_ID == 0UL"
        #endif
            {.state = ETHXCVR_STATE_UNINIT,
             .isAutoNegStarted = FALSE,
             .linkStateChangeCount = 0UL,
             .jumboMode = ETHXCVR_BOOLEAN_FALSE,
            },
        #if (SGMII_MAX_HW_ID > 1UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isAutoNegStarted = FALSE,
             .linkStateChangeCount = 0UL,
             .jumboMode = ETHXCVR_BOOLEAN_FALSE,
            },
        #endif
        #if (SGMII_MAX_HW_ID > 2UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isAutoNegStarted = FALSE,
             .linkStateChangeCount = 0UL,
             .jumboMode = ETHXCVR_BOOLEAN_FALSE,
            },
        #endif
        #if (SGMII_MAX_HW_ID > 3UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isAutoNegStarted = FALSE,
             .linkStateChangeCount = 0UL,
             .jumboMode = ETHXCVR_BOOLEAN_FALSE,
            },
        #endif
        #if (SGMII_MAX_HW_ID > 4UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isAutoNegStarted = FALSE,
             .linkStateChangeCount = 0UL,
             .jumboMode = ETHXCVR_BOOLEAN_FALSE,
            },
        #endif
        #if (SGMII_MAX_HW_ID > 5UL)
        #error "SGMII_MAX_HW_ID > 5UL not supported"
        #endif
        };


COMP_INLINE int32_t SGMII_CheckConfigParams(const ETHXCVR_PortConfigType *const aCfg)
{

    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    /* check supported speeds */
    if ((aCfg->speed == ETHXCVR_SPEED_100MBPS) ||
            (aCfg->speed == ETHXCVR_SPEED_10MBPS)) {
        goto err;
    }

    if (aCfg->duplex != ETHXCVR_DUPLEXMODE_FULL) {
        goto err;
    }

    if (aCfg->flowControl != ETHXCVR_FLOWCONTROL_NONE) {
        goto err;
    }

    retVal = BCM_ERR_OK;

err:
    return retVal;
}

static int32_t SGMII_GetLinkState(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LinkStateType *const aLinkState)
{
    uint16_t miiStat;
    uint32_t retVal = BCM_ERR_OK;

    if ((SGMII_MAX_HW_ID <= aBusIdx)
            || (NULL == aLinkState)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aLinkState = ETHXCVR_LINKSTATE_DOWN;
        miiStat = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_STAT;
        miiStat = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_STAT;

        if ((miiStat & MII_STAT_LNK_STAT_MASK) == MII_STAT_LNK_STAT_MASK) {
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
        }
    }
    return retVal;
}

static int32_t SGMII_Reset(uint8_t aBusIdx,
                           const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t loopCnt;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL |= MII_CTRL_RESET_MASK;
        /* Wait for Reset bit to auto clear */
        loopCnt = SGMII_RESET_TIMEOUT_LOOPCNT;
        while (((SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL
                        & MII_CTRL_RESET_MASK) == MII_CTRL_RESET_MASK)
                && (loopCnt > 0UL)) {
            loopCnt--;
        }
        if (0UL == loopCnt) {
            retVal = BCM_ERR_TIME_OUT;
        }
    }

    return retVal;
}

static int32_t SGMII_IntSetMode(uint8_t aBusIdx,
                                ETHXCVR_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;
#ifndef __BCM8956X__
    uint32_t loopCnt = 0UL;
    uint16_t regVal;

    if (ETHXCVR_MODE_ACTIVE == aMode) {
        regVal = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL;
        regVal &= (~MII_CTRL_POWER_DOWN_MASK);
        SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL = regVal;
        do {
            regVal = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL;
            loopCnt++;
        } while (((regVal & MII_CTRL_POWER_DOWN_MASK) != 0U)
                && (SGMII_POWER_DOWN_BIT_TIMEOUT_CNT > loopCnt));
    }

    if (ETHXCVR_MODE_DOWN == aMode) {
        regVal = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL;
        regVal |= (MII_CTRL_POWER_DOWN_MASK);
        SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL = regVal;
        do {
            regVal = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL;
            loopCnt++;
        } while (((regVal & MII_CTRL_POWER_DOWN_MASK) == 0U)
                && (SGMII_POWER_DOWN_BIT_TIMEOUT_CNT > loopCnt));
    }

    if (SGMII_POWER_DOWN_BIT_TIMEOUT_CNT == loopCnt) {
        retVal = BCM_ERR_TIME_OUT;
    }
#endif
    return retVal;
}

static int32_t SGMII_SetMode(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = SGMII_IntSetMode(aBusIdx, aMode);
    }

    return retVal;
}

static int32_t SGMII_GetMode(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t mask = (MII_CTRL_POWER_DOWN_MASK | MII_CTRL_RESET_MASK);

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if ((SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL & mask) == 0U) {
            *aMode = ETHXCVR_MODE_ACTIVE;
        } else {
            *aMode = ETHXCVR_MODE_DOWN;
        }
    }

    return retVal;
}

static int32_t SGMII_SetMasterMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_BooleanType aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

static int32_t SGMII_GetMasterMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

static int32_t SGMII_GetSpeed(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType *const aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t speedBits = 0UL;
    uint16_t miiCtrl;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aSpeed)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        miiCtrl = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL;
        if ((miiCtrl & MII_CTRL_AUTONEG_EN_MASK) != 0UL) {
            *aSpeed = ETHXCVR_SPEED_1000MBPS;
        } else {
            speedBits |= ((miiCtrl & MII_CTRL_MANUAL_SPEED1_MASK)
                    >> (MII_CTRL_MANUAL_SPEED1_SHIFT - 1U));
            speedBits |= ((miiCtrl & MII_CTRL_MANUAL_SPEED0_MASK)
                    >> MII_CTRL_MANUAL_SPEED0_SHIFT);
            switch (speedBits) {
            case 0U:
                *aSpeed = ETHXCVR_SPEED_10MBPS;
                break;
            case 1U:
                *aSpeed = ETHXCVR_SPEED_100MBPS;
                break;
            case 2U:
                *aSpeed = ETHXCVR_SPEED_1000MBPS;
                break;
            default:
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
        }
    }

    return retVal;
}

static int32_t SGMII_SetSpeed(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType aSpeed)
{
    int32_t retVal;
    ETHXCVR_SpeedType currSpeed;

    if ((SGMII_MAX_HW_ID <= aBusIdx)
            || (ETHXCVR_SPEED_1000MBPS < aSpeed)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = SGMII_GetSpeed(aBusIdx, aConfig, &currSpeed);
        if (BCM_ERR_OK == retVal) {
            if (aSpeed != currSpeed) {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
        }
    }

    return retVal;
}

static int32_t SGMII_GetDuplexMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t miiCtrl;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aDuplexMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aDuplexMode = ETHXCVR_DUPLEXMODE_HALF;
        miiCtrl = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL;
        if ((miiCtrl & MII_CTRL_FULL_DUPLEX_MASK) != 0U) {
            *aDuplexMode = ETHXCVR_DUPLEXMODE_FULL;
        }
    }

    return retVal;
}

static int32_t SGMII_SetDuplexMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aDuplexMode) {
            case ETHXCVR_DUPLEXMODE_HALF:
                SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL &= ~MII_CTRL_FULL_DUPLEX_MASK;
                break;
            case ETHXCVR_DUPLEXMODE_FULL:
                SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL |= MII_CTRL_FULL_DUPLEX_MASK;
                break;
            default:
                retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

static int32_t SGMII_GetFlowControl(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aFlowControl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aFlowControl = ETHXCVR_FLOWCONTROL_NONE;
    }

    return retVal;
}

static int32_t SGMII_SetFlowControl(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

static int32_t SGMII_GetSQIValue(uint8_t aBusIdx,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 uint32_t *const aSQIValue)
{
    int32_t retVal = BCM_ERR_OK;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aSQIValue)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
            retVal = BCM_ERR_UNINIT;
        }
        *aSQIValue = 0;
    }
    return retVal;
}

static int32_t SGMII_Init(uint8_t aBusIdx,
                          const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;


    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = SGMII_CheckConfigParams(aConfig);
    if (retVal != BCM_ERR_OK) {
        goto err;
    }

    if (ETHXCVR_STATE_UNINIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    SGMII_RWData[aBusIdx].state = ETHXCVR_STATE_INIT;
err:
    return retVal;
}

static int32_t SGMII_DeInit(uint8_t aBusIdx,
                            const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;


    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    SGMII_RWData[aBusIdx].state = ETHXCVR_STATE_UNINIT;
err:
    return retVal;
}

static int32_t SGMII_SetLoopbackMode(uint8_t aBusIdx,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
            SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL |=
                MII_CTRL_LOOPBACK_MASK;
        } else {
            SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL &=
                ~MII_CTRL_LOOPBACK_MASK;
        }
    }

    return retVal;
}

static int32_t SGMII_GetLoopbackMode(uint8_t aBusIdx,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if ((SGMII_CL22_IEEE_REGS[aBusIdx]->MII_CTRL
                    & MII_CTRL_LOOPBACK_MASK) == MII_CTRL_LOOPBACK_MASK) {
            *aMode = ETHXCVR_BOOLEAN_TRUE;
        } else {
            *aMode = ETHXCVR_BOOLEAN_FALSE;
        }
    }

    return retVal;
}

static int32_t SGMII_SetJumboMode(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_BooleanType aMode)
{
    int32_t retVal;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        SGMII_RWData[aBusIdx].jumboMode = aMode;
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

static int32_t SGMII_GetJumboMode(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aMode = SGMII_RWData[aBusIdx].jumboMode;
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

static int32_t SGMII_SetAutoNegMode(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_BooleanType aMode)
{
    int32_t retVal;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

static int32_t SGMII_RestartAutoNeg(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

static int32_t SGMII_GetAutoNegStatus(uint8_t aBusIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t miiStat;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aStatus)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        miiStat = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_STAT;
        miiStat = SGMII_CL22_IEEE_REGS[aBusIdx]->MII_STAT;
        if (MII_STAT_AUTONEG_ABILITY_MASK
                != (miiStat & MII_STAT_AUTONEG_ABILITY_MASK)) {
            *aStatus = ETHXCVR_AUTONEGSTATUS_NO_ABILITY;
        } else if (MII_STAT_AUTONEG_COMPLETE_MASK
                != (miiStat & MII_STAT_AUTONEG_COMPLETE_MASK)) {
            *aStatus = ETHXCVR_AUTONEGSTATUS_INCOMPLETE;
        } else {
            *aStatus = ETHXCVR_AUTONEGSTATUS_COMPLETE;
        }
    }

    return retVal;
}

static int32_t SGMII_GetStats(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_StatsType *const aStats)
{
    int32_t retVal = BCM_ERR_OK;

    if ((SGMII_MAX_HW_ID <= aBusIdx) || (NULL == aStats)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        aStats->linkStateChangeCount =
            SGMII_RWData[aBusIdx].linkStateChangeCount;
    }

    return retVal;
}

static int32_t SGMII_StateHandler(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  uint32_t *const aIsModeChanged,
                                  ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

static int32_t SGMII_UpdateHWStatus(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if (SGMII_MAX_HW_ID <= aBusIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

int32_t SGMII_LinkIRQHandler(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    }
    return retVal;
}

int32_t SGMII_LinkChangeIndHandler(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_LinkStateType *const aLinkState,
                                   uint32_t *const aIsLinkStateChanged)
{
    int32_t retVal = BCM_ERR_OK;

    if (ETHXCVR_STATE_INIT != SGMII_RWData[aBusIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aIsLinkStateChanged = FALSE;
    }

    return retVal;
}

const ETHXCVR_FuncTblType SGMII_FuncTbl = {
    .init = SGMII_Init,
    .deinit = SGMII_DeInit,
    .reset = SGMII_Reset,
    .setMode = SGMII_SetMode,
    .getMode = SGMII_GetMode,
    .setMasterMode = SGMII_SetMasterMode,
    .getMasterMode = SGMII_GetMasterMode,
    .getSpeed = SGMII_GetSpeed,
    .setSpeed = SGMII_SetSpeed,
    .getDuplexMode = SGMII_GetDuplexMode,
    .setDuplexMode = SGMII_SetDuplexMode,
    .setFlowControl = SGMII_SetFlowControl,
    .getFlowControl = SGMII_GetFlowControl,
    .getLinkState = SGMII_GetLinkState,
    .getSQIValue = SGMII_GetSQIValue,
    .setLoopbackMode = SGMII_SetLoopbackMode,
    .getLoopbackMode = SGMII_GetLoopbackMode,
    .setJumboMode = SGMII_SetJumboMode,
    .getJumboMode = SGMII_GetJumboMode,
    .setAutoNegMode = SGMII_SetAutoNegMode,
    .getAutoNegStatus = SGMII_GetAutoNegStatus,
    .restartAutoNeg = SGMII_RestartAutoNeg,
    .getStats = SGMII_GetStats,
    .stateHandler = SGMII_StateHandler,
    .updateHWStatus = SGMII_UpdateHWStatus,
    .linkChangeIndHandler = SGMII_LinkChangeIndHandler,
    .linkIRQHandler  = SGMII_LinkIRQHandler,
};

/* Nothing past this line */
