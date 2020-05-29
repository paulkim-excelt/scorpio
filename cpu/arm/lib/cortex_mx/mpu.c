/*****************************************************************************
Copyright 2016-2019 Broadcom Limited.  All rights reserved.
This program is the proprietary software of Broadcom Limited and/or its
licensors, and may only be used, duplicated, modified or distributed pursuant
to the terms and conditions of a separate, written license agreement executed
between you and Broadcom (an "Authorized License").

Except as set forth in an Authorized License, Broadcom grants no
license(express or implied), right to use, or waiver of any kind with respect
to the Software, and Broadcom expressly reserves all rights in and to the
Software and all intellectual property rights therein.  IF YOU HAVE NO
AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,
1. This program, including its structure, sequence and organization,
constitutes the valuable trade secrets of Broadcom, and you shall use all
reasonable efforts to protect the confidentiality thereof, and to use this
information only in connection with your use of Broadcom integrated circuit
products.

2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND
WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE,
MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF
VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
PERFORMANCE OF THE SOFTWARE.

3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
ANY LIMITED REMEDY.
***************************************************************************/

#include <stdint.h>
#include <cortex_mx.h>

/**
    @defgroup grp_cortex_mx_mpu_impl MPU related implementation
    @ingroup grp_cortex_mx

    @addtogroup grp_cortex_mx_mpu_impl
    @{

    @file cpu/arm/cortex_mx/mpu.c
    @brief Implementation of MPU related functionality for Cortex-M

    This source file contains the implementation of MPU related
    functions for Cortex-M Processors

    @version 0.1 Initial version
*/

/**
    @name Cortex-M MPU Design IDs
    @{
    @brief Design IDs for Cortex-M MPU related functionality
*/
#define BRCM_SWDSGN_CORTEX_MX_MPU_TYPE         (0x80U)   /**< @brief #CORTEX_MX_MPUType              */
#define BRCM_SWDSGN_CORTEX_MX_MPU_BASE_MACRO   (0x81U)   /**< @brief #CORTEX_MX_MPU_BASE             */
#define BRCM_SWDSGN_CORTEX_MX_MPU_MACRO        (0x82U)   /**< @brief #CORTEX_MX_MPU_CTRL_ENABLE_MASK */
#define BRCM_SWDSGN_CORTEX_MX_MPU_DISABLE_PROC (0x83U)   /**< @brief #CORTEX_MX_MPUDisable           */
#define BRCM_SWDSGN_CORTEX_MX_MPU_ENABLE_PROC  (0x84U)   /**< @brief #CORTEX_MX_MPUEnable            */
#define BRCM_SWDSGN_CORTEX_MX_MPU_CONFIG_PROC  (0x85U)   /**< @brief #CORTEX_MX_MPUConfigure         */
#define BRCM_SWDSGN_CORTEX_MX_GET_MPUGETSIZEATTRIB_PROC      (0x86U)   /**< @brief #CORTEX_MX_MPUGetSizeAttrib */
#define BRCM_SWDSGN_CORTEX_MX_MPUSUBREGIONDISABLEATTRIB_PROC (0x87U)   /**< @brief #CORTEX_MX_MPUSubRegionDisableAttrib */
/** @} */

/**
    @brief Cortex-M MPU register structure

    @trace #BRCM_SWARCH_CORTEX_MX_MPU_DISABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_ENABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_CONFIG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
 */
typedef volatile struct _CORTEX_MX_MPUType {
	uint32_t TYPE;
	uint32_t CTRL;
	uint32_t RNR;
	uint32_t RBAR;
	uint32_t RASR;
	uint32_t RBARA1;
	uint32_t RASRA1;
	uint32_t RBARA2;
	uint32_t RASRA2;
	uint32_t RBARA3;
	uint32_t RASRA3;
} CORTEX_MX_MPUType;

