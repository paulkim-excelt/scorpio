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
    @defgroup grp_pinmuxdrv_impl PinMux Client Design
    @ingroup grp_pinmuxdrv

    @addtogroup grp_pinmuxdrv_impl
    @{

    @file pinmux.c

    @version 0.1 Initial version
*/

#include <string.h>
#include <stddef.h>
#include <pinmux.h>
#include <pinmux_osil.h>
#include <bcm_err.h>
#include <compiler.h>
#include <utils.h>

/**
    @name PINMUX driver Design IDs
    @{
    @brief Design IDs for PINMUX driver
*/
#define BRCM_SWDSGN_PINMUX_REPORTERROR_PROC               (0x40U) /**< @brief #PINMUX_ReportError */
#define BRCM_SWDSGN_PINMUX_INIT_PROC                      (0x41U) /**< @brief #PINMUX_Init */
#define BRCM_SWDSGN_PINMUX_SET_PIN_DIRECTION_PROC         (0x42U) /**< @brief #PINMUX_SetPinDirection */
#define BRCM_SWDSGN_PINMUX_REFRESH_PORT_DIRECTION_PROC    (0x43U) /**< @brief #PINMUX_RefreshPortDirection */
#define BRCM_SWDSGN_PINMUX_SET_PIN_MODE_PROC              (0x44U) /**< @brief #PINMUX_SetPinMode */
/** @} */

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static void PINMUX_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_PMX_ID & BCM_LOGMASK_USER), 0U, aApiID, aErr,
            4UL, values);
}

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
void PINMUX_Init(void)
{
    PINMUX_IOType pinmuxIO;
    int32_t retVal;

    retVal = PINMUX_SysCmdReq(PINMUX_CMD_INIT, &pinmuxIO);
    (void)retVal;
}

/**
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
void PINMUX_SetPinDirection(PINMUX_PinType aPin, PINMUX_PinDirectionType aDirection)
{
    PINMUX_IOType pinmuxIO;
    int32_t retVal;

    pinmuxIO.pin = aPin;
    pinmuxIO.direction = aDirection;

    retVal = PINMUX_SysCmdReq(PINMUX_CMD_SET_PIN_DIRECTION, &pinmuxIO);
    if (BCM_ERR_OK != retVal) {
        PINMUX_ReportError(BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC, retVal, 0UL,
                           0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
void PINMUX_RefreshPortDirection(void)
{
    PINMUX_IOType pinmuxIO;
    int32_t retVal;

    retVal = PINMUX_SysCmdReq(PINMUX_CMD_REFRESH_PORT_DIRECTION, &pinmuxIO);
    if (BCM_ERR_OK != retVal) {
        PINMUX_ReportError(BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC, retVal, 0UL,
                           0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
void PINMUX_SetPinMode(PINMUX_PinType aPin, PINMUX_PinModeType aMode)
{
    PINMUX_IOType pinmuxIO;
    int32_t retVal;

    pinmuxIO.pin = aPin;
    pinmuxIO.mode = aMode;

    retVal = PINMUX_SysCmdReq(PINMUX_CMD_SET_PIN_MODE, &pinmuxIO);
    if (BCM_ERR_OK != retVal) {
        PINMUX_ReportError(BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC, retVal, 0UL, 0UL,
                           0UL, 0UL);
    }
}


/** @} */
