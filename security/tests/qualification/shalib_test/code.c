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
#include <ee.h>
#include <utils.h>
#include <bcm_test.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <sha.h>
#include <osil/bcm_osil.h>

#include <ulog.h>

uint32_t ShaData[8];
uint32_t RefShaData[8] =
{
0x34d0f5c8,
0x51d9541d,
0x6e131ba7,
0xb1fc2a6e,
0x84ed114d,
0x12aea789,
0x0dee8f6a,
0x93f1ecf6
};
#define SHA_LIB_TEST_DATA_SIZE (4096UL)

uint8_t sampleData[SHA_LIB_TEST_DATA_SIZE];
uint32_t shalib_qt1_result = BCM_AT_NOT_AVAILABLE;

BCM_TimeType GStartTime,GEndTime,GTotalTime;

void SHA_lib_test()
{
    int32_t i;

    /* sample data */
    for (i=0;i<SHA_LIB_TEST_DATA_SIZE;i++) {
        sampleData[i] = (uint8_t)i;
    }

    BCM_GetTime(&GStartTime);
    BCM_SHA256(ShaData,sampleData,(SHA_LIB_TEST_DATA_SIZE*8UL));
    BCM_GetTime(&GEndTime);
    GTotalTime = BCM_GetTimeAbsDiff(GStartTime,GEndTime);
    ULOG_INFO(" Time: %dns\n",GTotalTime.ns);

    for (i=0;i<8;i++) {
        if (ShaData[i] != RefShaData[i]) {
            ULOG_ERR("Output Image SHA Check failed 0x%08x\n",ShaData[i]);
            shalib_qt1_result = BCM_ERR_DATA_INTEG;
        }
    }

    if  (BCM_ERR_DATA_INTEG != shalib_qt1_result) {
        shalib_qt1_result = BCM_ERR_OK;
    }

    ULOG_INFO("**********%s completed ***********\n",__func__);
}

/** @brief Main task for pp driver test
 *
 * Main task for pp driver test
 *
 * @return  void
 */
TASK(SHALibTest)
{
    SHA_lib_test();
    BCM_TerminateTask();

}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        shalib_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(SHALibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = shalib_qt1_result;
    }
    return ret;
}
void ApplicationInit()
{
}
