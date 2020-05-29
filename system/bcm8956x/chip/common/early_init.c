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
#include <bcm_err.h>
#include <io.h>
#include <utils.h>
#include <ulog.h>
#include <clk.h>
#include <mcu.h>
#include <osil/uart_osil.h>
#include <osil/flsmgr_osil.h>
#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#endif
#include <gpt.h>
#include <build_info.h>
#include <board.h>
#include <init.h>
#include <interrupt.h>
#include <chip_config.h>
#include <compiler.h>
#include <gpio.h>
#include "ee.h"
#include "ee_internal.h"
#if defined(ENABLE_THREAD_PROTECTION)
#include <thread_safety.h>
#endif

#define GetModuleLogLevel()     ULOG_LVL_ERROR
static volatile uint32_t MCU_InitErr = FALSE;
static volatile uint32_t MCU_InitDone = FALSE;

#if defined(ENABLE_THREAD_PROTECTION)
extern uint8_t __init_thread_stack__[];
extern uint8_t init_thread_stack_size[];
#endif

uint32_t EarlyInitTime;

/* MCU specific MPU region start addresses */
#define BCM8956X_DEVICE_GRP1_START       (0x40100000UL)
#define BCM8956X_DEVICE_GRP2_START       (0x49000000UL)
#define BCM8956X_DEVICE_GRP3_START       (0x4A000000UL)
#define BCM8956X_DEVICE_GRP4_START       (0x4B000000UL)
#define BCM8956X_DEVICE_GRP5_START       (0x4B280000UL)
#define BCM8956X_DEVICE_GRP6_START       (0x4C000000UL)

#define BCM8956X_DEVICE_MEMORY_ATTRIB (CORTEX_MX_MPU_ATTRIB_TEX_0\
                            | CORTEX_MX_MPU_ATTRIB_BUFFERABLE\
                            | CORTEX_MX_MPU_ATTRIB_PRIVILEGE\
                            | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC)

#define BCM8956X_TASK_ATTRIB    (CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                | CORTEX_MX_MPU_ATTRIB_CACHEABLE \
                                | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS \
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC)

typedef struct _BCM8956X_MPUTableType {
    uint32_t baseAddress;
    CORTEX_MX_MPUAttribType attrib;
} BCM8956X_MPUTableType;

#define BCM8956X_DEFINE_MPU_TABLE(aVar, aSize) \
    const BCM8956X_MPUTableType aVar[aSize]

#define BCM8956X_DEVICE_MPU_TABLE_SIZE  (6UL)
BCM8956X_DEFINE_MPU_TABLE(BCM8956X_DeviceMPUTable, BCM8956X_DEVICE_MPU_TABLE_SIZE) = {
    {BCM8956X_DEVICE_GRP1_START, CORTEX_MX_MPU_ATTRIB_SIZE_512K
                                | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE
                                | BCM8956X_DEVICE_MEMORY_ATTRIB },
    {BCM8956X_DEVICE_GRP2_START, CORTEX_MX_MPU_ATTRIB_SIZE_16M
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE
                                | BCM8956X_DEVICE_MEMORY_ATTRIB },
    {BCM8956X_DEVICE_GRP3_START, CORTEX_MX_MPU_ATTRIB_SIZE_16M
                                | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE
                                | BCM8956X_DEVICE_MEMORY_ATTRIB },
    {BCM8956X_DEVICE_GRP4_START, CORTEX_MX_MPU_ATTRIB_SIZE_128K
                                | BCM8956X_DEVICE_MEMORY_ATTRIB },
    {BCM8956X_DEVICE_GRP5_START, CORTEX_MX_MPU_ATTRIB_SIZE_32B
                                | BCM8956X_DEVICE_MEMORY_ATTRIB },
    {BCM8956X_DEVICE_GRP6_START, CORTEX_MX_MPU_ATTRIB_SIZE_2M
                                | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                                | BCM8956X_DEVICE_MEMORY_ATTRIB },
};

#define BCM8956X_MEMORY_ITCM_CODE_RO_START      (0x00000000UL)
#define BCM8956X_MEMORY_ITCM_CODE_RO2_START     (0x00040000UL)
#define BCM8956X_MEMORY_ITCM_DATA_RW_START      (0x00050000UL)
#define BCM8956X_MEMORY_DTCM_DATA_RW_START      (0x20000000UL)
#define BCM8956X_MEMORY_DTCM_IPC_RW_START       (0x2003C000UL)
#define BCM8956X_MEMORY_SCRATCH_PORT0_START     (0x64000000UL)

#define BCM8956X_MEMORY_MPU_TABLE_SIZE  (6UL)

