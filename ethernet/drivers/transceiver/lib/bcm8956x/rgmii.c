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
/**
    @defgroup grp_eth_xcvrdrv_rgmii_impl IO block based RGMII/RVMII/RMII/MII driver for bcm8953x
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_rgmii_impl
    @{

    @file rgmii.c
    @brief This file implements RGMII driver of Ethernet driver.

    @version 0.1 Initial version
*/

#include <stdlib.h> /*For debug message*/
#include <compiler.h>
#include <utils.h>
#include <bcm_err.h>
#include <eth_xcvr.h>
#include <io_rdb.h>

/**
    @name RGMII driver Design IDs
    @{
    @brief Design IDs for RGMII driver
*/
#define BRCM_SWDSGN_RGMII_INIT_PROC                   (0xA1U) /**< @brief #RGMII_Init */
#define BRCM_SWDSGN_RGMII_RESET_PROC                  (0xA2U) /**< @brief #RGMII_Command */
#define BRCM_SWDSGN_RGMII_GETSQIVALUE_PROC            (0xA3U) /**< @brief #RGMII_GetSQIValue */
#define BRCM_SWDSGN_RGMII_GETDUPLEXMODE_PROC          (0xA4U) /**< @brief #RGMII_GetDuplexMode */
#define BRCM_SWDSGN_RGMII_GETSPEED_PROC               (0xA5U) /**< @brief #RGMII_GetSpeed */
#define BRCM_SWDSGN_RGMII_GETLINKSTATE_PROC           (0xA6U) /**< @brief #RGMII_GetLinkState */
#define BRCM_SWDSGN_RGMII_GETMODE_PROC                (0xA7U) /**< @brief #RGMII_GetMode */
#define BRCM_SWDSGN_RGMII_SETMODE_PROC                (0xA8U) /**< @brief #RGMII_SetMode */
#define BRCM_SWDSGN_RGMII_SETSPEED_PROC               (0xA9U) /**< @brief #RGMII_SetSpeed */
#define BRCM_SWDSGN_RGMII_GETAUTONEGSTATUS_PROC       (0xAAU) /**< @brief #RGMII_GetAutoNegStatus */
#define BRCM_SWDSGN_RGMII_SETDUPLEXMODE_PROC          (0xABU) /**< @brief #RGMII_SetDuplexMode */
#define BRCM_SWDSGN_RGMII_SETFLOWCONTROL_PROC         (0xACU) /**< @brief #RGMII_SetFlowControl */
#define BRCM_SWDSGN_RGMII_GETFLOWCONTROL_PROC         (0xADU) /**< @brief #RGMII_GetFlowControl */
#define BRCM_SWDSGN_RGMII_LINKCHANGEINDHANDLER_PROC   (0xAEU) /**< @brief #RGMII_LinkChangeIndHandler */
#define BRCM_SWDSGN_RGMII_DEINIT_PROC                 (0xAFU) /**< @brief #RGMII_DeInit */
#define BRCM_SWDSGN_RGMII_GETSTATS_PROC               (0xB1U) /**< @brief #RGMII_GetStats */
#define BRCM_SWDSGN_RGMII_RWDATA_TYPE                 (0xB2U) /**< @brief #RGMII_RWDataType */
#define BRCM_SWDSGN_RGMII_REGS_GLOBAL                 (0xB3U) /**< @brief #RGMII_REGS */
#define BRCM_SWDSGN_RGMII_RWDATA_GLOBAL               (0xB4U) /**< @brief #RGMII_RWData */
#define BRCM_SWDSGN_RGMII_FUNCTBL_GLOBAL              (0xB5U) /**< @brief #RGMII_FuncTbl */
/** @} */

/**
    @brief IO block base addresses

    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static volatile IO_RDBType* const RGMII_REGS = (IO_RDBType *const) IO_BASE;

/**
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
*/
typedef struct _RGMII_RWDataType {
    ETHXCVR_StateType state[IO_NUM_RGMII_PORTS];
} RGMII_RWDataType;

/**
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
*/
static RGMII_RWDataType RGMII_RWData[IO_MAX_HW_ID]
COMP_SECTION(".data.drivers") =
{
#if (IO_MAX_HW_ID != 1UL)
#error "IO_MAX_HW_ID should be 1UL"
#endif
    {
        .state = {ETHXCVR_STATE_UNINIT, ETHXCVR_STATE_UNINIT},
    },
};

