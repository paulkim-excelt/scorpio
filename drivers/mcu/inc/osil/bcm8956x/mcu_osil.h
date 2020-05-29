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

/**

    @addtogroup grp_mcudrv_il
    @{

    @file mcu_osil.h
    @brief MCU Driver Integration Interface
    This header file contains the integration interface for MCU Driver

    @version 0.86 Imported from docx
*/
#ifndef INCLUDE_BCM8956X_OSIL_MCU_OSIL_H
#define INCLUDE_BCM8956X_OSIL_MCU_OSIL_H

#include <inttypes.h>
#include <system.h>
#include <mcu.h>
#include <mcu_ext.h>

/**
    @name MCU Integration Interface IDs
    @{
    @brief Integration Interface IDs for MCU Driver
*/
#define BRCM_SWARCH_MCU_CMD_TYPE               (0x40U) /**< @brief #MCU_CmdType */
#define BRCM_SWARCH_MCU_IO_TYPE                (0x41U) /**< @brief #MCU_IOType */
/** @} */

/**
    @name MCU_CmdType
    @{
    @brief MCU Driver SVC commands. This command types are used by integration layer to post system command request to
    MCU driver (for details refer to #MCU_SysCmdReq)

    @trace #BRCM_SWREQ_MCU_KERNEL_INTERFACE
*/
typedef uint32_t MCU_CmdType;               /**< @brief typedef for MCU command */
#define MCU_CMD_RESET_REQ           (0x1UL) /**< @brief Reset request command */
#define MCU_CMD_GET_RESET_REASON    (0x2UL) /**< @brief Get reset reason command */
#define MCU_CMD_GET_RAW_VAL         (0x3UL) /**< @brief Get reset reason raw value command */
#define MCU_CMD_SET_LPM_MODE        (0x4UL) /**< @brief Set MCU LPM mode command */
#define MCU_CMD_GET_VERSION         (0x5UL) /**< @brief Get MCU version command */
#define MCU_CMD_GET_RESET_MODE      (0x6UL) /**< @brief Get Reset Mode command */
#define MCU_CMD_SET_RESET_MODE      (0x7UL) /**< @brief Set Reset Mode command */
#define MCU_CMD_GET_FW_BOOT_INFO    (0x8UL) /**< @brief Get MCU Firmware Boot Information */
#define MCU_CMD_ADJUST_CLK          (0x9UL) /**< @brief Adjust clock frequency */
#define MCU_CMD_GET_SWT_PORT_2_TIME_FIFO_MAP (0xAUL) /**< @brief Ethernet Switch
                                                      port to time fifo map */
#define MCU_CMD_ENABLE_SWT_CPU_PORT (0xBUL) /**< @brief Enable Ethernet Switch
                                                      CPU port */
#define MCU_CMD_DISABLE_SWT_CPU_PORT (0xCUL) /**< @brief Disable Ethernet Switch
                                                      CPU port */
#define MCU_CMD_GET_STACKING_INFO   (0xDUL) /**< @brief Get Stacking Info */
#define MCU_CMD_GET_BOOT_TIME       (0xEUL) /**< @brief Get boot time info */
/** @} */

/**
    @brief MCU IO structure

    @trace #BRCM_SWREQ_MCU_KERNEL_INTERFACE
*/
typedef struct _MCU_IOType {
    int32_t retVal;                  /**< @brief Error return from MCU command handler */
    MCU_ResetReqType resetReq;       /**< @brief Reset request */
    MCU_ResetModeType resetMode;     /**< @brief Reset mode */
    MCU_ResetReasonType resetReason; /**< @brief Reset reason */
    uint32_t resetRaw;               /**< @brief Reset raw value */
    MCU_LpmModeType lpmMode;         /**< @brief Low power mode */
    uint32_t bufAttr;                /**< @brief DHS buffer attribute */
    MCU_FWBootInfoType *bootInfo;    /**< @brief Pointer to retrieve Firmware Boot Information */
    MCU_VersionType *version;        /**< @brief MCU version */
    MCU_ClkCfgIDType clkCfgID;       /**< @brief Clock configuration ID */
    int32_t correction;              /**< @brief Frequency correction */
    uint32_t *swtPort2TimeFifoMap;   /**< @brief Ethernet Switch to time
                                       hardware FIFO map */
    uint32_t stackingInfo;           /**< @brief Holds stacking Information */
    MCU_BootTimeInfoType *bootTime;  /**< @brief Boot time information */
} MCU_IOType;

#endif /* INCLUDE_BCM8956X_OSIL_MCU_OSIL_H */

/** @} */
