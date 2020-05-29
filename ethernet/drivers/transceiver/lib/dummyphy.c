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
#include "dummyphy_osil.h"

/**
    @name DUMMYPHY driver Design IDs
    @{
    @brief Design IDs for DUMMYPHY driver
*/
#define BRCM_SWDSGN_DUMMYPHY_INIT_PROC                   (0xA3U) /**< @brief #DUMMYPHY_Init */
#define BRCM_SWDSGN_DUMMYPHY_RESET_PROC                  (0xA4U) /**< @brief #DUMMYPHY_Reset */
#define BRCM_SWDSGN_DUMMYPHY_GETSQIVALUE_PROC            (0xA5U) /**< @brief #DUMMYPHY_GetSQIValue */
#define BRCM_SWDSGN_DUMMYPHY_GETDUPLEXMODE_PROC          (0xA6U) /**< @brief #DUMMYPHY_GetDuplexMode */
#define BRCM_SWDSGN_DUMMYPHY_GETSPEED_PROC               (0xA7U) /**< @brief #DUMMYPHY_GetSpeed */
#define BRCM_SWDSGN_DUMMYPHY_GETLINKSTATE_PROC           (0xA8U) /**< @brief #DUMMYPHY_GetLinkState */
#define BRCM_SWDSGN_DUMMYPHY_GETMASTERMODE_PROC          (0xA9U) /**< @brief #DUMMYPHY_GetMasterMode */
#define BRCM_SWDSGN_DUMMYPHY_SETMASTERMODE_PROC          (0xAAU) /**< @brief #DUMMYPHY_SetMasterMode */
#define BRCM_SWDSGN_DUMMYPHY_GETMODE_PROC                (0xABU) /**< @brief #DUMMYPHY_GetMode */
#define BRCM_SWDSGN_DUMMYPHY_SETMODE_PROC                (0xACU) /**< @brief #DUMMYPHY_SetMode */
#define BRCM_SWDSGN_DUMMYPHY_RESET_TIMEOUT_LOOPCNT       (0xADU) /**< @brief #DUMMYPHY_RESET_TIMEOUT_LOOPCNT */
#define BRCM_SWDSGN_DUMMYPHY_POWER_DOWN_BIT_TIMEOUT_CNT  (0xAEU) /**< @brief #DUMMYPHY_POWER_DOWN_BIT_TIMEOUT_CNT */
#define BRCM_SWDSGN_DUMMYPHY_SQI_CFG_VALUE               (0xAFU) /**< @brief #DUMMYPHY_SQI_CFG_VALUE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_LOOP_CNT                (0xB0U) /**< @brief #DUMMYPHY_SQI_LOOP_CNT */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0D                      (0xB1U) /**< @brief #DUMMYPHY_SQI_0D */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0C                      (0xB2U) /**< @brief #DUMMYPHY_SQI_0C */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0B                      (0xB3U) /**< @brief #DUMMYPHY_SQI_0B */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0A                      (0xB4U) /**< @brief #DUMMYPHY_SQI_0A */
#define BRCM_SWDSGN_DUMMYPHY_SQI_1                       (0xB5U) /**< @brief #DUMMYPHY_SQI_1 */
#define BRCM_SWDSGN_DUMMYPHY_SQI_2                       (0xB6U) /**< @brief #DUMMYPHY_SQI_2 */
#define BRCM_SWDSGN_DUMMYPHY_SQI_3                       (0xB7U) /**< @brief #DUMMYPHY_SQI_3 */
#define BRCM_SWDSGN_DUMMYPHY_SQI_4                       (0xB8U) /**< @brief #DUMMYPHY_SQI_4 */
#define BRCM_SWDSGN_DUMMYPHY_SQI_5                       (0xB9U) /**< @brief #DUMMYPHY_SQI_5 */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0D_RANGE                (0xBAU) /**< @brief #DUMMYPHY_SQI_0D_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0C_RANGE                (0xBBU) /**< @brief #DUMMYPHY_SQI_0C_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0B_RANGE                (0xBCU) /**< @brief #DUMMYPHY_SQI_0B_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_0A_RANGE                (0xBDU) /**< @brief #DUMMYPHY_SQI_0A_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_1_RANGE                 (0xBEU) /**< @brief #DUMMYPHY_SQI_1_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_2_RANGE                 (0xBFU) /**< @brief #DUMMYPHY_SQI_2_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_3_RANGE                 (0xC0U) /**< @brief #DUMMYPHY_SQI_3_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_4_RANGE                 (0xC1U) /**< @brief #DUMMYPHY_SQI_4_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_SQI_5_RANGE                 (0xC2U) /**< @brief #DUMMYPHY_SQI_5_RANGE */
#define BRCM_SWDSGN_DUMMYPHY_CORE_REGS_GLOBAL            (0xC3U) /**< @brief #DUMMYPHY_CORE_REGS */
#define BRCM_SWDSGN_DUMMYPHY_CL22_IEEE_REGS_GLOBAL       (0xC4U) /**< @brief #DUMMYPHY_CL22_IEEE_REGS */
#define BRCM_SWDSGN_DUMMYPHY_DSP_TAP_CTRL_REGS_GLOBAL    (0xC5U) /**< @brief #DUMMYPHY_DSP_TAP_CTRL_REGS */
#define BRCM_SWDSGN_DUMMYPHY_RWDATA_GLOBAL               (0xC6U) /**< @brief #DUMMYPHY_RWData */
#define BRCM_SWDSGN_DUMMYPHY_LINKIRQHANDLER_PROC         (0xCBU) /**< @brief #DUMMYPHY_LinkIRQHandler */
#define BRCM_SWDSGN_DUMMYPHY_LINKCHANGEINDHANDLER_PROC   (0xCCU) /**< @brief #DUMMYPHY_LinkChangeIndHandler */
#define BRCM_SWDSGN_DUMMYPHY_FUNCTBL_GLOBAL              (0xCDU) /**< @brief #DUMMYPHY_FuncTbl */
#define BRCM_SWDSGN_DUMMYPHY_RWDATA_TYPE                 (0xCEU) /**< @brief #DUMMYPHY_RWDataType */
#define BRCM_SWDSGN_DUMMYPHY_DEINIT_PROC                 (0xCFU) /**< @brief #DUMMYPHY_DeInit */
#define BRCM_SWDSGN_DUMMYPHY_SETSPEED_PROC               (0xCFU) /**< @brief #DUMMYPHY_SetSpeed */
#define BRCM_SWDSGN_DUMMYPHY_GETLOOPBACKMODE_PROC        (0xCFU) /**< @brief #DUMMYPHY_GetLoopbackMode */
#define BRCM_SWDSGN_DUMMYPHY_SETLOOPBACKMODE_PROC        (0xCFU) /**< @brief #DUMMYPHY_SetLoopbackMode */
#define BRCM_SWDSGN_DUMMYPHY_GETJUMBOMODE_PROC           (0xCFU) /**< @brief #DUMMYPHY_GetJumboMode */
#define BRCM_SWDSGN_DUMMYPHY_SETJUMBOMODE_PROC           (0xCFU) /**< @brief #DUMMYPHY_SetJumboMode */
#define BRCM_SWDSGN_DUMMYPHY_SETAUTONEGMODE_PROC         (0xCFU) /**< @brief #DUMMYPHY_SetAutoNegMode */
#define BRCM_SWDSGN_DUMMYPHY_RESTARTAUTONEG_PROC         (0xCFU) /**< @brief #DUMMYPHY_RestartAutoNeg */
#define BRCM_SWDSGN_DUMMYPHY_GETAUTONEGSTATUS_PROC       (0xCFU) /**< @brief #DUMMYPHY_GetAutoNegStatus */
#define BRCM_SWDSGN_DUMMYPHY_GETSTATS_PROC               (0xCFU) /**< @brief #DUMMYPHY_GetStats */
#define BRCM_SWDSGN_DUMMYPHY_SETDUPLEXMODE_PROC          (0xA6U) /**< @brief #DUMMYPHY_SetDuplexMode */
#define BRCM_SWDSGN_DUMMYPHY_SETFLOWCONTROL_PROC         (0xA6U) /**< @brief #DUMMYPHY_SetFlowControl */
#define BRCM_SWDSGN_DUMMYPHY_GETFLOWCONTROL_PROC         (0xA6U) /**< @brief #DUMMYPHY_GetFlowControl */
/** @} */

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetLinkState(uint8_t aID,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_LinkStateType *const aLinkState)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_Reset(uint8_t aID,
                              const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetMode(uint8_t aID,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_ModeType aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetMode(uint8_t aID,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_ModeType *const aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetMasterMode(uint8_t aID,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_BooleanType aMasterMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetMasterMode(uint8_t aID,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_BooleanType *const aConnMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetSpeed(uint8_t aID,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_SpeedType aSpeed)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetSpeed(uint8_t aID,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_SpeedType *const aSpeed)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetDuplexMode(uint8_t aID,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_DuplexModeType *const aDuplexMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetDuplexMode(uint8_t aPhyIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_DuplexModeType aDuplexMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetFlowControl(uint8_t aPhyIdx,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_FlowControlType aFlowControl)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetFlowControl(uint8_t aPhyIdx,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_FlowControlType *const aFlowControl)
{
    return BCM_ERR_NOSUPPORT;
}


/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetSQIValue(uint8_t aID,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    uint32_t *const aSQIValue)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_Init(uint8_t aID,
                             const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_DeInit(uint8_t aID,
                               const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetLoopbackMode(uint8_t aID,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetLoopbackMode(uint8_t aID,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_BooleanType *const aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetJumboMode(uint8_t aID,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetJumboMode(uint8_t aID,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType *const aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_SetAutoNegMode(uint8_t aID,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_RestartAutoNeg(uint8_t aID,
                                       const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetAutoNegStatus(uint8_t aID,
                                         const ETHXCVR_PortConfigType *const aConfig,
                                         ETHXCVR_AutoNegStatusType *const aStatus)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_GetStats(uint8_t aID,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_StatsType *const aStats)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_StateHandler(uint8_t aPhyIdx,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     uint32_t *const aIsModeChanged,
                                     ETHXCVR_ModeType *const aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_UpdateHWStatus(uint8_t aPhyIdx,
                                       const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_LinkIRQHandler(uint8_t aID,
                                       const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
static int32_t DUMMYPHY_LinkChangeIndHandler(uint8_t aID,
                                             const ETHXCVR_PortConfigType *const aConfig,
                                             ETHXCVR_LinkStateType *const aLinkState,
                                             uint32_t *const aIsLinkStateChanged)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace #BRCM_SWARCH_DUMMYPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_DUMMYPHY_FUNCTBL
*/
const ETHXCVR_FuncTblType DUMMYPHY_FuncTbl = {
    .init = DUMMYPHY_Init,
    .deinit = DUMMYPHY_DeInit,
    .reset = DUMMYPHY_Reset,
    .setMode = DUMMYPHY_SetMode,
    .getMode = DUMMYPHY_GetMode,
    .setMasterMode = DUMMYPHY_SetMasterMode,
    .getMasterMode = DUMMYPHY_GetMasterMode,
    .getSpeed = DUMMYPHY_GetSpeed,
    .setSpeed = DUMMYPHY_SetSpeed,
    .getDuplexMode = DUMMYPHY_GetDuplexMode,
    .setDuplexMode = DUMMYPHY_SetDuplexMode,
    .setFlowControl = DUMMYPHY_SetFlowControl,
    .getFlowControl = DUMMYPHY_GetFlowControl,
    .getLinkState = DUMMYPHY_GetLinkState,
    .getSQIValue = DUMMYPHY_GetSQIValue,
    .setLoopbackMode = DUMMYPHY_SetLoopbackMode,
    .getLoopbackMode = DUMMYPHY_GetLoopbackMode,
    .setJumboMode = DUMMYPHY_SetJumboMode,
    .getJumboMode = DUMMYPHY_GetJumboMode,
    .setAutoNegMode = DUMMYPHY_SetAutoNegMode,
    .getAutoNegStatus = DUMMYPHY_GetAutoNegStatus,
    .restartAutoNeg = DUMMYPHY_RestartAutoNeg,
    .getStats = DUMMYPHY_GetStats,
    .stateHandler = DUMMYPHY_StateHandler,
    .updateHWStatus = DUMMYPHY_UpdateHWStatus,
    .linkChangeIndHandler = DUMMYPHY_LinkChangeIndHandler,
    .linkIRQHandler  = DUMMYPHY_LinkIRQHandler,
};

/* Nothing past this line */
