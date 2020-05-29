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

#include "ee.h"
#include <string.h>
#include <ulog.h>
#include <bcm_err.h>
#include <mcu.h>
#include <clk.h>
#include <bcm_test.h>
#include <otp.h>
#include <atomic.h>
#include <utils.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel() ULOG_LVL_ERROR

/* Test IDs */
#define OTP_IT1     (1UL)

/* Macros for error reporting */
#define ATOMIC_CHECK_UPDATE(status, ret)                    \
{                                                           \
    if (BCM_AT_EXECUTING == (*status)) {                    \
        (*status) = ((int32_t)(ret));                       \
    }                                                       \
}
#define MARK_ERR(status, ret, line)                         \
{                                                           \
    ULOG_ERR("\nError: 0x%x \t line %d", ret ,line);        \
    ATOMIC_CHECK_UPDATE((status), (ret))                    \
}
#define CHECK_ERR(status, ret, line)                        \
    if (BCM_ERR_OK != (ret))                                \
    {                                                       \
        MARK_ERR((status), (ret), (line));                  \
        goto err;                                           \
    }

#define CHECK_ERR_BRK(status, ret, line)                    \
    if (BCM_ERR_OK != (ret))                                \
    {                                                       \
        MARK_ERR((status), (ret), (line));                  \
        break;                                              \
    }

#define OTP_HW_ID_0     (0UL)

#define OTP_TEST_ROW_BASE_ADDR  (0x41UL)
#define OTP_TEST_ROW_CNT        (10UL)

static uint32_t Otp_TestPattern[OTP_TEST_ROW_CNT] = {
#if defined(__BCM8953X__)
0xff87ae, 0xc29e09, 0x84055c, 0x78f7aa, 0x4c320f,
0xbc4873, 0xd954d7, 0x3cc998, 0xecd494, 0x4cefbb
#elif defined(__BCM8910X__)
0x938d55, 0x6590b2, 0x3a81db, 0x31747e, 0x1e199e,
0xbab7df, 0xa973e1, 0x28613c, 0x10cd9e, 0x7d0b67
#endif
};

/* Test Status */
static int32_t OtpIT1_Status = BCM_AT_NOT_STARTED;

/* Test the otp */
static void OtpTest(OTP_HwIDType aID, int32_t * const aStatus)
{
    int32_t retVal;
    uint32_t data, otpAddr, i;

    ULOG_ERR("OtpTest (aID: %d): START\n", aID);
    /* Initialize OTP */
    retVal = OTP_Init(aID);
    CHECK_ERR(aStatus, retVal, __LINE__);

    otpAddr = OTP_TEST_ROW_BASE_ADDR;
    for (i = 0UL; i < OTP_TEST_ROW_CNT; i++) {
        retVal = OTP_Read(aID, otpAddr, &data);
        CHECK_ERR_BRK(aStatus, retVal, __LINE__);

        if (data != Otp_TestPattern[i]) {
            CHECK_ERR_BRK(aStatus, BCM_ERR_DATA_INTEG, __LINE__);
        }

        ULOG_ERR("OtpTest: OTP[address:0x%x value:0x%x]\n", otpAddr, data);
        otpAddr += 1UL;
    }

err:
    return;
}

/* Task for OTP_IT1 */
TASK(OtpIT1_Task)
{
    OtpTest(OTP_HW_ID_0, &OtpIT1_Status);
    if(BCM_AT_EXECUTING == OtpIT1_Status) {
        OtpIT1_Status = BCM_ERR_OK;
    }
    (void)BCM_TerminateTask();
}

/* Start Test */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex) {
    case OTP_IT1:
        OtpIT1_Status = BCM_AT_EXECUTING;
        (void)BCM_ActivateTask(OtpIT1_Task);
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

/* Get Test results */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex) {
    case OTP_IT1:
        retVal = OtpIT1_Status;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

void ApplicationInit(void)
{
    /* Dummy Init */
}
