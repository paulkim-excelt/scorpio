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
    @defgroup grp_timdrv_ifc Interface
    @ingroup grp_timdrv

    @addtogroup grp_timdrv_ifc
    @{
    @section sec_gpt_overview Overview
    @image html gpt_overview.jpg "Driver Overview Diagram"
    GPT driver provides APIs to start/stop the timer and to get the time
    elapsed/remaining. It also provides APIs to enable/disable notifications at
    runtime.

    The driver supports the following three timer modes:
    - Free running
    - Periodic
    - One shot

    @section sec_gpt_fsm State Machine
    @image html gpt_state_machine.jpg "Timer state machine"

    @section sec_gpt_seq Sequence Diagrams
    @subsection subsec_gpt_seq_one_shot One shot timer
    @image html gpt_seq_diagram_one_shot.jpg "One-shot timer sequence diagram"

    @subsection subsec_gpt_seq_periodic Periodic timer
    @image html gpt_seq_diagram_periodic.jpg "Periodic timer sequence diagram"

    @subsection subsec_gpt_seq_free_runnning Free running timer
    @image html gpt_seq_diagram_free_runnning.jpg "Free running timer sequence diagram"

    @section sec_gpt_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_gpt.dep

    @limitations None

    @file gpt.h
    @brief GPT driver application interface
    This header file contains the interfaces for GPT driver
    @version 0.30 Imported from docx
*/

#ifndef GPT_H
#define GPT_H

#include <stdint.h>

/**
    @name GPT driver API IDs
    @{
    @brief API IDs for GPT driver
*/
#define BRCM_SWARCH_TIM_CHANID_TYPE                   (0x00U) /**< @brief #TIM_ChanIDType */
#define BRCM_SWARCH_TIM_WIDTH_TYPE                    (0x01U) /**< @brief #TIM_WidthType */
#define BRCM_SWARCH_TIM_TICK_TYPE                     (0x02U) /**< @brief #TIM_TickType */
#define BRCM_SWARCH_TIM_CHAN_MODE_TYPE                 (0x03U) /**< @brief #TIM_ChanModeType */
#define BRCM_SWARCH_TIM_CB_PROC                       (0x04U) /**< @brief #TIM_CbType */
#define BRCM_SWARCH_TIM_PRESCALE_TYPE                 (0x05U) /**< @brief #TIM_PrescaleType */
#define BRCM_SWARCH_TIM_CONFIG_TYPE                   (0x06U) /**< @brief #TIM_ConfigType */
#define BRCM_SWARCH_TIM_INIT_PROC                     (0x07U) /**< @brief #TIM_Init */
#define BRCM_SWARCH_TIM_DEINIT_PROC                   (0x08U) /**< @brief #TIM_DeInit */
#define BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC           (0x09U) /**< @brief #TIM_GetTimeElapsed */
#define BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC         (0x0AU) /**< @brief #TIM_GetTimeRemaining */
#define BRCM_SWARCH_TIM_STARTTIMER_PROC               (0x0BU) /**< @brief #TIM_StartTimer */
#define BRCM_SWARCH_TIM_STOPTIMER_PROC                (0x0CU) /**< @brief #TIM_StopTimer */
#define BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC       (0x0DU) /**< @brief #TIM_EnableNotification */
#define BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC      (0x0EU) /**< @brief #TIM_DisableNotification */
#define BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC       (0x0FU) /**< @brief #TIM_GetFreeRunningTime */
#define BRCM_SWARCH_TIM_GETTIME_PROC                  (0x10U) /**< @brief #TIM_GetTime */
#define BRCM_SWARCH_TIM_GETSYSTEMTIME_PROC            (0x11U) /**< @brief #TIM_GetSystemTime */
#define BRCM_SWARCH_TIM_GETTIMERAW_PROC               (0x12U) /**< @brief #TIM_GetTimeRaw */
#define BRCM_SWARCH_TIM_SYSTEMSTARTTIMER_PROC         (0x13U) /**< @brief #TIM_SystemStartTimer */
#define BRCM_SWARCH_TIM_SYSTEMSTOPTIMER_PROC          (0x14U) /**< @brief #TIM_SystemStopTimer */
#define BRCM_SWARCH_TIM_SYSTEMENABLENOTIFICATION_PROC (0x15U) /**< @brief #TIM_SystemEnableNotification */
#define BRCM_SWARCH_TIM_SYSTEMDISABLENOTIFICATION_PROC (0x16U) /**< @brief #TIM_SystemDisableNotification */
/** @} */

/**
    @brief Index of the Timer channel

    Index of the Timer channel (shall be less than #TIM_MAX_HW_ID).

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef uint32_t TIM_ChanIDType;        /**< @brief Timer HW Channel ID Type */

/**
    @name TIM_WidthType
    @{
    @brief Width of the Timer counter

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef uint32_t TIM_WidthType;             /**< @brief Timer counter width type */
#define TIM_WIDTH_16_BIT            (0UL)   /**< @brief 16 bit counter */
#define TIM_WIDTH_32_BIT            (1UL)   /**< @brief 32 bit counter */
/** @} */

/**
    @brief Timer ticks

    Timer ticks type

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef uint32_t TIM_TickType;

/**
    @name TIM_ChanModeType
    @{
    @brief Mode of the timer channel

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef uint32_t TIM_ChanModeType;         /**< @brief Timer channel mode type */
#define TIM_CHAN_MODE_FREE_RUNNING  (0UL)   /**< @brief Free running mode. The
                                                timer runs continuously and
                                                restarts each time when it
                                                reaches the max value
                                                (0xFFFFFFFF if its 32 bit timer
                                                or 0xFFFF if its 16 bit timer). */
#define TIM_CHAN_MODE_PERIODIC      (1UL)   /**< @brief Periodic mode. The timer
                                                runs continuously and restarts
                                                each time when it reaches
                                                target value. */
#define TIM_CHAN_MODE_ONE_SHOT      (2UL)   /**< @brief One shot mode. The timer
                                                runs till it reaches the target
                                                value and then stops. */
/** @} */

/** @brief Timer callback type

    This is the function prototype for timer callback. Timer driver gives a
    callback when the target time is reached.

    @note
    -# This shall be implemented by the client.
    -# This callback is raised in interrupt context, so callback function shall
    return as soon as possible (and avoid any heavy processing in the callback).

    @behavior Sync, Re-entrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM_KERNEL_INTERFACE

    @limitations None
*/
typedef void (*TIM_CbType)(void);

/**
    @name Timer pre-scaler
    @{
    @brief Prescale division values


    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef uint32_t TIM_PrescaleType;
#define TIM_PRESCALE_DIV_1      (0UL)   /**< @brief Division by 1 */
#define TIM_PRESCALE_DIV_16     (1UL)   /**< @brief Division by 16 */
#define TIM_PRESCALE_DIV_256    (2UL)   /**< @brief Division by 256 */
/** @} */

/**
    @brief Timer configuration

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef struct _TIM_ConfigType {
    uint32_t prescale;          /**< @brief Prescale division value
                                     - #TIM_PRESCALE_DIV_1
                                     - #TIM_PRESCALE_DIV_16
                                     - #TIM_PRESCALE_DIV_256 */
    uint32_t sysTimeEn;         /**< @brief Flag to indicate the timer usage
                                     - #TRUE
                                     - #FALSE */
    TIM_ChanIDType chanID;      /**< @brief Channel ID */
    TIM_ChanModeType chanMode;  /**< @brief Channel mode */
    TIM_WidthType width;        /**< @brief Timer counter width */
    TIM_CbType const cb;        /**< @brief Interrupt notification callback. It
                                     shall be NULL if chanMode is
                                     #TIM_CHAN_MODE_FREE_RUNNING */
} TIM_ConfigType;

