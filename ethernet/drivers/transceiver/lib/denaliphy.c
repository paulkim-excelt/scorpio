/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
#include <bcm_err.h>
#include <eth_xcvr.h>
#include "denaliphy_osil.h"

/**
    @name DENALIPHY driver Design IDs
    @{
    @brief Design IDs for DENALIPHY driver
*/
#define BRCM_SWDSGN_DENALIPHY_INIT_PROC                   (0xA3U) /**< @brief #DENALIPHY_Init */
#define BRCM_SWDSGN_DENALIPHY_RESET_PROC                  (0xA4U) /**< @brief #DENALIPHY_Reset */
#define BRCM_SWDSGN_DENALIPHY_GETSQIVALUE_PROC            (0xA5U) /**< @brief #DENALIPHY_GetSQIValue */
#define BRCM_SWDSGN_DENALIPHY_GETDUPLEXMODE_PROC          (0xA6U) /**< @brief #DENALIPHY_GetDuplexMode */
#define BRCM_SWDSGN_DENALIPHY_GETSPEED_PROC               (0xA7U) /**< @brief #DENALIPHY_GetSpeed */
#define BRCM_SWDSGN_DENALIPHY_GETLINKSTATE_PROC           (0xA8U) /**< @brief #DENALIPHY_GetLinkState */
#define BRCM_SWDSGN_DENALIPHY_GETMASTERMODE_PROC          (0xA9U) /**< @brief #DENALIPHY_GetMasterMode */
#define BRCM_SWDSGN_DENALIPHY_SETMASTERMODE_PROC          (0xAAU) /**< @brief #DENALIPHY_SetMasterMode */
#define BRCM_SWDSGN_DENALIPHY_GETMODE_PROC                (0xABU) /**< @brief #DENALIPHY_GetMode */
#define BRCM_SWDSGN_DENALIPHY_SETMODE_PROC                (0xACU) /**< @brief #DENALIPHY_SetMode */
#define BRCM_SWDSGN_DENALIPHY_LINKIRQHANDLER_PROC         (0xCBU) /**< @brief #DENALIPHY_LinkIRQHandler */
#define BRCM_SWDSGN_DENALIPHY_LINKCHANGEINDHANDLER_PROC   (0xCCU) /**< @brief #DENALIPHY_LinkChangeIndHandler */
#define BRCM_SWDSGN_DENALIPHY_FUNCTBL_GLOBAL              (0xCDU) /**< @brief #DENALIPHY_FuncTbl */
#define BRCM_SWDSGN_DENALIPHY_RWDATA_TYPE                 (0xCEU) /**< @brief #DENALIPHY_RWDataType */
#define BRCM_SWDSGN_DENALIPHY_DEINIT_PROC                 (0xCFU) /**< @brief #DENALIPHY_DeInit */
#define BRCM_SWDSGN_DENALIPHY_SETSPEED_PROC               (0xCFU) /**< @brief #DENALIPHY_SetSpeed */
#define BRCM_SWDSGN_DENALIPHY_GETLOOPBACKMODE_PROC        (0xCFU) /**< @brief #DENALIPHY_GetLoopbackMode */
#define BRCM_SWDSGN_DENALIPHY_SETLOOPBACKMODE_PROC        (0xCFU) /**< @brief #DENALIPHY_SetLoopbackMode */
#define BRCM_SWDSGN_DENALIPHY_GETJUMBOMODE_PROC           (0xCFU) /**< @brief #DENALIPHY_GetJumboMode */
#define BRCM_SWDSGN_DENALIPHY_SETJUMBOMODE_PROC           (0xCFU) /**< @brief #DENALIPHY_SetJumboMode */
#define BRCM_SWDSGN_DENALIPHY_SETAUTONEGMODE_PROC         (0xCFU) /**< @brief #DENALIPHY_SetAutoNegMode */
#define BRCM_SWDSGN_DENALIPHY_RESTARTAUTONEG_PROC         (0xCFU) /**< @brief #DENALIPHY_RestartAutoNeg */
#define BRCM_SWDSGN_DENALIPHY_GETAUTONEGSTATUS_PROC       (0xCFU) /**< @brief #DENALIPHY_GetAutoNegStatus */
#define BRCM_SWDSGN_DENALIPHY_GETSTATS_PROC               (0xCFU) /**< @brief #DENALIPHY_GetStats */
#define BRCM_SWDSGN_DENALIPHY_SETDUPLEXMODE_PROC          (0xA6U) /**< @brief #DENALIPHY_SetDuplexMode */
#define BRCM_SWDSGN_DENALIPHY_SETFLOWCONTROL_PROC         (0xA6U) /**< @brief #DENALIPHY_SetFlowControl */
#define BRCM_SWDSGN_DENALIPHY_GETFLOWCONTROL_PROC         (0xA6U) /**< @brief #DENALIPHY_GetFlowControl */
/** @} */

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetLinkState(uint8_t aID,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_LinkStateType *const aLinkState)
{
    *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_Reset(uint8_t aID,
                               const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetMode(uint8_t aID,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_ModeType aMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetMode(uint8_t aID,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_ModeType *const aMode)
{
    *aMode = ETHXCVR_MODE_ACTIVE;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetMasterMode(uint8_t aID,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_BooleanType aMasterMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetMasterMode(uint8_t aID,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_BooleanType *const aConnMode)
{
    *aConnMode = ETHXCVR_BOOLEAN_TRUE;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetSpeed(uint8_t aID,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_SpeedType aSpeed)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetSpeed(uint8_t aID,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_SpeedType *const aSpeed)
{
    *aSpeed = ETHXCVR_SPEED_1000MBPS;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetDuplexMode(uint8_t aID,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_DuplexModeType *const aDuplexMode)
{
    *aDuplexMode = ETHXCVR_DUPLEXMODE_FULL;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetDuplexMode(uint8_t aPhyIdx,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_DuplexModeType aDuplexMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetFlowControl(uint8_t aPhyIdx,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_FlowControlType aFlowControl)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetFlowControl(uint8_t aPhyIdx,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_FlowControlType *const aFlowControl)
{
    *aFlowControl = ETHXCVR_FLOWCONTROL_NONE;
    return BCM_ERR_OK;
}


/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetSQIValue(uint8_t aID,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     uint32_t *const aSQIValue)
{
    *aSQIValue = 5UL;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_Init(uint8_t aID,
                             const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_DeInit(uint8_t aID,
                                const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetLoopbackMode(uint8_t aID,
                                         const ETHXCVR_PortConfigType *const aConfig,
                                         ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetLoopbackMode(uint8_t aID,
                                         const ETHXCVR_PortConfigType *const aConfig,
                                         ETHXCVR_BooleanType *const aMode)
{
    *aMode = ETHXCVR_BOOLEAN_FALSE;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetJumboMode(uint8_t aID,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetJumboMode(uint8_t aID,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_BooleanType *const aMode)
{
    *aMode = ETHXCVR_BOOLEAN_FALSE;
    return BCM_ERR_OK;
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_SetAutoNegMode(uint8_t aID,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_RestartAutoNeg(uint8_t aID,
                                        const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetAutoNegStatus(uint8_t aID,
                                          const ETHXCVR_PortConfigType *const aConfig,
                                          ETHXCVR_AutoNegStatusType *const aStatus)
{
#ifdef __BCM89559G__
    /* NOTE: aID is slave ID */
    if (1U == aConfig->phy.slaveID) {
        *aStatus = ETHXCVR_AUTONEGSTATUS_INCOMPLETE;
    } else if (0U == aConfig->phy.slaveID) {
        *aStatus = ETHXCVR_AUTONEGSTATUS_COMPLETE;
    }
#endif
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_GetStats(uint8_t aID,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_StatsType *const aStats)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_StateHandler(uint8_t aPhyIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      uint32_t *const aIsModeChanged,
                                      ETHXCVR_ModeType *const aMode)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_UpdateHWStatus(uint8_t aPhyIdx,
                                        const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_LinkIRQHandler(uint8_t aID,
                                        const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
static int32_t DENALIPHY_LinkChangeIndHandler(uint8_t aID,
                                              const ETHXCVR_PortConfigType *const aConfig,
                                              ETHXCVR_LinkStateType *const aLinkState,
                                              uint32_t *const aIsLinkStateChanged)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DENALIPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DENALIPHY_FUNCTBL
*/
const ETHXCVR_FuncTblType DENALIPHY_FuncTbl = {
    .init = DENALIPHY_Init,
    .deinit = DENALIPHY_DeInit,
    .reset = DENALIPHY_Reset,
    .setMode = DENALIPHY_SetMode,
    .getMode = DENALIPHY_GetMode,
    .setMasterMode = DENALIPHY_SetMasterMode,
    .getMasterMode = DENALIPHY_GetMasterMode,
    .getSpeed = DENALIPHY_GetSpeed,
    .setSpeed = DENALIPHY_SetSpeed,
    .getDuplexMode = DENALIPHY_GetDuplexMode,
    .setDuplexMode = DENALIPHY_SetDuplexMode,
    .setFlowControl = DENALIPHY_SetFlowControl,
    .getFlowControl = DENALIPHY_GetFlowControl,
    .getLinkState = DENALIPHY_GetLinkState,
    .getSQIValue = DENALIPHY_GetSQIValue,
    .setLoopbackMode = DENALIPHY_SetLoopbackMode,
    .getLoopbackMode = DENALIPHY_GetLoopbackMode,
    .setJumboMode = DENALIPHY_SetJumboMode,
    .getJumboMode = DENALIPHY_GetJumboMode,
    .setAutoNegMode = DENALIPHY_SetAutoNegMode,
    .getAutoNegStatus = DENALIPHY_GetAutoNegStatus,
    .restartAutoNeg = DENALIPHY_RestartAutoNeg,
    .getStats = DENALIPHY_GetStats,
    .stateHandler = DENALIPHY_StateHandler,
    .updateHWStatus = DENALIPHY_UpdateHWStatus,
    .linkChangeIndHandler = DENALIPHY_LinkChangeIndHandler,
    .linkIRQHandler  = DENALIPHY_LinkIRQHandler,
};

/* Nothing past this line */
