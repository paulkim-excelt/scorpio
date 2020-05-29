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

/** @defgroup grp_pinmuxdrv_il PINMUX Integration Guide
    @ingroup grp_pinmuxdrv

    @addtogroup grp_pinmuxdrv_il
    @{
    @section sec_pinmux_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes PinMux Driver IL (Integration Layer) in
    detail.

    PinMux driver is implemented as a #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for PinMux Driver on
    to #SysCmdHandler.

    @section sec_pinmux_il_seq Sequence Diagram
    Below figure describes the sequence of #PINMUX_SetPinDirection API. Other
    PinMux APIs also follow a similar sequence as depicted below:

    @image html pinmux_seq_il_diagram_svc.jpg "PinMux Driver SVC Layer interaction"

    Brief description of life lines shown in above sequence diagram:
    - @b Client: Client or user layer
    - <b>PinMux Driver Interface</b>: PinMux driver interface layer
    - <b>SVC Layer</b>: SVC Layer to be implemented by the integrator
    - <b>PinMux Driver IL</b>: PinMux Driver Integration Layer
    - <b>PinMux Driver</b>: PinMux driver


    @file pinmux_osil.h
    @brief PinMux driver Integration Interfaces
    This header file contains the integration interface for PinMux driver
    @version 1.12 Imported from docx
*/

#ifndef PINMUX_OSIL_H
#define PINMUX_OSIL_H

#include <system.h>
#include <pinmux.h>

/**
    @name PinMux interface IDs
    @{
    @brief PinMux driver integration interface IDs
*/
#define BRCM_SWARCH_PINMUX_CMD_TYPE                (0x80U) /**< @brief #PINMUX_CmdType */
#define BRCM_SWARCH_PINMUX_IO_TYPE                 (0x81U) /**< @brief #PINMUX_IOType */
#define BRCM_SWARCH_PINMUX_SYS_CMD_REQ_PROC        (0x82U) /**< @brief #PINMUX_SysCmdReq */
#define BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC    (0x83U) /**< @brief #PINMUX_SysCmdHandler */
#define BRCM_SWARCH_PINMUX_SETPADCONFIG_PROC       (0x84U) /**< @brief #PINMUX_SetPadCfg */
/** @} */

/**
    @brief PinMux command type

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_CmdType;
#define PINMUX_CMD_INIT                     (0UL)
#define PINMUX_CMD_SET_PIN_DIRECTION        (1UL)
#define PINMUX_CMD_REFRESH_PORT_DIRECTION   (2UL)
#define PINMUX_CMD_SET_PIN_MODE             (3UL)

/**
    @brief PinMux IO structure

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
 */
typedef struct _PINMUX_IOType {
    int32_t retVal;                         /**< @brief Return value as set by #PINMUX_SysCmdHandler */
    PINMUX_PinType pin;                     /**< @brief ID of PinMux pin */
    PINMUX_PinDirectionType direction;      /**< @brief PinMux Pin Direction */
    PINMUX_PinModeType mode;                /**< @brief PinMux Pin Mode */
} PINMUX_IOType;

/** @brief Raise System Command Request

    Interface to raise System Command Request for PinMux driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by the user
    @param[inout]   aPINMUXIO   Pointer to In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic ID
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  As returned by #PINMUX_CmdHandler

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t PINMUX_SysCmdReq(PINMUX_CmdType aCmd, PINMUX_IOType *aPINMUXIO);

/** @brief Set Pad Configuration

    This interface sets pad configuration for a given pin

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aPin        Pin to be configured
    @param[inout]   aMode       Pin Mode to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command processed successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command

    @post TBD

    @note This interface shall be implemented for each Chip Family.

    @limitations None

    @trace  #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
extern int32_t PINMUX_SetPadCfg(PINMUX_PinType aPin, PINMUX_PinModeType aMode);

/** @brief Handle system request commands

    This interface handles all the PinMUX driver commands requested from System
    command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    PinMux driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #PINMUXIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_PINMUX_KERNEL_HANDLER

    @limitations None
*/
extern void PINMUX_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);
#endif /* PINMUX_OSIL_H */
/** @} */
