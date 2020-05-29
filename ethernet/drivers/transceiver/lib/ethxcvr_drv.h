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
    @defgroup grp_eth_xcvrdrv_drv_impl Ethernet Transceiver Driver Interface
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_drv_impl
    @{

    @file ethxcvr_drv.h
*/

#ifndef ETHXCVR_DRV_H
#define ETHXCVR_DRV_H

#include <eth_xcvr.h>

/**
    @name Ethernet Transceiver API IDs
    @{
    @brief API IDs for Ethernet Transceiver
*/
#define BRCM_SWARCH_ETHXCVR_DRVINIT_PROC              (0x8CU) /**< @brief #ETHXCVR_DrvInit */
#define BRCM_SWARCH_ETHXCVR_DRVGETSPEED_PROC          (0x8DU) /**< @brief #ETHXCVR_DrvGetSpeed */
#define BRCM_SWARCH_ETHXCVR_DRVSETMODE_PROC           (0x8EU) /**< @brief #ETHXCVR_DrvSetMode */
#define BRCM_SWARCH_ETHXCVR_DRVGETMODE_PROC           (0x8FU) /**< @brief #ETHXCVR_DrvGetMode */
#define BRCM_SWARCH_ETHXCVR_DRVGETLINKSTATE_PROC      (0x90U) /**< @brief #ETHXCVR_DrvGetLinkState */
#define BRCM_SWARCH_ETHXCVR_DRVGETDUPLEXMODE_PROC     (0x91U) /**< @brief #ETHXCVR_DrvGetDuplexMode */
#define BRCM_SWARCH_ETHXCVR_DRVGETSQIVALUE_PROC       (0x92U) /**< @brief #ETHXCVR_DrvGetSQIValue */
#define BRCM_SWARCH_ETHXCVR_DRVLINKIRQHANDLER_PROC    (0x93U) /**< @brief #ETHXCVR_DrvLinkIRQHandler */
#define BRCM_SWARCH_ETHXCVR_DRVLINKCHGINDHANDLER_PROC (0x94U) /**< @brief #ETHXCVR_DrvLinkChgIndHandler */
#define BRCM_SWARCH_ETHXCVR_DRVDEINIT_PROC            (0x95U) /**< @brief #ETHXCVR_DrvDeInit */
#define BRCM_SWARCH_ETHXCVR_DRVSETSPEED_PROC          (0x96U) /**< @brief #ETHXCVR_DrvSetSpeed */
#define BRCM_SWARCH_ETHXCVR_DRVSETLOOPBACKMODE_PROC   (0x97U) /**< @brief #ETHXCVR_DrvSetLoopbackMode */
#define BRCM_SWARCH_ETHXCVR_DRVGETLOOPBACKMODE_PROC   (0x98U) /**< @brief #ETHXCVR_DrvGetLoopbackMode */
#define BRCM_SWARCH_ETHXCVR_DRVSETJUMBOMODE_PROC      (0x99U) /**< @brief #ETHXCVR_DrvSetJumboMode */
#define BRCM_SWARCH_ETHXCVR_DRVGETJUMBOMODE_PROC      (0x9AU) /**< @brief #ETHXCVR_DrvGetJumboMode */
#define BRCM_SWARCH_ETHXCVR_DRVSETAUTONEGMODE_PROC    (0x9BU) /**< @brief #ETHXCVR_DrvSetAutoNegMode */
#define BRCM_SWARCH_ETHXCVR_DRVRESTARTAUTONEG_PROC    (0x9CU) /**< @brief #ETHXCVR_DrvRestartAutoNeg */
#define BRCM_SWARCH_ETHXCVR_DRVGETAUTONEGSTATUS_PROC  (0x9DU) /**< @brief #ETHXCVR_DrvGetAutoNegStatus */
#define BRCM_SWARCH_ETHXCVR_DRVGETSTATS_PROC          (0x9EU) /**< @brief #ETHXCVR_DrvGetStats */
#define BRCM_SWARCH_ETHXCVR_DRVSETMASTERMODE_PROC     (0x9FU) /**< @brief #ETHXCVR_DrvSetMasterMode */
#define BRCM_SWARCH_ETHXCVR_DRVGETMASTERMODE_PROC     (0xA0U) /**< @brief #ETHXCVR_DrvGetMasterMode */
#define BRCM_SWARCH_ETHXCVR_DRVSETDUPLEXMODE_PROC     (0xA1U) /**< @brief #ETHXCVR_DrvSetDuplexMode */
#define BRCM_SWARCH_ETHXCVR_DRVSETFLOWCONTROL_PROC    (0xA2U) /**< @brief #ETHXCVR_DrvSetFlowControl */
#define BRCM_SWARCH_ETHXCVR_DRVGETFLOWCONTROL_PROC    (0xA3U) /**< @brief #ETHXCVR_DrvGetFlowControl */
#define BRCM_SWARCH_ETHXCVR_DRVSTATEHANDLER_PROC      (0xA4U) /**< @brief #ETHXCVR_DrvStateHandler */
#define BRCM_SWARCH_ETHXCVR_DRVUPDATEHWSTATUS_PROC    (0xA5U) /**< @brief #ETHXCVR_DrvUpdateHWStatus */
#define BRCM_SWARCH_ETHXCVR_DRVGETPORTCONFIG_PROC     (0xA6U) /**< @brief #ETHXCVR_DrvGetPortConfig */
/** @} */


/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvInit(ETHXCVR_IDType aID,
                               const ETHXCVR_PortConfigType *const aConfig);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvDeInit(ETHXCVR_IDType aID);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType aSpeed);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetSpeed(ETHXCVR_IDType aID,
                                   ETHXCVR_SpeedType *const aSpeed);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetMode(ETHXCVR_IDType aID,
                                  ETHXCVR_ModeType aMode,
                                  uint32_t *const aIsModeChanged);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetMode(ETHXCVR_IDType aID,
                                  ETHXCVR_ModeType *const aMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetLinkState(ETHXCVR_IDType aID,
                                       ETHXCVR_LinkStateType *const aLinkState);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetDuplexMode(ETHXCVR_IDType aID,
                                        ETHXCVR_DuplexModeType *const aDuplexMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetSQIValue(ETHXCVR_IDType aID,
                                      uint32_t *const aSQIValue);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetMasterMode(ETHXCVR_IDType aID,
                                        ETHXCVR_BooleanType aMasterMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetMasterMode(ETHXCVR_IDType aID,
                                        ETHXCVR_BooleanType *const aMasterMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetLoopbackMode(ETHXCVR_IDType aID,
                                          ETHXCVR_BooleanType aMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetLoopbackMode(ETHXCVR_IDType aID,
                                          ETHXCVR_BooleanType *const aMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetJumboMode(ETHXCVR_IDType aID,
                                       ETHXCVR_BooleanType aMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetJumboMode(ETHXCVR_IDType aID,
                                       ETHXCVR_BooleanType *const aJumboMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetAutoNegStatus(ETHXCVR_IDType aID,
                        ETHXCVR_AutoNegStatusType *const aAutoNegStatus);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetStats(ETHXCVR_IDType aID,
                                ETHXCVR_StatsType *const aStats);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvLinkIRQHandler(ETHXCVR_IDType aID);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvLinkChgIndHandler(ETHXCVR_IDType aID,
                                            ETHXCVR_LinkStateType *const aLinkState,
                                            uint32_t *const aIsLinkStateChanged);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetFlowControl(ETHXCVR_IDType aID,
                                         ETHXCVR_FlowControlType aFlowControl);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvGetFlowControl(ETHXCVR_IDType aID,
                                         ETHXCVR_FlowControlType *const aFlowControl);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetDuplexMode(ETHXCVR_IDType aID,
                                        ETHXCVR_DuplexModeType aDuplexMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvSetAutoNegMode(ETHXCVR_IDType aID,
                                         ETHXCVR_BooleanType aMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvRestartAutoNeg(ETHXCVR_IDType aID);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvStateHandler(ETHXCVR_IDType aID,
                                       uint32_t *const aIsModeChanged,
                                       ETHXCVR_ModeType *const aMode);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_DrvUpdateHWStatus(ETHXCVR_IDType aID);

/**
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_DrvGetPortConfig(ETHXCVR_IDType aID,
                                 ETHXCVR_PortConfigType *const aConfig);
#endif /* ETHXCVR_DRV_H */

/** @} */