BCM8956X_DEFINE_MPU_TABLE(BCM8956X_MemoryMPUTable, BCM8956X_MEMORY_MPU_TABLE_SIZE) = {
    {BCM8956X_MEMORY_ITCM_CODE_RO_START, CORTEX_MX_MPU_ATTRIB_SIZE_512K
                                | CORTEX_MX_MPU_ATTRIB_TEX_1
                                | CORTEX_MX_MPU_ATTRIB_USER_READ
                                | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
    {BCM8956X_MEMORY_ITCM_CODE_RO_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K
                                | CORTEX_MX_MPU_ATTRIB_TEX_1
                                | CORTEX_MX_MPU_ATTRIB_READ_ONLY
                                | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE },
    {BCM8956X_MEMORY_ITCM_CODE_RO2_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K
                                | CORTEX_MX_MPU_ATTRIB_TEX_1
                                | CORTEX_MX_MPU_ATTRIB_READ_ONLY
                                | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
    {BCM8956X_MEMORY_DTCM_DATA_RW_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K
                                | CORTEX_MX_MPU_ATTRIB_TEX_1
                                | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
    {BCM8956X_MEMORY_DTCM_IPC_RW_START, CORTEX_MX_MPU_ATTRIB_SIZE_16K
                                | CORTEX_MX_MPU_ATTRIB_TEX_1
                                | CORTEX_MX_MPU_ATTRIB_USER_READ
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
    {BCM8956X_MEMORY_SCRATCH_PORT0_START, CORTEX_MX_MPU_ATTRIB_SIZE_64K
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC
                                    | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
};

static void BCM8956X_CPUInit(void)
{
    extern uint8_t __bss_start__[];
    extern uint8_t __bss_end__[];

    CORTEX_MX_MPUDisable();

    uint32_t i = 0UL;
    for (i = 0UL; i < BCM8956X_DEVICE_MPU_TABLE_SIZE; i++) {
        CORTEX_MX_MPUConfigure(i, BCM8956X_DeviceMPUTable[i].baseAddress,
                                    BCM8956X_DeviceMPUTable[i].attrib);
    }
    for (i = 0UL; i < BCM8956X_MEMORY_MPU_TABLE_SIZE; i++) {
        CORTEX_MX_MPUConfigure(BCM8956X_DEVICE_MPU_TABLE_SIZE + i,
                                    BCM8956X_MemoryMPUTable[i].baseAddress,
                                    BCM8956X_MemoryMPUTable[i].attrib);
    }

    CORTEX_MX_MPUEnable();

    /* CLEAR BSS section */
    BCM_MemSet(__bss_start__, 0U, (uint32_t)__bss_end__ - (uint32_t)__bss_start__);

    /*Enable Faults and Disable Interrupts */
    CORTEX_MX_FAULT_ENABLE();
    CORTEX_MX_INTR_DISABLE();
}

#if defined(ENABLE_THREAD_PROTECTION)

void BCM_AddTaskMPUEntry(uint32_t aTaskID)
{
    uint32_t sizeAttr;

    CORTEX_MX_MPUDisable();
    sizeAttr = CORTEX_MX_MPUGetSizeAttrib(BRCM_TaskInfo[aTaskID].size << 2UL, BRCM_TaskInfo[aTaskID].size << 2UL);

    if (sizeAttr > 0UL) {
        CORTEX_MX_MPUConfigure(BCM8956X_DEVICE_MPU_TABLE_SIZE + BCM8956X_MEMORY_MPU_TABLE_SIZE,
                (uint32_t)BRCM_TaskInfo[aTaskID].base, (sizeAttr | BCM8956X_TASK_ATTRIB));
    }
    CORTEX_MX_MPUEnable();
}

void BCM_MPUAddInitThreadStack(void)
{
    uint32_t sizeAttr;
    CORTEX_MX_MPUDisable();
    sizeAttr = CORTEX_MX_MPUGetSizeAttrib((uint32_t)init_thread_stack_size, (uint32_t)init_thread_stack_size);

    if (sizeAttr > 0UL) {
        CORTEX_MX_MPUConfigure(BCM8956X_DEVICE_MPU_TABLE_SIZE + BCM8956X_MEMORY_MPU_TABLE_SIZE,
                (uint32_t)__init_thread_stack__ - (uint32_t)init_thread_stack_size,
                (sizeAttr | BCM8956X_TASK_ATTRIB));
    }
    CORTEX_MX_MPUEnable();
}
#endif

static void BCM8956X_InitMCU(void)
{
    uint32_t i;
    int32_t err = BCM_ERR_OK;
    MCU_PllStatusType pllStatus = MCU_PLLSTATUS_UNLOCKED;

    MCU_Init(mcu_cfg_table);

    /**
     * Initialize all the clocks
     * and RAM sections
     */

   /**
     * Wait for all the PLLs to be locked
     */
    while (pllStatus != MCU_PLLSTATUS_LOCKED) {
        pllStatus = MCU_GetPllStatus();
    }

    /**
     * Initialize QSPI clock
     */
#ifdef ENABLE_FLASH
    if (flash_clk_table != NULL) {
        for (i = 0UL; i < flash_clk_table_sz; i++) {
            err = MCU_ClkInit(flash_clk_table[i]);
            if (BCM_ERR_OK != err) {
                break;
            }
        }
        if (BCM_ERR_OK != err) {
            goto err;
        }
    }
#endif /* ENABLE_FLASH */

    /**
     * Initialize all the RAM sections
     */
    if (mcu_cfg_table->ramCfgTbl != NULL) {
        for (i = 0UL; i < mcu_cfg_table->ramCfgTblSize; i++) {
            MCU_RamSectionInit(i);
        }
    }
err:
    if (err != BCM_ERR_OK) {
        MCU_InitErr = TRUE;
        while (1UL) { }
    }
}

void BCM8956X_EarlyInit(void)
{
    BCM8956X_CPUInit();

    EarlyInitTime = TIM_GetTimeRaw(0UL) / 1000000UL;

    /* initialize MCU */
    BCM8956X_InitMCU();

    MCU_InitDone = TRUE;

}

