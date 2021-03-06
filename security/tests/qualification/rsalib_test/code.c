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

#include <ee.h>
#include <utils.h>
#include <bcm_test.h>
#include <bcm_err.h>
#include <ulog.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <rsa.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

#define SHALEN 32
static const unsigned char SHA[SHALEN+512] = {
    0xb0,0xd9,0x85,0xc2,0xa3,0xf8,0xa2,0x0d,0x6d,0x39,0x82,0xc8,0x22,0xa9,0xbf,0xfb,
    0x5f,0x8a,0xfc,0xa1,0x4b,0xfb,0x25,0xf8,0xd2,0x1e,0x09,0xf3,0x2b,0x83,0x90,0xd4
};

#define MAX_MSGLEN 4096
int RSATest_MsgLen = 134;
unsigned char RSATest_Msg[MAX_MSGLEN+512] = {
    0x33,0x30,0x37,0x38,0x36,0x32,0x33,0x34,0x33,0x31,0x33,0x32,0x36,0x35,0x36,0x36,
    0x36,0x36,0x33,0x30,0x36,0x32,0x33,0x36,0x33,0x39,0x33,0x30,0x36,0x32,0x33,0x38,
    0x36,0x32,0x33,0x39,0x36,0x31,0x33,0x34,0x36,0x33,0x36,0x34,0x36,0x31,0x33,0x37,
    0x36,0x35,0x33,0x31,0x33,0x32,0x36,0x33,0x33,0x36,0x33,0x35,0x33,0x33,0x33,0x33,
    0x36,0x33,0x36,0x36,0x33,0x38,0x33,0x37,0x33,0x35,0x33,0x38,0x36,0x33,0x33,0x38,
    0x33,0x31,0x36,0x35,0x33,0x32,0x36,0x36,0x33,0x33,0x33,0x37,0x33,0x38,0x36,0x34,
    0x36,0x32,0x33,0x38,0x33,0x36,0x33,0x37,0x33,0x38,0x36,0x36,0x36,0x31,0x33,0x38,
    0x33,0x39,0x33,0x39,0x33,0x39,0x33,0x38,0x33,0x37,0x36,0x32,0x33,0x37,0x33,0x38,
    0x36,0x33,0x36,0x32,0x34,0x63
};

static const unsigned char Msg2[MAX_MSGLEN+512] = {
    0x36,0x32,0x33,0x38,0x33,0x36,0x33,0x37,0x33,0x38,0x36,0x36,0x36,0x31,0x33,0x38,
    0x36,0x36,0x33,0x30,0x36,0x32,0x33,0x36,0x33,0x39,0x33,0x30,0x36,0x32,0x33,0x38,
    0x36,0x33,0x36,0x36,0x33,0x38,0x33,0x37,0x33,0x35,0x33,0x38,0x36,0x33,0x33,0x38,
    0x36,0x32,0x33,0x39,0x36,0x31,0x33,0x34,0x36,0x33,0x36,0x34,0x36,0x31,0x33,0x37,
    0x36,0x35,0x33,0x31,0x33,0x32,0x36,0x33,0x33,0x36,0x33,0x35,0x33,0x33,0x33,0x33,
    0x33,0x30,0x37,0x38,0x36,0x32,0x33,0x34,0x33,0x31,0x33,0x32,0x36,0x35,0x36,0x36,
    0x33,0x31,0x36,0x35,0x33,0x32,0x36,0x36,0x33,0x33,0x33,0x37,0x33,0x38,0x36,0x34,
    0x33,0x39,0x33,0x39,0x33,0x39,0x33,0x38,0x33,0x37,0x36,0x32,0x33,0x37,0x33,0x38,
    0x36,0x33,0x36,0x32,0x34,0x63
};