/**
    @brief Base address for Cortex-M MPU

    @trace #BRCM_SWARCH_CORTEX_MX_MPU_DISABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_ENABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_CONFIG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_MPU_BASE  (0xE000ED90UL)

/**
    @name Macros for Cortex-M MPU
    @{
    @brief Bitmask macros for Cortex-M MPU registers

    MPU region attributes
    [21..19] 18 17 16
    TEX      S  C  B

    @trace #BRCM_SWARCH_CORTEX_MX_MPU_DISABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_ENABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_CONFIG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_MPU_CTRL_ENABLE_MASK        (0x1UL)
#define CORTEX_MX_MPU_CTRL_ENABLE_SHIFT       (0UL)
#define CORTEX_MX_MPU_CTRL_HFNMIENA_MASK      (0x2UL)
#define CORTEX_MX_MPU_CTRL_HFNMIENA_SHIFT     (1UL)
#define CORTEX_MX_MPU_CTRL_PRIVDEFENA_MASK    (0x4UL)
#define CORTEX_MX_MPU_CTRL_PRIVDEFENA_SHIFT   (2UL)
#define CORTEX_MX_MPU_RNR_REGION_MASK         (0xFFUL)
#define CORTEX_MX_MPU_RBAR_REGION_MASK        (0x0FUL)
#define CORTEX_MX_MPU_RBAR_REGION_SHIFT       (0UL)
#define CORTEX_MX_MPU_RBAR_VALID_MASK         (0x10UL)
#define CORTEX_MX_MPU_RBAR_VALID_SHIFT        (4UL)
#define CORTEX_MX_MPU_RBAR_ADDR_MASK          (0xFFFFFFE0UL)
#define CORTEX_MX_MPU_RBAR_ADDR_SHIFT         (5UL)
#define CORTEX_MX_MPU_RASR_ENABLE_MASK        (0x1UL)
#define CORTEX_MX_MPU_RASR_ENABLE_SHIFT       (0UL)
#define CORTEX_MX_MPU_RASR_SIZE_MASK          (0x3EUL)
#define CORTEX_MX_MPU_RASR_SIZE_SHIFT         (1UL)
#define CORTEX_MX_MPU_RASR_SDR_MASK           (0xFF00UL)
#define CORTEX_MX_MPU_RASR_SDR_SHIFT          (8UL)
#define CORTEX_MX_MPU_RASR_ATTRS_MASK         (0xFFFF0000UL)
#define CORTEX_MX_MPU_RASR_ATTRS_SHIFT        (16UL)
#define CORTEX_MX_MPU_REGION_ENABLED  	      (0x1UL)
#define CORTEX_MX_MPU_REGION_VALID    	      (0x10UL)
/** @} */

/** @} */

/**
    @addtogroup grp_cortex_mx
    @{
*/

/**
    @trace #BRCM_SWARCH_CORTEX_MX_GET_MPUGETSIZEATTRIB_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
static uint32_t CORTEX_MX_MPUSubRegionDisableAttrib(uint32_t aRegionSize, uint32_t aActualSize)
{
    uint32_t attrib = 0UL;

    if (0UL == aRegionSize) {
        goto done;
    }

    switch (((aRegionSize - aActualSize) * 8UL) / aRegionSize) {
    case 0UL:
        break;
    case 1UL:
        attrib |= CORTEX_MX_MPU_ATTRIB_SR7_DISABLE;
        break;
    case 2UL:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    case 3UL:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    case 4UL:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    case 5UL:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    case 6UL:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    case 7UL:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    default:
        attrib |= (CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                        | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
        break;
    }

done:
    return attrib;
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_DISABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_MPUDisable(void)
{
    CORTEX_MX_MPUType *mpu = (CORTEX_MX_MPUType *)CORTEX_MX_MPU_BASE;
	mpu->CTRL = 0UL;
	CORTEX_DMB();
	CORTEX_ISB();
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_MPUEnable(void)
{
    CORTEX_MX_MPUType *mpu = (CORTEX_MX_MPUType *)CORTEX_MX_MPU_BASE;
    mpu->CTRL = CORTEX_MX_MPU_CTRL_ENABLE_MASK | CORTEX_MX_MPU_CTRL_HFNMIENA_MASK;
	CORTEX_DMB();
	CORTEX_ISB();
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_MPU_CONFIG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_MPUConfigure(uint32_t aRegionNum, uint32_t aBaseAddr,
                                CORTEX_MX_MPUAttribType aRegionAttrib)
{
    CORTEX_MX_MPUType *mpu = (CORTEX_MX_MPUType *)CORTEX_MX_MPU_BASE;
    mpu->RNR = aRegionNum;
    mpu->RBAR = aBaseAddr & CORTEX_MX_MPU_RBAR_ADDR_MASK;
    mpu->RASR = aRegionAttrib | CORTEX_MX_MPU_REGION_ENABLED;
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_GET_MPUGETSIZEATTRIB_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
uint32_t CORTEX_MX_MPUGetSizeAttrib(uint32_t aSizePwr2, uint32_t aActualSize)
{
    uint32_t sizeAttrib = 0UL;
    uint32_t disableAttrib;

    disableAttrib = CORTEX_MX_MPUSubRegionDisableAttrib(aSizePwr2, aActualSize);

    COMP_ASM("CLZ %0, %0;"
            : "=r" (sizeAttrib)
            : "r" (aSizePwr2)
            :);

    return (((31UL - sizeAttrib - 1) << 1) | disableAttrib);
}

/** @} */
