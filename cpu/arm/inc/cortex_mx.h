/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_cortex_mx Cortex-M Functionality
    @ingroup grp_cortex

    @addtogroup grp_cortex_mx
    @{

    @file cortex_mx.h
    @brief ARM Cortex-M Special functionality
    This header file contains the interface functions/macros for ARM Cortex-M
    Processor architecture special instructions

    @version 0.1 Initial Version
*/

#ifndef CORTEX_MX_H
#define CORTEX_MX_H

#include <compiler.h>
#include <cortex.h>
#include <inttypes.h>
#include <bcm_err.h>
/**
    @name Cortex-M Architecture IDs
    @{
    @brief Architecture IDs for Cortex-M Proecessor Specifics
*/
#define BRCM_SWARCH_CORTEX_MX_REG_MACRO             (0x00U)   /**< @brief #CORTEX_MX_REG_ACTLR         */
#define BRCM_SWARCH_CORTEX_MX_MPU_ATTRIB_TYPE       (0x01U)   /**< @brief #CORTEX_MX_MPUAttribType     */
#define BRCM_SWARCH_CORTEX_MX_MPU_DISABLE_PROC      (0x02U)   /**< @brief #CORTEX_MX_MPUDisable        */
#define BRCM_SWARCH_CORTEX_MX_MPU_ENABLE_PROC       (0x03U)   /**< @brief #CORTEX_MX_MPUEnable         */
#define BRCM_SWARCH_CORTEX_MX_MPU_CONFIG_PROC       (0x04U)   /**< @brief #CORTEX_MX_MPUConfigure      */
#define BRCM_SWARCH_CORTEX_MX_SYSTEM_RESET_PROC     (0x05U)   /**< @brief #CORTEX_MX_SystemReset       */
#define BRCM_SWARCH_CORTEX_MX_VECTOR_RESET_PROC     (0x06U)   /**< @brief #CORTEX_MX_VectorReset       */
#define BRCM_SWARCH_CORTEX_MX_FAULT_MACRO           (0x07U)   /**< @brief #CORTEX_MX_FAULT_ENABLE      */
#define BRCM_SWARCH_CORTEX_MX_INTR_MACRO            (0x08U)   /**< @brief #CORTEX_MX_INTR_ENABLE       */
#define BRCM_SWARCH_CORTEX_MX_ICACHE_ENABLE_PROC    (0x09U)   /**< @brief #CORTEX_MX_ICacheEnable      */
#define BRCM_SWARCH_CORTEX_MX_DCACHE_ENABLE_PROC    (0x0AU)   /**< @brief #CORTEX_MX_DCacheEnable      */
#define BRCM_SWARCH_CORTEX_MX_DCACHE_CLEAN_PROC     (0x0BU)   /**< @brief #CORTEX_MX_DCacheCleanAll    */
#define BRCM_SWARCH_CORTEX_MX_DCACHE_CLRNG_PROC     (0x0CU)   /**< @brief #CORTEX_MX_DCacheCleanRange  */
#define BRCM_SWARCH_CORTEX_MX_DCACHE_INRNG_PROC     (0x0DU)   /**< @brief #CORTEX_MX_DCacheInvalRange  */
#define BRCM_SWARCH_CORTEX_MX_DCACHE_FLRNG_PROC     (0x0EU)   /**< @brief #CORTEX_MX_DCacheFlushRange  */
#define BRCM_SWARCH_CORTEX_MX_INIT_SYSTICK_PROC     (0x0FU)   /**< @brief #CORTEX_MX_SysTickInit       */
#define BRCM_SWARCH_CORTEX_MX_GTCNT_SYSTICK_PROC    (0x10U)   /**< @brief #CORTEX_MX_SysTickGetCnt     */
#define BRCM_SWARCH_CORTEX_MX_INTR_ENABLE_PROC      (0x11U)   /**< @brief #CORTEX_MX_INTREnable        */
#define BRCM_SWARCH_CORTEX_MX_INTR_DISABLE_PROC     (0x12U)   /**< @brief #CORTEX_MX_INTRDisable       */
#define BRCM_SWARCH_CORTEX_MX_ICACHE_DISABLE_PROC   (0x13U)   /**< @brief #CORTEX_MX_ICacheDisable     */
#define BRCM_SWARCH_CORTEX_MX_DCACHE_DISABLE_PROC   (0x13U)   /**< @brief #CORTEX_MX_DCacheDisable     */
#define BRCM_SWARCH_CORTEX_MX_FPU_ENABLE_PROC       (0x14U)   /**< @brief #CORTEX_MX_FPUEnable         */
#define BRCM_SWARCH_CORTEX_MX_FPU_DISABLE_PROC      (0x15U)   /**< @brief #CORTEX_MX_FPUDisable        */
#define BRCM_SWARCH_CORTEX_MX_SET_THREADMODE_PROC   (0x16U)   /**< @brief #CORTEX_MX_SetThreadMode     */
#define BRCM_SWARCH_CORTEX_MX_GET_MPUGETSIZEATTRIB_PROC      (0x17U)   /**< @brief #CORTEX_MX_MPUGetSizeAttrib */
/** @} */

/**
    @name Important Register addresses
    @{
    @brief Macros for important register addresses

    Macros for important registers to be accessed in
    exceptions.

    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_REG_ACTLR      (0xE000E008UL)
#define CORTEX_MX_REG_CPUID      (0xE000ED00UL)
#define CORTEX_MX_REG_ICSR       (0xE000ED04UL)
#define CORTEX_MX_REG_VCTOR      (0xE000ED08UL)
#define CORTEX_MX_REG_AIRCR      (0xE000ED0CUL)
#define CORTEX_MX_REG_SCR        (0xE000ED10UL)
#define CORTEX_MX_REG_HFSR       (0xE000ED2CUL)
#define CORTEX_MX_REG_DFSR       (0xE000ED30UL)
#define CORTEX_MX_REG_MMFAR      (0xE000ED34UL)
#define CORTEX_MX_REG_BFAR       (0xE000ED38UL)
/** @} */

/**
    @name Cortex-M MPU Region Attributes
    @{
    @brief Macros for Cortex-M MPU region size

    Type and Macros for valid region sizes for Cortex-M MPU

    @trace #BRCM_SWREQ_CORTEX_MX
*/
typedef uint32_t CORTEX_MX_MPUAttribType;
#define CORTEX_MX_MPU_ATTRIB_SIZE_32B       (0x08UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_64B       (0x0AUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_128B      (0x0CUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_256B      (0x0EUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_512B      (0x10UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_1K        (0x12UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_2K        (0x14UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_4K        (0x16UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_8K        (0x18UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_16K       (0x1AUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_32K       (0x1CUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_64K       (0x1EUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_128K      (0x20UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_256K      (0x22UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_512K      (0x24UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_1M        (0x26UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_2M        (0x28UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_4M        (0x2AUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_8M        (0x2CUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_16M       (0x2EUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_32M       (0x30UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_64M       (0x32UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_128M      (0x34UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_256M      (0x36UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_512M      (0x38UL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_1G        (0x3AUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_2G        (0x3CUL)
#define CORTEX_MX_MPU_ATTRIB_SIZE_4G        (0x3EUL)
#define CORTEX_MX_MPU_ATTRIB_TEX_0          (0x0UL << 19UL)
#define CORTEX_MX_MPU_ATTRIB_TEX_1          (0x1UL << 19UL)
#define CORTEX_MX_MPU_ATTRIB_TEX_2          (0x2UL << 19UL)
#define CORTEX_MX_MPU_ATTRIB_TEX_3          (0x3UL << 19UL)
#define CORTEX_MX_MPU_ATTRIB_SHAREABLE      (0x1UL << 18UL)
#define CORTEX_MX_MPU_ATTRIB_CACHEABLE      (0x1UL << 17UL)
#define CORTEX_MX_MPU_ATTRIB_BUFFERABLE     (0x1UL << 16UL)
#define CORTEX_MX_MPU_ATTRIB_NO_ACCESS      (0x00000000UL)
#define CORTEX_MX_MPU_ATTRIB_PRIVILEGE      (0x01000000UL)
#define CORTEX_MX_MPU_ATTRIB_USER_READ      (0x02000000UL)
#define CORTEX_MX_MPU_ATTRIB_FULL_ACCESS    (0x03000000UL)
#define CORTEX_MX_MPU_ATTRIB_PRIVILEGE_READ (0x05000000UL)
#define CORTEX_MX_MPU_ATTRIB_READ_ONLY      (0x06000000UL)
#define CORTEX_MX_MPU_ATTRIB_SR0_DISABLE    (0x01UL << 8UL)
#define CORTEX_MX_MPU_ATTRIB_SR1_DISABLE    (0x01UL << 9UL)
#define CORTEX_MX_MPU_ATTRIB_SR2_DISABLE    (0x01UL << 10UL)
#define CORTEX_MX_MPU_ATTRIB_SR3_DISABLE    (0x01UL << 11UL)
#define CORTEX_MX_MPU_ATTRIB_SR4_DISABLE    (0x01UL << 12UL)
#define CORTEX_MX_MPU_ATTRIB_SR5_DISABLE    (0x01UL << 13UL)
#define CORTEX_MX_MPU_ATTRIB_SR6_DISABLE    (0x01UL << 14UL)
#define CORTEX_MX_MPU_ATTRIB_SR7_DISABLE    (0x01UL << 15UL)
#define CORTEX_MX_MPU_ATTRIB_NEVER_EXEC     (0x10000000UL)
/** @} */

/** @brief Disable MPU for Cortex-M processor

    @behavior Sync, Non-reentrant

    @pre Caches are disabled, if there is a change in cache attributes for\
    any memory region

    @param[in]      void

    @return void

    @post MPU is disabled

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_MPUDisable(void);

/** @brief Enable MPU for Cortex-M processor

    @behavior Sync, Non-reentrant

    @pre MPU is disabled using #CORTEX_MX_MPU_Disable

    @param[in]      void

    @return void

    @post MPU is enabled

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_MPUEnable(void);

/** @brief Configure a specific region entry in MPU

    Apply user provided configuration to the user specified region in
    Cortex-M MPU

    @behavior Sync, Non-reentrant

    @pre MPU is disabled using #CORTEX_MX_MPU_Disable

    @param[in]      aRegionNum      Region to which @a aRegion need to be applied
    @param[in]      aBaseAddr       MPU Region Base address
    @param[in]      aRegionAttrib   MPU Region Configuration to be applied

    @return void

    @post MPU Region is updated with @a aRegion

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_MPUConfigure(uint32_t aRegionNum, uint32_t aBaseAddr,
                                CORTEX_MX_MPUAttribType aRegionAttrib);


/** @brief Enable I-Cache

    This API enables instruction cache. This API may be called
    during startup sequence to improve performance.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post Instruction Cache is enabled

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_ICacheEnable(void);

/** @brief Disable I-Cache

    This API disables instruction cache. This API may be called
    during shutdown sequence.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post Instruction Cache is disabled

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_ICacheDisable(void);

/** @brief Enable D-Cache

    This API enables Data cache. This API may be called
    during startup sequence. This API must be called before
    using any other D-Cache APIs.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post Data Cache is enabled

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_DCacheEnable(void);

/** @brief Disable D-Cache

    This API disables Data cache. This API may be called
    during shutdown sequence. Only DCacheEnable can be called
    after using this API.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post Data Cache is disabled

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_DCacheDisable(void);

/** @brief Clean complete D-Cache

    This API cleans all dirty line in Data cache.

    @behavior Sync, Reentrant

    @pre None

    @param[in]      void

    @return void

    @post Data Cache is synchronized to main memory

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_DCacheCleanAll(void);

/** @brief Clean D-Cache for specific memory range

    This API cleans all dirty lines within specific address range in
    Data cache.

    @behavior Sync, Reentrant

    @pre None

    @param[in]      void

    @return void

    @post Data Cache is synchronized to main memory for given address range

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_DCacheCleanRange(uint32_t aAddr, uint32_t aSize);

/** @brief Invalidate D-Cache for specific memory range

    This API invalidates all lines within specific address range in
    Data cache.

    @behavior Sync, Reentrant

    @pre None

    @param[in]      void

    @return void

    @post Specific Data Cache lines are invalidated for given address range

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_DCacheInvalRange(uint32_t aAddr, uint32_t aSize);

/** @brief Flush (Clean + Invalidate) D-Cache for specific memory range

    This API cleans and invalidates all lines within specific address range in
    Data cache.

    @behavior Sync, Reentrant

    @pre None

    @param[in]      void

    @return void

    @post Specific Data Cache lines are cleaned and invalidated for given
    address range

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None
*/
void CORTEX_MX_DCacheFlushRange(uint32_t aAddr, uint32_t aSize);

/**
    @name Cortex-M Fault
    @{
    @brief Macros to enable/disable Cortex-M Faults/Exceptions

    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_FAULT_ENABLE()      COMP_ASM("cpsie f")
#define CORTEX_MX_FAULT_DISABLE()     COMP_ASM("cpsid f")
/** @} */

/**
    @name Cortex-M Interrupt
    @{
    @brief Macros to enable/disable Cortex-M Interrupts

    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_INTR_ENABLE()      COMP_ASM("cpsie i")
#define CORTEX_MX_INTR_DISABLE()     COMP_ASM("cpsid i")
/** @} */

/** @brief Perform System/Global reset

    API to perform Global reset

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return This function never returns

    @post System reboot

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_SystemReset(void);

/** @brief Perform ARM local reset

    This reset active state of all active exceptions and clears the exception
    number in IPSR register.
    It does not affect any pending state of exceptions

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return This function never returns

    @post System reboot

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_VectorReset(void);

/** @brief Initialize SysTick block

    Initialize the systick block with a given tick count

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aTickCnt    Tick count to be set in SysTick block

    @retval     #BCM_ERR_INVAL_PARAMS    If Tick count is larger than
                                            Max allowed value
    @retval     #BCM_ERR_INVAL_STATE     If Sys Tick is already initialized
    @retval     #BCM_ERR_OK              Sys Tick is initialized correctly

    @post SysTick block is intialized and will raise SysTick Exceptions on
    counter expiry

    @trace #BRCM_SWREQ_CORTEX_MX
*/
int32_t CORTEX_MX_SysTickInit(uint32_t aTickCnt);

/** @brief Get SysTick counter value

    Get the counter value in hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return uint32_t value containing counter value in SysTick block

    @post None

    @trace #BRCM_SWREQ_CORTEX_MX
*/
uint32_t CORTEX_MX_SysTickGetCnt(void);

/** @brief Enable interrupts and fault handlers

    This API enables all interrupts and fault handlers

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void (for void functions)

    @post Interrupts and Exceptions are enabled

    @trace  #BRCM_SWREQ_CORTEX_MX

    @limitations This API shall only be called during system boot up
*/
void CORTEX_MX_INTREnable(void);

/** @brief Disable interrupts and fault handlers

    This API disables all interrupts and fault handlers

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void (for void functions)

    @post Interrupts and fault handlers are disabled

    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations This API shall only be called during shutdown sequence
*/
void CORTEX_MX_INTRDisable(void);

/** @brief Enable the FPU

    This API enables the FPU

    @behavior Sync, Reentrant

    @pre None

    @param[in]      void

    @return void (for void functions)

    @post FPU enabled.

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_FPUEnable(void);

/** @brief Disable the FPU

    This API disables the FPU

    @behavior Sync, Reentrant

    @pre None

    @param[in]      void

    @return void (for void functions)

    @post FPU disabled.

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_FPUDisable(void);

/** @brief Set the thread mode to privileged/unprivileged

    This API sets the MX thread mode to privileged/unpriviledged
    mode and switch to PSP stack for thread mode.

    This API must be called in priviledged mode

    Refer to ARMV7M ARM section B1.4.4

    @behavior Sync, Reentrant

    @pre None

    @param[in]  aPriv       0: privileged, 1: unprivileged
    @param[in]  aSpSel      1: use PSP for thread stack, 0: use MSP
    @param[in]  aStackPtr   stack pointer to initialize for Thread mode

    @return void

    @post Thread mode has unprivileged access

    @trace #BRCM_SWREQ_CORTEX_MX
*/

void CORTEX_MX_SetThreadMode(uint32_t aPriv, uint32_t aSpSel, uint32_t aStackPtr);

/** @brief Get MPU size attribute

    This API return the Cortex MX MPU size field for given input size

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aSizePwr2       Size in power of 2
    @param[in]      aActualSize     Actual size

    @return MPU size attribute field

    @post None

    @trace #BRCM_SWREQ_CORTEX_MX
*/
uint32_t CORTEX_MX_MPUGetSizeAttrib(uint32_t aSizePwr2, uint32_t aActualSize);

#endif /* CORTEX_MX_H */

/** @} */
