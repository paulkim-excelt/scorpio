/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_eth_xcvrdrv_brphy_impl BRPHY Driver
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_brphy_impl
    @{

    @file brphy.c
    @brief This file implements BRPHY driver of Ethernet driver.

    @version 1.26 Imported from docx
*/

#include <string.h>
#include <compiler.h>
#include <utils.h>
#include <bcm_err.h>
#include <eth_xcvr.h>
#include <ethxcvr_osil.h>
#include <brphy_osil.h>
#include <brphy_core_rdb.h>
#include <brphy_mii_rdb.h>
#include <brphy_dsp_rdb.h>
#if defined(BRPHY_ENABLE_VEN)
#include <brphy_cl45ven_rdb.h>
#endif
#if defined(BRPHY_ENABLE_CL45DEV1)
#include <brphy_cl45dev1_rdb.h>
#endif

/**
    @name BRPHY driver Design IDs
    @{
    @brief Design IDs for BRPHY driver
*/
#define BRCM_SWDSGN_BRPHY_INIT_PROC                   (0xA3U) /**< @brief #BRPHY_Init */
#define BRCM_SWDSGN_BRPHY_RESET_PROC                  (0xA4U) /**< @brief #BRPHY_Reset */
#define BRCM_SWDSGN_BRPHY_GETSQIVALUE_PROC            (0xA5U) /**< @brief #BRPHY_GetSQIValue */
#define BRCM_SWDSGN_BRPHY_GETDUPLEXMODE_PROC          (0xA6U) /**< @brief #BRPHY_GetDuplexMode */
#define BRCM_SWDSGN_BRPHY_GETSPEED_PROC               (0xA7U) /**< @brief #BRPHY_GetSpeed */
#define BRCM_SWDSGN_BRPHY_GETLINKSTATE_PROC           (0xA8U) /**< @brief #BRPHY_GetLinkState */
#define BRCM_SWDSGN_BRPHY_GETMASTERMODE_PROC          (0xA9U) /**< @brief #BRPHY_GetMasterMode */
#define BRCM_SWDSGN_BRPHY_SETMASTERMODE_PROC          (0xAAU) /**< @brief #BRPHY_SetMasterMode */
#define BRCM_SWDSGN_BRPHY_GETMODE_PROC                (0xABU) /**< @brief #BRPHY_GetMode */
#define BRCM_SWDSGN_BRPHY_SETMODE_PROC                (0xACU) /**< @brief #BRPHY_SetMode */
#define BRCM_SWDSGN_BRPHY_RESET_TIMEOUT_LOOPCNT       (0xADU) /**< @brief #BRPHY_RESET_TIMEOUT_LOOPCNT */
#define BRCM_SWDSGN_BRPHY_POWER_DOWN_BIT_TIMEOUT_CNT  (0xAEU) /**< @brief #BRPHY_POWER_DOWN_BIT_TIMEOUT_CNT */
#define BRCM_SWDSGN_BRPHY_SQI_CFG_VALUE               (0xAFU) /**< @brief #BRPHY_SQI_CFG_VALUE */
#define BRCM_SWDSGN_BRPHY_SQI_LOOP_CNT                (0xB0U) /**< @brief #BRPHY_SQI_LOOP_CNT */
#define BRCM_SWDSGN_BRPHY_SQI_0D                      (0xB1U) /**< @brief #BRPHY_SQI_0D */
#define BRCM_SWDSGN_BRPHY_SQI_0C                      (0xB2U) /**< @brief #BRPHY_SQI_0C */
#define BRCM_SWDSGN_BRPHY_SQI_0B                      (0xB3U) /**< @brief #BRPHY_SQI_0B */
#define BRCM_SWDSGN_BRPHY_SQI_0A                      (0xB4U) /**< @brief #BRPHY_SQI_0A */
#define BRCM_SWDSGN_BRPHY_SQI_1                       (0xB5U) /**< @brief #BRPHY_SQI_1 */
#define BRCM_SWDSGN_BRPHY_SQI_2                       (0xB6U) /**< @brief #BRPHY_SQI_2 */
#define BRCM_SWDSGN_BRPHY_SQI_3                       (0xB7U) /**< @brief #BRPHY_SQI_3 */
#define BRCM_SWDSGN_BRPHY_SQI_4                       (0xB8U) /**< @brief #BRPHY_SQI_4 */
#define BRCM_SWDSGN_BRPHY_SQI_5                       (0xB9U) /**< @brief #BRPHY_SQI_5 */
#define BRCM_SWDSGN_BRPHY_SQI_0D_RANGE                (0xBAU) /**< @brief #BRPHY_SQI_0D_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_0C_RANGE                (0xBBU) /**< @brief #BRPHY_SQI_0C_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_0B_RANGE                (0xBCU) /**< @brief #BRPHY_SQI_0B_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_0A_RANGE                (0xBDU) /**< @brief #BRPHY_SQI_0A_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_1_RANGE                 (0xBEU) /**< @brief #BRPHY_SQI_1_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_2_RANGE                 (0xBFU) /**< @brief #BRPHY_SQI_2_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_3_RANGE                 (0xC0U) /**< @brief #BRPHY_SQI_3_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_4_RANGE                 (0xC1U) /**< @brief #BRPHY_SQI_4_RANGE */
#define BRCM_SWDSGN_BRPHY_SQI_5_RANGE                 (0xC2U) /**< @brief #BRPHY_SQI_5_RANGE */
#define BRCM_SWDSGN_BRPHY_CORE_REGS_GLOBAL            (0xC3U) /**< @brief #BRPHY_CORE_REGS */
#define BRCM_SWDSGN_BRPHY_CL22_IEEE_REGS_GLOBAL       (0xC4U) /**< @brief #BRPHY_CL22_IEEE_REGS */
#define BRCM_SWDSGN_BRPHY_DSP_TAP_CTRL_REGS_GLOBAL    (0xC5U) /**< @brief #BRPHY_DSP_TAP_CTRL_REGS */
#define BRCM_SWDSGN_BRPHY_RWDATA_GLOBAL               (0xC6U) /**< @brief #BRPHY_RWData */
#define BRCM_SWDSGN_BRPHY_CHECKCONFIGPARAMS_PART_PROC (0xC7U) /**< @brief #BRPHY_CheckConfigParams */
#define BRCM_SWDSGN_BRPHY_INTSETMODE_PART_PROC        (0xC9U) /**< @brief #BRPHY_IntSetMode */
#define BRCM_SWDSGN_BRPHY_INTSETMASTERMODE_PART_PROC  (0xCAU) /**< @brief #BRPHY_IntSetMasterMode */
#define BRCM_SWDSGN_BRPHY_LINKIRQHANDLER_PROC         (0xCBU) /**< @brief #BRPHY_LinkIRQHandler */
#define BRCM_SWDSGN_BRPHY_LINKCHANGEINDHANDLER_PROC   (0xCCU) /**< @brief #BRPHY_LinkChangeIndHandler */
#define BRCM_SWDSGN_BRPHY_FUNCTBL_GLOBAL              (0xCDU) /**< @brief #BRPHY_FuncTbl */
#define BRCM_SWDSGN_BRPHY_RWDATA_TYPE                 (0xCEU) /**< @brief #BRPHY_RWDataType */
#define BRCM_SWDSGN_BRPHY_DEINIT_PROC                 (0xCFU) /**< @brief #BRPHY_DeInit */
#define BRCM_SWDSGN_BRPHY_SETSPEED_PROC               (0xD0U) /**< @brief #BRPHY_SetSpeed */
#define BRCM_SWDSGN_BRPHY_GETLOOPBACKMODE_PROC        (0xD1U) /**< @brief #BRPHY_GetLoopbackMode */
#define BRCM_SWDSGN_BRPHY_SETLOOPBACKMODE_PROC        (0xD2U) /**< @brief #BRPHY_SetLoopbackMode */
#define BRCM_SWDSGN_BRPHY_GETJUMBOMODE_PROC           (0xD3U) /**< @brief #BRPHY_GetJumboMode */
#define BRCM_SWDSGN_BRPHY_SETJUMBOMODE_PROC           (0xD4U) /**< @brief #BRPHY_SetJumboMode */
#define BRCM_SWDSGN_BRPHY_SETAUTONEGMODE_PROC         (0xD5U) /**< @brief #BRPHY_SetAutoNegMode */
#define BRCM_SWDSGN_BRPHY_GETAUTONEGSTATUS_PROC       (0xD6U) /**< @brief #BRPHY_GetAutoNegStatus */
#define BRCM_SWDSGN_BRPHY_GETSTATS_PROC               (0xD7U) /**< @brief #BRPHY_GetStats */
#define BRCM_SWDSGN_BRPHY_CL45VEN_CTRL_REGS_GLOBAL    (0xD8U) /**< @brief #BRPHY_CL45VEN_CTRL_REGS */
#define BRCM_SWDSGN_BRPHY_SETDUPLEXMODE_PROC          (0xD9U) /**< @brief #BRPHY_SetDuplexMode */
#define BRCM_SWDSGN_BRPHY_SETFLOWCONTROL_PROC         (0xDAU) /**< @brief #BRPHY_SetFlowControl */
#define BRCM_SWDSGN_BRPHY_GETFLOWCONTROL_PROC         (0xDBU) /**< @brief #BRPHY_GetFlowControl */
#define BRCM_SWDSGN_BRPHY_RESTARTAUTONEG_PROC         (0xDCU) /**< @brief #BRPHY_RestartAutoNeg */
#define BRCM_SWDSGN_BRPHY_STATEHANDLER_PROC           (0xDDU) /**< @brief #BRPHY_StateHandler */
#define BRCM_SWDSGN_BRPHY_UPDATEHWSTATUS_PROC         (0xDEU) /**< @brief #BRPHY_UpdateHWStatus */
#define BRCM_SWDSGN_BRPHY_CL45DEV1_CTRL_REGS_GLOBAL   (0xDFU) /**< @brief #BRPHY_CL45DEV1_CTRL_REGS */
/** @} */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_RESET_TIMEOUT_LOOPCNT         (2000UL)

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_POWER_DOWN_BIT_TIMEOUT_CNT    (1000UL)

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_CFG_VALUE         0xC30U

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_LOOP_CNT          (100UL)

/* M = -AvgdB - 20 */
/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0D                0xDU /* 11% M is between -8.0 and -6.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0C                0xCU /* 22% M is between -6.0 and -4.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0B                0xBU /* 33% M is between -4.0 and -2.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0A                0xAU /* 44% M is between -2.0 and 0.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_1                 0x1U /* 55% M is between 0.0 and 2.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_2                 0x2U /* 66% M is between 2.0 and 4.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_3                 0x3U /* 77% M is between 4.0 and 6.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_4                 0x4U /* 88% M is between 6.0 and 8.0 */

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_5                 0x5U /* 99% M is greater than 8.0 */


/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0D_RANGE(val)     (((val) > 1305U) && ((val) <= 2068U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0C_RANGE(val)     (((val) > 823U)  && ((val) <= 1305U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0B_RANGE(val)     (((val) > 519U)  && ((val) <= 823U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_0A_RANGE(val)     (((val) > 328U)  && ((val) <= 519U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_1_RANGE(val)      (((val) > 206U)  && ((val) <= 328U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_2_RANGE(val)      (((val) > 130U)  && ((val) <= 206U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_3_RANGE(val)      (((val) > 82U)   && ((val) <= 130U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_4_RANGE(val)      (((val) > 52U)   && ((val) <= 82U))

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
#define BRPHY_SQI_5_RANGE(val)      ((val) <= 52U)

/**
    @brief BR_MII block base addresses

    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static BRPHY_MII_RDBType * const BRPHY_CL22_IEEE_REGS[BRPHY_MII_MAX_HW_ID] =
{
#if (BRPHY_MII_MAX_HW_ID == 0UL)
#error "BRPHY_MII_MAX_HW_ID == 0UL"
#endif
    (BRPHY_MII_RDBType *const)BRPHY0_CL22_IEEE_BASE,
#if (BRPHY_MII_MAX_HW_ID > 1UL)
    (BRPHY_MII_RDBType *const)BRPHY1_CL22_IEEE_BASE,
#endif
#if (BRPHY_MII_MAX_HW_ID > 2UL)
    (BRPHY_MII_RDBType *const)BRPHY2_CL22_IEEE_BASE,
#endif
#if (BRPHY_MII_MAX_HW_ID > 3UL)
    (BRPHY_MII_RDBType *const)BRPHY3_CL22_IEEE_BASE,
#endif
#if (BRPHY_MII_MAX_HW_ID > 4UL)
    (BRPHY_MII_RDBType *const)BRPHY4_CL22_IEEE_BASE,
#endif
#if (BRPHY_MII_MAX_HW_ID > 5UL)
#error "BRPHY_MII_MAX_HW_ID > 5UL is not supported"
#endif
};

/**
    @brief GPHY Core block base addresses

    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static BRPHY_CORE_RDBType * const BRPHY_CORE_REGS[BRPHY_CORE_MAX_HW_ID] =
{
#if (BRPHY_CORE_MAX_HW_ID == 0UL)
#error "BRPHY_CORE_MAX_HW_ID == 0UL"
#endif
    (BRPHY_CORE_RDBType *const)BRPHY0_CORE_BASE,
#if (BRPHY_CORE_MAX_HW_ID > 1UL)
    (BRPHY_CORE_RDBType *const)BRPHY1_CORE_BASE,
#endif
#if (BRPHY_CORE_MAX_HW_ID > 2UL)
    (BRPHY_CORE_RDBType *const)BRPHY2_CORE_BASE,
#endif
#if (BRPHY_CORE_MAX_HW_ID > 3UL)
    (BRPHY_CORE_RDBType *const)BRPHY3_CORE_BASE,
#endif
#if (BRPHY_CORE_MAX_HW_ID > 4UL)
    (BRPHY_CORE_RDBType *const)BRPHY4_CORE_BASE,
#endif
#if (BRPHY_CORE_MAX_HW_ID > 5UL)
#error "BRPHY_CORE_MAX_HW_ID > 5UL is not supported"
#endif
};

/**
    @brief DSP TAP block base addresses

    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static BRPHY_DSP_RDBType * const
    BRPHY_DSP_TAP_CTRL_REGS[BRPHY_DSP_MAX_HW_ID] =
{
#if (BRPHY_DSP_MAX_HW_ID == 0UL)
#error "BRPHY_DSP_MAX_HW_ID == 0UL"
#endif
    (BRPHY_DSP_RDBType *const)BRPHY0_DSP_TAP_CTRL_BASE,
#if (BRPHY_DSP_MAX_HW_ID > 1UL)
    (BRPHY_DSP_RDBType *const)BRPHY1_DSP_TAP_CTRL_BASE,
#endif
#if (BRPHY_DSP_MAX_HW_ID > 2UL)
    (BRPHY_DSP_RDBType *const)BRPHY2_DSP_TAP_CTRL_BASE,
#endif
#if (BRPHY_DSP_MAX_HW_ID > 3UL)
    (BRPHY_DSP_RDBType *const)BRPHY3_DSP_TAP_CTRL_BASE,
#endif
#if (BRPHY_DSP_MAX_HW_ID > 4UL)
    (BRPHY_DSP_RDBType *const)BRPHY4_DSP_TAP_CTRL_BASE,
#endif
#if (BRPHY_DSP_MAX_HW_ID > 5UL)
#error "BRPHY_DSP_MAX_HW_ID > 5UL is not supported"
#endif
};

#if defined(BRPHY_ENABLE_VEN)
/**
    @brief CL45 VEN block base addresses

    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static uint16_t * const BRPHY_CL45VEN_CTRL_REGS[BRPHY_CL45VEN_MAX_HW_ID] =
{
#if (BRPHY_CL45VEN_MAX_HW_ID == 0UL)
#error "BRPHY_CL45VEN_MAX_HW_ID == 0UL"
#endif
    (uint16_t *const)BRPHY0_CL45VEN_CTRL_BASE,
#if (BRPHY_CL45VEN_MAX_HW_ID > 1UL)
    (uint16_t *const)BRPHY1_CL45VEN_CTRL_BASE,
#endif
#if (BRPHY_CL45VEN_MAX_HW_ID > 2UL)
    (uint16_t *const)BRPHY2_CL45VEN_CTRL_BASE,
#endif
#if (BRPHY_CL45VEN_MAX_HW_ID > 3UL)
    (uint16_t *const)BRPHY3_CL45VEN_CTRL_BASE,
#endif
#if (BRPHY_CL45VEN_MAX_HW_ID > 4UL)
    (uint16_t *const)BRPHY4_CL45VEN_CTRL_BASE,
#endif
#if (BRPHY_CL45VEN_MAX_HW_ID > 5UL)
#error "BRPHY_CL45VEN_MAX_HW_ID > 5UL is not supported"
#endif
};
#endif

#if defined(BRPHY_ENABLE_CL45DEV1)
/**
    @brief CL45 DEV1 block base addresses

    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static BRPHY_CL45DEV1_RDBType * const BRPHY_CL45DEV1_CTRL_REGS[BRPHY_CL45DEV1_MAX_HW_ID] =
{
#if (BRPHY_CL45DEV1_MAX_HW_ID == 0UL)
#error "BRPHY_CL45DEV1_MAX_HW_ID == 0UL"
#endif
    (BRPHY_CL45DEV1_RDBType *const)BRPHY0_CL45DEV1_CTRL_BASE,
#if (BRPHY_CL45DEV1_MAX_HW_ID > 1UL)
    (BRPHY_CL45DEV1_RDBType *const)BRPHY1_CL45DEV1_CTRL_BASE,
#endif
#if (BRPHY_CL45DEV1_MAX_HW_ID > 2UL)
    (BRPHY_CL45DEV1_RDBType *const)BRPHY2_CL45DEV1_CTRL_BASE,
#endif
#if (BRPHY_CL45DEV1_MAX_HW_ID > 3UL)
    (BRPHY_CL45DEV1_RDBType *const)BRPHY3_CL45DEV1_CTRL_BASE,
#endif
#if (BRPHY_CL45DEV1_MAX_HW_ID > 4UL)
    (BRPHY_CL45DEV1_RDBType *const)BRPHY4_CL45DEV1_CTRL_BASE,
#endif
#if (BRPHY_CL45DEV1_MAX_HW_ID > 5UL)
#error "BRPHY_CL45DEV1_MAX_HW_ID > 5UL is not supported"
#endif
};
#endif

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
typedef struct _BRPHY_RWDataType {
    ETHXCVR_StateType       state;
    ETHXCVR_LinkStateType   linkState;
    uint32_t                isLinkStateChanged;
    uint32_t                linkStateChangeCount;
    uint32_t                isAutoNegStarted;
    uint16_t                intStatus;
} BRPHY_RWDataType;

/**
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
*/
static BRPHY_RWDataType
    BRPHY_RWData[BRPHY_CORE_MAX_HW_ID] COMP_SECTION(".data.drivers") =
        {
        #if (BRPHY_CORE_MAX_HW_ID == 0UL)
        #error "BRPHY_CORE_MAX_HW_ID == 0UL"
        #endif
            {.state = ETHXCVR_STATE_UNINIT,
             .isLinkStateChanged = FALSE,
             .linkStateChangeCount = 0UL,
             .isAutoNegStarted = FALSE,
             .intStatus = 0U},
        #if (BRPHY_CORE_MAX_HW_ID > 1UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isLinkStateChanged = FALSE,
             .linkStateChangeCount = 0UL,
             .isAutoNegStarted = FALSE,
             .intStatus = 0U},
        #endif
        #if (BRPHY_CORE_MAX_HW_ID > 2UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isLinkStateChanged = FALSE,
             .linkStateChangeCount = 0UL,
             .isAutoNegStarted = FALSE,
             .intStatus = 0U},
        #endif
        #if (BRPHY_CORE_MAX_HW_ID > 3UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isLinkStateChanged = FALSE,
             .linkStateChangeCount = 0UL,
             .isAutoNegStarted = FALSE,
             .intStatus = 0U},
        #endif
        #if (BRPHY_CORE_MAX_HW_ID > 4UL)
            {.state = ETHXCVR_STATE_UNINIT,
             .isLinkStateChanged = FALSE,
             .linkStateChangeCount = 0UL,
             .isAutoNegStarted = FALSE,
             .intStatus = 0U},
        #endif
        #if (BRPHY_CORE_MAX_HW_ID > 5UL)
        #error "BRPHY_CORE_MAX_HW_ID > 5UL not supported"
        #endif
        };


/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static inline int32_t BRPHY_CheckConfigParams(const ETHXCVR_PortConfigType *const aCfg)
{

    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    /* check supported speeds */
    if (aCfg->speed != ETHXCVR_SPEED_100MBPS) {
        goto err;
    }

    if (aCfg->duplex != ETHXCVR_DUPLEXMODE_FULL) {
        goto err;
    }

    retVal = BCM_ERR_OK;

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetLinkState(uint8_t aPhyIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LinkStateType *const aLinkState)
{
    uint16_t reg;
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx)
            || (NULL == aLinkState)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        reg = BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_stat;
        reg = BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_stat;

        if ((reg & BRPHY_MII_MII_STAT_LNK_STAT_MASK) == BRPHY_MII_MII_STAT_LNK_STAT_MASK) {
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
        } else {
            *aLinkState = ETHXCVR_LINKSTATE_DOWN;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_Reset(uint8_t aPhyIdx,
                           const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t loopCnt;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl |= BRPHY_MII_MII_CTRL_RESET_MASK;
        /* Wait for Reset bit to auto clear */
        loopCnt = BRPHY_RESET_TIMEOUT_LOOPCNT;
        while (((BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl
                        & BRPHY_MII_MII_CTRL_RESET_MASK) == BRPHY_MII_MII_CTRL_RESET_MASK)
                && (loopCnt > 0UL)) {
            loopCnt--;
        }
        if (0UL == loopCnt) {
            retVal = BCM_ERR_TIME_OUT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_IntSetMode(uint8_t aPhyIdx,
                          ETHXCVR_ModeType aMode)
{
    uint32_t loopCnt = 0UL;
    uint16_t regVal;
    int32_t retVal = BCM_ERR_OK;

    if (ETHXCVR_MODE_ACTIVE == aMode) {
        regVal = BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl;
        regVal &= (~BRPHY_MII_MII_CTRL_POWER_DOWN_MASK);
        BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl = regVal;
        do {
            regVal = BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl;
            loopCnt++;
        } while (((regVal & BRPHY_MII_MII_CTRL_POWER_DOWN_MASK) != 0U)
                && (BRPHY_POWER_DOWN_BIT_TIMEOUT_CNT > loopCnt));
    }

    if (ETHXCVR_MODE_DOWN == aMode) {
        regVal = BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl;
        regVal |= (BRPHY_MII_MII_CTRL_POWER_DOWN_MASK);
        BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl = regVal;
        do {
            regVal = BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl;
            loopCnt++;
        } while (((regVal & BRPHY_MII_MII_CTRL_POWER_DOWN_MASK) == 0U)
                && (BRPHY_POWER_DOWN_BIT_TIMEOUT_CNT > loopCnt));
    }

    if (BRPHY_POWER_DOWN_BIT_TIMEOUT_CNT == loopCnt) {
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetMode(uint8_t aPhyIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BRPHY_IntSetMode(aPhyIdx, aMode);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetMode(uint8_t aPhyIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t mask = (BRPHY_MII_MII_CTRL_POWER_DOWN_MASK | BRPHY_MII_MII_CTRL_RESET_MASK);

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if ((BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl & mask) == 0U) {
            *aMode = ETHXCVR_MODE_ACTIVE;
        } else {
            *aMode = ETHXCVR_MODE_DOWN;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static void BRPHY_IntSetMasterMode(uint8_t aPhyIdx,
                                   ETHXCVR_BooleanType aMasterMode)
{
    if (ETHXCVR_BOOLEAN_TRUE == aMasterMode) {
#if defined(BRPHY_ENABLE_VEN)
        /* BRPHY_CL45VEN_CTRL_REGS base address + 0x102 register */
        (BRPHY_CL45VEN_CTRL_REGS[aPhyIdx])[129] =
            (BRPHY_CL45VEN_0x102REG_MANUAL_FORCE_MASK
             | BRPHY_CL45VEN_0x102REG_MASTER_MASK
             | BRPHY_CL45VEN_0x102REG_100MBPS_MASK);
#endif
        BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl |=
            BRPHY_MII_MII_CTRL_MASTER_MODE_MASK;
    } else {
#if defined(BRPHY_ENABLE_VEN)
        /* BRPHY_CL45VEN_CTRL_REGS base address + 0x102 register */
        (BRPHY_CL45VEN_CTRL_REGS[aPhyIdx])[129] =
            (BRPHY_CL45VEN_0x102REG_MANUAL_FORCE_MASK
             | BRPHY_CL45VEN_0x102REG_100MBPS_MASK);
        (BRPHY_CL45VEN_CTRL_REGS[aPhyIdx])[129] &=
            ~BRPHY_CL45VEN_0x102REG_MASTER_MASK;
#endif
        BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl &=
            ~BRPHY_MII_MII_CTRL_MASTER_MODE_MASK;
    }
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetMasterMode(uint8_t aPhyIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_BooleanType aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        BRPHY_IntSetMasterMode(aPhyIdx, aMasterMode);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetMasterMode(uint8_t aPhyIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aMasterMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#if defined(BRPHY_ENABLE_VEN)
        /* BRPHY_CL45VEN_CTRL_REGS base address + 0x102 register */
        if (((BRPHY_CL45VEN_CTRL_REGS[aPhyIdx])[129]
                & BRPHY_CL45VEN_0x102REG_MASTER_MASK)
            == BRPHY_CL45VEN_0x102REG_MASTER_MASK) {
            *aMasterMode = ETHXCVR_BOOLEAN_TRUE;
        } else {
            *aMasterMode = ETHXCVR_BOOLEAN_FALSE;
        }
#else
        if ((BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl
                    & BRPHY_MII_MII_CTRL_MASTER_MODE_MASK)
                == BRPHY_MII_MII_CTRL_MASTER_MODE_MASK) {
            *aMasterMode = ETHXCVR_BOOLEAN_TRUE;
        } else {
            *aMasterMode = ETHXCVR_BOOLEAN_FALSE;
        }
#endif
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetSpeed(uint8_t aPhyIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType aSpeed)
{
    int32_t retVal;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (ETHXCVR_SPEED_100MBPS != aSpeed) {
            retVal = BCM_ERR_NOSUPPORT;
        } else {
            retVal = BCM_ERR_OK;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetSpeed(uint8_t aPhyIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType *const aSpeed)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aSpeed)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else if ((BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl
                & BRPHY_MII_MII_CTRL_SPEED_SELECTION_MASK) != 0UL) {
        /* For Port 4 in 100Tx mode  */
        *aSpeed = ETHXCVR_SPEED_100MBPS;
    } else {
        *aSpeed = ETHXCVR_SPEED_100MBPS;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetDuplexMode(uint8_t aPhyIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aDuplexMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aDuplexMode = ETHXCVR_DUPLEXMODE_FULL;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetDuplexMode(uint8_t aPhyIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetFlowControl(uint8_t aPhyIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aFlowControl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aFlowControl = ETHXCVR_FLOWCONTROL_NONE;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetFlowControl(uint8_t aPhyIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}
/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetSQIValue(uint8_t aPhyIdx,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 uint32_t *const aSQIValue)
{
    uint16_t regval_base;
    uint16_t regval;
    uint32_t sum = 0UL;
    uint32_t avg;
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aSQIValue)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        /* Read the original value */
        regval_base = BRPHY_CORE_REGS[aPhyIdx]->shd18_000;

        /* Write the config value */
        BRPHY_CORE_REGS[aPhyIdx]->shd18_000 = BRPHY_SQI_CFG_VALUE;

        for(i = 0UL; i < BRPHY_SQI_LOOP_CNT; i++) {
            regval = BRPHY_DSP_TAP_CTRL_REGS[aPhyIdx]->tap2_c0;
            sum += regval;
        }
        avg = sum / BRPHY_SQI_LOOP_CNT;

        /* Restore the value */
        BRPHY_CORE_REGS[aPhyIdx]->shd18_000 = regval_base;

        /* Now the average SQI value is found, */
        /* the quality indicator needs to be determined */
        if (BRPHY_SQI_0D_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_0D;
        } else if (BRPHY_SQI_0C_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_0C;
        } else if (BRPHY_SQI_0B_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_0B;
        } else if (BRPHY_SQI_0A_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_0A;
        } else if (BRPHY_SQI_1_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_1;
        } else if (BRPHY_SQI_2_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_2;
        } else if (BRPHY_SQI_3_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_3;
        } else if (BRPHY_SQI_4_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_4;
        } else if (BRPHY_SQI_5_RANGE(avg)) {
            *aSQIValue = BRPHY_SQI_5;
        } else {
            *aSQIValue = 0UL;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_Init(uint8_t aPhyIdx,
                          const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;


    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = BRPHY_CheckConfigParams(aConfig);
    if (retVal != BCM_ERR_OK) {
        goto err;
    }

    if (ETHXCVR_STATE_UNINIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    BRPHY_RWData[aPhyIdx].state = ETHXCVR_STATE_INIT;
    /* Enable the link status change interrupt. The following is not a typo      */
    /* RDB does not define any bitfields for the mask register, they are assumed */
    /* to be the same as status (BASE1A) register                                */
    BRPHY_CORE_REGS[aPhyIdx]->base1b &= ~BRPHY_CORE_BASE1A_LINK_STATUS_CHANGE_MASK;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_DeInit(uint8_t aPhyIdx,
                            const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    BRPHY_RWData[aPhyIdx].state = ETHXCVR_STATE_UNINIT;
    /* Disable the link status change interrupt. The following is not a typo      */
    /* RDB does not define any bitfields for the mask register, they are assumed */
    /* to be the same as status (BASE1A) register                                */
    BRPHY_CORE_REGS[aPhyIdx]->base1b |= BRPHY_CORE_BASE1A_LINK_STATUS_CHANGE_MASK;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetLoopbackMode(uint8_t aPhyIdx,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
#if defined(BRPHY_ENABLE_CL45DEV1)
            BRPHY_CL45DEV1_CTRL_REGS[aPhyIdx]->ctl1 |= BRPHY_CL45DEV1_PMD_IEEE_CTL1_LPBK_MASK;
#else
            BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl |=
                BRPHY_MII_MII_CTRL_LOOPBACK_MASK;
#endif
        } else if (ETHXCVR_BOOLEAN_FALSE == aMode) {
#if defined(BRPHY_ENABLE_CL45DEV1)
            BRPHY_CL45DEV1_CTRL_REGS[aPhyIdx]->ctl1 &= ~BRPHY_CL45DEV1_PMD_IEEE_CTL1_LPBK_MASK;
#else
            BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl &=
                ~BRPHY_MII_MII_CTRL_LOOPBACK_MASK;
#endif
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetLoopbackMode(uint8_t aPhyIdx,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#if defined(BRPHY_ENABLE_CL45DEV1)
        if ((BRPHY_CL45DEV1_CTRL_REGS[aPhyIdx]->ctl1
            & BRPHY_CL45DEV1_PMD_IEEE_CTL1_LPBK_MASK)
            == BRPHY_CL45DEV1_PMD_IEEE_CTL1_LPBK_MASK) {
            *aMode = ETHXCVR_BOOLEAN_TRUE;
        } else {
            *aMode = ETHXCVR_BOOLEAN_FALSE;
        }
#else
        if ((BRPHY_CL22_IEEE_REGS[aPhyIdx]->mii_ctrl
                    & BRPHY_MII_MII_CTRL_LOOPBACK_MASK)
                == BRPHY_MII_MII_CTRL_LOOPBACK_MASK) {
            *aMode = ETHXCVR_BOOLEAN_TRUE;
        } else {
            *aMode = ETHXCVR_BOOLEAN_FALSE;
        }
#endif

    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetJumboMode(uint8_t aPhyIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_BooleanType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
            /* Enable extended frame length */
            BRPHY_CORE_REGS[aPhyIdx]->shd18_000 |=
                BRPHY_CORE_SHD18_000_EXT_PKT_LEN_MASK;
            /* Enable DSP clock */
            BRPHY_CORE_REGS[aPhyIdx]->shd18_000 |=
                BRPHY_CORE_SHD18_000_SM_DSP_CLK_EN_MASK;
        } else if (ETHXCVR_BOOLEAN_FALSE == aMode) {
            /* Disable extended frame length */
            BRPHY_CORE_REGS[aPhyIdx]->shd18_000 &=
                ~BRPHY_CORE_SHD18_000_EXT_PKT_LEN_MASK;
            /* Disable DSP clock */
            BRPHY_CORE_REGS[aPhyIdx]->shd18_000 &=
                ~BRPHY_CORE_SHD18_000_SM_DSP_CLK_EN_MASK;
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetJumboMode(uint8_t aPhyIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t regValMask;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        regValMask = (BRPHY_CORE_SHD18_000_EXT_PKT_LEN_MASK
                    | BRPHY_CORE_SHD18_000_SM_DSP_CLK_EN_MASK);
        if ((BRPHY_CORE_REGS[aPhyIdx]->shd18_000 & regValMask) == regValMask) {
            *aMode = ETHXCVR_BOOLEAN_TRUE;
        } else {
            *aMode = ETHXCVR_BOOLEAN_FALSE;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_SetAutoNegMode(uint8_t aPhyIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_BooleanType aAutonegMode)
{
    int32_t retVal;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_RestartAutoNeg(uint8_t aPhyIdx,
                                    const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetAutoNegStatus(uint8_t aPhyIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aStatus)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aStatus = ETHXCVR_AUTONEGSTATUS_NO_ABILITY;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_GetStats(uint8_t aPhyIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_StatsType *const aStats)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx) || (NULL == aStats)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        aStats->linkStateChangeCount =
            BRPHY_RWData[aPhyIdx].linkStateChangeCount;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_StateHandler(uint8_t aPhyIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  uint32_t *const aIsModeChanged,
                                  ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((BRPHY_CORE_MAX_HW_ID <= aPhyIdx)
            || (NULL == aConfig)
            || (NULL == aIsModeChanged)
            || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_UpdateHWStatus(uint8_t aPhyIdx,
                                    const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY_CORE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_LinkIRQHandler(uint8_t aPhyIdx,
                                    const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        BRPHY_RWData[aPhyIdx].intStatus |= BRPHY_CORE_REGS[aPhyIdx]->base1a;
        if ((BRPHY_RWData[aPhyIdx].intStatus & BRPHY_CORE_BASE1A_LINK_STATUS_CHANGE_MASK)
                == BRPHY_CORE_BASE1A_LINK_STATUS_CHANGE_MASK) {
            BRPHY_RWData[aPhyIdx].intStatus = 0U;
            retVal = BRPHY_GetLinkState(aPhyIdx, aConfig,
                    &(BRPHY_RWData[aPhyIdx].linkState));
            if (BCM_ERR_OK == retVal) {
                BRPHY_RWData[aPhyIdx].isLinkStateChanged = TRUE;
                (BRPHY_RWData[aPhyIdx].linkStateChangeCount)++;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
static int32_t BRPHY_LinkChangeIndHandler(uint8_t aPhyIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_LinkStateType *const aLinkState,
                                   uint32_t *const aIsLinkStateChanged)
{
    int32_t retVal = BCM_ERR_OK;

    if (ETHXCVR_STATE_INIT != BRPHY_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (TRUE == BRPHY_RWData[aPhyIdx].isLinkStateChanged) {
            BRPHY_RWData[aPhyIdx].isLinkStateChanged = FALSE;
            *aLinkState = BRPHY_RWData[aPhyIdx].linkState;
            *aIsLinkStateChanged = TRUE;
        } else {
            *aIsLinkStateChanged = FALSE;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY_FUNCTBL
*/
const ETHXCVR_FuncTblType BRPHY_FuncTbl = {
    .init = BRPHY_Init,
    .deinit = BRPHY_DeInit,
    .reset = BRPHY_Reset,
    .setMode = BRPHY_SetMode,
    .getMode = BRPHY_GetMode,
    .setMasterMode = BRPHY_SetMasterMode,
    .getMasterMode = BRPHY_GetMasterMode,
    .getSpeed = BRPHY_GetSpeed,
    .setSpeed = BRPHY_SetSpeed,
    .getDuplexMode = BRPHY_GetDuplexMode,
    .setDuplexMode = BRPHY_SetDuplexMode,
    .setFlowControl = BRPHY_SetFlowControl,
    .getFlowControl = BRPHY_GetFlowControl,
    .getLinkState = BRPHY_GetLinkState,
    .getSQIValue = BRPHY_GetSQIValue,
    .setLoopbackMode = BRPHY_SetLoopbackMode,
    .getLoopbackMode = BRPHY_GetLoopbackMode,
    .setJumboMode = BRPHY_SetJumboMode,
    .getJumboMode = BRPHY_GetJumboMode,
    .setAutoNegMode = BRPHY_SetAutoNegMode,
    .getAutoNegStatus = BRPHY_GetAutoNegStatus,
    .restartAutoNeg = BRPHY_RestartAutoNeg,
    .getStats = BRPHY_GetStats,
    .stateHandler = BRPHY_StateHandler,
    .updateHWStatus = BRPHY_UpdateHWStatus,
    .linkChangeIndHandler = BRPHY_LinkChangeIndHandler,
    .linkIRQHandler  = BRPHY_LinkIRQHandler,
};

/** @} */