#define KEYLEN 256
uint8_t RSATest_Key[KEYLEN+512] = {
    0xae,0x87,0xff,0x09,0x9e,0xc2,0x5c,0x05,0x84,0xaa,0xf7,0x78,0x0f,0x32,0x4c,0x73,
    0x48,0xbc,0xd7,0x54,0xd9,0x98,0xc9,0x3c,0x94,0xd4,0xec,0xbb,0xef,0x4c,0x61,0x0a,
    0x17,0x9f,0x64,0x51,0x2b,0x5f,0xed,0x54,0x7a,0x45,0x70,0xca,0x0a,0xae,0x8e,0xbb,
    0xa5,0x4a,0xeb,0x8b,0x0f,0xaa,0xfb,0xbd,0xd1,0x63,0xa1,0x88,0x48,0x55,0x13,0xba,
    0x35,0xfc,0xa1,0xdb,0xc3,0xb1,0xd6,0x4e,0xd2,0x79,0x93,0x9d,0x5f,0xac,0x56,0x75,
    0x0c,0xb7,0x26,0xb3,0x3e,0x7b,0x06,0x43,0xb3,0x19,0x1c,0xeb,0x8c,0xf1,0x1a,0x20,
    0xc4,0x68,0xdf,0x9b,0x68,0x30,0xa0,0x55,0xdd,0x5a,0x55,0x57,0xe8,0x98,0xa6,0x3e,
    0x42,0x28,0xbc,0xe1,0xd8,0xa8,0x44,0xc0,0x42,0xb9,0x8f,0xf8,0xb7,0x6a,0xcd,0x3c,
    0xdf,0xa4,0x25,0xc6,0xa1,0xf3,0xc8,0x47,0x72,0xbb,0xc2,0x74,0xb4,0x74,0x33,0x89,
    0x80,0x95,0xba,0x69,0x6a,0x21,0xd2,0xe0,0xaa,0x5d,0x33,0x03,0x0f,0xf8,0x13,0xcf,
    0xeb,0x29,0xf9,0x06,0x4d,0x0a,0x5a,0xe1,0xfa,0xc3,0x50,0x3d,0x46,0x96,0x4d,0x77,
    0xef,0x5a,0x52,0xe9,0x08,0x04,0xaf,0xae,0x3a,0xb7,0xa0,0x76,0x8a,0x3e,0x66,0xf1,
    0xc3,0x23,0x71,0xf8,0x64,0x53,0x54,0x55,0xa1,0xa5,0xce,0xe0,0x7f,0xe6,0xef,0xde,
    0x4c,0x55,0x59,0xea,0x0d,0x1c,0xf1,0xa4,0x10,0xca,0xf6,0x7d,0x4c,0x7c,0xd5,0x21,
    0x0c,0x41,0xf4,0x3a,0x39,0x43,0x7b,0x2a,0x8f,0x86,0xe7,0x51,0xf0,0x78,0x9d,0xe9,
    0xf7,0x18,0x60,0x5c,0x50,0x99,0x73,0xe3,0x50,0xf2,0x7b,0x34,0x07,0x46,0xd0,0x9b
};
uint8_t RSATest_Sig[KEYLEN+512] = {
    0x76,0xfb,0x0b,0x81,0x13,0x78,0x4b,0xc9,0x80,0x10,0xb7,0x2e,0x4c,0xf9,0x6d,0x04,
    0x23,0x4d,0xce,0x32,0x89,0x71,0xaf,0xc5,0xb9,0xf2,0xed,0x47,0x07,0x2a,0xce,0x1a,
    0x31,0x67,0x6c,0x18,0x47,0x97,0xe3,0x8f,0x25,0x02,0x8b,0x7e,0x28,0x9d,0x82,0xe9,
    0x6a,0xf5,0x42,0x3d,0x2c,0x87,0x0d,0x98,0x15,0xa1,0x75,0x00,0xa9,0xe1,0xea,0x82,
    0x51,0x15,0x29,0x90,0xf1,0xc4,0x05,0x37,0x8c,0xfa,0x23,0x0c,0x0c,0x3b,0xd2,0x7a,
    0x32,0x40,0x55,0x9a,0x8f,0xcf,0x34,0xe0,0x7a,0x1e,0x86,0x37,0x94,0xf5,0x83,0xba,
    0x57,0xb1,0x0d,0x12,0xdc,0x96,0xe1,0x27,0x7d,0x6b,0x35,0x1d,0x6e,0x8d,0xb4,0xb1,
    0x5f,0xff,0x55,0x91,0x66,0xa2,0xc2,0x97,0x07,0xd2,0xfe,0x1b,0xbd,0x33,0x32,0xbd,
    0xd0,0x1f,0x9f,0x47,0xa5,0xee,0xcb,0x37,0x23,0xed,0xfe,0xba,0x1b,0xa8,0xcf,0xf3,
    0xd9,0x6d,0xe9,0x63,0x8f,0xb5,0xa6,0x3a,0x67,0xca,0x7e,0x05,0xc2,0x84,0xe8,0x55,
    0xd5,0xaf,0x92,0xff,0xf6,0x09,0x9a,0x4b,0x6f,0x48,0x99,0xe9,0x0c,0x13,0x29,0x84,
    0x39,0xba,0x1a,0x97,0xe1,0xa7,0x80,0xd5,0xd6,0x74,0x5f,0xd8,0xe6,0x66,0x09,0x8e,
    0x40,0x96,0x24,0x22,0x8b,0xb4,0x34,0x72,0x5d,0xac,0x7c,0x30,0x1a,0x3b,0x9d,0x9c,
    0x48,0x47,0x75,0x66,0x3c,0xcd,0x43,0x75,0x1b,0xb7,0x08,0xef,0x01,0x1b,0xe8,0x9d,
    0x6b,0xb0,0xaf,0x91,0xa1,0xca,0x17,0x3d,0x7e,0xea,0x95,0x9a,0x69,0x4f,0x55,0x17,
    0x92,0x62,0x08,0x6f,0x44,0x37,0x47,0x8d,0xcd,0x71,0x49,0x0e,0xa4,0xfc,0x89,0x96
};

