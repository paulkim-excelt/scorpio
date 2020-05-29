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

#include <stdlib.h>
#include <string.h>
#include <system.h>
#include <ulog.h>
#include <console.h>
#include <bcm_utils.h>
#include <build_info.h>
#include "ee.h"
#ifdef ENABLE_ETS
#include <ets_osil.h>
#endif
#ifdef ENABLE_INET
#include <inet_osil.h>
#endif
#if defined (MEDIA_SYSTEM_STREAM_LOG_COMP_ID)
#include <media_system.h>
#include <component.h>
#include <bcm_time.h>
#endif
#ifdef ENABLE_SHELL_AUTO_TEST_FUNC
#include <bcm_test.h>
#endif
#include <board.h>
#include <init.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel() ULOG_LVL_ERROR

#define SHELL_CMD_STR_LEN       (25UL)

#if defined (MEDIA_SYSTEM_STREAM_LOG_COMP_ID)
#define SHELL_MS_PER_SEC      (1000ULL)
#define SHELL_BITS_PER_BYTE   (8ULL)

static uint64_t lastNumBytes       = 0ULL;
static uint32_t lastFrameCount     = 0UL;
static BCM_TimeType lastSystemTime = {0UL, 0UL};

#define SHELL_COMP_STATS_IN_SECONDS  (3UL)
#endif

extern char* __version_data_start__[];
extern char* __version_data_end__[];

extern int main(int argc, char *argv[]);

typedef struct _ShellCmd {
    char *cmd;
    CmdHandler cmdHandlerFunc;
    char *cmdHelp;
} ShellCmd;

/* Shell command handlers */
void Shell_Help(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);

extern void Shell_Profile(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
extern void Shell_TaskInfo(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);

void Shell_Versions(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);

#ifdef ENABLE_SHELL_TEST_FUNC
extern void Shell_TestFunc(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
#endif  /* ENABLE_SHELL_TEST_FUNC */

#ifdef ENABLE_SHELL_AUTO_TEST_FUNC
static void Shell_AutoTest(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    if (NULL != aArgString) {
        char *remain = NULL;
        uint32_t remLen = 0UL;
        CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);
        if (0 == strncmp(aArgString, "at", 2UL)) {
            int32_t index = atoi(&aArgString[2UL]);
            int32_t ret = BCM_ExecuteAT((uint32_t)index);
            if (BCM_AT_NOT_AVAILABLE == ret) {
                CONSOLE_Write(aConsoleID, "NotAvailable\n");
            } else if (BCM_ERR_OK == ret) {
                CONSOLE_Write(aConsoleID, "Started\n");
            } else {
                CONSOLE_Write(aConsoleID, "Unknown\n");
            }
        } else if (0 == strncmp(aArgString, "help", 4UL)) {
            CONSOLE_Write(aConsoleID, "Usage: test at1\n");
        } else {
            CONSOLE_Write(aConsoleID, "NotAvailable\n");
        }
    } else {
        CONSOLE_Write(aConsoleID, "NotAvailable\n");
    }
}

static void Shell_GetTestResult(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    if (NULL != aArgString) {
        char *remain = NULL;
        uint32_t remLen = 0UL;
        CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);
        if (0 == strncmp(aArgString, "at", 2UL)) {
            int32_t index = atoi(&aArgString[2UL]);
            int32_t result = BCM_GetResultAT((uint32_t)index);
            if (BCM_AT_NOT_STARTED == result) {
                CONSOLE_Write(aConsoleID, "NotStarted\n");
            } else if (BCM_AT_NOT_AVAILABLE == result) {
                CONSOLE_Write(aConsoleID, "NotAvailable\n");
            } else if (BCM_AT_EXECUTING == result) {
                CONSOLE_Write(aConsoleID, "Running\n");
            } else if (BCM_ERR_OK == result) {
                CONSOLE_Write(aConsoleID, "Pass\n");
            } else {
                CONSOLE_Write(aConsoleID, "Fail\n");
            }
        } else if (0 == strncmp(aArgString, "help", 4UL)) {
            CONSOLE_Write(aConsoleID, "Usage: getresult it1\n");
        } else {
            CONSOLE_Write(aConsoleID, "NotAvailable\n");
        }
    } else {
            CONSOLE_Write(aConsoleID, "NotAvailable\n");
    }
}
#endif  /* ENABLE_SHELL_AUTO_TEST_FUNC */

