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
#include <bcm_time.h>
#include <init.h>
#include "ee.h"
#include "ee_internal.h"

extern volatile uint64_t Profile_Data[EE_MAX_TASK];
static BCM_TimeType Profile_StartNS;
static BCM_TimeType Profile_EndNS;

#ifdef __ENABLE_OSEK__
extern void OSEK_StartupHook(void);
extern void OSEK_PreTaskHook(void);
extern void OSEK_PostTaskHook(void);
#endif

void StartupHook(void)
{
#ifdef __ENABLE_OSEK__
    OSEK_StartupHook();
#endif
}

void PreTaskHook(void)
{
    // TODO
    /* Reset the waiting mask */
    EE_th_event_waitmask[EE_stk_queryfirst()] = 0U;

    BCM_GetSystemTime(&Profile_StartNS);
#ifdef __ENABLE_OSEK__
    OSEK_PreTaskHook();
#endif
}

void PostTaskHook(void)
{
    TaskType currentTaskID;
    BCM_TimeType timeTaken;
    StatusType status;

    BCM_GetSystemTime(&Profile_EndNS);
    timeTaken = BCM_GetTimeAbsDiff(Profile_StartNS, Profile_EndNS);
    status = GetTaskID(&currentTaskID);
    if ((status == E_OK) && (INVALID_TASK < currentTaskID)) {
       Profile_Data[currentTaskID]  += ((uint64_t)(timeTaken.s
                                         * (uint64_t)BCM_NS_PER_SEC))
                                        + (uint64_t)timeTaken.ns;
    }
    // TODO

#ifdef __ENABLE_OSEK__
    OSEK_PostTaskHook();
#endif
}


