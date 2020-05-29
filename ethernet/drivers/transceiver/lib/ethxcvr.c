/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_eth_xcvrdrv_impl Ethernet Transceiver
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_impl
    @{

    @file ethxcvr.c
    @brief This file implements interface layer APIs for Ethernet Transceiver driver.

    @version 1.26 Imported from docx
*/

#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <interrupt.h>
#include <bcm_err.h>
#include <utils.h>
#include <eth_xcvr.h>
#include <ethxcvr_osil.h>
#include "ethxcvr_drv.h"

/**
    @name Transceiver driver Design IDs
    @{
    @brief Design IDs for Transceiver driver
*/
#define BRCM_SWDSGN_ETHXCVR_REPORTERROR_PART_PROC  (0x80U) /**< @brief #ETHXCVR_ReportError */
#define BRCM_SWDSGN_ETHXCVR_GETSPEED_PROC          (0x81U) /**< @brief #ETHXCVR_GetSpeed */
#define BRCM_SWDSGN_ETHXCVR_INIT_PROC              (0x82U) /**< @brief #ETHXCVR_Init */
#define BRCM_SWDSGN_ETHXCVR_SETMODE_PROC           (0x83U) /**< @brief #ETHXCVR_SetMode */
#define BRCM_SWDSGN_ETHXCVR_GETMODE_PROC           (0x84U) /**< @brief #ETHXCVR_GetMode */
#define BRCM_SWDSGN_ETHXCVR_GETLINKSTATE_PROC      (0x85U) /**< @brief #ETHXCVR_GetLinkState */
#define BRCM_SWDSGN_ETHXCVR_GETDUPLEXMODE_PROC     (0x86U) /**< @brief #ETHXCVR_GetDuplexMode */
#define BRCM_SWDSGN_ETHXCVR_GETSQIVALUE_PROC       (0x87U) /**< @brief #ETHXCVR_GetSQIValue */
#define BRCM_SWDSGN_ETHXCVR_CMDHANDLER_PROC        (0x88U) /**< @brief #ETHXCVR_CmdHandler */
#define BRCM_SWDSGN_ETHXCVR_LINKIRQHANDLER_PROC    (0x89U) /**< @brief #ETHXCVR_LinkIRQHandler */
#define BRCM_SWDSGN_ETHXCVR_LINKCHGINDHANDLER_PROC (0x8AU) /**< @brief #ETHXCVR_LinkChgIndHandler */
#define BRCM_SWDSGN_ETHXCVR_DEINIT_PROC            (0x8BU) /**< @brief #ETHXCVR_DeInit */
#define BRCM_SWDSGN_ETHXCVR_SETMASTERMODE_PROC     (0x8CU) /**< @brief #ETHXCVR_SetMasterMode */
#define BRCM_SWDSGN_ETHXCVR_GETMASTERMODE_PROC     (0x8DU) /**< @brief #ETHXCVR_GetMasterMode */
#define BRCM_SWDSGN_ETHXCVR_SETSPEED_PROC          (0x8EU) /**< @brief #ETHXCVR_SetSpeed */
#define BRCM_SWDSGN_ETHXCVR_SETLOOPBACKMODE_PROC   (0x8FU) /**< @brief #ETHXCVR_SetLoopbackMode */
#define BRCM_SWDSGN_ETHXCVR_GETLOOPBACKMODE_PROC   (0x90U) /**< @brief #ETHXCVR_GetLoopbackMode */
#define BRCM_SWDSGN_ETHXCVR_SETJUMBOMODE_PROC      (0x91U) /**< @brief #ETHXCVR_SetJumboMode */
#define BRCM_SWDSGN_ETHXCVR_GETJUMBOMODE_PROC      (0x92U) /**< @brief #ETHXCVR_GetJumboMode */
#define BRCM_SWDSGN_ETHXCVR_SETAUTONEG_PROC        (0x93U) /**< @brief #ETHXCVR_SetAutoNegMode */
#define BRCM_SWDSGN_ETHXCVR_GETAUTONEGSTATUS_PROC  (0x94U) /**< @brief #ETHXCVR_GetAutoNegStatus */
#define BRCM_SWDSGN_ETHXCVR_RESTARTAUTONEG_PROC    (0x95U) /**< @brief #ETHXCVR_RestartAutoNeg */
#define BRCM_SWDSGN_ETHXCVR_GETSTATS_PROC          (0x96U) /**< @brief #ETHXCVR_GetStats */
#define BRCM_SWDSGN_ETHXCVR_STATEHANDLER_PROC      (0x97U) /**< @brief #ETHXCVR_StateHandler */
#define BRCM_SWDSGN_ETHXCVR_UPDATEHWSTATUS_PROC    (0x98U) /**< @brief #ETHXCVR_UpdateHWStatus */
#define BRCM_SWDSGN_ETHXCVR_GETPORTCONFIG_PROC     (0x99U) /**< @brief #ETHXCVR_GetPortConfig */
#define BRCM_SWDSGN_ETHXCVR_SETDUPLEXMODE_PROC     (0x9AU) /**< @brief #ETHXCVR_SetDuplexMode */
#define BRCM_SWDSGN_ETHXCVR_GETFLOWCONTROL_PROC    (0x9BU) /**< @brief #ETHXCVR_GetFlowControl */
#define BRCM_SWDSGN_ETHXCVR_SETFLOWCONTROL_PROC    (0x9CU) /**< @brief #ETHXCVR_SetFlowControl */
/** @} */

/** @brief Report error information to DET/DEM

    @trace #BRCM_SWARCH_ETHXCVR_CMDHANDLER_PROC
    @trace #BRCM_SWARCH_ETHXCVR_GETSQIVALUE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_INIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
static void ETHXCVR_ReportError(uint16_t aCompID, uint8_t aInstanceID,
        uint8_t aApiID, int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_INIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
void ETHXCVR_Init(ETHXCVR_IDType aID,
                  const ETHXCVR_PortConfigType *const aConfig)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.xcvrCfg = aConfig;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_INIT, &ethIO);
    }
    if (BCM_ERR_OK != retVal) {
        ETHXCVR_ReportError((BCM_XVR_ID & BCM_LOGMASK_USER), aID,
                             BRCM_SWARCH_ETHXCVR_INIT_PROC, retVal,
                             (uint32_t)aConfig, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DEINIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
void ETHXCVR_DeInit(ETHXCVR_IDType aID)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if (ETHXCVR_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_DEINIT, &ethIO);
    }
    if (BCM_ERR_OK != retVal) {
        ETHXCVR_ReportError((BCM_XVR_ID & BCM_LOGMASK_USER), aID,
                             BRCM_SWARCH_ETHXCVR_DEINIT_PROC, retVal, 0UL,
                             0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetMode(ETHXCVR_IDType aID, ETHXCVR_ModeType aMode)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) ||
        (ETHXCVR_MODE_DOWN > aMode) ||
        (ETHXCVR_MODE_ACTIVE < aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.mode = aMode;
        ethIO.isModeChanged = FALSE;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_MODE, &ethIO);
        if ((BCM_ERR_OK == retVal) && (ethIO.isModeChanged == TRUE)) {
            ETHXCVR_ModeIndication(aID, aMode);
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetMode(ETHXCVR_IDType aID,
                        ETHXCVR_ModeType *const aMode)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_MODE, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aMode = ethIO.mode;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETLINKSTATE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetLinkState(ETHXCVR_IDType aID,
                             ETHXCVR_LinkStateType *const aLinkState)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aLinkState)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Get PHY link status */
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_LINK_STATE, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aLinkState = ethIO.linkState;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetDuplexMode(ETHXCVR_IDType aID,
                              ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aDuplexMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_DUPLEXMODE, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aDuplexMode = ethIO.duplex;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetDuplexMode(ETHXCVR_IDType aID,
                              ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) ||
        ((ETHXCVR_DUPLEXMODE_HALF != aDuplexMode) && (ETHXCVR_DUPLEXMODE_FULL != aDuplexMode))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.duplex = aDuplexMode;
        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_DUPLEXMODE, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETFLOWCONTROL_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetFlowControl(ETHXCVR_IDType aID,
                               ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aFlowControl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_FLOWCONTROL, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aFlowControl = ethIO.flowControl;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETFLOWCONTROL_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetFlowControl(ETHXCVR_IDType aID,
                               ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (ETHXCVR_FLOWCONTROL_RXONLY < aFlowControl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.flowControl = aFlowControl;
        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_FLOWCONTROL, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETSQIVALUE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetSQIValue(ETHXCVR_IDType aID,
                            uint32_t *const aSQIValue)
{
    int32_t retVal = BCM_ERR_OK;
    ETHXCVR_IOType ethIO;
    ETHXCVR_LinkStateType linkState = ETHXCVR_LINKSTATE_DOWN;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aSQIValue)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        *aSQIValue = 0UL;
        /* Get the link Status */
        retVal = ETHXCVR_GetLinkState(aID, &linkState);
        if (BCM_ERR_OK == retVal) {
            if (linkState == ETHXCVR_LINKSTATE_ACTIVE) {
                /* Compute the SQI only if link is active */

                /* Construct the SVC request */
                ethIO.retVal = BCM_ERR_UNKNOWN;
                ethIO.hwID = aID;
                retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_SQI_VALUE, &ethIO);

                if (BCM_ERR_OK == retVal) {
                    *aSQIValue = ethIO.sqiValue;
                }
            }
        } else {
            ETHXCVR_ReportError(BCM_XVR_ID, aID,
                    BRCM_SWARCH_ETHXCVR_GETSQIVALUE_PROC,
                    retVal, linkState, 0UL, 0UL, 0UL);
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETMASTERMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetMasterMode(ETHXCVR_IDType aID,
                            ETHXCVR_BooleanType aMasterMode)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) ||
        ((ETHXCVR_BOOLEAN_TRUE != aMasterMode) &&
        (ETHXCVR_BOOLEAN_FALSE != aMasterMode))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.paramEnable = aMasterMode;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_MASTERMODE, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETMASTERMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetMasterMode(ETHXCVR_IDType aID,
                            ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aMasterMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_MASTERMODE, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aMasterMode = ethIO.paramEnable;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetSpeed(ETHXCVR_IDType aID,
                         ETHXCVR_SpeedType aSpeed)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) && (aSpeed != (aSpeed & ETHXCVR_SPEED_VALID_MASK))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.speed = aSpeed;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_SPEED, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetSpeed(ETHXCVR_IDType aID,
                         ETHXCVR_SpeedType *const aSpeed)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aSpeed)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_SPEED, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aSpeed = ethIO.speed;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETLOOPBACKMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetLoopbackMode(ETHXCVR_IDType aID,
                                ETHXCVR_BooleanType aMode)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) ||
        ((ETHXCVR_BOOLEAN_TRUE != aMode) &&
        (ETHXCVR_BOOLEAN_FALSE != aMode))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.paramEnable = aMode;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_LOOPBACK, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETLOOPBACKMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetLoopbackMode(ETHXCVR_IDType aID,
                                ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_LOOPBACK, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aMode = ethIO.paramEnable;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETJUMBOMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetJumboMode(ETHXCVR_IDType aID,
                            ETHXCVR_BooleanType aMode)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) ||
        ((ETHXCVR_BOOLEAN_TRUE != aMode) &&
        (ETHXCVR_BOOLEAN_FALSE != aMode))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.paramEnable= aMode;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_JUMBO_MODE, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETJUMBOMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetJumboMode(ETHXCVR_IDType aID,
                             ETHXCVR_BooleanType *const aJumboMode)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aJumboMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_JUMBO_MODE, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aJumboMode = ethIO.paramEnable;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_SETAUTONEGMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_SetAutoNegMode(ETHXCVR_IDType aID, ETHXCVR_BooleanType aMode)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) ||
        ((ETHXCVR_BOOLEAN_TRUE != aMode) &&
        (ETHXCVR_BOOLEAN_FALSE != aMode))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.paramEnable = aMode;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_SET_AUTONEG_MODE, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_RESTARTAUTONEG_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_RestartAutoNeg(ETHXCVR_IDType aID)
{
    ETHXCVR_IOType ethIO;
    int32_t retVal;

    if (ETHXCVR_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_RESTART_AUTONEG, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETAUTONEGSTATUS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetAutoNegStatus(ETHXCVR_IDType aID,
                                 ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aStatus)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_AUTONEG_STATUS, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aStatus = ethIO.autoNegStatus;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETSTATS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetStats(ETHXCVR_IDType aID,
                        ETHXCVR_StatsType *const aStats)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aStats)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.stats = aStats;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_GET_STATS, &ethIO);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_STATEHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_StateHandler(ETHXCVR_IDType aID)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if (ETHXCVR_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.isModeChanged = FALSE;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_STATEHANDLER, &ethIO);
        if ((BCM_ERR_OK == retVal) && (TRUE == ethIO.isModeChanged)) {
            ETHXCVR_ModeIndication(aID, ethIO.mode);
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_UPDATEHWSTATUS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_UpdateHWStatus(ETHXCVR_IDType aID)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if (ETHXCVR_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;

        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_UPDATE_HW_STATUS, &ethIO);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_GETPORTCONFIG_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_GetPortConfig(ETHXCVR_IDType aID,
                              ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal;

    if ((ETHXCVR_HW_ID_MAX <= aID) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = ETHXCVR_DrvGetPortConfig(aID, aConfig);
    }
    if (BCM_ERR_OK != retVal) {
        ETHXCVR_ReportError((BCM_XVR_ID & BCM_LOGMASK_USER), aID,
                             BRCM_SWARCH_ETHXCVR_GETPORTCONFIG_PROC, retVal,
                             (uint32_t)aConfig, 0UL, 0UL, 0UL);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
void ETHXCVR_LinkIRQHandler(ETHXCVR_IDType aID)
{
    int32_t retVal = ETHXCVR_DrvLinkIRQHandler(aID);
    if (BCM_ERR_OK != retVal) {
        ETHXCVR_ReportError(BCM_XVR_ID, aID,
                BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_PROC,
                retVal, 0UL, 0UL, 0UL, __LINE__);
    }
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_LINKCHGINDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
void ETHXCVR_LinkChgIndHandler(ETHXCVR_IDType aID)
{
    int32_t retVal;
    ETHXCVR_IOType ethIO;

    if (ETHXCVR_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        retVal = ETHXCVR_SysCmdReq(ETHXCVR_CMD_LINK_CHGIND_HANDLER, &ethIO);
    }

    if (BCM_ERR_OK != retVal) {
        ETHXCVR_ReportError(BCM_XVR_ID, aID, BRCM_SWARCH_ETHXCVR_LINKCHGINDHANDLER_PROC,
                retVal, 0UL, 0UL, 0UL, __LINE__);
    } else {
        if (TRUE == ethIO.isLinkStateChanged) {
            ETHXCVR_LinkStateChangeInd(aID, ethIO.linkState);
        }
    }
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_CmdHandler(ETHXCVR_CmdType aCmd, ETHXCVR_IOType *const aIO)
{
    int32_t retVal;

    if (NULL != aIO) {
        switch (aCmd) {
        case ETHXCVR_CMD_INIT:
            retVal = ETHXCVR_DrvInit(aIO->hwID, aIO->xcvrCfg);
            break;
        case ETHXCVR_CMD_DEINIT:
            retVal = ETHXCVR_DrvDeInit(aIO->hwID);
            break;
        case ETHXCVR_CMD_SET_MODE:
            retVal = ETHXCVR_DrvSetMode(aIO->hwID, aIO->mode, &(aIO->isModeChanged));
            break;
        case ETHXCVR_CMD_GET_MODE:
            retVal = ETHXCVR_DrvGetMode(aIO->hwID, &(aIO->mode));
            break;
        case ETHXCVR_CMD_GET_LINK_STATE:
            retVal = ETHXCVR_DrvGetLinkState(aIO->hwID,
                                               &(aIO->linkState));
            break;
        case ETHXCVR_CMD_GET_SQI_VALUE:
            retVal = ETHXCVR_DrvGetSQIValue(aIO->hwID, &(aIO->sqiValue));
            break;
        case ETHXCVR_CMD_SET_MASTERMODE:
            retVal = ETHXCVR_DrvSetMasterMode(aIO->hwID, aIO->paramEnable);
            break;
        case ETHXCVR_CMD_GET_MASTERMODE:
            retVal = ETHXCVR_DrvGetMasterMode(aIO->hwID, &(aIO->paramEnable));
            break;
        case ETHXCVR_CMD_SET_SPEED:
            retVal = ETHXCVR_DrvSetSpeed(aIO->hwID, aIO->speed);
            break;
        case ETHXCVR_CMD_GET_SPEED:
            retVal = ETHXCVR_DrvGetSpeed(aIO->hwID, &(aIO->speed));
            break;
        case ETHXCVR_CMD_SET_LOOPBACK:
            retVal = ETHXCVR_DrvSetLoopbackMode(aIO->hwID, aIO->paramEnable);
            break;
        case ETHXCVR_CMD_GET_LOOPBACK:
            retVal = ETHXCVR_DrvGetLoopbackMode(aIO->hwID, &(aIO->paramEnable));
            break;
        case ETHXCVR_CMD_SET_JUMBO_MODE:
            retVal = ETHXCVR_DrvSetJumboMode(aIO->hwID, aIO->paramEnable);
            break;
        case ETHXCVR_CMD_GET_JUMBO_MODE:
            retVal = ETHXCVR_DrvGetJumboMode(aIO->hwID, &(aIO->paramEnable));
            break;
        case ETHXCVR_CMD_SET_FLOWCONTROL:
            retVal = ETHXCVR_DrvSetFlowControl(aIO->hwID, aIO->flowControl);
            break;
        case ETHXCVR_CMD_GET_FLOWCONTROL:
            retVal = ETHXCVR_DrvGetFlowControl(aIO->hwID, &(aIO->flowControl));
            break;
        case ETHXCVR_CMD_SET_DUPLEXMODE:
            retVal = ETHXCVR_DrvSetDuplexMode(aIO->hwID, aIO->duplex);
            break;
        case ETHXCVR_CMD_GET_DUPLEXMODE:
            retVal = ETHXCVR_DrvGetDuplexMode(aIO->hwID, &(aIO->duplex));
            break;
        case ETHXCVR_CMD_SET_AUTONEG_MODE:
            retVal = ETHXCVR_DrvSetAutoNegMode(aIO->hwID, aIO->paramEnable);
            break;
        case ETHXCVR_CMD_RESTART_AUTONEG:
            retVal = ETHXCVR_DrvRestartAutoNeg(aIO->hwID);
            break;
        case ETHXCVR_CMD_GET_AUTONEG_STATUS:
            retVal = ETHXCVR_DrvGetAutoNegStatus(aIO->hwID,
                                                &(aIO->autoNegStatus));
            break;
        case ETHXCVR_CMD_GET_STATS:
            retVal = ETHXCVR_DrvGetStats(aIO->hwID, aIO->stats);
            break;
        case ETHXCVR_CMD_STATEHANDLER:
            retVal = ETHXCVR_DrvStateHandler(aIO->hwID, &(aIO->isModeChanged), &(aIO->mode));
            break;
        case ETHXCVR_CMD_UPDATE_HW_STATUS:
            retVal = ETHXCVR_DrvUpdateHWStatus(aIO->hwID);
            break;
        case ETHXCVR_CMD_LINK_CHGIND_HANDLER:
            retVal = ETHXCVR_DrvLinkChgIndHandler(aIO->hwID, &aIO->linkState, &aIO->isLinkStateChanged);
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            ETHXCVR_ReportError(BCM_XVR_ID, aIO->hwID,
                BRCM_SWARCH_ETHXCVR_CMDHANDLER_PROC, retVal, aCmd, 0UL, 0UL, 0UL);
            break;
        }
    } else {
        retVal = BCM_ERR_UNKNOWN;
    }

    return retVal;
}

/** @} */
