/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_wdtdrv_il Integration Guide
    @ingroup grp_wdtdrv

    @addtogroup grp_wdtdrv_il
    @{
    @section sec_wdt_integration Integration Guide
    Detailed Integration Guide to be populated here.
    Any differences in specific documentation is described here.
    Other templates shall be picked up from wdt.h.

    @section sec_wdt_il_seq Sequence Diagram
    @subsection subsec_wdt_it_seq_svc SVC Layer Interaction
    @image html wdt_seq_il_diagram_svc.jpg "SVC Layer Interaction"
    #WDT_SetMode and #WDT_SetTriggerCondition also follow a similar sequence as
    #WDT_Init.

    @subsection subsec_wdt_it_seq_service Watchdog servicing
    @image html wdt_seq_il_diagram_service.jpg "Watchdog servicing"

    @file wdt_osil.h
    @brief Watchdog driver Integration Interfaces
    This header file contains the integration interface for Watchdog driver
    @version 1.01 Imported from docx
*/

#ifndef WDT_OSIL_H
#define WDT_OSIL_H

#include <wdt.h>
#include <system.h>

/**
    @name Watchdog driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for Watchdog driver
*/
#define BRCM_SWARCH_WDT_CMD_TYPE             (0x40U) /**< @brief #WDT_CmdType       */
#define BRCM_SWARCH_WDT_IO_TYPE              (0x41U) /**< @brief #WDT_IOType        */
#define BRCM_SWARCH_WDT_SYSCMDREQ_PROC       (0x42U) /**< @brief #WDT_SysCmdReq     */
#define BRCM_SWARCH_WDT_SYSCMDHANDLER_PROC   (0x43U) /**< @brief #WDT_SysCmdHandler */
/** @} */

/**
    @brief Watchdog system command type

    Watchdog system command request type

    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef uint32_t WDT_CmdType;
#define WDT_CMD_INIT            (0UL)   /**< Init command                   */
#define WDT_CMD_SET_MODE        (1UL)   /**< SetMode command                */
#define WDT_CMD_SET_TRIG_COND   (2UL)   /**< SetTriggerCondition command    */
#define WDT_CMD_SERVICE         (3UL)   /**< Service Watchdog command       */

/**
    @brief IO structure for watchdog driver

    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef struct _WDT_IOType {
    int32_t             retVal;             /**< @brief Return value set by #WDT_SysCmdHandler  */
    WDT_HwIDType        hwID;               /**< @brief Hardware ID                             */
    const WDT_CfgType   *config;           /**< @brief Pointer to configuration structure      */
    WDT_ModeType        mode;               /**< @brief Mode of watchdog driver                 */
    uint32_t            timeout_ms;         /**< @brief Timeout value of the watchdog
                                                timer in milliseconds                           */
} WDT_IOType;


/** @brief Raise a command request to Watchdog driver

    This interface raises a command request to the Watchdog driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aWDTIO      Pointer to in-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request raised successfully
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  As returned by WDT_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic

    @post TBD

    @note interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_WDT_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t WDT_SysCmdReq(WDT_CmdType aCmd, WDT_IOType *const aWDTIO);

/** @brief Handles all the Watchdog driver commands

    This interface handles all the watchdog driver commands requested from
    System command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    Watchdog driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #WDT_IOType

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_WDT_KERNEL_HANDLER

    @limitations None
*/
extern void WDT_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

#endif /* WDT_OSIL_H */
/** @} */
