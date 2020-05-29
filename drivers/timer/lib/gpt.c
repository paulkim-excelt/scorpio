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
    @defgroup grp_timdrv_gpt_impl GPT Driver Implementation
    @ingroup grp_timdrv

    @addtogroup grp_timdrv_gpt_impl
    @{

    @file gpt.c
    @brief GPT driver interface
    This header file contains the interface implementation for GPT driver
    @version 0.30 Imported from docx
*/

#include <string.h>
#include <gpt.h>
#include <bcm_err.h>
#include <system.h>
#include <gpt_osil.h>

/**
    @name GPT driver design IDs
    @{
    @brief Design IDs for GPT driver
*/
#define BRCM_SWDSGN_TIM_REPORTERROR_PROC          (0x30U) /**< @brief #TIM_ReportError          */
#define BRCM_SWDSGN_TIM_STARTTIMER_PROC           (0x31U) /**< @brief #TIM_StartTimer           */
#define BRCM_SWDSGN_TIM_STOPTIMER_PROC            (0x32U) /**< @brief #TIM_StopTimer            */
#define BRCM_SWDSGN_TIM_GETTIMEELAPSED_PROC       (0x33U) /**< @brief #TIM_GetTimeElapsed       */
#define BRCM_SWDSGN_TIM_GETTIMEREMAINING_PROC     (0x34U) /**< @brief #TIM_GetTimeRemaining     */
#define BRCM_SWDSGN_TIM_DEINIT_PROC               (0x35U) /**< @brief #TIM_DeInit               */
#define BRCM_SWDSGN_TIM_INIT_PROC                 (0x36U) /**< @brief #TIM_Init                 */
#define BRCM_SWDSGN_TIM_ENABLENOTIFICATION_PROC   (0x37U) /**< @brief #TIM_EnableNotification   */
#define BRCM_SWDSGN_TIM_DISABLENOTIFICATION_PROC  (0x38U) /**< @brief #TIM_DisableNotification  */
#define BRCM_SWDSGN_TIM_GETFREERUNNINGTIME_PROC   (0x39U) /**< @brief #TIM_GetFreeRunningTime   */
#define BRCM_SWDSGN_TIM_GETTIME_PROC              (0x3AU) /**< @brief #TIM_GetTime              */
/** @} */

/**
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
static void TIM_ReportError(uint32_t aInstanceID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_SPT_ID & BCM_LOGMASK_USER), (uint8_t)aInstanceID, aApiID,
            aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
void TIM_StartTimer(TIM_ChanIDType aChanID,
                    TIM_TickType aValue)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    TIM_IOType timIO;

    if (0UL != aValue) {
        timIO.retVal = BCM_ERR_UNKNOWN;
        timIO.chanID = aChanID;
        timIO.targetTime = aValue;
        retVal = TIM_SysCmdReq(TIM_CMD_START_TIMER, &timIO);
    }
    if (BCM_ERR_OK != retVal) {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_STARTTIMER_PROC, retVal, aValue,
                0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
void TIM_StopTimer(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_IOType timIO;

    timIO.retVal = BCM_ERR_UNKNOWN;
    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_STOP_TIMER, &timIO);
    if (BCM_ERR_OK != retVal) {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_STOPTIMER_PROC, retVal, 0UL, 0UL,
                0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWREQ_TIM
*/
TIM_TickType TIM_GetTimeElapsed(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_TickType elapsedTime = 0UL;
    TIM_IOType timIO;

    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_GET_TIME_ELAPSED, &timIO);
    if (BCM_ERR_OK == retVal) {
        elapsedTime = timIO.elapsedTime;
    } else {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC, retVal, 0UL, 0UL,
                0UL, 0UL);
    }

    return elapsedTime;
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWREQ_TIM
*/
TIM_TickType TIM_GetTimeRemaining(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_TickType remainingTime = 0UL;
    TIM_IOType timIO;

    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_GET_TIME_REMAINING, &timIO);
    if (BCM_ERR_OK == retVal) {
        remainingTime = timIO.remainingTime;
    } else {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }

    return remainingTime;
}

/**
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
void TIM_DeInit(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_IOType timIO;

    timIO.retVal = BCM_ERR_UNKNOWN;
    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_DEINIT, &timIO);
    if (BCM_ERR_OK != retVal) {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_DEINIT_PROC, retVal, 0UL, 0UL, 0UL,
                0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
void TIM_Init(TIM_ChanIDType aChanID,
              const TIM_ConfigType *const aConfig)
{
    int32_t retVal;
    TIM_IOType timIO;

    if ((NULL == aConfig) || (aConfig->chanID != aChanID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        timIO.retVal = BCM_ERR_UNKNOWN;
        timIO.config = aConfig;
        timIO.chanID = aChanID;
        retVal = TIM_SysCmdReq(TIM_CMD_INIT, &timIO);
    }

    if (BCM_ERR_OK != retVal) {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_INIT_PROC, retVal, 0UL, 0UL, 0UL,
                0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM_NOTIFICATION
*/
void TIM_EnableNotification(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_IOType timIO;

    timIO.retVal = BCM_ERR_UNKNOWN;
    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_ENABLE_NOTIFICATION, &timIO);

    if (BCM_ERR_OK != retVal) {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM_NOTIFICATION
*/
void TIM_DisableNotification(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_IOType timIO;

    timIO.retVal = BCM_ERR_UNKNOWN;
    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_DISABLE_NOTIFICATION, &timIO);

    if (BCM_ERR_OK != retVal) {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWREQ_TIM
*/
TIM_TickType TIM_GetFreeRunningTime(TIM_ChanIDType aChanID)
{
    int32_t retVal;
    TIM_TickType elapsedTime = 0UL;
    TIM_IOType timIO;

    timIO.retVal = BCM_ERR_UNKNOWN;
    timIO.chanID = aChanID;
    retVal = TIM_SysCmdReq(TIM_CMD_GET_FREE_RUNNING_TIME, &timIO);
    if (BCM_ERR_OK == retVal) {
        elapsedTime = timIO.elapsedTime;
    } else {
        TIM_ReportError(aChanID, BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }

    return elapsedTime;
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWREQ_TIM
*/
uint64_t TIM_GetTime(void)
{
    int32_t retVal;
    uint64_t sysTime = 0ULL;
    TIM_IOType timIO;

    timIO.retVal = BCM_ERR_UNKNOWN;
    retVal = TIM_SysCmdReq(TIM_CMD_GET_TIME, &timIO);
    if (BCM_ERR_OK == retVal) {
        sysTime = timIO.sysTime;
    }

    return sysTime;
}
/** @} */