#if defined(__BCM8910X__) || defined(__BCM8953X__)
static void Shell_RebootHandler(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
#define MODE_BL             ("bl")
#define MODE_BL_SZ          (strlen(MODE_BL))
#define MODE_SOFT           ("soft")
#define MODE_SOFT_SZ        (strlen(MODE_SOFT))
#define MODE_HELP           ("help")
#define MODE_HELP_SZ        (strlen(MODE_HELP))

    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, MODE_BL, MODE_BL_SZ)) {
            MCU_SetResetMode(MCU_RESETMODE_DWNL);
            MCU_ResetReq(MCU_RESETREQ_GLOBAL);
        } else if (0 == strncmp(aArgString, MODE_SOFT, MODE_SOFT_SZ)) {
            MCU_ResetReq(MCU_RESETREQ_GLOBAL);
        } else if (0 == strncmp (aArgString, MODE_HELP, MODE_HELP_SZ)) {
            CONSOLE_Write(aConsoleID, "Command to perform system reboot\nOptions:\n");
            CONSOLE_Write(aConsoleID, "bl\treboot to downloader mode\n");
            CONSOLE_Write(aConsoleID, "soft\tsoftware reset\n");
        } else {
            CONSOLE_Write(aConsoleID, "Invalid parameter. Try reboot help\n");
        }
    } else {
        CONSOLE_Write(aConsoleID, "No options provided. Try reboot help\n");
    }
}
#endif