/**
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
*/
static uint32_t RGMII_ConvertPortToIndex(ETHXCVR_IDType aID)
{
    uint32_t index = 0UL;
    switch (aID) {
        case 5U:
            index = 1U;
            break;
        case 8U:
            index = 0U;
            break;
        default:
            break;
    }

    return index;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_Init(uint8_t aBusIdx,
                          const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t index;

    if ((IO_MAX_HW_ID <= aBusIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    index = RGMII_ConvertPortToIndex(aConfig->id);
    if (ETHXCVR_STATE_UNINIT != RGMII_RWData[aBusIdx].state[index]) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    RGMII_RWData[aBusIdx].state[index] = ETHXCVR_STATE_INIT;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_DeInit(uint8_t aBusIdx,
                            const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t index;

    if ((IO_MAX_HW_ID <= aBusIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    index = RGMII_ConvertPortToIndex(aConfig->id);
    if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[index]) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    RGMII_RWData[aBusIdx].state[index] = ETHXCVR_STATE_UNINIT;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static void RGMII_SetBusMode(const ETHXCVR_PortConfigType *const aConfig)
{
    uint16_t val = 0U;
    switch (aConfig->busMode) {
        case ETHXCVR_BUSMODE_MII:
            val = 1U;
            break;
        case ETHXCVR_BUSMODE_RVMII:
            val = 2U;
            break;
        case ETHXCVR_BUSMODE_RMII:
            val = 3U;
            break;
        default:
            break;
    }
    switch (aConfig->id) {
        case 8U:
            RGMII_REGS->straps_ov &= ~(IO_STRAPS_OV_MII1_MODE_0_MASK|IO_STRAPS_OV_MII1_MODE_1_MASK);
            RGMII_REGS->straps_ov |= (val << IO_STRAPS_OV_MII1_MODE_0_SHIFT);
            break;
        case 5U:
            RGMII_REGS->straps_ov &= ~(IO_STRAPS_OV_MII2_MODE_0_MASK|IO_STRAPS_OV_MII2_MODE_1_MASK);
            RGMII_REGS->straps_ov |= (val << IO_STRAPS_OV_MII2_MODE_0_SHIFT);
            break;
        default:
            break;
    }
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_IntSetSpeed(ETHXCVR_IDType aID,
                                 ETHXCVR_SpeedType aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t speed;

    switch (aSpeed) {
        case ETHXCVR_SPEED_10MBPS:
            speed = 0U;
            break;
        case ETHXCVR_SPEED_100MBPS:
            speed = 1U;
            break;
        case ETHXCVR_SPEED_1000MBPS:
            speed = 2U;
            break;
        default:
            retVal = BCM_ERR_NOSUPPORT;
            break;
    }
    if (BCM_ERR_OK == retVal) {
        switch (aID) {
            case 8U:
                RGMII_REGS->rgmii1_gmii_ctl &=  ~IO_RGMII1_GMII_CTL_RGMII_SPD_MASK;
                RGMII_REGS->rgmii1_gmii_ctl |=  speed << IO_RGMII1_GMII_CTL_RGMII_SPD_SHIFT;
                break;
            case 5U:
                RGMII_REGS->rgmii2_gmii_ctl &=  ~IO_RGMII2_GMII_CTL_RGMII_SPD_MASK;
                RGMII_REGS->rgmii2_gmii_ctl |=  speed << IO_RGMII2_GMII_CTL_RGMII_SPD_SHIFT;
                break;
            default:
                retVal = BCM_ERR_NOT_FOUND;
                break;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetMode(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((IO_MAX_HW_ID <= aBusIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aConfig->busMode != ETHXCVR_BUSMODE_RGMII) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->id) {
            case 8U:
                if (ETHXCVR_MODE_ACTIVE == aMode) {
                    RGMII_REGS->rgmii1_ctl &= ~IO_RGMII1_CTL_DIS_IMP_MASK;
                    RGMII_REGS->rgmii1_ctl |= IO_RGMII1_CTL_RMII_CLOCK_DIRECTION_MASK;
                    RGMII_REGS->rgmii1_ctl &= ~IO_RGMII1_CTL_RGMII1_CTL_MASK;
                    RGMII_REGS->sgmii_rgmii_ctl |= IO_SGMII_RGMII_CTL_SEL_RGMII1_MASK;
                    RGMII_REGS->rgmii1_gmii_ctl |= IO_RGMII1_GMII_CTL_RGMII_LINK_MASK;
                    RGMII_SetBusMode(aConfig);
                    RGMII_IntSetSpeed(aConfig->id, aConfig->speed);
                    RGMII_REGS->mii1_config = IO_MII1_CONFIG_CLOCK_EN_MII1_MASK |
                                              IO_MII1_CONFIG_SEL_MII1_MASK;
                } else if (ETHXCVR_MODE_DOWN == aMode) {
                    RGMII_REGS->rgmii1_ctl |= IO_RGMII1_CTL_DIS_IMP_MASK;
                    RGMII_REGS->sgmii_rgmii_ctl &= ~IO_SGMII_RGMII_CTL_SEL_RGMII1_MASK;
                    RGMII_REGS->rgmii1_gmii_ctl &= ~IO_RGMII1_GMII_CTL_RGMII_LINK_MASK;
                }
                break;
            case 5U:
                if (ETHXCVR_MODE_ACTIVE == aMode) {
                    RGMII_REGS->rgmii2_ctl &= ~IO_RGMII2_CTL_DIS_IMP_MASK;
                    RGMII_REGS->rgmii2_ctl |= IO_RGMII2_CTL_RMII_CLOCK_DIRECTION_MASK;
                    RGMII_REGS->rgmii2_ctl &= ~IO_RGMII2_CTL_RGMII2_CTL_MASK;
                    RGMII_REGS->rgmii2_ctl |= (2U << IO_RGMII2_CTL_RGMII2_CTL_SHIFT);
		    //RGMII_REGS->rgmii2_ctl &= 0xfff0U; /*paulkim 2019-12-03 register write test code*/ 
                    RGMII_REGS->sgmii_rgmii_ctl |= IO_SGMII_RGMII_CTL_SEL_RGMII2_MASK;
                    RGMII_REGS->rgmii2_gmii_ctl |= IO_RGMII2_GMII_CTL_RGMII_LINK_MASK;
                    RGMII_SetBusMode(aConfig);
                    RGMII_IntSetSpeed(aConfig->id, aConfig->speed);
                    RGMII_REGS->mii2_config = IO_MII2_CONFIG_CLOCK_EN_MII2_MASK |
                                              IO_MII2_CONFIG_SEL_MII2_MASK;
                } else if (ETHXCVR_MODE_DOWN == aMode) {
                    RGMII_REGS->rgmii2_ctl |= IO_RGMII2_CTL_DIS_IMP_MASK;
                    RGMII_REGS->sgmii_rgmii_ctl &= ~IO_SGMII_RGMII_CTL_SEL_RGMII2_MASK;
                    RGMII_REGS->rgmii2_gmii_ctl &= ~IO_RGMII2_GMII_CTL_RGMII_LINK_MASK;
		    //RGMII_REGS->rgmii2_ctl &= 0xfff0U; /*paulkim 2019-12-03 register write test code*/
                }
                break;
            default:
                retVal = BCM_ERR_NOT_FOUND;
                break;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetMode(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->id) {
            case 8U:
                if (0U == (RGMII_REGS->rgmii1_ctl & IO_RGMII1_CTL_DIS_IMP_MASK)) {
                    *aMode = ETHXCVR_MODE_ACTIVE;
                } else {
                    *aMode = ETHXCVR_MODE_DOWN;
                }
                break;
            case 5U:
                if (0U == (RGMII_REGS->rgmii2_ctl & IO_RGMII2_CTL_DIS_IMP_MASK)) {
                    *aMode = ETHXCVR_MODE_ACTIVE;
                } else {
                    *aMode = ETHXCVR_MODE_DOWN;
                }
                break;
            default:
                retVal = BCM_ERR_NOT_FOUND;
                break;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetLinkState(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LinkStateType *const aLinkState)
{
    uint32_t retVal = BCM_ERR_OK;
    uint16_t link = 0U;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aConfig)
        || (NULL == aLinkState)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->id) {
            case 8U:
                link = (RGMII_REGS->rgmii1_gmii_ctl & IO_RGMII1_GMII_CTL_RGMII_LINK_MASK)
                    >> IO_RGMII1_GMII_CTL_RGMII_LINK_SHIFT;
                break;
            case 5U:
                link = (RGMII_REGS->rgmii2_gmii_ctl & IO_RGMII2_GMII_CTL_RGMII_LINK_MASK)
                    >> IO_RGMII2_GMII_CTL_RGMII_LINK_SHIFT;
                break;
            default:
                break;
        }

        if (0U == link) {
            *aLinkState = ETHXCVR_LINKSTATE_DOWN;
        } else {
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_Command(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetSpeed(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType *const aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t speed;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aSpeed)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->id) {
            case 8U:
                speed = (RGMII_REGS->rgmii1_gmii_ctl & IO_RGMII1_GMII_CTL_RGMII_SPD_MASK)
                         >> IO_RGMII1_GMII_CTL_RGMII_SPD_SHIFT;
                break;
            case 5U:
                speed = (RGMII_REGS->rgmii2_gmii_ctl & IO_RGMII2_GMII_CTL_RGMII_SPD_MASK)
                         >> IO_RGMII2_GMII_CTL_RGMII_SPD_SHIFT;
                break;
            default:
                retVal = BCM_ERR_NOT_FOUND;
                break;
        }
        if (BCM_ERR_OK == retVal) {
            switch (speed) {
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

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetSpeed(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType aSpeed)
{
    int32_t retVal;

    if ((IO_MAX_HW_ID <= aBusIdx)
         || (NULL == aConfig)
         || (ETHXCVR_SPEED_1000MBPS < aSpeed)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = RGMII_IntSetSpeed(aConfig->id, aSpeed);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetDuplexMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aDuplexMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aDuplexMode = ETHXCVR_DUPLEXMODE_FULL;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetDuplexMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (aDuplexMode == ETHXCVR_DUPLEXMODE_HALF) {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetFlowControl(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t  retVal  = BCM_ERR_OK;
    uint16_t txPause = 0U;
    uint16_t rxPause = 0U;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aConfig)
        || (NULL == aFlowControl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->id) {
            case 8U:
                rxPause = (RGMII_REGS->rgmii1_gmii_ctl & IO_RGMII1_GMII_CTL_RGMII_RX_PAUSE_MASK)
                           >> IO_RGMII1_GMII_CTL_RGMII_RX_PAUSE_SHIFT;
                txPause = (RGMII_REGS->rgmii1_gmii_ctl & IO_RGMII1_GMII_CTL_RGMII_TX_PAUSE_MASK)
                           >> IO_RGMII1_GMII_CTL_RGMII_TX_PAUSE_SHIFT;
                break;
            case 5U:
                rxPause = (RGMII_REGS->rgmii2_gmii_ctl & IO_RGMII2_GMII_CTL_RGMII_RX_PAUSE_MASK)
                           >> IO_RGMII2_GMII_CTL_RGMII_RX_PAUSE_SHIFT;
                txPause = (RGMII_REGS->rgmii2_gmii_ctl & IO_RGMII2_GMII_CTL_RGMII_TX_PAUSE_MASK)
                           >> IO_RGMII2_GMII_CTL_RGMII_TX_PAUSE_SHIFT;
                break;
            default:
                break;
        }
        if (rxPause == txPause) {
            if (rxPause == 0U) {
                *aFlowControl = ETHXCVR_FLOWCONTROL_NONE;
            } else {
                *aFlowControl = ETHXCVR_FLOWCONTROL_TXRX;
            }
        } else {
            if (rxPause == 1U) {
                *aFlowControl = ETHXCVR_FLOWCONTROL_RXONLY;
            } else {
                *aFlowControl = ETHXCVR_FLOWCONTROL_TXONLY;
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetFlowControl(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t txPause = 0U;
    uint16_t rxPause = 0U;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aFlowControl) {
            case ETHXCVR_FLOWCONTROL_NONE:
                txPause = 0U;
                rxPause = 0U;
                break;
            case ETHXCVR_FLOWCONTROL_TXRX:
                txPause = 1U;
                rxPause = 1U;
                break;
            case ETHXCVR_FLOWCONTROL_RXONLY:
                txPause = 0U;
                rxPause = 1U;
                break;
            case ETHXCVR_FLOWCONTROL_TXONLY:
                txPause = 1U;
                rxPause = 0U;
                break;
            default:
                break;
        }
        switch (aConfig->id) {
            case 8U:
                RGMII_REGS->rgmii1_gmii_ctl &= ~(IO_RGMII1_GMII_CTL_RGMII_RX_PAUSE_MASK|IO_RGMII1_GMII_CTL_RGMII_TX_PAUSE_MASK);
                RGMII_REGS->rgmii1_gmii_ctl |= (txPause << IO_RGMII1_GMII_CTL_RGMII_TX_PAUSE_SHIFT);
                RGMII_REGS->rgmii1_gmii_ctl |= (rxPause << IO_RGMII1_GMII_CTL_RGMII_RX_PAUSE_SHIFT);
                break;
            case 5U:
                RGMII_REGS->rgmii2_gmii_ctl &= ~(IO_RGMII2_GMII_CTL_RGMII_RX_PAUSE_MASK|IO_RGMII2_GMII_CTL_RGMII_TX_PAUSE_MASK);
                RGMII_REGS->rgmii2_gmii_ctl |= (txPause << IO_RGMII2_GMII_CTL_RGMII_TX_PAUSE_SHIFT);
                RGMII_REGS->rgmii2_gmii_ctl |= (rxPause << IO_RGMII2_GMII_CTL_RGMII_RX_PAUSE_SHIFT);
                break;
            default:
                break;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetSQIValue(uint8_t aBusIdx,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 uint32_t *const aSQIValue)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aSQIValue)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
            retVal = BCM_ERR_UNINIT;
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetParamMode(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_BooleanType aMode)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
         || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetParamMode(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetAutoNegStatus(uint8_t aBusIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aStatus)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetStats(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_StatsType *const aStats)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aStats)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_StateHandler(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  uint32_t *const aIsModeChanged,
                                  ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aIsModeChanged)
        || (NULL == aMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
int32_t RGMII_LinkChangeIndHandler(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_LinkStateType *const aLinkState,
                                   uint32_t *const aIsLinkStateChanged)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((IO_MAX_HW_ID <= aBusIdx)
        || (NULL == aLinkState)
        || (NULL == aIsLinkStateChanged)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((aConfig->id != 5U) && (aConfig->id != 8U)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData[aBusIdx].state[RGMII_ConvertPortToIndex(aConfig->id)]) {
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

const ETHXCVR_FuncTblType RGMII_FuncTbl = {
    .init = RGMII_Init,
    .deinit = RGMII_DeInit,
    .reset = RGMII_Command,
    .setMode = RGMII_SetMode,
    .getMode = RGMII_GetMode,
    .setMasterMode = RGMII_SetParamMode,
    .getMasterMode = RGMII_GetParamMode,
    .getSpeed = RGMII_GetSpeed,
    .setSpeed = RGMII_SetSpeed,
    .getDuplexMode = RGMII_GetDuplexMode,
    .setDuplexMode = RGMII_SetDuplexMode,
    .setFlowControl = RGMII_SetFlowControl,
    .getFlowControl = RGMII_GetFlowControl,
    .getLinkState = RGMII_GetLinkState,
    .getSQIValue = RGMII_GetSQIValue,
    .setLoopbackMode = RGMII_SetParamMode,
    .getLoopbackMode = RGMII_GetParamMode,
    .setJumboMode = RGMII_SetParamMode,
    .getJumboMode = RGMII_GetParamMode,
    .setAutoNegMode = RGMII_SetParamMode,
    .getAutoNegStatus = RGMII_GetAutoNegStatus,
    .restartAutoNeg = RGMII_Command,
    .getStats = RGMII_GetStats,
    .stateHandler = RGMII_StateHandler,
    .updateHWStatus = RGMII_Command,
    .linkChangeIndHandler = RGMII_LinkChangeIndHandler,
    .linkIRQHandler  = RGMII_Command,
};

/* Nothing past this line */
