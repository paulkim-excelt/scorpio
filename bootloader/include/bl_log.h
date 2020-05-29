/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_bl_utils_log Logging
    @ingroup grp_bl_utils

    @addtogroup grp_bl_utils_log
    @{
    @section sec_bl_utils_log_overview Overview
    This section describes the various logging related APIs.
    <BR>Print debug messages on the port(s). Each module need to implement GetModuleLogLevel
    API which shall return the current log level of the module. If the module log level is
    greater than print level (LOG_XXX) then message will be printed on the port

    @code{.c}
    uint32_t BL_GetAPPLogLevel();
    #define BL_GetModuleLogLevel BL_GetAPPLogLevel
    uint32_t BL_GetAPPLogLevel() { return BL_LOG_LVL_INFO; }
    @endcode

    @file bl_log.h
    @brief debug print macros

    @version 0.30 Imported from docx
*/

#ifndef BL_LOG_H
#define BL_LOG_H

#include <stdint.h>
#include <bcm_comp.h>
#include <bcm_err.h>

/**
    @name Logging API IDs
    @{
    @brief API IDs for BCM Logging
*/
#define BRCM_ARCH_BL_UTILS_LOG_LEVELS_MACRO             (0x00U) /**< @brief #BL_LOG_LVL_NONE */
#define BRCM_ARCH_BL_UTILS_LOG_STR_MAX_SIZE_MACRO       (0x01U) /**< @brief #BL_LOG_STR_MAX_SIZE */
#define BRCM_ARCH_BL_UTILS_LOG_STATIC_LOG_LEVEL_MACRO   (0x02U) /**< @brief #BL_STATIC_LOG_LEVEL */
#define BRCM_ARCH_BL_UTILS_LOG_DYNAMIC_LOG_LEVEL_MACRO  (0x03U) /**< @brief #BL_DYNAMIC_LOG_LEVEL */
#define BRCM_ARCH_BL_UTILS_LOG_CRIT_MACRO               (0x04U) /**< @brief #BL_LOG_CRIT */
#define BRCM_ARCH_BL_UTILS_LOG_ERR_MACRO                (0x05U) /**< @brief #BL_LOG_ERR */
#define BRCM_ARCH_BL_UTILS_LOG_INFO_MACRO               (0x06U) /**< @brief #BL_LOG_INFO */
#define BRCM_ARCH_BL_UTILS_LOG_VERB_MACRO               (0x07U) /**< @brief #BL_LOG_VERB */
#define BRCM_ARCH_BL_UTILS_LOG_PRINTF_PROC              (0x07U) /**< @brief #BL_Log_Printf */
/** @} */

/**
    @name Component Setting
    @{
    @brief Debug Log levels
    @trace #BRCM_ARCH_BL_UTILS_LOG_LEVELS_MACRO #BRCM_REQ_BL_UTILS_LOG_LEVELS
*/
#define BL_LOG_LVL_NONE        (0UL)                /**< @brief log level: None */
#define BL_LOG_LVL_CRITICAL    (1UL)                /**< @brief log level: Critical */
#define BL_LOG_LVL_ERROR       (2UL)                /**< @brief log level: Error */
#define BL_LOG_LVL_INFO        (3UL)                /**< @brief log level: Info */
#define BL_LOG_LVL_VERBOSE     (4UL)                /**< @brief log level: Verbose */
#define BL_LOG_LVL_MAX         (BL_LOG_LVL_VERBOSE) /**< @brief Maximum log level */
/** @} */

/**
    @brief Maximum supported length of log string

    @trace #BRCM_ARCH_BL_UTILS_LOG_STR_MAX_SIZE_MACRO #BRCM_REQ_BL_UTILS_LOG_STR_MAX_SIZE
*/
#define BL_LOG_STR_MAX_SIZE    (128UL)

#ifndef BL_STATIC_LOG_LEVEL
/**
    @brief Compilation macro for static log level

    @trace #BRCM_ARCH_BL_UTILS_LOG_STR_MAX_SIZE_MACRO #BRCM_REQ_BL_UTILS_LOG_STR_MAX_SIZE
*/
#define BL_STATIC_LOG_LEVEL    (BL_LOG_LVL_INFO)
#endif /* BL_STATIC_LOG_LEVEL */


#ifndef BL_DYNAMIC_LOG_LEVEL
/** @brief Dynamic log level

    @trace #BRCM_ARCH_BL_UTILS_LOG_DYNAMIC_LOG_LEVEL_MACRO #BRCM_REQ_BL_UTILS_LOG_DYNAMIC_LOG_LEVEL
    @limitations In the current version, feature to change log level dynamically is not supported.
*/
#define BL_DYNAMIC_LOG_LEVEL   (BL_LOG_LVL_ERROR)
#endif /* DYNAMIC_LOG_LEVEL */

#if (BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_CRITICAL)
/** @brief Print critical message

    This API prints a critical message to the log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_BL_UTILS_LOG_CRIT_MACRO #BRCM_REQ_BL_UTILS_LOG_CRIT

    @limitations
    - All characters beyond #BL_LOG_STR_MAX_SIZE will be cut off.
    - If the static/dynamic log level is set to a value less than #BL_LOG_LVL_CRITICAL, no critical/error/info/verbose
    messages shall be logged.
*/
#define BL_LOG_CRIT(...)                                        \
    do {                                                        \
        if (BL_GetModuleLogLevel() >= BL_LOG_LVL_CRITICAL) {    \
            BL_Log_Printf(BL_LOG_STR_MAX_SIZE, __VA_ARGS__);    \
        }                                                       \
    } while (0)
#else   /* BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_CRITICAL */
#define BL_LOG_CRIT(...)
#endif

#if (BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_ERROR)
/** @brief Print error message

    This API prints a error message to the log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_BL_UTILS_LOG_ERR_MACRO #BRCM_REQ_BL_UTILS_LOG_ERR

    @limitations
    - All characters beyond #BL_LOG_STR_MAX_SIZE will be cut off.
    - If the static/dynamic log level is set to a value less than #BL_LOG_LVL_ERROR, no error/info/verbose messages shall
    be logged.
*/
#define BL_LOG_ERR(...)                                         \
    do {                                                        \
        if (BL_GetModuleLogLevel() >= BL_LOG_LVL_ERROR) {       \
            BL_Log_Printf(BL_LOG_STR_MAX_SIZE, __VA_ARGS__);    \
        }                                                       \
    } while (0)
#else   /* BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_ERROR */
#define BL_LOG_ERR(...)
#endif

#if (BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_INFO)
/** @brief Print informatory message

    This API prints a informatory message to the log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_BL_UTILS_LOG_INFO_MACRO #BRCM_REQ_BL_UTILS_LOG_INFO

    @limitations
    - All characters beyond #BL_LOG_STR_MAX_SIZE will be cut off.
    - If the static/dynamic log level is set to a value less than #BL_LOG_LVL_INFO, no info/verbose messages shall be logged.
*/
#define BL_LOG_INFO(...)                                        \
    do {                                                        \
        if (BL_GetModuleLogLevel() >= BL_LOG_LVL_INFO) {        \
            BL_Log_Printf(BL_LOG_STR_MAX_SIZE, __VA_ARGS__);    \
        }                                                       \
    } while (0)
#else   /* BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_INFO */
#define BL_LOG_INFO(...)
#endif

#if (BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_VERBOSE)
/** @brief Print verbose message

    This API prints a verbose message to the log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_BL_UTILS_LOG_VERB_MACRO #BRCM_REQ_BL_UTILS_LOG_VERB

    @limitations
    - All characters beyond #BL_LOG_STR_MAX_SIZE will be cut off.
    - If the static/dynamic log level is set to a value less than #BL_LOG_LVL_VERB, no verbose messages shall be logged.
*/
#define BL_LOG_VERB(...)                                        \
    do {                                                        \
        if (BL_GetModuleLogLevel() >= BL_LOG_LVL_VERBOSE) {     \
            BL_Log_Printf(BL_LOG_STR_MAX_SIZE,  __VA_ARGS__);   \
        }                                                       \
    } while (0)
#else   /* BL_STATIC_LOG_LEVEL >= BL_LOG_LVL_VERBOSE */
#define BL_LOG_VERB(...)
#endif

/** @brief Print a string to log buffer

    This API prints the given string to log buffer.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aStrMaxSize     Maximum length of the string to be printed
    @param[in]   aFmt            Format string @ref sec_ref_c11

    @retval     void

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_LOG_PRINTF_PROC  #BRCM_REQ_BL_UTILS_LOG_PRINTF

    @limitations    Only Following format specifiers are supported:
                    Flags :  – (minus), + (plus), space, 0
                    Minimum width : @todo whats the min width for log
                    Length modifier :  l, ll
                    Conversion specifier : d, u, x, p, s, c, %
*/
extern void BL_Log_Printf(uint32_t aStrMaxSize,
                          const char *const aFmt,
                          ...);

extern void CONSOLE_PrintToUart(uint32_t aStrMaxSize,
                          const char *const aFmt,
                          ...);
#endif  /* BL_LOG_H */

/** @} */