#ifdef __ENABLE_OSEK__
extern void Shell_OSEK(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
#endif /* __ENABLE_OSEK__ */

#if ENABLE_SHELL_APP
extern void Shell_App(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
#endif /* ENABLE_SHELL_APP*/


#ifdef ENABLE_ULOG
extern void Shell_ULogDump(uint32_t aConsoleID, char* const aArgString, uint32_t aArgStringLen);
#endif /* ENABLE_ULOG */

#ifdef ENABLE_ETH_SWITCH
extern void Shell_Switch(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
#endif
#ifdef ENABLE_CFP
extern void CFP_Shell(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
#endif

#if defined(ENABLE_WLAN) && !defined(ENABLE_WLAN_TEST)
extern void Shell_WlCmdHandler(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen);
#endif

void Shell_TaskInfo(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    int32_t i;
    for (i = 0; i < EE_BRCM_SYSTEM_TOS_SIZE; i++) {
        CONSOLE_Write(aConsoleID, BRCM_TaskInfo[i].name);
        CONSOLE_Write(aConsoleID, "\n");
    }
}

void Shell_ULogDump(uint32_t aConsoleID,
                    char * const aArgString,
                    uint32_t aArgStringLen)
{
    if (NULL != aArgString) {
        if (0L == strncmp(aArgString, "uart", 5UL)) {
            CONSOLE_SetUlogID(CONSOLE_UART);
        } else if (0L == strncmp(aArgString, "none", 5UL)) {
            CONSOLE_SetUlogID(CONSOLE_MAX_NUM);
        }
    }
}

/*
 * List of supported shell commands
 */
static const ShellCmd Shell_CmdList [] = {
    {"help", Shell_Help, "Prints this help info"},
    {"taskinfo", Shell_TaskInfo, "Prints task info"},
    {"cpuprof", Shell_Profile, "Print cpu profile info"},
    {"versions", Shell_Versions, "Print the library versions"},

#ifdef ENABLE_SHELL_TEST_FUNC
    {"systemtest", Shell_TestFunc, "Execute systemtest commands"},
    {"test", Shell_TestFunc, "test: run \"test help\" for details"},
#endif  /* ENABLE_SHELL_TEST_FUNC */

#ifdef ENABLE_SHELL_AUTO_TEST_FUNC
    {"test", Shell_AutoTest, "test: run \"test help\" for details"},
    {"getresult", Shell_GetTestResult, "getresult: run \"getresult help\" for details"},
#endif  /* ENABLE_SHELL_GET_TEST_RESULT_FUNC */

#ifdef ENABLE_ETS
    {"ets", ETS_Shell, "ETS commands"},
#endif
#ifdef ENABLE_CFP
    {"cfp", CFP_Shell, "CFP commands"},
#endif
#ifdef ENABLE_INET
    {"inet", INET_Shell, "Internet Stack commands"},
#endif
#ifdef __ENABLE_OSEK__
    {"OSEK", Shell_OSEK, "OSEK control"},
#endif /* __ENABLE_OSEK__ */

#if ENABLE_SHELL_APP
    {"app", Shell_App, "Application Specific"},
#endif /* ENABLE_SHELL_APP*/

#if ENABLE_ULOG
    {"ulog", Shell_ULogDump, "User Logging"},
#endif /* ENABLE_ULOG */

#ifdef ENABLE_ETH_SWITCH
    {"switch", Shell_Switch, "switch test commands"},
#endif /* ENABLE_ETH_SWITCH */
#if defined(ENABLE_WLAN) && !defined(ENABLE_WLAN_TEST)
    {"wl" , Shell_WlCmdHandler, "wlan debug commands"},
#endif /* ENABLE_WLAN */

#if defined(__BCM8910X__) || defined(__BCM8953X__)
    {"reboot", Shell_RebootHandler, "Reboot. User \"reboot help\" for details"},
#endif

    {NULL, NULL, NULL}, };

void Shell_Help(uint32_t aConsoleID, char *aArgString, uint32_t aArgStringLen)
{
    uint32_t i = 0UL;
    (void)aArgStringLen;

    /* help doesnot have any arguments */
    if (NULL == aArgString) {
        while (NULL != Shell_CmdList[i].cmdHandlerFunc) {
            CONSOLE_Write(aConsoleID, "\n");
            CONSOLE_Write(aConsoleID, Shell_CmdList[i].cmd);
            CONSOLE_Write(aConsoleID, ": ");
            CONSOLE_Write(aConsoleID, Shell_CmdList[i].cmdHelp);
            ++i;
        }
        CONSOLE_Write(aConsoleID, "\n");
    } else {
        CONSOLE_Write(aConsoleID, "Invalid argument. Usage: help\n");
    }
}

void Shell_Versions(uint32_t aConsoleID, char *aArgString, uint32_t aArgStringLen)
{
    char** version_symbols_start = __version_data_start__;
    char** version_symbols_end  = __version_data_end__;
    uint32_t start = (uint32_t)version_symbols_start;
    uint32_t end = (uint32_t)version_symbols_end;

    while (start < end) {
        start += 4UL;
        CONSOLE_Write(aConsoleID, *version_symbols_start++);
        CONSOLE_Write(aConsoleID, "\n");
    }
}

/*
 * Callback from console to handle the command received over the console
 */
void CONSOLE_ExecuteCmd(uint32_t aConsoleID, char *aCmdString, char *aArgString,
                        uint32_t aArgStringLen)
{
    uint32_t i, unknownCmdFlag;

    unknownCmdFlag = 1UL;

    if(NULL != aCmdString) {
        for (i = 0UL; NULL != Shell_CmdList[i].cmd; ++i) {
            if ((NULL != Shell_CmdList[i].cmdHandlerFunc)
                && (strncmp(aCmdString, Shell_CmdList[i].cmd, SHELL_CMD_STR_LEN) == 0L)) {
                (*Shell_CmdList[i].cmdHandlerFunc)(aConsoleID, aArgString, aArgStringLen);
                unknownCmdFlag = 0UL;
            }
        }
        if(unknownCmdFlag == 1UL) {
            CONSOLE_Write(aConsoleID, "Unknown Command. Try help\n");
        }
    }
}

void CONSOLE_AlarmCb(void)
{
    (void)SetEvent(TaskShell, SystemEvent0);
}

#if defined(__BCM8910X__)
void CONSOLE_MsysLogAlarmCb(void)
{
    (void)SetEvent(TaskShell, SystemEvent3);
}
#endif

TASK(TaskShell)
{
    BCM_EventMaskType mask;
    BCM_EventMaskType waitMask = SystemEvent0 | SystemEvent3;
    int32_t status = BCM_ERR_OK;

#if defined(ENABLE_UART_CONSOLE)
    char mcuLog[64UL];
    MCU_VersionType version;
#if !defined(ENABLE_UART_TEST)
    UART_Init(LOG_UART_HW_ID);
#endif
    BCM_MemSet(&version, 0U, sizeof(MCU_VersionType));
    CONSOLE_Write(CONSOLE_UART, BLDINFO_GetErikaInfo());
    CONSOLE_Write(CONSOLE_UART, "\n");
    MCU_GetVersion(&version);
    CONSOLE_PrintToBuffer(mcuLog, sizeof(mcuLog), "MCU:Model(0x%x)Rev(0x%x)\n", version.model, version.rev);
    CONSOLE_Write(CONSOLE_UART, mcuLog);
    CONSOLE_Write(CONSOLE_UART, "=>");
    waitMask |= UCONSOLE_RX_EVENT | UCONSOLE_TX_EVENT;
#endif  /* ENABLE_UART_CONSOLE */

    do {
        status = BCM_WaitEvent(waitMask);
        if (BCM_ERR_OK != status) {
            break;
        }
        status = BCM_GetEvent(TaskShell, &mask);
        if (BCM_ERR_OK != status) {
            break;
        }
        status = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != status) {
            break;
        }
#if defined (ENABLE_UART_CONSOLE)
        if ((mask & UCONSOLE_RX_EVENT) != 0UL) {
            CONSOLE_Read(CONSOLE_UART);
        }
        if ((mask & (UCONSOLE_RX_EVENT | UCONSOLE_TX_EVENT | SystemEvent0)) != 0UL) {
            CONSOLE_Process(CONSOLE_UART);
        }
#endif
#if defined (MEDIA_SYSTEM_STREAM_LOG_COMP_ID)
        if ((mask & SystemEvent3) != 0UL) {
            MSYS_PortStatsType stats;
            int32_t retVal;
            BCM_TimeType tCurr = {0UL, 0UL};
            BCM_TimeType tDelta = {0UL, 0UL};
            uint64_t dataSize = 0ULL;
            uint64_t timeInterval = 0ULL;
            uint64_t bitrate = 0ULL;

            BCM_GetTime(&tCurr);
            tDelta = BCM_GetTimeAbsDiff(tCurr, lastSystemTime);
            timeInterval = (((uint64_t)(tDelta.s)) * BCM_NS_PER_SEC) + tDelta.ns;
            BCM_MemSet(&stats, 0U, sizeof(stats));
            retVal = MSYS_GetInStats(MEDIA_SYSTEM_STREAM_LOG_COMP_ID,
                                     MEDIA_SYSTEM_STREAM_LOG_COMP_PORTID, &stats);
            if (BCM_ERR_OK == retVal) {

                /* frameCount/byteCount is a U32/U64 that is not expected to wrap around */
                /* They can be higher only when the application is stopped and started   */
                if ((lastFrameCount >= stats.frameCount) || (lastNumBytes >= stats.byteCount)){
                    lastSystemTime = tCurr;
                    lastFrameCount = stats.frameCount;
                    lastNumBytes = stats.byteCount;
                    continue;
                }
                dataSize = stats.byteCount - lastNumBytes;
                bitrate = (dataSize * SHELL_BITS_PER_BYTE * SHELL_MS_PER_SEC) / timeInterval;

                ULOG_ERR("FPS[ %u ]  BitRate[ %u Mbps ] \n",
                                        (stats.frameCount - lastFrameCount)/SHELL_COMP_STATS_IN_SECONDS,
                                        (uint32_t)bitrate);
            }
            lastSystemTime = tCurr;
            lastFrameCount = stats.frameCount;
            lastNumBytes = stats.byteCount;
        }
#endif
    } while (E_OK == status);
}
