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
    @defgroup grp_wds_ifc Interface
    @ingroup grp_wds

    @addtogroup grp_wds_ifc
    @{
    Safety is a responsibility of every component in the system. With the same thought process,
    Watchdog Service is a responsibility of every component. Logical supervision is carried out
    in corresponding component context. Deadline monitoring may happen in client context or
    System Monitor context with support from component. Frequency supervision is performed in
    System Monitor context with support from component.

    Watchdog Service provides mechanism to define a supervision entity (component/client to
    define it). Watchdog Service shall monitor these supervision entities.
    Each supervision entity has configuration of a graph defining the state transitions
    relevant for the component. Logical monitoring shall ensure that component is flowing through
    the defined graph.

    For the sake of simplicity, deadline monitoring is performing from entry to exit of the graph
    i.e., time taken by component to traverse from starting state to ending state should be
    constrained and monitored.

    Frequency of monitoring is done by system task (the highest priority task defined by architecture
    performing system initialization).

    Note that the WDS_CHECK_TIME_US should be designed to ensure all Supervised Entities (which need
    frequency monitoring) will have a minimum frequency of at least 1.

    @section sec_wds_ifc_seq Sequence Diagram
    @startseq
    group {label = "client"; color = "#EFFFFF";  Client;};
    group {label = "server"; color = "#FFFFEF"; ServerTask; WDS;};
    ServerTask => WDS [label = "WDS_Init"];
    === Initialization Complete ===
    Client => WDS [label = "WDS_EnableSE"];
    loop {
        Client => WDS [label = "WDS_NotifyState"];
        ServerTask => WDS [label = "WDS_Monitor"];
    };
    Client => WDS [label = "WDS_DisableSE"];
    ServerTask => WDS [label = "WDS_DeInit"];
    @endseq

    @section sec_wds_ifc_fsm State Machine
    @startfsm
    INIT; DEINIT;
    INIT -> DEINIT;
    DEINIT -> INIT;
    @endfsm
    The WDS shall be in DEINIT state at start. On Initialization,
    it moves to INIT state. On deinitialization, it moves back to
    DEINIT state.

    @file wds.h
    @brief Interface File
    This header file contains the interface functions for watchdog service
    @version 0.1 Initial Version
*/

#ifndef WDS_H
#define WDS_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>

/**
    @name Watchdog Service Architecture IDs
    @{
    @brief Architecture IDs for Watchdog Service
*/
#define BRCM_SWARCH_WDS_MODE_MACRO                       (0x00U)   /**< @brief #WDS_ModeType    */
#define BRCM_SWARCH_WDS_CONFIG_TYPE                      (0x01U)   /**< @brief #WDS_ConfigType  */
#define BRCM_SWARCH_WDS_CONTEXT_TYPE                     (0x02U)   /**< @brief #WDS_ContextType */
#define BRCM_SWARCH_WDS_MONITOR_TYPE                     (0x03U)   /**< @brief #WDS_MonitorType */
#define BRCM_SWARCH_WDS_STATE_TYPE                       (0x04U)   /**< @brief #WDS_StateType   */
#define BRCM_SWARCH_WDS_SE_TYPE                          (0x05U)   /**< @brief #WDS_SEType      */
#define BRCM_SWARCH_WDS_ENABLESE_PROC                    (0x06U)   /**< @brief #WDS_EnableSE    */
#define BRCM_SWARCH_WDS_NOTIFYSTATE_PROC                 (0x07U)   /**< @brief #WDS_NotifyState */
#define BRCM_SWARCH_WDS_DISABLESE_PROC                   (0x08U)   /**< @brief #WDS_DisableSE   */
#define BRCM_SWARCH_WDS_INVALID_GRAPH_BUF_ENTRY_MACRO    (0x09U)   /**< @brief #WDS_INVALID_GRAPH_BUF_ENTRY   */
#define BRCM_SWARCH_WDS_WDS_SE_MAGIC_MACRO               (0x0AU)   /**< @brief #WDS_SE_MAGIC    */
#define BRCM_SWARCH_WDS_CALLBACK_TYPE                    (0x0BU)   /**< @brief #WDS_CbType      */
/** @} */

/**
    @brief Invalid entry for a SE graph

    Value to be used to indicate a empty entry for the transition
    graph within a SE.

    @trace #BRCM_SWREQ_WDS_INIT
 */
#define WDS_INVALID_GRAPH_BUF_ENTRY (0xFFU)

/**
    @brief Magic for Supervised Entity structure

    Magic value to be used within the SE structure.

    @trace #BRCM_SWREQ_WDS_INIT
 */
#define WDS_SE_MAGIC                (0x57445330UL)

/**
    @name Watchdog Service Type Masks
    @{
    @brief Supervision Mode type masks

    @trace #BRCM_SWREQ_WDS_INIT
*/
typedef uint32_t WDS_ModeType;
#define WDS_MODE_NONE       (0x0UL)    /**< @brief Logical monitoring mask */
#define WDS_MODE_LOGICAL    (0x1UL)    /**< @brief Logical monitoring mask */
#define WDS_MODE_DEADLINE   (0x2UL)    /**< @brief Deadline monitoring mask */
#define WDS_MODE_FREQ       (0x4UL)    /**< @brief Frequency monitoring mask */
/** @} */

/**
    @brief Configuration of Watchdog Service Supervision Entity

    Client shall provide this configuration to the supervision
    entity before enabling supervision.

    @trace #BRCM_SWREQ_WDS_INIT
 */
typedef struct _WDS_ConfigType {
    WDS_ModeType mode;          /**< @brief enabled modes   */
    uint32_t     deadlineInMs;  /**< @brief Deadline in milli seconds, that
                                     graph should traverse from start to end */
    uint32_t     minFrequency;  /**< @brief minimum number of times the graph
                                     should execute (from start to end) in
                                     #WDS_CHECK_TIME_US time */
    uint32_t     maxFrequency;  /**< @brief maximum number of times the graph
                                     should execute (from start to end) in
                                     #WDS_CHECK_TIME_US time */
    uint8_t      lStart;        /**< @brief start index in graph where client
                                     should enter (first event in flow), typically
                                     zero */
    uint8_t      lEnd;          /**< @brief end index in graph where client
                                     should exit (last event in flow) */
} WDS_ConfigType;

/**
    @brief Watchdog service Supervision entity context

    This structure is typically updated in the context of client through
    @ref WDS_EnableSE, @ref WDS_DisableSE, @ref WDS_NotifyState

    @trace #BRCM_SWREQ_WDS_INIT
 */
typedef struct _WDS_ContextType {
    uint32_t            isActive;   /**< @brief 0UL - SE not entered start,
                                         1UL - SE entered start */
    volatile uint64_t   time;       /**< @brief time at which client entered graph */
    volatile uint64_t   counter;    /**< @brief counter to hold graph entry count */
    volatile uint32_t   lErrorCnt;  /**< @brief detected Logical error count  */
    volatile uint32_t   dErrorCnt;  /**< @brief detected Deadline error count */
    WDS_ConfigType      config;     /**< @brief configuration of Supervision Entity */
    uint8_t             lCurrent;   /**< @brief current logical state in graph,
                                         valid only if isActive is 1UL */
} WDS_ContextType;

/**
    @brief Watchdog service Supervision entity Monitor type

    This structure holds previous mode and counter of the Supervision entity
    when @ref WDS_Monitor was called previously. Content is updated only in @ref WDS_Init
    and @ref WDS_Monitor contexts. @ref WDS_Monitor could monitor new values against
    old snapshot and decide if new errors are detected for reporting.

    Old mode relevant for start of frequency monitoring. WDS monitoring cycle may not be
    aligned to SE execution. For the first cycle of monitoring (after SE is enabled), it
    is OK to have a frequency count not in limits. To handle this scenario, old mode is
    preserved.
    @trace #BRCM_SWREQ_WDS_INIT
 */
typedef struct _WDS_MonitorType {
    WDS_ModeType    mode;       /**< @brief previously enabled modes   */
    uint64_t        counter;    /**< @brief previous counter to hold graph entry count */
    uint32_t        lErrorCnt;  /**< @brief detected Logical error count */
    uint32_t        fErrorCnt;  /**< @brief detected Frequency error count */
    uint32_t        dErrorCnt;  /**< @brief detected Deadline error count */
} WDS_MonitorType;

/** @brief WDS Callback to be raised by WDS_Monitor

    This callback will be raised by @ref WDS_Monitor function when an error
    is detected. Error value is also passed as part of the callback.
    This function shall not block the execution (no calls to WaitEvent in this context)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aName       Name of WDS SE (for information)
    @param[in]      aError      #BCM_ERR_CUSTOM when errors are detected
                                #BCM_ERR_MAX_ATTEMPS when errors reach max limit
    @param[in]      aMonitor    Monitor structure containing error information
    @param[in]      aMaxAllowedErrors    Maximum allowed errors by SE

    @return void (for void functions)

    @post None

    @trace #BRCM_SWREQ_WDS_INIT

    @limitations This function shall not block the execution context (No calls to
    WaitEvent allowed)
*/
typedef void (*WDS_CbType)(const char * aName, int32_t aError,
                            const WDS_MonitorType *aMonitor, uint32_t aMaxAllowedErrors);

/**
    @brief Graph Buffer type

    @trace #BRCM_SWREQ_WDS_INIT
*/
typedef uint8_t WDS_StateType;

/**
    @brief Structure for a supervision entity

    @trace #BRCM_SWREQ_WDS_INIT
*/
typedef struct _WDS_SEType {
    uint32_t            magic;          /**< @brief magic value: Ascii of WDS0 */
    const char          *name;          /**< @brief Name of Supervision Entity */
    WDS_ContextType     *context;       /**< @brief Context pointer            */
    WDS_MonitorType     *monitor;       /**< @brief Monitor context            */
    WDS_StateType       *buffer;        /**< @brief Graph Buffer:
                                                Allowed transitions for i'th state are located
                                                in buffer[i * maxTransitionsPerState] to
                                                buffer[(i+1) * maxTransitionsPerState - 1]*/
    WDS_CbType  callback;               /**< @brief callback to be raised when errors are detected */
    uint32_t    maxAllowedErrors;       /**< @brief Maximum allowed errors after which Watchdog
                                                won't be serviced */
    uint8_t     maxState;               /**< @brief maximum number of states possible
                                                for client */
    uint8_t     maxTransitionsPerState; /**< @brief maximum number of transitions possible
                                                per state */
} WDS_SEType;

/** @brief Enable monitoring WDS Supervision entity

    Updates mode in @ref WDS_ContextType with aMode

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]       aSE                Supervision Entity
    @param[in]       aConfig            Configuration and mode for supervision

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_INVAL_PARAMS    aSE is NULL or aConfig is NULL or
                                              if the #aConfig.mode is invalid or
                                              If lStart or lEnd is more than maxState
    @retval          #BCM_ERR_INVAL_MAGIC     aSE has invalid magic
    @retval          #BCM_ERR_INVAL_STATE     aSE is not in disabled state
    @retval          #BCM_ERR_OK              On success

    @post None

    @trace #BRCM_SWREQ_WDS_CLIENT
*/
int32_t WDS_EnableSE(const WDS_SEType *const aSE, const WDS_ConfigType *const aConfig);


/** @brief WDS notify checkpoint/state

    Verifies that aState is either an entry point state or a valid
    transition state. If entry point state is encountered while a graph
    is active, logical error count is incremented.
    <BR>If aState is an entry point state and deadline monitoring is enabled,
    a snapshot of current local time is preserved in nanoseconds in the context
    (for future monitoring).
    <BR>If aState is an exit point state and deadline monitoring is enabled,
    current time is checked against snapshot of entry time for configured deadline
    monitoring.
    <BR>Any detected logical errors and updated in the context.

    @behavior Sync, Non-reentrant

    @pre WDS Supervision Entity is enabled

    @param[in]       aSE                  Supervision Entity
    @param[in]       aState               Checkpoint/State value that has been
                                              reached by the client.

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_INVAL_PARAMS    aSE is NULL OR
                                              if #aState is invalid
    @retval          #BCM_ERR_INVAL_MAGIC     aSE has invalid magic
    @retval          #BCM_ERR_INVAL_STATE     WDS is not initialized OR
                                              if WDS_ContextType.mode is zero
    @retval          #BCM_ERR_OK              On success

    @post Context is updated with errors, states and other monitoring status

    @trace #BRCM_SWREQ_WDS_CLIENT
*/
int32_t WDS_NotifyState(const WDS_SEType *const aSE,
                              WDS_StateType aState);

/** @brief Disable Supervision entity from monitoring

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]       aSE                  Supervision Entity

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_INVAL_PARAMS    aSE is NULL
    @retval          #BCM_ERR_INVAL_MAGIC     aSE has invalid magic
    @retval          #BCM_ERR_OK              On success

    @post WDS Supervision entity is disabled

    @trace #BRCM_SWREQ_WDS_CLIENT
*/
int32_t WDS_DisableSE(const WDS_SEType *const aSE);

#endif /* WDS_H */
/** @} */
