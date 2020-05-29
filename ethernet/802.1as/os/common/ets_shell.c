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

/** @defgroup grp_ets_il_shell Shell
    @ingroup grp_ets_il

    @addtogroup grp_ets_il_shell
    @{
    @section sec_ets_il_shell Shell
    This page describes the ETS shell

    @file ets_shell.c
    @brief Shell commands for ETS
    Shell commands to configure and query status/statistics
    @version 0.1 Initial version
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <console.h>
#include <utils.h>
#include "ets_osil.h"

#define GetModuleLogLevel() ULOG_LVL_INFO

/**
    @brief System event used for communicating with ETS protocol task
*/
#define TEST_EVENT SystemEvent1

/** @brief Parse number from string

    Parses a number from the input string and returns the leftover string

    @behavior Sync, Reentrant

    @pre None

    @param[in]   string                   Command string
    @param[in]   stringLen                Command string length
    @param[out]  remains                  Leftover string
    @param[out]  remainingLen             Leftover string length

    @return      Parsed signed number

    @post None

    @limitations Can parse 32 bit signed number only

    @code{.unparsed}
    if input string is NULL or string length is 0:
        Set remaining string to NULL and remaining length to 0
    else:
        Parse next token using CONSOLE_SplitLine()
        NULL != string && stringLen != 0UL) {
        CONSOLE_SplitLine(string, stringLen, remains, remainingLen);
        if number is signed 32 bit:
            Copy the number to the buffer and NULL terminate
            Convert string buffer to number
    @endcode
*/
static int32_t ETS_Configure_GetNextNumber(char* string,
                                            uint32_t stringLen,
                                            char** remains,
                                            uint32_t *remainingLen)
{
    char     buffer[11];
    char     *endptr= NULL;
    int32_t  svalue = LONG_MAX;

    if (NULL != string && stringLen != 0UL) {
        CONSOLE_SplitLine(string, stringLen, remains, remainingLen);
        /* remains points to start of field */

        if ((stringLen - *remainingLen) < sizeof(buffer)) {
            if(0UL != *remainingLen) {
                BCM_StrnCpy(buffer, string, stringLen - *remainingLen - 1UL);
                buffer[stringLen - *remainingLen - 1UL] = '\0';
            } else {
                BCM_StrnCpy(buffer, string, stringLen - *remainingLen);
                buffer[stringLen - *remainingLen] = '\0';
            }
            svalue = BCM_StrToLong(buffer, &endptr, 0L);
        } else {
            ULOG_ERR("Number too big, considering it as 2147483647\n");
        }
    } else {
        *remains = NULL;
        *remainingLen = 0UL;
    }
    return svalue;
}