/** @brief Initialize timer channel

    This API initializes the given Timer channel with the given configuration.

    @note If any channel is configured for free running mode, it is
    automatically started during initialization. The client shall not call
    #TIM_StartTimer explicitly.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aChanID     Timer channel ID
    @param[in]      aConfig     Timer Configuration structure

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern void TIM_Init(TIM_ChanIDType aChanID,
                     const TIM_ConfigType *const aConfig);

/** @brief De-initialize timer channel

    This API de-initializes the given Timer channel.

    @note
    -# All timers (except timers in free running mode) shall either be
    stopped explicitly or expire by itself before de-initialization.
    -# If the timer is running (except free running timers), this API shall
    return without taking any action.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern void TIM_DeInit(TIM_ChanIDType aChanID);

/** @brief Get time already elapsed

    This API returns the time already elapsed

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @retval     #TIM_TickType           Returns
                                        - Elapsed time in ticks if timer is
                                        running
                                        - Elapsed time in ticks at the moment of
                                        stopping if timer was already stopped
                                        - Target time (which was passed during
                                        #TIM_StartTimer) in ticks if timer has
                                        already expired
    @retval     0UL                     If
                                        - Timer configured for free running mode
                                        - Timer driver not initialized
                                        - Invalid channel ID
                                        - Timer initialized but not started

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern TIM_TickType TIM_GetTimeElapsed(TIM_ChanIDType aChanID);

/** @brief Get timer value remaining

    This API returns the timer value remaining until the target time will be
    reached next time.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @retval     #TIM_TickType           Returns
                                        - Remaining time in ticks if timer is
                                        running
                                        - Remaining time in ticks at the moment
                                        of stopping if timer was already stopped
    @retval     0UL                     If
                                        - Timer configured for free running mode
                                        - Timer driver not initialized
                                        - Invalid channel ID
                                        - Timer initialized but not started
                                        - Timer expired

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern TIM_TickType TIM_GetTimeRemaining(TIM_ChanIDType aChanID);

/** @brief Starts the selected timer channel

    This API starts the selected timer channel with the defined target time.

    @note If timer is configured for free running mode, this API has no effect.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID
    @param[in]      aValue      Target time in number of ticks

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern void TIM_StartTimer(TIM_ChanIDType aChanID,
                           TIM_TickType aValue);

/** @brief Stop the selected timer channel

    This API stops the selected timer channel.

    @note If timer is configured for free running mode, this API has no effect.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern void TIM_StopTimer(TIM_ChanIDType aChanID);

/** @brief Enable notification

    This API enables the interrupt notification of the selected channel
    (aChanID). If timer is configured for either one shot or periodic mode, the
    client gets a callback notification when the target time is reached.

    @note If timer (aChanID) is configured for free running mode, this API has
    no effect.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM_NOTIFICATION

    @limitations None
*/
extern void TIM_EnableNotification(TIM_ChanIDType aChanID);

