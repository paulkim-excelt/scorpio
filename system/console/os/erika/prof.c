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

#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <stdlib.h>
#include <string.h>
#include <ulog.h>
#include <console.h>
#include <utils.h>
#include <circ_queue.h>
#include "ee.h"

#define CMD_STR_LEN       (25UL)

/* Profiling */
#define PROFILE_STOP            (0UL)
#define PROFILE_START           (1UL)

#define PROF_MAX_TASK (((uint32_t)EE_MAX_TASK+7UL)&(~7UL))


static volatile uint32_t Profile_Status = PROFILE_STOP;
uint64_t Profile_Data[PROF_MAX_TASK];

typedef struct _Profile_InfoType {
    uint32_t relUsage[PROF_MAX_TASK];
} Profile_InfoType;

CIRCQ_V2_DEFINE(Profile_DataQ, 1UL, Profile_InfoType, Profile_DataBuffer, Profile_DataIdx,
                CIRCQ_OVERWRITE_ENABLE, ".data.profile");

#define PROFILE_STORE_DIV    (100000ULL)

/* (10000000ULL / PROFILE_STORE_DIV) */
#define PROFILE_PERCENT_DIV (100ULL)

void Profile_ToggleStatus(void)
{
    if (PROFILE_STOP == Profile_Status) {
        Profile_Status = PROFILE_START;
    } else {
        Profile_Status = PROFILE_STOP;
    }
}


void Shell_Profile(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, "start", 5UL)) {
            CONSOLE_SetProfileID(aConsoleID);
            Profile_Status = PROFILE_START;
        } else if (0 == strncmp(aArgString, "stop", 4UL)){
            CONSOLE_SetProfileID(CONSOLE_MAX_NUM);
            Profile_Status = PROFILE_STOP;
        } else {
            CONSOLE_Write(aConsoleID, "Invalid argument. Usage: cpuprof start/stop\n");
        }
    } else {
        CONSOLE_Write(aConsoleID, "Missing argument. Usage: cpuprof start/stop\n");
    }
}

void Profile_AlarmCb(void)
{
    int32_t i;
    Profile_InfoType info;
    if (PROFILE_START == Profile_Status) {
        for (i = EE_MAX_TASK; i < (int32_t)PROF_MAX_TASK; i++) {
            Profile_Data[i] = 0ULL;
            info.relUsage[i] = 0UL;
        }
        for (i = 0; i < EE_MAX_TASK; ++i) {
            info.relUsage[i] = (uint32_t)(Profile_Data[i] / PROFILE_STORE_DIV);
            if (info.relUsage[i] >= 100UL * PROFILE_PERCENT_DIV) {
                info.relUsage[i] = 99UL * PROFILE_PERCENT_DIV;
            }
            Profile_Data[i] = 0ULL;
        }
        (void)CIRCQ_Push(&Profile_DataQ, (char*)&info, 1UL);
    } else {
        for (i = 0; i < (int32_t)PROF_MAX_TASK; i++) {
            Profile_Data[i] = 0ULL;
        }
    }
}

int32_t Profile_GetBuffer(uint32_t* aCnt, uint32_t *aIdle, uint32_t *aPercentage)
{
    int32_t ret = BCM_ERR_NOMEM;
    uint32_t lCnt;
    uint32_t misc = 0UL;
    int32_t i;
    Profile_InfoType info;

    BCM_MemSet(&info, 0x0, sizeof(Profile_InfoType));
    i = CIRCQ_Pop(&Profile_DataQ, (char*)&info, 1UL);
    if (1UL == i) {
        if ((NULL != aCnt) && (NULL != aIdle) && (NULL != aPercentage)) {
            lCnt = MIN(EE_MAX_TASK + 1, *aCnt);
            for (i = 0; i < lCnt - 1UL; i++) {
                aPercentage[i] = info.relUsage[i] / PROFILE_PERCENT_DIV;
                misc += info.relUsage[i];
            }
            for (i = lCnt - 1UL; i < *aCnt; i++) {
                aPercentage[i] = 0UL;
            }
            if (misc > 100UL * PROFILE_PERCENT_DIV) {
                misc = 0UL;
            } else {
                misc = (100UL * PROFILE_PERCENT_DIV - misc) / PROFILE_PERCENT_DIV;
            }
            aPercentage[lCnt - 1UL] = misc;
            *aCnt = lCnt;
            *aIdle = aPercentage[IDLE_Task];
            aPercentage[IDLE_Task] = 0UL;
            ret = BCM_ERR_OK;
        }
    }
    return ret;
}
