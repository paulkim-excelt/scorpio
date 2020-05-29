/****************************************************************************
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
    @addtogroup grp_eth_xcvrdrv_drv_impl
    @{

    @file ethxcvr_drv.c
*/

#include <utils.h>
#include <ethxcvr_osil.h>
#include "ethxcvr_drv.h"
#include "ethxcvr_platform.h"

/**
    @name Transceiver driver Design IDs
    @{
    @brief Design IDs for Transceiver driver
*/
#define BRCM_SWDSGN_ETHXCVR_DRVINIT_PROC              (0x96U) /**< @brief #ETHXCVR_DrvInit */
#define BRCM_SWDSGN_ETHXCVR_DRVGETSPEED_PROC          (0x97U) /**< @brief #ETHXCVR_DrvGetSpeed */
#define BRCM_SWDSGN_ETHXCVR_DRVSETMODE_PROC           (0x98U) /**< @brief #ETHXCVR_DrvSetMode */
#define BRCM_SWDSGN_ETHXCVR_DRVGETMODE_PROC           (0x99U) /**< @brief #ETHXCVR_DrvGetMode */
#define BRCM_SWDSGN_ETHXCVR_DRVGETLINKSTATE_PROC      (0x9AU) /**< @brief #ETHXCVR_DrvGetLinkState */
#define BRCM_SWDSGN_ETHXCVR_DRVGETDUPLEXMODE_PROC     (0x9BU) /**< @brief #ETHXCVR_DrvGetDuplexMode */
#define BRCM_SWDSGN_ETHXCVR_DRVGETSQIVALUE_PROC       (0x9CU) /**< @brief #ETHXCVR_DrvGetSQIValue */
#define BRCM_SWDSGN_ETHXCVR_DRVLINKIRQHANDLER_PROC    (0x9DU) /**< @brief #ETHXCVR_DrvLinkIRQHandler */
#define BRCM_SWDSGN_ETHXCVR_DRVLINKCHGINDHANDLER_PROC (0x9EU) /**< @brief #ETHXCVR_DrvLinkChgIndHandler */
#define BRCM_SWDSGN_ETHXCVR_DRVRWDATA_TYPE            (0x9FU) /**< @brief #ETHXCVR_DrvRWDataType */
#define BRCM_SWDSGN_ETHXCVR_DRVRWDATA_GLOBAL          (0xA0U) /**< @brief #ETHXCVR_DrvRWData */
#define BRCM_SWDSGN_ETHXCVR_DRVISPHYFUNCTBLVALID_PROC (0xA1U) /**< @brief #ETHXCVR_DrvIsPhyFuncTblValid */
#define BRCM_SWDSGN_ETHXCVR_DRVDEINIT_PROC            (0xA2U) /**< @brief #ETHXCVR_DrvDeInit */
#define BRCM_SWDSGN_ETHXCVR_DRVSETSPEED_PROC          (0xA3U) /**< @brief #ETHXCVR_DrvSetSpeed */
#define BRCM_SWDSGN_ETHXCVR_DRVSETMASTERMODE_PROC     (0xA4U) /**< @brief #ETHXCVR_DrvSetMasterMode */
#define BRCM_SWDSGN_ETHXCVR_DRVGETMASTERMODE_PROC     (0xA5U) /**< @brief #ETHXCVR_DrvGetMasterMode */
#define BRCM_SWDSGN_ETHXCVR_DRVSETLOOPBACKMODE_PROC   (0xA6U) /**< @brief #ETHXCVR_DrvSetLoopbackMode */
#define BRCM_SWDSGN_ETHXCVR_DRVGETLOOPBACKMODE_PROC   (0xA7U) /**< @brief #ETHXCVR_DrvGetLoopbackMode */
#define BRCM_SWDSGN_ETHXCVR_DRVSETJUMBOMODE_PROC      (0xA8U) /**< @brief #ETHXCVR_DrvSetJumboMode */
#define BRCM_SWDSGN_ETHXCVR_DRVRESTARTAUTONEG_PROC    (0xA9U) /**< @brief #ETHXCVR_DrvRestartAutoNeg */
#define BRCM_SWDSGN_ETHXCVR_DRVGETAUTONEGSTATUS_PROC  (0xAAU) /**< @brief #ETHXCVR_DrvGetAutoNegStatus */
#define BRCM_SWDSGN_ETHXCVR_DRVGETSTATS_PROC          (0xABU) /**< @brief #ETHXCVR_DrvGetStats */
#define BRCM_SWDSGN_ETHXCVR_DRVGETJUMBOMODE_PROC      (0xACU) /**< @brief #ETHXCVR_DrvGetJumboMode */
#define BRCM_SWDSGN_ETHXCVR_DRVSETDUPLEXMODE_PROC     (0xADU) /**< @brief #ETHXCVR_DrvSetDuplexMode */
#define BRCM_SWDSGN_ETHXCVR_DRVSETFLOWCONTROL_PROC    (0xAEU) /**< @brief #ETHXCVR_DrvSetFlowControl */
#define BRCM_SWDSGN_ETHXCVR_DRVGETFLOWCONTROL_PROC    (0xAFU) /**< @brief #ETHXCVR_DrvGetFlowControl */
#define BRCM_SWDSGN_ETHXCVR_DRVSTATEHANDLER_PROC      (0xB0U) /**< @brief #ETHXCVR_DrvStateHandler */
#define BRCM_SWDSGN_ETHXCVR_DRVUPDATEHWSTATUS_PROC    (0xB1U) /**< @brief #ETHXCVR_DrvUpdateHWStatus */
#define BRCM_SWDSGN_ETHXCVR_DRVGETPORTCONFIG_PROC     (0xB2U) /**< @brief #ETHXCVR_DrvGetPortConfig */
#define BRCM_SWDSGN_ETHXCVR_DRVSETAUTONEGMODE_PROC    (0xB3U) /**< @brief #ETHXCVR_DrvSetAutoNegMode */
/** @} */

