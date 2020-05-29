/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

#include <inttypes.h>
#include <bcm_err.h>
#include <mcu.h>
#include <mcu_ext.h>
#include <mcu_osil.h>
#include <chip_config.h>
#include <otp.h>

/**
    @addtogroup grp_mcudrv_impl
    @{

    @file mcu_ext.c
    @brief MCU Extension implementation
    This header file contains the implementation of MCU extension

    @version 0.86 Imported from docx
*/

/**
    @name MCU Design IDs
    @{
    @brief Design IDs for MCU
*/
#define BRCM_SWDSGN_MCU_GETSWITCHPORT2TIMEFIFOMAP_PROC  (0xA0U) /**< @brief #MCU_GetSwitchPort2TimeFifoMap */
#define BRCM_SWDSGN_MCU_ENABLESWITCHCPUPORT_PROC        (0xA1U) /**< @brief #MCU_EnableSwitchCPUPort */
#define BRCM_SWDSGN_MCU_DISABLESWITCHCPUPORT_PROC       (0xA2U) /**< @brief #MCU_DisableSwitchCPUPort */
#define BRCM_SWDSGN_MCU_GETSTACKINGINFO_PROC            (0xA3U) /**< @brief #MCU_GetExtendedInfo */
#define BRCM_SWDSGN_MCU_STACKINGBITINFO_TYPE            (0xA4U) /**< @brief #MCU_StackingBitInfoType */
#define BRCM_SWDSGN_MCU_STACKINGINFO_OTP_ADDR_MACRO     (0xA5U) /**< @brief #MCU_STACKINGINFO_OTP_ADDR */
/** @} */

/**
   @name Stacking Bit Info
   @{
   @trace #BRCM_SWARCH_MCU_GETEXTENDEDINFO_PROC
   @trace #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
 */
typedef uint32_t MCU_StackingBitInfoType;
#define MCU_STACKINGBITINFO_EN_MASK            (0x1UL)     /**< Stacking enable mask            */
#define MCU_STACKINGBITINFO_EN_SHIFT           (0UL)       /**< Stacking enable shift           */
#define MCU_STACKINGBITINFO_MST_SLV_MASK       (0x30UL)    /**< Stacking master slave id mask   */
#define MCU_STACKINGBITINFO_MST_SLV_SHIFT      (4UL)       /**< Stacking master slave id shift  */
#define MCU_STACKINGBITINFO_PORT_0_MASK        (0x780UL)   /**< Stacking Port 0 mask Bit[10:7]  */
#define MCU_STACKINGBITINFO_PORT_0_SHIFT       (7UL)       /**< Stacking Port 0 Shift Bit[10:7] */
#define MCU_STACKINGBITINFO_PORT_1_MASK        (0x7800UL)  /**< Stacking Port 1 mask Bit[14:11] */
#define MCU_STACKINGBITINFO_PORT_1_SHIFT       (11UL)      /**< Stacking Port 1 mask Bit[14:11] */
/** @} */

/**
   @brief OTP Address of Stacking info bits.

   @trace #BRCM_SWARCH_MCU_GETEXTENDEDINFO_PROC
   @trace #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
 */
#define MCU_STACKINGINFO_OTP_ADDR      (13UL)      /**< Stacking Info OTP Address       */

/** @} */

/**
    @addtogroup grp_mcudrv_ifc
    @{
*/

/**
    @trace  #BRCM_SWARCH_MCU_GETSWITCHPORT2TIMEFIFOMAP_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
int32_t MCU_GetSwitchPort2TimeFifoMap(uint32_t *const aPort2TimeFifoMap)
{
    MCU_IOType mcuIO;

    mcuIO.swtPort2TimeFifoMap = aPort2TimeFifoMap;

    return MCU_SysCmdReq(MCU_CMD_GET_SWT_PORT_2_TIME_FIFO_MAP, &mcuIO);
}

/**
    @trace  #BRCM_SWARCH_MCU_ENABLESWITCHCPUPORT_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
int32_t MCU_EnableSwitchCPUPort(void)
{
    MCU_IOType mcuIO;

    return MCU_SysCmdReq(MCU_CMD_ENABLE_SWT_CPU_PORT, &mcuIO);
}

/**
    @trace  #BRCM_SWARCH_MCU_DISABLESWITCHCPUPORT_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
int32_t MCU_DisableSwitchCPUPort(void)
{
    MCU_IOType mcuIO;

    return MCU_SysCmdReq(MCU_CMD_DISABLE_SWT_CPU_PORT, &mcuIO);
}

/**
    @trace  #BRCM_SWARCH_MCU_GETEXTENDEDINFO_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
int32_t MCU_GetExtendedInfo(MCU_ExtendedInfoType * aStackingInfo)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    uint32_t stackingData = 0;

    if (NULL == aStackingInfo) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#ifdef ENABLE_STACKING_OTP
        /* TODO:
           Eventually we will use OTP for stacking information. We need to finalise OTP address
           and programming method across the chipsets and till then Spare Reg will be used */
        retVal = OTP_Read(0UL, MCU_STACKINGINFO_OTP_ADDR, &stackingData);