#ifdef ETS_CONFIGURE_SUPPORT
/**
    @brief ETS configuration

    Default configuration used to initialize ETS
*/
static ETS_Config config =
{
    .magicId = ETS_CONFIG_MAGIC_ID,
    .globalAdmin = ETS_ADMINMODE_DISABLE,
    .clockMode = ETS_CLOCKMODE_GM,
    .avnuSyncAbsenceTimeout = 0U,
    .gmRateRatio = 0x40000000,
    .intfCfg = {
        {
            .hwPortNum = 0U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INIT_LOG_PDELAY_1S,
            .operLogPdelayInterval = ETS_OPER_LOG_PDELAY_1S,
            .initLogSyncInterval = ETS_INIT_LOG_SYNC_31_25MS,
            .operLogSyncInterval = ETS_OPER_LOG_SYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
#if (ETS_MAX_INTERFACES > 1)
        {0U}, {0U}, {0U}, {0U}, {0U}, {0U}, {0U}
#endif
    }
};

/** @brief Start gPTP

    Start the gPTP operation

    @behavior Sync, Non-reentrant

    @pre None

    @return      void

    @post None

    @limitations None

    @code{.unparsed}
    if globalAdmin is enable:
        Invoke ETS_CompInit()
    @endcode
*/
static void ETS_Start(void)
{
    const ETS_Config  cfg = config;
    int32_t           rc  = BCM_ERR_OK;

    if (config.globalAdmin == ETS_ADMINMODE_DISABLE) {
        ULOG_ERR("First execute ets config command\n");
        return;
    }

    rc = ETS_CompInit(&cfg, TEST_EVENT);
    if (rc == BCM_ERR_OK) {
        ULOG_INFO("ets started ... \n");
    } else {
        ULOG_ERR("ets failed to start with error %ld\n", rc);
    }
}

/** @brief Stop gPTP

    Stop the gPTP operation

    @behavior Sync, Non-reentrant

    @pre None

    @return      void

    @post None

    @limitations None

    @code{.unparsed}
    Invoke ETS_CompDeInit()
    @endcode
*/
static void ETS_Stop(void)
{
    int32_t            rc      = BCM_ERR_OK;

    rc = ETS_CompDeInit(TEST_EVENT);
    if (rc == BCM_ERR_OK) {
        ULOG_INFO("ets stopped ... \n");
    } else {
        ULOG_ERR("ets failed to stop with error %ld\n", rc);
    }
}

/** @brief Configuration command handler

    Parses input string for gPTP configuration. Also performs validation of
    input values

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aConsoleID               Console handle
    @param[in]   aArgString               Command string
    @param[in]   aArgStringLen            Command string length

    @return     void

    @post None

    @limitations None

    @code{.unparsed}
    Parse next token using CONSOLE_SplitLine
    if token is "slave":
        Set role as Slave
        Set clockMode as Slave
    else if token is "master":
        Set role as Master
        Set clockMode as Grandmaster
    Parse initial log PDelay interval using ETS_Configure_GetNextNumber()
    if role is Slave and interval is not 0 OR role is Master and interval is not one of [0, 127]:
        Show error
    else:
        Set the initLogPdelayInterval to the parsed value
    Parse operational log PDelay interval using ETS_Configure_GetNextNumber()
    if role is Slave and interval is not one of [0, 1, 2, 3]  OR role is Master and interval is not one of [0, 1, 2, 3, 127]:
        Show error
    else:
        Set the operLogPdelayInterval to the parsed value
    Parse initial log Sync interval using ETS_Configure_GetNextNumber()
    if interval is not one of [-5, -4, -3]:
        Show error
    else:
        Set the initLogSyncInterval to the parsed value

    Parse operational log Sync interval using ETS_Configure_GetNextNumber()
    if interval is not one of [-3, -2, -1, 0]:
        Show error
    else:
        Set the operLogSyncInterval to the parsed value

    Parse Peer Delay using ETS_Configure_GetNextNumber()
    Set the nbrPropDelay to parsed value
    Set the globalAdmin to enable
    @endcode
*/
static void ETS_Configure(uint32_t aConsoleID,
                           char* aArgString,
                           uint32_t aArgStringLen)
{
    char     *pingRemains = NULL;
    char     *pongRemains = NULL;
    uint32_t pingRemainingLength = 0UL;
    uint32_t pongRemainingLength = 0UL;
    int32_t  svalue;

    /* Split string to get the configuration */
    CONSOLE_SplitLine(aArgString, aArgStringLen, &pingRemains, &pingRemainingLength);
    if (NULL != pingRemains) {

        /* Parse role */
        if (0 == strncmp(pingRemains, "slave", 5UL)) {
            config.intfCfg[0UL].role = ETS_ROLE_SLAVE;
            config.clockMode = ETS_CLOCKMODE_SLAVE;
        } else if (0 == strncmp(pingRemains, "master", 6UL)) {
            config.intfCfg[0UL].role = ETS_ROLE_MASTER;
            config.clockMode = ETS_CLOCKMODE_GM;
        } else {
            ULOG_ERR("Invalid Role\n");
            return;
        }
        CONSOLE_SplitLine(pingRemains, pingRemainingLength,
                  &pongRemains, &pongRemainingLength);

        /* Parse initLogPdelayInterval */
        svalue = ETS_Configure_GetNextNumber(pongRemains, pongRemainingLength,
                                            &pingRemains, &pingRemainingLength);

        /* Allowed values: Master [0, 127] Slave [0] */
        if(((config.intfCfg[0UL].role == ETS_ROLE_SLAVE) &&
            (svalue != ETS_INIT_LOG_PDELAY_1S)) ||
            ((config.intfCfg[0UL].role == ETS_ROLE_MASTER) &&
            ((svalue != ETS_INIT_LOG_PDELAY_1S) &&
            (svalue != ETS_INIT_LOG_PDELAY_INF)))) {
            ULOG_ERR("Invalid initLogPdelay:%ld\n",svalue);
            return;
        } else {
            config.intfCfg[0UL].initLogPdelayInterval = svalue;
        }

        /* Parse operLogPdelayInterval */
        svalue = ETS_Configure_GetNextNumber(pingRemains, pingRemainingLength,
                                        &pongRemains, &pongRemainingLength);
        /* Allowed values: Master [0, 1, 2, 3, 127] Slave [0, 1, 2, 3] */
        if (((config.intfCfg[0UL].role == ETS_ROLE_SLAVE) &&
            ((svalue < ETS_OPER_LOG_PDELAY_MIN) ||
            (svalue > ETS_OPER_LOG_PDELAY_MAX))) ||
            ((config.intfCfg[0UL].role == ETS_ROLE_MASTER) &&
            (((svalue < ETS_OPER_LOG_PDELAY_MIN) ||
            (svalue > ETS_OPER_LOG_PDELAY_MAX)) &&
            (svalue != ETS_OPER_LOG_PDELAY_INF)))) {
            ULOG_ERR("Invalid operLogPdelay:%ld\n",svalue);
            return;
        } else {
            config.intfCfg[0UL].operLogPdelayInterval = svalue;
        }

        /* Parse initLogSyncInterval */
        svalue = ETS_Configure_GetNextNumber(pongRemains, pongRemainingLength,
                                        &pingRemains, &pingRemainingLength);
        if ((svalue < ETS_INIT_LOG_SYNC_MIN) ||
            (svalue > ETS_INIT_LOG_SYNC_MAX)) {
            ULOG_ERR("Invalid initLogSync:%ld\n",svalue);
            return;
        } else {
            config.intfCfg[0UL].initLogSyncInterval = svalue;
        }

        /* Parse operLogSyncInterval */
        svalue = ETS_Configure_GetNextNumber(pingRemains, pingRemainingLength,
                                        &pongRemains, &pongRemainingLength);
        if ((svalue < ETS_OPER_LOG_SYNC_MIN) ||
            (svalue > ETS_OPER_LOG_SYNC_MAX)) {
            ULOG_ERR("Invalid operLogSync:%ld\n",svalue);
            return;
        } else {
            config.intfCfg[0UL].operLogSyncInterval = svalue;
        }

        svalue = ETS_Configure_GetNextNumber(pongRemains, pongRemainingLength,
                                        &pingRemains, &pingRemainingLength);

        if (svalue < 0) {
            ULOG_ERR("Invalid pdelay:%ld\n",svalue);
            return;
        }

        config.intfCfg[0UL].nbrPropDelay = (uint32_t) svalue;
        config.globalAdmin = ETS_ADMINMODE_ENABLE;

    } else {
        CONSOLE_Write(aConsoleID,
                "Invalid argument. Usage:\n"
                "ets config <master/slave> <initLogPdelay> <operLogPdelay> "
                "<initLogSync> <operLogSync> <pdelay>\n");
    }
}
#endif

/** @brief Display gPTP Statistics

    Retrieve & display gPTP statistics

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aConsoleID               Console identifier
    @param[in]   aArgString               Command string (containing time)
    @param[in]   aArgStringLen            Command string length

    @return      void

    @post None

    @limitations None

    @code{.unparsed}
    Split string using CONSOLE_SplitLine()
    Get interface number using ETS_Configure_GetNextNumber()
    if next token is "clear":
       Clear stats using ETS_ResetStats()
    else:
        Set stats magic
        Fetch stats using ETS_GetStats()
        Print all interface stats
    @endcode
*/
static void ETS_ShowStats(uint32_t aConsoleID,
                          char* aArgString,
                          uint32_t aArgStringLen)
{
    int32_t       rc      = BCM_ERR_OK;
    uint32_t      intf    = 0UL;
    char          *pingRemains = NULL;
    char          *pongRemains = NULL;
    uint32_t      pingRemainingLength = 0UL;
    uint32_t      pongRemainingLength = 0UL;
    int32_t       svalue;
    ETS_StatsType stats;

    /* Split string to get the configuration */
    CONSOLE_SplitLine(aArgString, aArgStringLen, &pingRemains, &pingRemainingLength);
    if (NULL != pingRemains) {
        /* Parse intf number */
        svalue = ETS_Configure_GetNextNumber(pingRemains, pingRemainingLength,
                                          &pongRemains, &pongRemainingLength);
        intf = (uint32_t)svalue;
        if ((pongRemains != NULL) &&
            (0 == strncmp(pongRemains, "clear", 5UL))) {
            rc = ETS_ResetStats(intf);
            if (rc != BCM_ERR_OK) {
                ULOG_ERR("Error %d while clearing stats for intf %lu\n",
                        rc, intf);
            }
        } else {
            BCM_MemSet(&stats, 0, sizeof(ETS_StatsType));
            stats.magicId   = ETS_STATS_MAGIC_ID;

            rc = ETS_GetStats(intf, &stats);
            if (rc == BCM_ERR_OK) {
                ULOG_INFO("sync Tx                  = %lu\n", stats.syncTxCount);
                ULOG_INFO("sync Tx Timeouts         = %lu\n", stats.syncTransmitTimeouts);
                ULOG_INFO("sync Rx                  = %lu\n", stats.syncRxCount);
                ULOG_INFO("sync Rx Timeouts         = %lu\n", stats.syncReceiptTimeouts);
                ULOG_INFO("sync Rx Discards         = %lu\n", stats.syncRxDiscards);
                ULOG_INFO("followUp Tx              = %lu\n", stats.followUpTxCount);
                ULOG_INFO("followUp Rx              = %lu\n", stats.followUpRxCount);
                ULOG_INFO("followup Rx Timeouts     = %lu\n",
                        stats.followupReceiptTimeouts);
                ULOG_INFO("followUp Rx Discards     = %lu\n\n",stats.followUpRxDiscards);
                ULOG_INFO("signaling Tx             = %lu\n", stats.signalingTxCount);
                ULOG_INFO("signaling Rx             = %lu\n", stats.signalingRxCount);
                ULOG_INFO("signaling Rx Discards    = %lu\n\n",
                        stats.signalingRxDiscards);
                ULOG_INFO("PDelay Req Tx            = %lu\n", stats.pDelayReqTxCount);
                ULOG_INFO("PDelay Req Rx            = %lu\n", stats.pDelayReqRxCount);
                ULOG_INFO("PDelay Req Rx Discards   = %lu\n", stats.pDelayReqRxDiscards);
                ULOG_INFO("PDelay Resp Tx           = %lu\n", stats.pDelayRespTxCount);
                ULOG_INFO("PDelay Resp Rx           = %lu\n", stats.pDelayRespRxCount);
                ULOG_INFO("PDelay Resp Rx Discards  = %lu\n",stats.pDelayRespRxDiscards);
                ULOG_INFO("PDelay Resp FollowUp Tx  = %lu\n",
                        stats.pDelayRespFollowUpTxCount);
                ULOG_INFO("PDelay Resp FollowUp Rx  = %lu\n\n",
                        stats.pDelayRespFollowUpRxCount);
                ULOG_INFO("PDelay Timeouts          = %lu\n",
                        stats.pDelayReceiptTimeouts);
                ULOG_INFO("Bad PDelay Values        = %lu\n\n", stats.badPdelayValues);
                ULOG_INFO("Lost Responses Exceeded  = %lu\n\n", stats.pDelayLostResponsesExceeded);
                ULOG_INFO("Tx Errors                = %lu\n", stats.txErrors);
                ULOG_INFO("Timestamp Errors         = %lu\n", stats.tsErrors);
                ULOG_INFO("PTP Discard Count        = %lu\n\n", stats.ptpDiscardCount);
                ULOG_INFO("Tx Confirmations         = %lu\n\n", stats.txConf);
                ULOG_INFO("Bad Headers              = %lu\n\n", stats.parseFailed);
            } else {
                ULOG_ERR("Error %d failed to retrieve stats\n", rc);
            }
        }
    }
}

/** @brief Set gPTP Time

    Set the gPTP time. Can be invoked only on the GM

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aArgString               Command string (containing time)
    @param[in]   aArgStringLen            Command string length

    @return      void

    @post None

    @limitations None

    @code{.unparsed}
    Parse the seconds field using CONSOLE_SplitLine() and ETS_Configure_GetNextNumber()
    if seconds value is not negative:
        Parse nanoseconds field using ETS_Configure_GetNextNumber()
        if nanoseconds value is not negative:
            Set global time using ETS_SetGlobalTime()
    @endcode
*/
static void ETS_SetTime(char* aArgString, uint32_t aArgStringLen)
{
    int32_t              rc           = BCM_ERR_OK;
    char                 *pingRemains = NULL;
    char                 *pongRemains = NULL;
    uint32_t             pingRemainingLength  = 0UL;
    uint32_t             pongRemainingLength  = 0UL;
    int32_t              parsedValue;
    uint64_t             seconds;
    uint32_t             nanoseconds;

    /* First parse the seconds and nanoseconds fields */
    CONSOLE_SplitLine(aArgString, aArgStringLen, &pingRemains, &pingRemainingLength);

    parsedValue = ETS_Configure_GetNextNumber(pingRemains, pingRemainingLength,
             &pongRemains, &pongRemainingLength);

    if (parsedValue < 0) {
        ULOG_ERR("Time cannot be negative: seconds=%ld\n",parsedValue);
        return;
    }

    seconds = (uint64_t)parsedValue;

    parsedValue = ETS_Configure_GetNextNumber(pongRemains, pongRemainingLength,
             &pingRemains, &pingRemainingLength);

    if (parsedValue < 0) {
        ULOG_ERR("Time cannot be negative: nanoseconds=%ld\n",parsedValue);
        return;
    }

    nanoseconds = (uint32_t)parsedValue % 1000000000UL;

    ULOG_INFO("Setting time to %lu:%lu\n", (uint32_t)seconds,nanoseconds);
    {
        const ETS_TimestampType globalTime = {seconds, nanoseconds};
        rc = ETS_SetGlobalTime(&globalTime);
        if (rc != BCM_ERR_OK) {
            ULOG_ERR("Failed to set time. Error %ld\n", rc);
        }
    }
}

/** @brief Display gPTP Status

    Display gPTP status parameters like peer delay, rate ratios, network time

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aArgString               Command string
    @param[in]   aArgStringLen            Command string length

    @return      void

    @post None

    @limitations None

    @code{.unparsed}
    Parse interface number using CONSOLE_SplitLine() and ETS_Configure_GetNextNumber()
    Set interface magic
    Get interface status using ETS_GetIntfStatus()
    Set global magic
    Get global status using ETS_GetGlobalStatus()
    Get system time using BCM_GetTime()
    Print all fields of interface and global status
    @endcode
*/
static void ETS_Status(char* aArgString, uint32_t aArgStringLen)
{
    int32_t                rc      = BCM_ERR_OK;
    uint32_t               intf;
    ETS_GlobalStatusType   globalStatus;
    ETS_IntfStatusType     intfStatus;
    BCM_TimeType               systemTime = {0UL, 0UL};
    char                   *pingRemains = NULL;
    char                   *pongRemains = NULL;
    uint32_t               pingRemainingLength  = 0UL;
    uint32_t               pongRemainingLength  = 0UL;

    /* First parse the seconds and nanoseconds fields */
    CONSOLE_SplitLine(aArgString, aArgStringLen, &pingRemains, &pingRemainingLength);

    intf = (uint32_t)ETS_Configure_GetNextNumber(pingRemains, pingRemainingLength,
                                       &pongRemains, &pongRemainingLength);


    intfStatus.magicId = ETS_INTF_STATUS_MAGIC_ID;
    rc = ETS_GetIntfStatus(intf, &intfStatus);
    if (rc == BCM_ERR_OK) {
        globalStatus.magicId = ETS_GLBL_STATUS_MAGIC_ID;
        rc = ETS_GetGlobalStatus(&globalStatus);
        if (rc == BCM_ERR_OK) {
            BCM_GetTime(&systemTime);
            ULOG_INFO("------------ Global Status -----------\n");
            ULOG_INFO("System Time             = %lus : %luns \n", systemTime.s, systemTime.ns);
            ULOG_INFO("Network Time            = ");
            switch (globalStatus.networkTimeState) {
                case ETS_TIMESTAMPSTATE_SYNC:
                    ULOG_INFO("%lus : %luns SYNC\n",
                            (uint32_t)(globalStatus.networkTime.seconds & 0xFFFFFFFFUL),
                            globalStatus.networkTime.nanoseconds);
                    break;
                case ETS_TIMESTAMPSTATE_UNSYNC:
                    ULOG_INFO("UNSYNC\n");
                    break;
                case ETS_TIMESTAMPSTATE_UNCERTAIN:
                    ULOG_INFO("%lus : %luns UNCERTAIN\n",
                            (uint32_t)(globalStatus.networkTime.seconds & 0xFFFFFFFFUL),
                            globalStatus.networkTime.nanoseconds);
                    break;
                default:
                    ULOG_INFO("\n");
                    break;
            }
            ULOG_INFO("In Rate Ratio           = 0x%x\n", globalStatus.rateRatioIn);
            ULOG_INFO("GM Rate Ratio           = 0x%x\n", globalStatus.gmRateRatio);
            ULOG_INFO("GM Status               = ");
            switch(globalStatus.gmStatus) {
                case ETS_GMSTATUS_STARTUP_ABSENT:
                    ULOG_INFO("Startup absent\n");
                    break;
                case ETS_GMSTATUS_OPER_ABSENT:
                    ULOG_INFO("Operational absent\n");
                    break;
                case ETS_GMSTATUS_UNDETERMINED:
                    ULOG_INFO("Undetermined\n");
                    break;
                case ETS_GMSTATUS_DISABLED:
                    ULOG_INFO("Disabled\n");
                    break;
                case ETS_GMSTATUS_NO_SLAVEPORT:
                    ULOG_INFO("No slave port\n");
                    break;
                case ETS_GMSTATUS_PRESENT:
                    ULOG_INFO("Present\n");
                    break;
                default:
                    ULOG_INFO("\n");
                    break;
            }

            if (0xFFFFUL == globalStatus.slavePort) {
                ULOG_INFO("Slave Port              = None\n");
            } else {
                ULOG_INFO("Slave Port              = %lu\n", globalStatus.slavePort);
            }
            ULOG_INFO("Signaling Tx SeqNum     = %u\n", globalStatus.signalingTxSeqId);
            ULOG_INFO("Requested Sync Interval = %d\n", globalStatus.reqSyncLogInterval);
            ULOG_INFO("Signaling Timer Started = %lu\n", globalStatus.isSignalingTimerStarted);
            ULOG_INFO("Is Sync Received        = %lu\n", globalStatus.isSyncReceived);
            ULOG_INFO("Clock State             = ");
            switch (globalStatus.clockState) {
                case ETS_CLOCKSTATE_INIT_GM:
                    ULOG_INFO("Init GM\n");
                    break;
                case ETS_CLOCKSTATE_UPDATE_GM:
                    ULOG_INFO("Update GM\n");
                    break;
                case ETS_CLOCKSTATE_INIT_SLAVE:
                    ULOG_INFO("Init Slave\n");
                    break;
                case ETS_CLOCKSTATE_UPDATE_SLAVE:
                    ULOG_INFO("Update Slave\n");
                    break;
                default:
                    ULOG_INFO("\n");
                    break;
            }
            ULOG_INFO("Clock Id                = %x:%x:%x:%x:%x:%x:%x:%x\n",
                    globalStatus.clockId.id[0], globalStatus.clockId.id[1],
                    globalStatus.clockId.id[2], globalStatus.clockId.id[3],
                    globalStatus.clockId.id[4], globalStatus.clockId.id[5],
                    globalStatus.clockId.id[6], globalStatus.clockId.id[7]);
            ULOG_INFO("------------ Intf %lu -----------\n", intf);
            ULOG_INFO("Peer Delay              = %luns\n", intfStatus.nbrPropDelay);
            ULOG_INFO("Peer Rate Ratio         = 0x%x\n", intfStatus.nbrRateRatio);
            ULOG_INFO("Measuring Delay         = %lu\n", intfStatus.isMeasuringPdelay);
            ULOG_INFO("Config Saved            = %lu\n", intfStatus.isAVnuPdelayConfigSaved);
            ULOG_INFO("Pdelay Interval         = %d\n", intfStatus.currentLogPdelayInterval);
            ULOG_INFO("Sync Interval           = %d\n", intfStatus.currentLogSyncInterval);
            ULOG_INFO("Sync Rx Timeout         = %luus\n",
                    (uint32_t)intfStatus.syncReceiptTimeoutInterval);
            ULOG_INFO("Partner Clock Id        = %x:%x:%x:%x:%x:%x:%x:%x\n",
                    intfStatus.partnerClockId.id[0],
                    intfStatus.partnerClockId.id[1],
                    intfStatus.partnerClockId.id[2],
                    intfStatus.partnerClockId.id[3],
                    intfStatus.partnerClockId.id[4],
                    intfStatus.partnerClockId.id[5],
                    intfStatus.partnerClockId.id[6],
                    intfStatus.partnerClockId.id[7]);
            ULOG_INFO("Partner Port            = %u\n", intfStatus.partnerPortId);
            ULOG_INFO("Pdelay Tx SeqNum        = %u\n", intfStatus.pDelayTxSeqId);
            ULOG_INFO("Sync Tx SeqNum          = %u\n", intfStatus.syncTxSeqId);
            ULOG_INFO("Sync Info Avail         = %lu\n", intfStatus.syncInfoAvailable);
            ULOG_INFO("Sync Rx SeqNum          = %u\n", intfStatus.syncLastRxSeqId);
            ULOG_INFO("Sync Rx Interval        = %d\n", intfStatus.rxPduInterval);
            {
                int64_t correctionUS = intfStatus.rxFollowupCorrection/1000LL;
                ULOG_INFO("Rx Correction           = %ldus\n", (int32_t)correctionUS);
            }
            ULOG_INFO("Tx Correction           = %luus\n",
                    (uint32_t)(intfStatus.txFollowupCorrection/1000ULL));
            ULOG_INFO("Rx POT                  = %lus : %luns\n",
                    (uint32_t)intfStatus.rxPOT.seconds, intfStatus.rxPOT.nanoseconds);
            ULOG_INFO("Lost responses          = %lu\n", intfStatus.numPdelayRespLost);
            ULOG_INFO("pDelay Tx State         = %lu\n", intfStatus.pDelayReqState);
            ULOG_INFO("pDelay Rx State         = %lu\n", intfStatus.pDelayRespState);
            ULOG_INFO("Sync Tx State           = %lu\n", intfStatus.syncTxState);
            ULOG_INFO("Sync Rx State           = %lu\n", intfStatus.syncRxState);
            ULOG_INFO("Sync Rx Timestamp       = %lus : %luns\n",
                  (uint32_t)intfStatus.syncRxTimestamp.seconds,
                  intfStatus.syncRxTimestamp.nanoseconds);
            ULOG_INFO("Sync Tx Timestamp       = %lus : %luns\n",
                  (uint32_t)intfStatus.syncTxTimestamp.seconds,
                  intfStatus.syncTxTimestamp.nanoseconds);
            ULOG_INFO("Pdelay T1               = %lus : %luns\n",
                  (uint32_t)intfStatus.pDelayT1.seconds, intfStatus.pDelayT1.nanoseconds);
            ULOG_INFO("Pdelay T2               = %lus : %luns\n",
                  (uint32_t)intfStatus.pDelayT2.seconds, intfStatus.pDelayT2.nanoseconds);
            ULOG_INFO("Pdelay T3               = %lus : %luns\n",
                  (uint32_t)intfStatus.pDelayT3.seconds, intfStatus.pDelayT3.nanoseconds);
            ULOG_INFO("Pdelay T4               = %lus : %luns\n",
                  (uint32_t)intfStatus.pDelayT4.seconds, intfStatus.pDelayT4.nanoseconds);
        } else {
            ULOG_ERR("ETS_GetGlobalStatus failed %d\n", rc);
        }
    } else {
        ULOG_ERR("ETS_IntfStatus for %lu failed %d\n", intf, rc);
    }
}

/**
    @code{.unparsed}
    Parse input command string if not NULL
    if command is "stats":
        Invoke ETS_ShowStats()
    else if command is "settime":
        Invoke ETS_SetTime()
    else if command is "status":
        Invoke ETS_Status()
    else if command is "start":
        Invoke ETS_Start()
    else if command is "stop":
        Invoke ETS_Stop()
    else if command is "config":
        Invoke ETS_Configure()
    else
        show help
    @endcode
*/
void ETS_Shell(uint32_t aConsoleID,
               char* aArgString,
               uint32_t aArgStringLen)
{
    if (NULL != aArgString) {

        if (0 == strncmp(aArgString, "stats", 5UL)) {
            ETS_ShowStats(aConsoleID, aArgString, aArgStringLen);
        } else if (0 == strncmp(aArgString, "settime", 7UL)) {
            ETS_SetTime(aArgString, aArgStringLen);
        } else if (0 == strncmp(aArgString, "status", 6UL)) {
            ETS_Status(aArgString, aArgStringLen);
#ifdef ETS_CONFIGURE_SUPPORT
        } else if (0 == strncmp(aArgString, "start", 6UL)) {
            ETS_Start();
        } else if (0 == strncmp(aArgString, "config", 6UL)) {
            ETS_Configure(aConsoleID, aArgString, aArgStringLen);
        } else if (0 == strncmp(aArgString, "stop", 5UL)) {
            ETS_Stop();
#endif
        } else {
            CONSOLE_Write(aConsoleID,
#ifdef ETS_CONFIGURE_SUPPORT
                   "ets config <master/slave> <initLogPdelay> "
                   "<operLogPdelay> <initLogSync> <operLogSync>"
                   " <pdelay>\n"
                   "ets start\n"
                   "ets stop\n"
#endif
                   "ets stats <intf> [clear]\n"
                   "ets settime <seconds> <nanoseconds>\n"
                   "ets status <intf>\n");
        }

    } else {
        CONSOLE_Write(aConsoleID,
#ifdef ETS_CONFIGURE_SUPPORT
              "No argument mentioned. Usage:\n"
              "ets config <master/slave> <initLogPdelay> <operLogPdelay> "
              "<initLogSync> <operLogSync> <pdelay>\n"
              "ets start\n"
              "ets stop\n"
#endif
              "ets stats <intf> [clear]\n"
              "ets settime <seconds> <nanoseconds>\n"
              "ets status <intf>\n");
    }
}
/** @} */