/** @brief Disable notification

    This API disables the interrupt notification of the selected channel
    (aChanID). The client will not get a callback notification when the target
    time is reached.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM_NOTIFICATION

    @limitations None
*/
extern void TIM_DisableNotification(TIM_ChanIDType aChanID);

/** @brief Get current time of the free running timer

    This API returns the current time of the free running timer. When the timer
    counter is wrapped around, the elapsed time from the timer max value is
    returned.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @retval     #TIM_TickType           Current time in ticks
    @retval     0UL                     - Timer driver not initialized
                                        - It’s not a free running timer

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern TIM_TickType TIM_GetFreeRunningTime(TIM_ChanIDType aChanID);

/** @brief Get current time of system timer

    This API returns the current time of the timer which is configured for
    system time usage.

    @behavior Sync, Re-entrant

    @pre TBD

    @param          void

    @retval     uint64_t                Current time in ms
    @retval     0UL                     If no timer is configured for system
                                        time usage

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern uint64_t TIM_GetTime(void);

/** @brief Get current time of system timer

    This API returns the current time of the timer which is configured for
    system time usage

    @behavior Sync, Re-entrant

    @pre TBD

    @param          void

    @retval     uint64_t                Current time in ms
    @retval     0UL                     If no timer is configured for system
                                        time usage

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations This API shall be called only in privileged context.
*/
extern uint64_t TIM_GetSystemTime(void);

/** @brief Get the raw value of the timer in ns

    This API returns the raw value of the timer hardware in nano seconds.
    This API can be called without initialization of the timer
    (without calling TIM_Init())

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aChanID     Timer channel ID

    @retval     uint64_t                Current time in ns
    @retval     0UL                     If no timer is configured for system
                                        time usage

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations This is a privileged API and can be called only when processor
                 is running in privileged mode.
*/
extern uint64_t TIM_GetTimeRaw(TIM_ChanIDType aChanID);

/** @brief Starts the selected timer channel

    This priviledged API starts the selected timer channel with the defined
    target time.

    @note If timer is configured for free running mode, this API has no effect.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID
    @param[in]      aValue      Target time in number of ticks

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern void TIM_SystemStartTimer(TIM_ChanIDType aChanID,
                           TIM_TickType aValue);

/** @brief Stop the selected timer channel

    This priviledged API stops the selected timer channel.

    @note If timer is configured for free running mode, this API has no effect.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM

    @limitations None
*/
extern void TIM_SystemStopTimer(TIM_ChanIDType aChanID);

/** @brief Enable notification

    This priviledged API enables the interrupt notification of the selected
    channel(aChanID). If timer is configured for either one shot or periodic
    mode, the client gets a callback notification when the target time is
    reached.

    @note If timer (aChanID) is configured for free running mode, this API has
    no effect.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM_NOTIFICATION

    @limitations None
*/
extern void TIM_SystemEnableNotification(TIM_ChanIDType aChanID);

/** @brief Disable notification

    This priviledged API disables the interrupt notification of the selected
    channel(aChanID). The client will not get a callback notification when the
    target time is reached.

    @behavior Sync, Re-entrant for different aChanID

    @pre TBD

    @param[in]      aChanID     Timer channel ID

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_TIM_NOTIFICATION

    @limitations None
*/
extern void TIM_SystemDisableNotification(TIM_ChanIDType aChanID);

#endif /* GPT_H */
/** @} */
