/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_circq_qt
    @{

    @file circ_queue_test.c
    @brief CIRCQ Qualification Test

    @version 0.1 Initial Version
*/
#include <stdlib.h>
#include <string.h>
#include <console.h>
#include <shell.h>

#include "ee.h"
#include <ulog.h>
#include <atomic.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <circ_queue.h>

#include "utils_test.h"
#include "circ_queue_qt.h"

#define GetModuleLogLevel()     ULOG_LVL_INFO

#define CIRCQ_LOG2SIZE      (4UL)

#define CIRCQ_MARK_ERR(ret)                                    \
    {                                                          \
        ULOG_ERR("\nError: %d \t line %d\n", ret ,__LINE__);   \
        if (BCM_AT_EXECUTING == CIRCQ_QTResult){               \
            (CIRCQ_QTResult) = (ret);                          \
        }                                                      \
    }

static volatile int32_t CIRCQ_QTResult = BCM_AT_NOT_STARTED;

/* Declaring an empty circular Queue */
CIRCQ_V2_DEFINE(CIRC_Q,
                CIRCQ_LOG2SIZE,
                uint8_t,
                CIRCQ_Buf,CIRCQ_Idx,
                CIRCQ_OVERWRITE_DISABLE,
                ".data");


int32_t CircQQT_Start(void)
{
    int32_t retUnknown = BCM_ERR_UNKNOWN;
    int32_t retVal;
    uint32_t isEmpty;
    uint32_t isFull;
    uint32_t localCnt = 0UL;
    uint32_t itemsPeeked;
    uint8_t bufpeekArray[16UL] = {0};
    uint8_t bufpopArray[16UL] = {0};
    uint8_t bufpushArrayOne[1UL] = {1};
    uint8_t bufpushArrayAll[16UL];
    uint32_t i;

    for (i=0UL; i<16UL; i++) {
        bufpushArrayAll[i] = (uint8_t)(i);
    }

    CIRCQ_QTResult = BCM_AT_EXECUTING;

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (1UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_INIT_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_INIT_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (0UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_INIT_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (0UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_INIT_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    if (0UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_INIT_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_INIT_POP,
                           CONSOLE_QUAL_FAIL);
    }

    /* Push one item into the empty CIRCQ */

    retVal = CIRCQ_Push(&CIRC_Q, (const char* const)bufpushArrayOne, 1UL);
    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (0UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (1UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (1UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    /* push, pop one item from the CIRCQ */

    if (1UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (1UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (0UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (0UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    if (0UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_POP,
                           CONSOLE_QUAL_FAIL);
    }

    /* push, pop and push one item into the CIRCQ */

    retVal = CIRCQ_Push(&CIRC_Q,(const char* const)bufpushArrayOne, 1UL);
    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (0UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (1UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (1UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    /* push, pop, push and push item into the CIRCQ */

    retVal = CIRCQ_Push(&CIRC_Q,(const char* const)bufpushArrayOne, 1UL);
    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (0UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (2UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 2UL);
    if (2UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    if (1UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_PUSH_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP,
                           CONSOLE_QUAL_FAIL);
    }

    /* push, pop, push and pop one item from the CIRCQ */

    if (1UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)){
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (1UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (0UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (0UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_POP_PUSH_POP_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    /* Now the CIRCQ is empty, Push all the items into the CIRCQ */

    retVal = CIRCQ_Push(&CIRC_Q,(const char* const)bufpushArrayAll, 15UL);
    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (0UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (1UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (localCnt == 15UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 15UL);
    if (itemsPeeked == 15UL) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_Push(&CIRC_Q,(const char* const)bufpushArrayOne, 1UL);
    if (BCM_ERR_NOMEM == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_PUSH:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_PUSH,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_PUSH,
                           CONSOLE_QUAL_FAIL);
    }

    /* pop one item from the full CIRCQ and check */

    if (1UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (0UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (14UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 14UL);
    if (itemsPeeked == 14UL) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_Push(&CIRC_Q,(const char* const)bufpushArrayOne, 1UL);
    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_PUSH:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_PUSH,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_PUSH,
                           CONSOLE_QUAL_FAIL);
    }

    /* Now the CIRCQ is full pop all the items from the CIRCQ and check */

    if (15UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 15UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (1UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (0UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (0UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    if (0UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_FULL_POP_ALL_ITEMS_POP,
                           CONSOLE_QUAL_FAIL);
    }

    /* Now the CIRCQ is empty push one item into the CIRCQ and reset the CIRCQ */

    retVal = CIRCQ_Push(&CIRC_Q,(const char* const)bufpushArrayOne, 1UL);

    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_PUSH_RESET:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_RESET,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_PUSH_RESET,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_Reset(&CIRC_Q);
    if (BCM_ERR_OK == retVal) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_RESET:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsEmpty(&CIRC_Q, &isEmpty);
    if ((BCM_ERR_OK == retVal) && (1UL == isEmpty)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_RESET_ISEMPTY:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_ISEMPTY,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_ISEMPTY,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_IsFull(&CIRC_Q, &isFull);
    if ((BCM_ERR_OK == retVal) && (0UL == isFull)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_RESET_ISFULL:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_ISFULL,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_ISFULL,
                           CONSOLE_QUAL_FAIL);
    }

    retVal = CIRCQ_GetFilledSize(&CIRC_Q, &localCnt);
    if ((BCM_ERR_OK == retVal) && (0UL == localCnt)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_RESET_GETFILLEDSIZE:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_GETFILLEDSIZE,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retVal);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_GETFILLEDSIZE,
                           CONSOLE_QUAL_FAIL);
    }

    itemsPeeked = CIRCQ_Peek(&CIRC_Q, (char* const)bufpeekArray, 1UL);
    if (0UL == itemsPeeked) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_RESET_PEEK:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_PEEK,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_PEEK,
                           CONSOLE_QUAL_FAIL);
    }

    if (0UL == CIRCQ_Pop(&CIRC_Q, (char* const)bufpopArray, 1UL)) {
        ULOG_INFO("BRCM_SWQTST_CIRCQ_RESET_POP:Pass\n");
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_POP,
                           CONSOLE_QUAL_PASS);
    } else {
        CIRCQ_MARK_ERR(retUnknown);
        CONSOLE_QualReport(CONSOLE_UART,
                           BCM_UTL_ID,
                           BRCM_SWQTSEQ_CIRCQ_SEQ0,
                           BRCM_SWQTST_CIRCQ_RESET_POP,
                           CONSOLE_QUAL_FAIL);
    }

    return BCM_ERR_OK;

}

int32_t CircQQT_GetResult(void)
{
    int32_t ret = CIRCQ_QTResult;
    if (BCM_AT_EXECUTING == ret) {
        ret = BCM_ERR_OK;
    }
    return ret;
}

/** @} */
