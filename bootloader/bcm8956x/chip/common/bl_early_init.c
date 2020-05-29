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

#include <cortex_mx.h>
#include <bcm_utils.h>

/* MCU specific MPU region start addresses */
#define BL_DEV_GRP1_REGION_START       (0x40100000UL)
#define BL_DEV_GRP2_REGION_START       (0x49000000UL)
#define BL_DEV_GRP3_REGION_START       (0x4A000000UL)
#define BL_DEV_GRP4_REGION_START       (0x4B000000UL)
#define BL_DEV_GRP5_REGION_START       (0x4B280000UL)
#define BL_DEV_GRP6_REGION_START       (0x4C000000UL)

#define BL_ITCM_REGION_START           (0x00000000UL)
#define BL_ITCM2_REGION_START          (0x00040000UL)
#define BL_DTCM_REGION_START           (0x20000000UL)
#define BL_SCRATCH0_REGION_START       (0x64000000UL)

typedef struct {
    uint32_t regionBaseAddr;
    uint32_t regionSz;
    uint32_t regionAttr;
} BL_MPU_TblType;

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];
extern uint8_t BL_DWNLD_StartAddr[];
extern uint8_t __text_start__[];
extern uint8_t bl_bin_size[];

#define BL_MPU_TABLE_SIZE_DEVICE       (6UL)
#define BL_DEV_MEM_ATTRIB   (CORTEX_MX_MPU_ATTRIB_TEX_0 | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                            | CORTEX_MX_MPU_ATTRIB_PRIVILEGE | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC)

/* NOTE: Any change made here must be reflected in the ld script as well */
const BL_MPU_TblType BL_MPU_TblDevice[BL_MPU_TABLE_SIZE_DEVICE] = {
    /**********************************************************************/
    /*  regionBaseAddr          SIZE            regionAttr
     *  **********************************************************************/
    {BL_DEV_GRP1_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_512K, BL_DEV_MEM_ATTRIB \
                                                              | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE},
    {BL_DEV_GRP2_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_16M, BL_DEV_MEM_ATTRIB \
                                                              | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE},
    {BL_DEV_GRP3_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_16M, BL_DEV_MEM_ATTRIB 
                                                              | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE 
                                                              | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE 
                                                              | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE},
    {BL_DEV_GRP4_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_128K, BL_DEV_MEM_ATTRIB},
    {BL_DEV_GRP5_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_32B,  BL_DEV_MEM_ATTRIB},
    {BL_DEV_GRP6_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_2M,   BL_DEV_MEM_ATTRIB \
                                                              | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE},
};

#define BL_MPU_TABLE_SIZE_MEMORY  (4UL)

const BL_MPU_TblType BL_MPU_TblMemory[BL_MPU_TABLE_SIZE_MEMORY] =
{
    /**********************************************************************/
    /*  regionBaseAddr          SIZE            regionAttr
     *  **********************************************************************/
    {BL_ITCM_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K, CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                                          | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS \
                                                          | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE},
    {BL_ITCM2_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K, CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                                          | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS},
    {BL_DTCM_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K, CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                                          | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS},
    {BL_SCRATCH0_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_64K, CORTEX_MX_MPU_ATTRIB_TEX_1 
                                                          | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS \
                                                          | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC \
                                                          | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE \
                                                          | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE \
                                                          | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE},
};

static void BL_CPUInit(void)
{
    uint32_t i = 0UL;
    uint32_t reg_num = 0UL;

    CORTEX_MX_MPUDisable();

    for (i = 0; i < BL_MPU_TABLE_SIZE_DEVICE; i++) {
        CORTEX_MX_MPUConfigure(reg_num++, BL_MPU_TblDevice[i].regionBaseAddr, 
                BL_MPU_TblDevice[i].regionAttr | BL_MPU_TblDevice[i].regionSz);
    }

    for (i = 0UL; i < BL_MPU_TABLE_SIZE_MEMORY; i++) {
        CORTEX_MX_MPUConfigure(reg_num++, BL_MPU_TblMemory[i].regionBaseAddr,
                BL_MPU_TblMemory[i].regionAttr | BL_MPU_TblMemory[i].regionSz);
    }

    CORTEX_MX_MPUEnable();

    /* CLEAR BSS section */
    BCM_MemSet(__bss_start__, 0U, (uint32_t)__bss_end__ - (uint32_t)__bss_start__);
}


void BL_EarlyInit(void)
{
    BL_CPUInit();
#ifdef BL_ENABLE_SLAVE_BOOT
    BCM_MemCpy(BL_DWNLD_StartAddr, __text_start__, (uint32_t)bl_bin_size);
#endif
}
