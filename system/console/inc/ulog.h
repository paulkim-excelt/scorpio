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
    @defgroup grp_utils_ulog User Logging
    @ingroup grp_utils

    @addtogroup grp_utils_ulog
    @{
    @section sec_comp_overview Overview
    This section describes the various user logging related APIs.
    <BR>Print debug messages on the port(s). Each module need to implement GetModuleLogLevel()
    API which shall return the current log level of the module. If the module log level is
    greater than print level (ULOG_XXX) then message will be printed on the port.
    .code{.c}
    uint32_t GetAPPLogLevel();
    #define GetModuleLogLevel GetAPPLogLevel
    uint32_t GetAPPLogLevel() { return ULOG_LVL_INFO; }
    .endcode

    @file ulog.h
    @brief User space loggging interface

    @version 1.3 Import comments from docx
*/

#ifndef ULOG_H
#define ULOG_H

#include <stdint.h>

/**
    @name Component Setting
    @{
    @brief User Debug Log levels
    @trace #BRCM_ARCH_ULOG_SETTING_MACRO #BRCM_REQ_ULOG_SETTING
*/
#define ULOG_LVL_NONE           (0UL)       /**< @brief User log level: None */
#define ULOG_LVL_CRITICAL       (1UL)       /**< @brief User log level: Critical */
#define ULOG_LVL_ERROR          (2UL)       /**< @brief User log level: Error */
#define ULOG_LVL_INFO           (3UL)       /**< @brief User log level: Info */
#define ULOG_LVL_VERBOSE        (4UL)       /**< @brief User log level: Verbose */
#define ULOG_LVL_MAX            (ULOG_LVL_VERBOSE) /**< @brief Maximum user log level */
/** @} */


#ifndef STATIC_ULOG_LEVEL
/**
    @brief Compilation macro for static user space log level

    @trace BRCM_ARCH_ULOG_STATIC_LEVEL_MACRO #BRCM_REQ_ULOG_STATIC_LEVEL
*/
#define STATIC_ULOG_LEVEL    ULOG_LVL_INFO
#endif /* STATIC_ULOG_LEVEL */

#ifndef DYNAMIC_ULOG_LEVEL
/** @brief Dynamic user space log level

    @trace BRCM_ARCH_ULOG_DYNAMIC_LEVEL_MACRO #BRCM_REQ_ULOG_DYNAMIC_LEVEL
    @limitations In the current version, feature to change user space log level dynamically is not supported.
*/
#define DYNAMIC_ULOG_LEVEL   ULOG_LVL_ERROR
#endif /* DYNAMIC_ULOG_LEVEL */

#if (STATIC_ULOG_LEVEL >= ULOG_LVL_CRITICAL)
/** @brief Print critical message

    This API prints a critical message to the user space log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_ULOG #BRCM_REQ_COMP_COMP_MAX

    @limitations    This API shall only be called from user space (unprivileged context).
                    <BR>All characters beyong ULOG_STR_MAX_SIZE will be cut off.
                    <BR>If the static/dynamic user space log level is set to a value
                    less than ULOG_LVL_CRITICAL, no critical/error/info/verbose messages shall be logged.
*/
#define ULOG_CRIT(...)                                          \
    do {                                                        \
        if (GetModuleLogLevel() >= ULOG_LVL_CRITICAL) {         \
            ULOG_Print(__VA_ARGS__);    \
        }                                                       \
    } while (0UL == 1UL)
#else   /* STATIC_ULOG_LEVEL >= ULOG_LVL_CRITICAL */
#define ULOG_CRIT(...)
#endif

#if (STATIC_ULOG_LEVEL >= ULOG_LVL_ERROR)
/** @brief Print error message

    This API prints a error message to the user space log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_ULOG #BRCM_REQ_COMP_COMP_MAX

    @limitations    This API shall only be called from user space (unprivileged context).
                    <BR>All characters beyong ULOG_STR_MAX_SIZE will be cut off.
                    <BR>If the static/dynamic user space log level is set to a value
                    less than ULOG_LVL_ERROR, no error/info/verbose messages shall be logged.
*/
#define ULOG_ERR(...)                                           \
    do {                                                        \
        if (GetModuleLogLevel() >= ULOG_LVL_ERROR) {            \
            ULOG_Print(__VA_ARGS__);    \
        }                                                       \
    } while (0UL == 1UL)
#else   /* STATIC_ULOG_LEVEL >= ULOG_LVL_ERROR */
#define ULOG_ERR(...)
#endif

#if (STATIC_ULOG_LEVEL >= ULOG_LVL_INFO)
/** @brief Print informatory message

    This API prints a informatory message to the user space log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_ULOG #BRCM_REQ_COMP_COMP_MAX

    @limitations    This API shall only be called from user space (unprivileged context).
                    <BR>All characters beyong ULOG_STR_MAX_SIZE will be cut off.
                    <BR>If the static/dynamic user space log level is set to a value
                    less than ULOG_LVL_INFO, no info/verbose messages shall be logged.
*/
#define ULOG_INFO(...)                                          \
    do {                                                        \
        if (GetModuleLogLevel() >= ULOG_LVL_INFO) {             \
            ULOG_Print(__VA_ARGS__);    \
        }                                                       \
    } while (0UL == 1UL)
#else   /* STATIC_ULOG_LEVEL >= ULOG_LVL_INFO */
#define ULOG_INFO(...)
#endif

#if (STATIC_ULOG_LEVEL >= ULOG_LVL_VERBOSE)
/** @brief Print verbose message

    This API prints a verbose message to the user space log buffer.

    @param[in]   fmt     Format string

    @trace #BRCM_ARCH_ULOG #BRCM_REQ_COMP_COMP_MAX

    @limitations    This API shall only be called from user space (unprivileged context).
                    <BR>All characters beyong ULOG_STR_MAX_SIZE will be cut off.
                    <BR>If the static/dynamic user space log level is set to a value
                    less than ULOG_LVL_VERBOSE, no verbose messages shall be logged.
*/
#define ULOG_VERB(...)                                          \
    do {                                                        \
        if (GetModuleLogLevel() >= ULOG_LVL_VERBOSE) {          \
            ULOG_Print(__VA_ARGS__);    \
        }                                                       \
    } while (0UL == 1UL)
#else   /* STATIC_ULOG_LEVEL >= ULOG_LVL_VERBOSE */
#define ULOG_VERB(...)
#endif


/** @brief Print a string to User log buffer

    This API prints the given string to User log buffer.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aStrMaxSize     Maximum length of the string to be printed
    @param[in]   aFmt            Format string @ref sec_ref_c11

    @retval     void

    @post None

    @limitations    Internally, this function uses snprintf.
                    Limitations of library function will apply.
*/
extern void ULOG_Print(const char *const aFmt, ...);

uint32_t ULOG_GetData(char * const aData, uint32_t aSize);

#endif /* ULOG_H */
/** @} */