uint32_t rsalib_qt1_result[2];
int32_t GTestID = 0xFFFFFFFFUL;
BCM_TimeType GStartTime,GEndTime,GTimeTaken;

static void RSALib_test(uint32_t aIdx)
{

    int res = BCM_ERR_UNKNOWN;
    RSA_Status status = RSA_ERR_MISMATCH;

#if LOG_INFO
    int i;

    ULOG_INFO("\nMsg(%d):\n",RSATest_MsgLen);
    for(i=0;i<RSATest_MsgLen;i++) {
        ULOG_INFO("%02x",RSATest_Msg[i]);
    }
    ULOG_INFO("\nKey:\n");
    for(i=0;i<KEYLEN;i++) {
        ULOG_INFO("%02x",RSATest_Key[i]);
    }
    ULOG_INFO("\nSignature:\n");
    for(i=0;i<KEYLEN;i++) {
        ULOG_INFO("%02x",RSATest_ig[i]);
    }
    ULOG_INFO("\nSHA:\n");
    for(i=0;i<SHALEN;i++) {
        ULOG_INFO("%02x",SHA[i]);
    }
    ULOG_INFO("\n");
#endif
    BCM_GetTime(&GStartTime);
    if (0UL == aIdx){
        res = BCM_RSAVerify((uint32_t*)RSATest_Sig,256,(uint32_t*)RSATest_Key,256,RSATest_Msg,RSATest_MsgLen, &status);
        BCM_GetTime(&GEndTime);
        if ((1UL >= aIdx)  && ((RSA_SUCCESS != status) || (BCM_ERR_OK != res))) {
            ULOG_INFO("RSA verification fails : %d,%d\n",status,res);
            rsalib_qt1_result[aIdx] = BCM_ERR_DATA_INTEG;
        }
    }
    else if (1UL == aIdx){
        res = BCM_RSAVerify((uint32_t*)RSATest_Sig,256,(uint32_t*)RSATest_Key,256,Msg2,RSATest_MsgLen, &status);
        BCM_GetTime(&GEndTime);
        if ((1UL >= aIdx)  && ((RSA_ERR_MISMATCH != status) || (BCM_ERR_AUTH_FAILED != res))) {
            ULOG_INFO("RSA verification(Negative test) fails : %d,%d\n",status,res);
            rsalib_qt1_result[aIdx] = BCM_ERR_DATA_INTEG;
        }
    }


    GTimeTaken = BCM_GetTimeAbsDiff(GStartTime,GEndTime);
    ULOG_INFO("RSA Time: %dns\n",GTimeTaken.ns);

    if  ((1UL >= aIdx)  && (BCM_ERR_DATA_INTEG != rsalib_qt1_result[aIdx])) {
        rsalib_qt1_result[aIdx] = BCM_ERR_OK;
        ULOG_INFO("RSA verified\n");
    }

    ULOG_INFO("**********%s completed ***********\n",__func__);
}

/** @brief Main task for pp driver test
 *
 * Main task for pp driver test
 *
 * @return  void
 */
TASK(RSALibTest)
{
    if ((0UL == GTestID) || (1UL == GTestID)){
        RSALib_test(GTestID);
    }
    GTestID = 0xFFFFFFFFUL;
    BCM_TerminateTask();

}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if ((1UL == aIndex) || (2UL == aIndex)) {
        if (GTestID == 0xFFFFFFFFUL){
            GTestID = aIndex-1;
            rsalib_qt1_result[aIndex-1] = BCM_AT_EXECUTING;
            BCM_ActivateTask(RSALibTest);
            ret = BCM_ERR_OK;
        }else {
            ret = BCM_AT_NOT_STARTED;
        }
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = rsalib_qt1_result[0];
    }
    else if (2UL == aIndex){
        ret = rsalib_qt1_result[1];
    }
    return ret;
}
void ApplicationInit()
{
    rsalib_qt1_result[0] = BCM_AT_NOT_STARTED;
    rsalib_qt1_result[1] = BCM_AT_NOT_STARTED;
    GTestID = 0xFFFFFFFFUL;
}

