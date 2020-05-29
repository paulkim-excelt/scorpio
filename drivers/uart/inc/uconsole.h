/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_uconsoledrv_ifc UART Console
    @ingroup grp_uartdrv

    @addtogroup grp_uconsoledrv_ifc
    @{


    @file uconsole.h
    @brief Interface for UART Console

    @version 0.1 Initial version
*/

#ifndef UCONSOLE_H
#define UCONSOLE_H

#include <uart.h>

/**
    @name UCONSOLE API IDs
    @{
    @brief API IDs for UCONSOLE
 */
#define BRCM_SWARCH_UCONSOLE_WRITE_PROC     (0x20U) /**< @brief #UCONSOLE_Write    */
#define BRCM_SWARCH_UCONSOLE_READ_PROC      (0x21U) /**< @brief #UCONSOLE_Read     */
/** @} */

/** @brief Send data over uart console

    @behavior Async, Non-Reentrant

    @pre None

    @param[in]      aHWId       Index of the UART controller
    @param[in]      aBuffer     Pointer to data to be sent
    @param[in]      aSize       Length of data to be sent

    @retval     #BCM_ERR_INVAL_PARAMS   aBuffer is NULL and aSize is not zero
                #BCM_ERR_UNKNOWN        UART Driver has reported error
                #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_UCONSOLE_COMMUNICATION

    @limitations None
*/
extern int32_t UCONSOLE_Write(UART_HwIDType aHWId, const char * const aBuffer, uint32_t aSize);

/** @brief Send data over uart

    This function can be called with appropriate data and size to push data over UART.
    If the function is called with aBuffer is NULL and size is zero, it will trigger UART with
    pending data.

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aData       Pointer to data to be sent
    @param[in]      aSize       Length of data to be sent

    @return     #BCM_ERR_INVAL_PARAMS   aCmdStr is NULL or
                                        aSize is NULL or
                                        *aSize is zero
                #BCM_ERR_OK             Success

    @post UART driver state changes to #UART_ENABLED

    @trace  #BRCM_SWREQ_UCONSOLE_COMMUNICATION

    @limitations None
*/
extern int32_t UCONSOLE_Read(char * const aCmdStr, uint32_t *aSize);
#endif /* UCONSOLE_H */

/** @} */
