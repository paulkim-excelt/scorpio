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
/* TODO: Update this file */

#include <stdint.h>
#include <arm.h>
#include <chip_config.h>
#include <utils.h>
#include <cortex_mx.h>
/* NOTE: Any change made here must be reflected in the ld script as well */

/*
 * NOTE: code-ro, driver-data and data regions are in TCM memory.
 * [cortex-r4 TRM section 7.1.4]
 * TCM  interface is given Normal, Non-shared type attributes, regardless of
 * the attributes of any MPU region that the address also belongs to.
 *
 * [cortex-r4 TRM section 8.4.1]
 * TCMs always behave as Non-cacheable Non-shared Normal memory, irrespective of
 * the memory type attributes defined in the MPU for a memory region containing
 * addresses held in the TCM.
 */

#define CODE_RO_REGION_START    (0x00000000UL)
#define DRV_DATA_REGION_START   (0x00030000UL)
#define DATA_REGION_START       (0x00040000UL)
#define DEV_GRP1_REGION_START   (0x00100000UL)
#define DEV_GRP2_REGION_START   (0x00180000UL)
#define DEV_GRP3_REGION_START   (0x08000000UL)

void MPU_Config()
{
    CORTEX_RX_MPUDisable();
    CORTEX_RX_MPUConfigure(0UL, DEV_GRP1_REGION_START,
                            (MPU_SIZE_512K | SR5_DISABLE | SR6_DISABLE | SR7_DISABLE),
                            (MEM_NONSHARED_DEVICE | PRIVILAGE | XN));
    CORTEX_RX_MPUConfigure(1UL, DEV_GRP2_REGION_START,
                            (MPU_SIZE_64K | SR5_DISABLE | SR6_DISABLE | SR7_DISABLE),
                            (MEM_NONSHARED_DEVICE | PRIVILAGE | XN));
    CORTEX_RX_MPUConfigure(2UL, DEV_GRP3_REGION_START,
                            (MPU_SIZE_64M | SR0_DISABLE | SR1_DISABLE | SR6_DISABLE | SR7_DISABLE),
                            (MEM_NONSHARED_DEVICE | PRIVILAGE | XN));
    CORTEX_RX_MPUConfigure(3UL, CODE_RO_REGION_START,
                            (MPU_SIZE_256K | SR6_DISABLE | SR7_DISABLE),
                            (MEM_NORMAL_NONCACHEABLE | READ_ONLY));
    CORTEX_RX_MPUConfigure(4UL, DRV_DATA_REGION_START,
                            MPU_SIZE_64K,
                            (MEM_NORMAL_NONCACHEABLE | USER_READ | XN));
    CORTEX_RX_MPUConfigure(5UL, DATA_REGION_START,
                            (MPU_SIZE_256K | SR7_DISABLE),
                            (MEM_NORMAL_NONCACHEABLE | FULL_ACCESS | XN));
    CORTEX_RX_MPUEnable();
}