/**
    @trace #BRCM_SWARCH_ETHXCVR_INIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DEINIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_SETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_GETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_GETLINKSTATE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_GETSPEED_PROC
    @trace #BRCM_SWARCH_ETHXCVR_GETDUPLEXMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_GETSQIVALUE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_PROC
    @trace #BRCM_SWARCH_ETHXCVR_LINKCHGINDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef struct _ETHXCVR_DrvRWDataType {
    ETHXCVR_StateType      xcvrState;
    ETHXCVR_PortConfigType xcvrCfg;
} ETHXCVR_DrvRWDataType;

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVINIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVDEINIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETLINKSTATE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETSPEED_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETDUPLEXMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETSQIVALUE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_PROC
    @trace #BRCM_SWARCH_ETHXCVR_LINKCHGINDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
static ETHXCVR_DrvRWDataType ETHXCVR_DrvRWData[ETHXCVR_HW_ID_MAX] COMP_SECTION(".data.drivers") = {
#if (ETHXCVR_HW_ID_MAX == 0UL)
#error "ETHXCVR_HW_ID_MAX == 0"
#endif
#if (ETHXCVR_HW_ID_MAX > 0UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 1UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 2UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 3UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 4UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 5UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 6UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 7UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 8UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 9UL)
    {
        .xcvrState = ETHXCVR_STATE_UNINIT,
        .xcvrCfg = {0U},
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 10UL)
#error "ETHXCVR_HW_ID_MAX > 10UL not supported"
#endif
};


/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVINIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvInit(ETHXCVR_IDType aID,
                        const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal;
    const ETHXCVR_FuncTblType *phyFunc = NULL;
    const ETHXCVR_FuncTblType *busFunc = NULL;
    ETHXCVR_PhyCntxtType      *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(aConfig, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            if ((NULL == busFunc) && (NULL == phyFunc)) {
                retVal = BCM_ERR_INVAL_PARAMS;
            } else {
                if (NULL != busFunc) {
                    retVal = busFunc->init(aConfig->bus.cntrlID, aConfig);
                }

                if ((BCM_ERR_OK == retVal) && (NULL != phyFunc)) {
                    retVal = phyFunc->init(aConfig->phy.hwID, aConfig);
                }

                if (BCM_ERR_OK == retVal) {
                    ETHXCVR_DrvRWData[aID].xcvrState = ETHXCVR_STATE_INIT;
                    /* Cache the config */
                    BCM_MemCpy(&ETHXCVR_DrvRWData[aID].xcvrCfg, aConfig, sizeof(ETHXCVR_PortConfigType));
                }
            }
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVDEINIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvDeInit(ETHXCVR_IDType aID)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_INIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            if (NULL != phyFunc) {
                retVal = phyFunc->deinit(config->phy.hwID, config);
            }

            if ((BCM_ERR_OK == retVal) && (NULL != busFunc)) {
                retVal = busFunc->deinit(config->bus.cntrlID, config);
            }

            if (BCM_ERR_OK == retVal) {
                ETHXCVR_DrvRWData[aID].xcvrState = ETHXCVR_STATE_UNINIT;
                BCM_MemSet(&ETHXCVR_DrvRWData[aID].xcvrCfg, 0U, sizeof(ETHXCVR_PortConfigType));
            }
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetMode(ETHXCVR_IDType aID,
                           ETHXCVR_ModeType aMode,
                           uint32_t *const aIsModeChanged)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->setMode(config->bus.cntrlID, config, aMode);
                if (BCM_ERR_OK == retVal) {
                    *aIsModeChanged = TRUE;
                }
            }
            if ((BCM_ERR_OK == retVal) && (NULL != phyFunc)) {
                retVal = phyFunc->setMode(config->phy.hwID, config, aMode);
                if (BCM_ERR_OK == retVal) {
                    *aIsModeChanged = (ETHXCVR_DrvRWData[aID].xcvrCfg.phy.accessMode == ETHXCVR_ACCESSMODE_MMAP) ?
                        TRUE: FALSE;
                }
            }
            if (BCM_ERR_OK == retVal) {
                /* Update the configuration */
                ETHXCVR_DrvRWData[aID].xcvrCfg.portEnable = (aMode == ETHXCVR_MODE_ACTIVE) ?
                    ETHXCVR_BOOLEAN_TRUE:ETHXCVR_BOOLEAN_FALSE;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetMode(ETHXCVR_IDType aID,
                           ETHXCVR_ModeType *const aMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_ModeType             phyMode;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->getMode(config->bus.cntrlID, config, aMode);
            }
            if ((BCM_ERR_OK == retVal) && (NULL != phyFunc)) {
                retVal = phyFunc->getMode(config->phy.hwID, config, &phyMode);
                /* Check for mismatch between bus and phy */
                if (BCM_ERR_OK == retVal)  {
                    if (NULL != busFunc) {
                        if (*aMode != phyMode) {
                            retVal = BCM_ERR_UNKNOWN;
                        }
                    } else {
                        *aMode = phyMode;
                    }
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETLINKSTATE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetLinkState(ETHXCVR_IDType aID,
                                ETHXCVR_LinkStateType *const aLinkState)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    ETHXCVR_LinkStateType        phyLinkState = ETHXCVR_LINKSTATE_ACTIVE;
    ETHXCVR_LinkStateType        busLinkState = ETHXCVR_LINKSTATE_ACTIVE;
    const ETHXCVR_PortConfigType *config = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->getLinkState(config->bus.cntrlID, config, &busLinkState);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                retVal = phyFunc->getLinkState(config->phy.hwID, config, &phyLinkState);
            }
            /* Report down if any one of the status is down. Do not report error */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                if ((ETHXCVR_LINKSTATE_DOWN == phyLinkState) || (ETHXCVR_LINKSTATE_DOWN == busLinkState)) {
                    *aLinkState = ETHXCVR_LINKSTATE_DOWN;
                } else {
                    *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetSpeed(ETHXCVR_IDType aID,
                            ETHXCVR_SpeedType aSpeed)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->setSpeed(config->bus.cntrlID, config, aSpeed);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                retVal = phyFunc->setSpeed(config->phy.hwID, config, aSpeed);
            }
            /* Update the configuration */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                ETHXCVR_DrvRWData[aID].xcvrCfg.speed = aSpeed;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetSpeed(ETHXCVR_IDType aID,
                            ETHXCVR_SpeedType *const aSpeed)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_SpeedType            phySpeed;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->getSpeed(config->bus.cntrlID, config, aSpeed);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {

                if (BCM_ERR_NOSUPPORT == retVal) {
                    retVal = phyFunc->getSpeed(config->phy.hwID, config, aSpeed);
                } else {
                    retVal = phyFunc->getSpeed(config->phy.hwID, config, &phySpeed);
                    /* Check for mismatch between phy and bus */
                    if (BCM_ERR_OK == retVal) {
                        if (NULL != busFunc) {
                            if (*aSpeed != phySpeed) {
                                retVal = BCM_ERR_UNKNOWN;
                            }
                        } else {
                            *aSpeed = phySpeed;
                        }
                    }
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetDuplexMode(ETHXCVR_IDType aID,
                                 ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_DuplexModeType       phyDuplex;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->getDuplexMode(config->bus.cntrlID, config, aDuplexMode);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                if (BCM_ERR_NOSUPPORT == retVal) {
                    retVal = phyFunc->getDuplexMode(config->phy.hwID, config, aDuplexMode);
                } else {
                    retVal = phyFunc->getDuplexMode(config->phy.hwID, config, &phyDuplex);
                    /* Check for mismatch between phy and bus */
                    if (BCM_ERR_OK == retVal) {
                        if (NULL != busFunc) {
                            if (*aDuplexMode != phyDuplex) {
                                retVal = BCM_ERR_UNKNOWN;
                            }
                        } else {
                            *aDuplexMode = phyDuplex;
                        }
                    }
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetDuplexMode(ETHXCVR_IDType aID,
                                 ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->setDuplexMode(config->bus.cntrlID, config, aDuplexMode);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                retVal = phyFunc->setDuplexMode(config->phy.hwID, config, aDuplexMode);
            }
            /* Update the configuration */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                ETHXCVR_DrvRWData[aID].xcvrCfg.duplex = aDuplexMode;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETFLOWCONTROL_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetFlowControl(ETHXCVR_IDType aID,
                                 ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_FlowControlType      phyFlowControl;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->getFlowControl(config->bus.cntrlID, config, aFlowControl);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                if (BCM_ERR_NOSUPPORT == retVal) {
                    retVal = phyFunc->getFlowControl(config->phy.hwID, config, aFlowControl);
                } else {
                    retVal = phyFunc->getFlowControl(config->phy.hwID, config, &phyFlowControl);
                    /* Check for mismatch between phy and bus */
                    if (BCM_ERR_OK == retVal) {
                        if (NULL != busFunc) {
                            if (*aFlowControl != phyFlowControl) {
                                retVal = BCM_ERR_UNKNOWN;
                            }
                        } else {
                            *aFlowControl = phyFlowControl;
                        }
                    }
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETFLOWCONTROL_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetFlowControl(ETHXCVR_IDType aID,
                                  ETHXCVR_FlowControlType aFlowControl)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->setFlowControl(config->bus.cntrlID, config, aFlowControl);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                retVal = phyFunc->setFlowControl(config->phy.hwID, config, aFlowControl);
            }
            /* Update the configuration */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                ETHXCVR_DrvRWData[aID].xcvrCfg.flowControl = aFlowControl;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETSQIVALUE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetSQIValue(ETHXCVR_IDType aID,
                               uint32_t *const aSQIValue)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->getSQIValue(config->phy.hwID, config, aSQIValue);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETMASTERMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetMasterMode(ETHXCVR_IDType aID,
                                 ETHXCVR_BooleanType aMasterMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->setMasterMode(config->phy.hwID, config, aMasterMode);
                /* Update the configuration */
                if (BCM_ERR_OK == retVal) {
                    ETHXCVR_DrvRWData[aID].xcvrCfg.phy.masterMode = aMasterMode;
                }
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETMASTERMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetMasterMode(ETHXCVR_IDType aID,
                                 ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->getMasterMode(config->phy.hwID, config, aMasterMode);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETLOOPBACKMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetLoopbackMode(ETHXCVR_IDType aID,
                                   ETHXCVR_BooleanType aMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->setLoopbackMode(config->phy.hwID, config, aMode);
            } else if (NULL != busFunc) {
                retVal = busFunc->setLoopbackMode(config->bus.cntrlID, config, aMode);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETLOOPBACKMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetLoopbackMode(ETHXCVR_IDType aID,
                                   ETHXCVR_BooleanType *const aMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->getLoopbackMode(config->phy.hwID, config, aMode);
            } else if (NULL != busFunc) {
                retVal = busFunc->getLoopbackMode(config->bus.cntrlID, config, aMode);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETJUMBOMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetJumboMode(ETHXCVR_IDType aID,
                                ETHXCVR_BooleanType aMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->setJumboMode(config->bus.cntrlID, config, aMode);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                retVal = phyFunc->setJumboMode(config->phy.hwID, config, aMode);
            }
            /* Update the configuration */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                ETHXCVR_DrvRWData[aID].xcvrCfg.jumbo = aMode;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETJUMBOMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetJumboMode(ETHXCVR_IDType aID,
                                ETHXCVR_BooleanType *const aMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_BooleanType          phyJumbo;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != busFunc) {
                retVal = busFunc->getJumboMode(config->bus.cntrlID, config, aMode);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != phyFunc)) {
                if (BCM_ERR_NOSUPPORT == retVal) {
                    retVal = phyFunc->getJumboMode(config->phy.hwID, config, aMode);
                } else {
                    retVal = phyFunc->getJumboMode(config->phy.hwID, config, &phyJumbo);
                    /* Check for mismatch between phy and bus */
                    if (BCM_ERR_OK == retVal) {
                        if (NULL != busFunc) {
                            if (*aMode != phyJumbo) {
                                retVal = BCM_ERR_UNKNOWN;
                            }
                        } else {
                            *aMode = phyJumbo;
                        }
                    }
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSETAUTONEGMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvSetAutoNegMode(ETHXCVR_IDType aID, ETHXCVR_BooleanType aMode)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->setAutoNegMode(config->phy.hwID, config, aMode);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != busFunc)) {
                retVal = busFunc->setAutoNegMode(config->bus.cntrlID, config, aMode);
            }
            /* Update the configuration */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                ETHXCVR_DrvRWData[aID].xcvrCfg.autoNeg = aMode;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETAUTONEGSTATUS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetAutoNegStatus(ETHXCVR_IDType aID,
                                    ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            ETHXCVR_AutoNegStatusType autoNegStatus = ETHXCVR_AUTONEGSTATUS_COMPLETE;
            if (NULL != phyFunc) {
                retVal = phyFunc->getAutoNegStatus(config->phy.hwID, config, &autoNegStatus);
            }
            if (((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) && (NULL != busFunc)) {
                retVal = busFunc->getAutoNegStatus(config->bus.cntrlID, config, aStatus);
            }
            /* Report down if any one of the status is down. Do not report error */
            if ((BCM_ERR_OK == retVal) || (BCM_ERR_NOSUPPORT == retVal)) {
                if ((ETHXCVR_AUTONEGSTATUS_COMPLETE == autoNegStatus) && (ETHXCVR_AUTONEGSTATUS_COMPLETE == *aStatus)) {
                } else {
                    *aStatus = ETHXCVR_AUTONEGSTATUS_INCOMPLETE;
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVRESTARTAUTONEG_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvRestartAutoNeg(ETHXCVR_IDType aID)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (config->autoNeg == ETHXCVR_BOOLEAN_TRUE) {
                if (NULL != phyFunc) {
                    retVal = phyFunc->restartAutoNeg(config->phy.hwID, config);
                } else if (NULL != busFunc) {
                    retVal = busFunc->restartAutoNeg(config->bus.cntrlID, config);
                } else {
                    retVal = BCM_ERR_NOSUPPORT;
                }
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETSTATS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetStats(ETHXCVR_IDType aID,
                            ETHXCVR_StatsType *const aStats)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->getStats(config->phy.hwID, config, aStats);
            } else if (NULL != busFunc) {
                retVal = busFunc->getStats(config->bus.cntrlID, config, aStats);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVSTATEHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvStateHandler(ETHXCVR_IDType aID,
                                uint32_t *const aIsModeChanged,
                                ETHXCVR_ModeType *const aMode)
{
    int32_t                      retVal   = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    if (ETHXCVR_BOOLEAN_TRUE == config->portEnable) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

        if (BCM_ERR_OK == retVal) {
            if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
                retVal = BCM_ERR_INVAL_STATE;
            } else {
                if ((NULL != phyFunc) && (config->phy.accessMode != ETHXCVR_ACCESSMODE_MMAP)) {
                    retVal = phyFunc->stateHandler(config->phy.hwID, config, aIsModeChanged, aMode);
                } else {
                    retVal = BCM_ERR_NOSUPPORT;
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVUPDATEHWSTATUS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvUpdateHWStatus(ETHXCVR_IDType aID)
{
    int32_t                      retVal   = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    if (ETHXCVR_BOOLEAN_TRUE == config->portEnable) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

        if (BCM_ERR_OK == retVal) {
            if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
                retVal = BCM_ERR_INVAL_STATE;
            } else {
                if (NULL != phyFunc) {
                    retVal = phyFunc->updateHWStatus(config->phy.hwID, config);
                } else {
                    retVal = BCM_ERR_NOSUPPORT;
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVLINKIRQHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvLinkIRQHandler(ETHXCVR_IDType aID)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->linkIRQHandler(config->phy.hwID, config);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVLINKCHGINDHANDLER_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvLinkChgIndHandler(ETHXCVR_IDType aID,
                                     ETHXCVR_LinkStateType *const aLinkState,
                                     uint32_t *const aIsLinkStateChanged)
{
    int32_t                      retVal;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_FuncTblType    *busFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_PhyCntxtType         *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(config, &busFunc, &phyFunc, &phyCntxt);

    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            if (NULL != phyFunc) {
                retVal = phyFunc->linkChangeIndHandler(config->phy.hwID, config, aLinkState, aIsLinkStateChanged);
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRVGETPORTCONFIG_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetPortConfig(ETHXCVR_IDType aID,
                              ETHXCVR_PortConfigType *const aConfig)
{
    BCM_MemCpy(aConfig, &ETHXCVR_DrvRWData[aID].xcvrCfg, sizeof(ETHXCVR_PortConfigType));
    return BCM_ERR_OK;
}

/** @} */