#else /* ENABLE_STACKING_OTP */
        MCU_IOType mcuIO;
        retVal = MCU_SysCmdReq(MCU_CMD_GET_STACKING_INFO, &mcuIO);
        stackingData = mcuIO.stackingInfo; /* If retVal is not OK, stackingData is invalid and will not be used */
#endif /* ENABLE_STACKING_OTP */
    }

    if (BCM_ERR_OK == retVal) {
#ifdef __BCM89564G__
        if (0 == stackingData) {
            stackingData = MCU_STACKINGBITINFO_EN_MASK;
            stackingData |= (MCU_DEVICE_MASTER << MCU_STACKINGBITINFO_MST_SLV_SHIFT);
            stackingData |= (0x6 << MCU_STACKINGBITINFO_PORT_0_SHIFT);
        }
#endif
        aStackingInfo->stackingEn = (stackingData & MCU_STACKINGBITINFO_EN_MASK) >> MCU_STACKINGBITINFO_EN_SHIFT;
        aStackingInfo->mstSlvMode = (stackingData & MCU_STACKINGBITINFO_MST_SLV_MASK) >> MCU_STACKINGBITINFO_MST_SLV_SHIFT;
        aStackingInfo->stackPort0 = (stackingData & MCU_STACKINGBITINFO_PORT_0_MASK) >> MCU_STACKINGBITINFO_PORT_0_SHIFT;
        aStackingInfo->stackPort1 = (stackingData & MCU_STACKINGBITINFO_PORT_1_MASK) >> MCU_STACKINGBITINFO_PORT_1_SHIFT;

        /* Valid OTP Combinations                                            */
        /* ------------------------------------------------------------------*/
        /*  ID      | EN  | MST-SLV | Port-0 | Port-1 |   Details            */
        /* ------------------------------------------------------------------*/
        /*  MASTER  |  0  |    x    |   x    |    x   |   Stacking Disabled  */
        /*  MASTER  |  1  |    0    |   5    |    0   |   Single Slave       */
        /*  MASTER  |  1  |    0    |   6    |    0   |   Single Slave       */
        /*  MASTER  |  1  |    0    |   5    |    6   |   Dual Slave         */
        /*  MASTER  |  1  |    0    |   6    |    5   |   Dual Slave         */
        /* ------------------------------------------------------------------*/
        /*  SLAVE-1 |  1  |    1    |   8    |    0   |   Single Slave       */
        /*  SLAVE-1 |  1  |    1    |   8    |    5   |   Dual Slave         */
        /*  SLAVE-1 |  1  |    1    |   8    |    6   |   Dual Slave         */
        /* ------------------------------------------------------------------*/
        /*  SLAVE-2 |  2  |    1    |   8    |    5   |   Dual Slave         */
        /*  SLAVE-2 |  2  |    1    |   8    |    6   |   Dual Slave         */
        /* ------------------------------------------------------------------*/

        /* Check If Stacking is enabled */
        if (1UL == aStackingInfo->stackingEn) {
            /* Check Master Configuration */
            if (MCU_DEVICE_MASTER == aStackingInfo->mstSlvMode) {
                if (5UL == aStackingInfo->stackPort0) {
                    if ((6UL != aStackingInfo->stackPort1) &&
                            (0UL != aStackingInfo->stackPort1)) {
                        /* stackPort1 is other than 0 or 6 */
                        retVal = BCM_ERR_NOSUPPORT;
                    }
                } else if (6UL == aStackingInfo->stackPort0) {
                    if ((5UL != aStackingInfo->stackPort1) &&
                            (0UL != aStackingInfo->stackPort1)) {
                        /* stackPort1 is other than 0 or 5 */
                        retVal = BCM_ERR_NOSUPPORT;
                    }
                } else {
                    /* stackPort0 other than 5 or 6 */
                    retVal = BCM_ERR_NOSUPPORT;
                }
            } else if (MCU_DEVICE_SLAVE_1 == aStackingInfo->mstSlvMode) {
                if ((8UL != aStackingInfo->stackPort0) ||
                        ((5UL != aStackingInfo->stackPort1) &&
                         (6UL != aStackingInfo->stackPort1) &&
                         (0UL != aStackingInfo->stackPort1))) {
                    retVal = BCM_ERR_NOSUPPORT;
                }
            } else if (MCU_DEVICE_SLAVE_2 == aStackingInfo->mstSlvMode) {
                if ((8UL != aStackingInfo->stackPort0) ||
                        ((5UL != aStackingInfo->stackPort1) &&
                         (6UL != aStackingInfo->stackPort1))){
                    retVal = BCM_ERR_NOSUPPORT;
                }
            } else {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    }

    return retVal;
}

/** @} */
