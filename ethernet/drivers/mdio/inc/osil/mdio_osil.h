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
/** @defgroup grp_mdiodrv_il Integration Guide
    @ingroup grp_mdiodrv

    @addtogroup grp_mdiodrv_il
    @{
    @section sec_mdio_integration Integration Guide
    TDB

    @section sec_mdio_il_seq Sequence Diagram
    @subsection subsec_mdio_il_sync_seq MDIO Synchronous Operation IL sequence
    TDB

    @subsection subsec_mdio_il_async_seq MDIO Asynchronous Operation IL sequence
    TDB

    @subsection subsec_mdio_il_cancel_seq MDIO Cancel Operation IL sequence
    TDB

    @file mdio_osil.h
    @brief MDIO Integration Interfaces
    This header file contains the integration interface for MDIO driver.
    @version 0.01 Initial version
*/
#ifndef MDIO_OSIL_H
#define MDIO_OSIL_H

#include <mdio.h>
#include <system.h>

/**
    @name MDIO interface IDs
    @{
    @brief MDIO integration interface IDs.
*/
#define BRCM_SWARCH_MDIO_STATEHANDLER_PROC  (0x40U) /**< @brief #MDIO_StateHandler  */
#define BRCM_SWARCH_MDIO_TIMERCB_PROC       (0x41U) /**< @brief #MDIO_TimerCb */
#define BRCM_SWARCH_MDIO_GETINFO_PROC       (0x42U) /**< @brief #MDIO_GetInfo */
#define BRCM_SWARCH_MDIO_STARTTIMER_PROC    (0x43U) /**< @brief #MDIO_StartTimer */
#define BRCM_SWARCH_MDIO_STOPTIMER_PROC     (0x44U) /**< @brief #MDIO_StopTimer */
/** @} */

/** @brief MDIO State handler

    Function to handle mdio driver states. This function shall be called from
    MDIO timer callback. This function would out job completion flag, if atleast
    a job is completed during that call. If flag out is #TRUE, a notification
    to client shall be sent.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[out]     aIsJobComp  Pointer to retrieve the flag, which indicates
                                if atleast a job is complete during state
                                machine processing. If flag retrieved is
                                #TRUE, a job completion notification to client
                                shall be sent.

    @return     void

    @post TBD

    @note This interface shall be called from #MDIO_TimerCb function,
    on expiration of high resolution timer.

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern void MDIO_StateHandler(uint32_t *const aIsJobComp);

/** @brief MDIO timer callback function

    Function to be registered as callback function to one channel of timer
    driver. This function must be invoked on expiration of this timer.
    This function would be invoked in interrupt mode by timer driver. This
    function would call #MDIO_StateHandler to process MDIO drive state machine.

    @behavior Sync, Non Re-entrant (if aID is different)

    @pre TBD

    @param      void

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern void MDIO_TimerCb(void);

/** @brief Get MDIO driver information

    API to query to retrieve driver information like timer start/stop command.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[out]     aTimerCmd   Pointer to retrieve timer command

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Controller info is successfully
                                        retrieved.
    @retval     #BCM_ERR_INVAL_PARAMS   (aID is invalid) or
                                        (aTimerAct is NULL)

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern int32_t MDIO_GetInfo(MDIO_TimerCmdType *const aTimerCmd);

/** @brief Start timer

    API to start timer, upon whose interrupt, MDIO controller is polled

    @behavior Sync, Re-entrant

    @pre TBD

    @param  void

    @return void

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern void MDIO_StartTimer();

/** @brief Stop timer

    API to stop timer events

    @behavior Sync, Re-entrant

    @pre TBD

    @param  void

    @return void

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern void MDIO_StopTimer();

#endif /* MDIO_OSIL_H */
/** @} */
