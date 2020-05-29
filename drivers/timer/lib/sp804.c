/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_timdrv_impl Implementation
    @ingroup grp_timdrv

    @addtogroup grp_timdrv_impl
    @{

    @file sp804.c
    @brief GPT driver implementation
    @version 0.30 Imported from docx
*/

#include <string.h>
#include <bcm_err.h>
#include <system.h>
#include <utils.h>
#include <interrupt.h>
#include <gpt_osil.h>
#include <tim_rdb.h>
#include <gpt.h>

/**
    @name GPT driver design IDs
    @{
    @brief Design IDs for GPT driver
*/
#define BRCM_SWDSGN_TIM_CONTROL_TYPE                 (0x60U) /**< @brief #TIM_ControlType               */
#define BRCM_SWDSGN_TIM_DRV_REPORTERROR_PROC         (0x61U) /**< @brief #TIM_DrvReportError            */
#define BRCM_SWDSGN_TIM_MAX_VALUE_MACRO              (0x62U) /**< @brief #TIM_MAX_VALUE                 */
#define BRCM_SWDSGN_TIM_DRV_CHANSTATE_TYPE           (0x63U) /**< @brief #TIM_DrvChanStateType          */
#define BRCM_SWDSGN_TIM_DRV_DEV_TYPE                 (0x64U) /**< @brief #TIM_DrvDevType                */
#define BRCM_SWDSGN_TIM_DRV_DEV_GLOBAL               (0x65U) /**< @brief #TIM_DrvDev                    */
#define BRCM_SWDSGN_TIM_SP804_REGS_GLOBAL            (0x66U) /**< @brief #TIM_SP804_Regs                */
#define BRCM_SWDSGN_TIM_SP804_GETTIMEELAPSED_PROC    (0x67U) /**< @brief #TIM_SP804GetTimeElapsed       */
#define BRCM_SWDSGN_TIM_SP804_INIT_PROC              (0x68U) /**< @brief #TIM_SP804Init                 */
#define BRCM_SWDSGN_TIM_SP804_IRQHANDLER_PROC        (0x69U) /**< @brief #TIM_SP804IRQHandler           */
#define BRCM_SWDSGN_TIM_DRV_GETFREERUNNINGTIME_PROC  (0x70U) /**< @brief #TIM_DrvGetFreeRunningTime     */
#define BRCM_SWDSGN_TIM_DRV_DISABLENOTIFICATION_PROC (0x71U) /**< @brief #TIM_DrvDisableNotification    */
#define BRCM_SWDSGN_TIM_SYSTEMDISABLENOTIFICATION_PROC (0x72U) /**< @brief #TIM_SystemDisableNotification    */
#define BRCM_SWDSGN_TIM_DRV_ENABLENOTIFICATION_PROC  (0x73U) /**< @brief #TIM_DrvEnableNotification     */
#define BRCM_SWDSGN_TIM_SYSTEMENABLENOTIFICATION_PROC  (0x74U) /**< @brief #TIM_SystemEnableNotification     */
#define BRCM_SWDSGN_TIM_DRV_GETTIMEREMAINING_PROC    (0x75U) /**< @brief #TIM_DrvGetTimeRemaining       */
#define BRCM_SWDSGN_TIM_DRV_GETTIMEELAPSED_PROC      (0x76U) /**< @brief #TIM_DrvGetTimeElapsed         */
#define BRCM_SWDSGN_TIM_DRV_STOPTIMER_PROC           (0x77U) /**< @brief #TIM_DrvStopTimer              */
#define BRCM_SWDSGN_TIM_SYSTEM_STOPTIMER_PROC        (0x78U) /**< @brief #TIM_SystemStopTimer              */
#define BRCM_SWDSGN_TIM_DRV_STARTTIMER_PROC          (0x79U) /**< @brief #TIM_DrvStartTimer             */
#define BRCM_SWDSGN_TIM_SYSTEM_STARTTIMER_PROC       (0x7AU) /**< @brief #TIM_SystemStartTimer             */
#define BRCM_SWDSGN_TIM_DRV_DEINIT_PROC              (0x7BU) /**< @brief #TIM_DrvDeInit                 */
#define BRCM_SWDSGN_TIM_DRV_INIT_PROC                (0x7CU) /**< @brief #TIM_DrvInit                   */
#define BRCM_SWDSGN_TIM_SVCIO_TYPE                   (0x7DU) /**< @brief #TIM_SVCIOType                 */
#define BRCM_SWDSGN_TIM_SYSCMDHANDLER_PROC           (0x7EU) /**< @brief #TIM_SysCmdHandler             */
#define BRCM_SWDSGN_TIM_IRQHANDLER_PROC              (0x7FU) /**< @brief #TIM_IRQHandler                */
#define BRCM_SWDSGN_TIM_GETSYSTEMTIME_PROC           (0x80U) /**< @brief #TIM_GetSystemTime             */
#define BRCM_SWDSGN_TIM_GETTIMERAW_PROC              (0x81U) /**< @brief #TIM_GetTimeRaw                */
/** @} */

/**
    @name TIM_ControlType
    @{
    @brief TIM_ControlType

    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM
*/
typedef uint32_t TIM_ControlType;                       /**< @brief Register values                     */
#define TIM_CONTROL_TIMER_EN_MASK             (0x80UL)  /**< @brief TIM_CONTROL_TIMER_EN_MASK           */
#define TIM_CONTROL_TIMER_EN_SHIFT            (7UL)     /**< @brief TIM_CONTROL_TIMER_EN_SHIFT          */
#define TIM_CONTROL_MODE_PERIOD_MASK          (0x40UL)  /**< @brief TIM_CONTROL_MODE_PERIOD_MASK        */
#define TIM_CONTROL_MODE_PERIOD_SHIFT         (6UL)     /**< @brief TIM_CONTROL_MODE_PERIOD_SHIFT       */
#define TIM_CONTROL_INT_EN_MASK               (0x20UL)  /**< @brief TIM_CONTROL_INT_EN_MASK             */
#define TIM_CONTROL_INT_EN_SHIFT              (5UL)     /**< @brief TIM_CONTROL_INT_EN_SHIFT            */
#define TIM_CONTROL_PRESCALE_MASK             (0xCUL)   /**< @brief TIM_CONTROL_PRESCALE_MASK           */
#define TIM_CONTROL_PRESCALE_SHIFT            (2UL)     /**< @brief TIM_CONTROL_PRESCALE_SHIFT          */
#define TIM_CONTROL_PRESCALE_STAGE_0          (0UL)     /**< @brief TIM_CONTROL_PRESCALE_STAGE_0        */
#define TIM_CONTROL_PRESCALE_STAGE_4          (1UL)     /**< @brief TIM_CONTROL_PRESCALE_STAGE_4        */
#define TIM_CONTROL_PRESCALE_STAGE_8          (2UL)     /**< @brief TIM_CONTROL_PRESCALE_STAGE_8        */
#define TIM_CONTROL_SIZE_32_BIT_MASK          (0x2UL)   /**< @brief TIM_CONTROL_SIZE_32_BIT_MASK        */
#define TIM_CONTROL_SIZE_32_BIT_SHIFT         (1UL)     /**< @brief TIM_CONTROL_SIZE_32_BIT_SHIFT       */
#define TIM_CONTROL_ONE_SHOT_MODE_MASK        (0x1UL)   /**< @brief TIM_CONTROL_ONE_SHOT_MODE_MASK      */
#define TIM_CONTROL_ONE_SHOT_MODE_SHIFT       (0UL)     /**< @brief TIM_CONTROL_ONE_SHOT_MODE_SHIFT     */
#define TIM_CONTROL_INT_CLR_MASK              (0x1UL)   /**< @brief TIM_CONTROL_INT_CLR_MASK            */

/** @} */

/**
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
static void TIM_DrvReportError(uint32_t aInstanceID, uint8_t aApiID,
                                int32_t aErr, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
                                uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_SPT_ID, (uint8_t)aInstanceID, aApiID, aErr, 4UL,
            values);
}

/**
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
#define TIM_MAX_VALUE(timerWidth) \
    ((TIM_WIDTH_32_BIT == (timerWidth)) ? 0xFFFFFFFFUL : 0xFFFFUL)

/**
    @trace #BRCM_SWARCH_TIM_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
typedef uint32_t TIM_DrvChanStateType;          /**< @brief TIM_DrvChanStateType        */
#define TIM_DRV_CHANSTATE_UNINIT       (0UL)    /**< @brief TIM_DRV_CHANSTATE_UNINIT    */
#define TIM_DRV_CHANSTATE_INIT         (1UL)    /**< @brief TIM_DRV_CHANSTATE_INIT      */
#define TIM_DRV_CHANSTATE_RUNNING      (2UL)    /**< @brief TIM_DRV_CHANSTATE_RUNNING   */
#define TIM_DRV_CHANSTATE_STOPPED      (3UL)    /**< @brief TIM_DRV_CHANSTATE_STOPPED   */

/**
    @trace #BRCM_SWARCH_TIM_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
typedef struct _TIM_DrvDevType {
    uint32_t                loadValue;             /**< @brief loadValue    */
    uint32_t                freq;                  /**< @brief freq         */
    uint32_t                sysTimeEn;             /**< @brief sysTimeEn    */
    uint64_t                sysTimeCnt;            /**< @brief sysTimeCnt   */
    TIM_DrvChanStateType    state;                 /**< @brief state        */
    TIM_ChanModeType        chanMode;              /**< @brief chanMode     */
    TIM_WidthType           width;                 /**< @brief width        */
    TIM_CbType              cb;                    /**< @brief cb           */
} TIM_DrvDevType;

/**
    @trace #BRCM_SWARCH_TIM_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
static TIM_DrvDevType TIM_DrvDev[TIM_MAX_CHANNELS] COMP_SECTION(".data.drivers") =
{
#if (TIM_MAX_CHANNELS == 0UL)
#error "TIM_MAX_CHANNELS == 0UL"
#endif
    {
        .sysTimeEn = FALSE,
        .sysTimeCnt = 0ULL,
        .state = TIM_DRV_CHANSTATE_UNINIT,
        .cb = NULL,
    },
#if (TIM_MAX_CHANNELS > 1UL)
    {
        .sysTimeEn = FALSE,
        .sysTimeCnt = 0ULL,
        .state = TIM_DRV_CHANSTATE_UNINIT,
        .cb = NULL,
    },
#endif
#if (TIM_MAX_CHANNELS > 2UL)
#error "TIM_MAX_CHANNELS > 2UL is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMERAW_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWARCH_TIM_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_TIM
*/
static TIM_RDBType *const TIM_SP804_Regs = (TIM_RDBType *const)TIM_BASE;

/**
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMERAW_PROC
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWREQ_TIM
*/
static TIM_TickType TIM_SP804GetTimeElapsed(TIM_ChanIDType aChanID)
{
    uint32_t targetTime;
    uint32_t currTime;
    TIM_TickType timeElapsed;

    targetTime = TIM_SP804_Regs->tim_chan_regs[aChanID].timer1load;
    currTime = TIM_SP804_Regs->tim_chan_regs[aChanID].timer1value;
    timeElapsed = targetTime - currTime;

    return timeElapsed;
}

/**
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
static int32_t TIM_SP804Init(const TIM_ConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t prescale;
    TIM_ChanIDType chanID = aConfig->chanID;

    /* Disable interrupts and timer module */
    TIM_SP804_Regs->tim_chan_regs[chanID].timer1control &= ~(TIM_CONTROL_INT_EN_MASK
            | TIM_CONTROL_TIMER_EN_MASK);
    /* Clear the interrupt */
    TIM_SP804_Regs->tim_chan_regs[chanID].timer1intclr |= TIM_CONTROL_INT_CLR_MASK;
    /* Set the timer mode */
    switch (aConfig->chanMode) {
    case TIM_CHAN_MODE_ONE_SHOT:
        TIM_SP804_Regs->tim_chan_regs[chanID].timer1control |= TIM_CONTROL_ONE_SHOT_MODE_MASK;
        break;
    case TIM_CHAN_MODE_FREE_RUNNING:
        TIM_SP804_Regs->tim_chan_regs[chanID].timer1control &= ~(TIM_CONTROL_ONE_SHOT_MODE_MASK
                    | TIM_CONTROL_MODE_PERIOD_MASK);
        break;
    case TIM_CHAN_MODE_PERIODIC:
        TIM_SP804_Regs->tim_chan_regs[chanID].timer1control |= TIM_CONTROL_MODE_PERIOD_MASK;
        TIM_SP804_Regs->tim_chan_regs[chanID].timer1control &= ~TIM_CONTROL_ONE_SHOT_MODE_MASK;
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }
    if (BCM_ERR_OK != retVal) {
        goto err;
    }
    /* Set the prescale value */
    switch (aConfig->prescale) {
    case TIM_PRESCALE_DIV_1:
        prescale = TIM_CONTROL_PRESCALE_STAGE_0;
        break;
    case TIM_PRESCALE_DIV_16:
        prescale = TIM_CONTROL_PRESCALE_STAGE_4;
        break;
    case TIM_PRESCALE_DIV_256:
        prescale = TIM_CONTROL_PRESCALE_STAGE_8;
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }
    if (BCM_ERR_OK != retVal) {
        goto err;
    }
    TIM_SP804_Regs->tim_chan_regs[chanID].timer1control &= ~TIM_CONTROL_PRESCALE_MASK;
    TIM_SP804_Regs->tim_chan_regs[chanID].timer1control |=
        ((prescale << TIM_CONTROL_PRESCALE_SHIFT) & TIM_CONTROL_PRESCALE_MASK);
    /* Set the size of the timer */
    switch (aConfig->width) {
    case TIM_WIDTH_16_BIT:
        TIM_SP804_Regs->tim_chan_regs[chanID].timer1control &= ~TIM_CONTROL_SIZE_32_BIT_MASK;
        break;
    case TIM_WIDTH_32_BIT:
        TIM_SP804_Regs->tim_chan_regs[chanID].timer1control |= TIM_CONTROL_SIZE_32_BIT_MASK;
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_TIM_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_TIM_KERNEL_HANDLER
*/
static void TIM_SP804IRQHandler(TIM_ChanIDType aChanID)
{
    /* Clear the interrupt */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1intclr |= TIM_CONTROL_INT_CLR_MASK;
    if (TRUE == TIM_DrvDev[aChanID].sysTimeEn) {
        TIM_DrvDev[aChanID].sysTimeCnt++;
    }
    if (TIM_DRV_CHANSTATE_RUNNING == TIM_DrvDev[aChanID].state) {
        if (TIM_CHAN_MODE_ONE_SHOT == TIM_DrvDev[aChanID].chanMode) {
            TIM_DrvDev[aChanID].state = TIM_DRV_CHANSTATE_STOPPED;
        }
        /* Give callback */
        if (NULL != TIM_DrvDev[aChanID].cb) {
            TIM_DrvDev[aChanID].cb();
        }
    }
}

/**
    @trace #BRCM_SWARCH_TIM_GETFREERUNNINGTIME_PROC
    @trace #BRCM_SWREQ_TIM
*/
static TIM_TickType TIM_DrvGetFreeRunningTime(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;
    TIM_TickType elapsedTime = 0UL;

    if (TIM_CHAN_MODE_FREE_RUNNING != TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOT_FOUND;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    elapsedTime = TIM_SP804GetTimeElapsed(aChanID);

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_GETFREERUNNINGTIME_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
    return elapsedTime;
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIME_PROC
    @trace #BRCM_SWREQ_TIM
*/
uint64_t TIM_GetSystemTime(void)
{
    uint32_t i;
    uint32_t ticks;
    uint64_t sysTime = 0ULL;
    INTR_FlagType flag;

    for (i = 0UL; i < TIM_MAX_CHANNELS; i++) {
        if (TRUE == TIM_DrvDev[i].sysTimeEn) {
            flag = INTR_Suspend();
            ticks = TIM_SP804GetTimeElapsed(i);
            sysTime = (((TIM_DrvDev[i].sysTimeCnt * TIM_DrvDev[i].loadValue) +
                        ticks) * (1000000000ULL / TIM_DrvDev[i].freq));
            INTR_Resume(flag);
            break;
        }
    }
    if (TIM_MAX_CHANNELS == i) {
        TIM_DrvReportError(i, BRCM_SWDSGN_TIM_GETSYSTEMTIME_PROC, BCM_ERR_NOT_FOUND, 0UL,
                0UL, 0UL, 0UL);
    }

    return sysTime;
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIMERAW_PROC
    @trace #BRCM_SWREQ_TIM
*/
uint64_t TIM_GetTimeRaw(TIM_ChanIDType aChanID)
{
    uint32_t ticks;
    uint32_t control;
    uint32_t prescale;
    uint32_t divisor;
    uint32_t freq;
    uint64_t sysTime = 0ULL;
    INTR_FlagType flag;

    flag = INTR_Suspend();
    control = TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control;

    prescale = (control & TIM_CONTROL_PRESCALE_MASK) >> TIM_CONTROL_PRESCALE_SHIFT;
    if (TIM_PRESCALE_DIV_1 == prescale) {
        divisor = 1UL;
    } else if (TIM_PRESCALE_DIV_16 == prescale) {
        divisor = 16UL;
    } else {
        divisor = 256UL;
    }

    ticks = TIM_SP804GetTimeElapsed(aChanID);
    freq = TIM_CLOCK / divisor;
    sysTime = ticks * (1000000000ULL / freq);
    INTR_Resume(flag);

    return sysTime;
}

/**
    @trace #BRCM_SWARCH_TIM_DISABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM_NOTIFICATION
*/
static void TIM_DrvDisableNotification(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;

    if (TIM_CHAN_MODE_FREE_RUNNING == TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (NULL == TIM_DrvDev[aChanID].cb) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Disable interrupt */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control &= ~TIM_CONTROL_INT_EN_MASK;

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_DISABLENOTIFICATION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_SYSTEMDISABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM_NOTIFICATION
*/
void TIM_SystemDisableNotification(TIM_ChanIDType aChanID)
{
    TIM_DrvDisableNotification(aChanID);
}

/**
    @trace #BRCM_SWARCH_TIM_ENABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM_NOTIFICATION
*/
static void TIM_DrvEnableNotification(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;

    if (TIM_CHAN_MODE_FREE_RUNNING == TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (NULL == TIM_DrvDev[aChanID].cb) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Enable interrupt */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control |= TIM_CONTROL_INT_EN_MASK;

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_ENABLENOTIFICATION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_SYSTEMENABLENOTIFICATION_PROC
    @trace #BRCM_SWREQ_TIM_NOTIFICATION
*/
void TIM_SystemEnableNotification(TIM_ChanIDType aChanID)
{
    TIM_DrvEnableNotification(aChanID);
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIMEREMAINING_PROC
    @trace #BRCM_SWREQ_TIM
*/
static TIM_TickType TIM_DrvGetTimeRemaining(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;
    TIM_TickType remainingTime = 0UL;

    if (TIM_CHAN_MODE_FREE_RUNNING == TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_INIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_OK;
        goto err;
    }
    remainingTime = TIM_SP804_Regs->tim_chan_regs[aChanID].timer1value;

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_GETTIMEREMAINING_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
    return remainingTime;
}

/**
    @trace #BRCM_SWARCH_TIM_GETTIMEELAPSED_PROC
    @trace #BRCM_SWREQ_TIM
*/
static TIM_TickType TIM_DrvGetTimeElapsed(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;
    TIM_TickType elapsedTime = 0UL;

    if (TIM_CHAN_MODE_FREE_RUNNING == TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_INIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_OK;
        goto err;
    }
    elapsedTime = TIM_SP804GetTimeElapsed(aChanID);

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_GETTIMEELAPSED_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
    return elapsedTime;
}

/**
    @trace #BRCM_SWARCH_TIM_STOPTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
static void TIM_DrvStopTimer(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;

    if (TIM_CHAN_MODE_FREE_RUNNING == TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_RUNNING != TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_OK;
        goto err;
    }

    /* Disable Timer module */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control &= ~TIM_CONTROL_TIMER_EN_MASK;

    TIM_DrvDev[aChanID].state = TIM_DRV_CHANSTATE_STOPPED;

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_STOPTIMER_PROC, retVal, 0UL, 0UL,
                0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_SYSTEMSTOPTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
void TIM_SystemStopTimer(TIM_ChanIDType aChanID)
{
    TIM_DrvStopTimer(aChanID);
}

/**
    @trace #BRCM_SWARCH_TIM_STARTTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
static void TIM_DrvStartTimer(TIM_ChanIDType aChanID,
                       TIM_TickType aValue)
{
    int32_t retVal = BCM_ERR_OK;

    if (TIM_CHAN_MODE_FREE_RUNNING == TIM_DrvDev[aChanID].chanMode) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (TIM_DRV_CHANSTATE_RUNNING == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }
    if (TIM_MAX_VALUE(TIM_DrvDev[aChanID].width) < aValue) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Disable Timer module */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control &= ~TIM_CONTROL_TIMER_EN_MASK;
    /* Load the value */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1load = aValue;
    /* Clear the interrupt */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1intclr |= TIM_CONTROL_INT_CLR_MASK;
    /* Enable Timer module */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control |= TIM_CONTROL_TIMER_EN_MASK;

    TIM_DrvDev[aChanID].state = TIM_DRV_CHANSTATE_RUNNING;
    TIM_DrvDev[aChanID].loadValue = aValue;

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_STARTTIMER_PROC, retVal,
                aValue, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_SYSTEMSTARTTIMER_PROC
    @trace #BRCM_SWREQ_TIM
*/
void TIM_SystemStartTimer(TIM_ChanIDType aChanID,
                       TIM_TickType aValue)
{
    TIM_DrvStartTimer(aChanID, aValue);
}

/**
    @trace #BRCM_SWARCH_TIM_DEINIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
static void TIM_DrvDeInit(TIM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;

    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[aChanID].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if ((TIM_DRV_CHANSTATE_RUNNING == TIM_DrvDev[aChanID].state) &&
            (TIM_CHAN_MODE_FREE_RUNNING != TIM_DrvDev[aChanID].chanMode)) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }

    /* Disable interrupts and timer module */
    TIM_SP804_Regs->tim_chan_regs[aChanID].timer1control &= ~(TIM_CONTROL_INT_EN_MASK
            | TIM_CONTROL_TIMER_EN_MASK);

    TIM_DrvDev[aChanID].state = TIM_DRV_CHANSTATE_UNINIT;

err:
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(aChanID, BRCM_SWDSGN_TIM_DRV_DEINIT_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_INIT_PROC
    @trace #BRCM_SWREQ_TIM
*/
static void TIM_DrvInit(const TIM_ConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_UNINIT;
    uint32_t divisor;
    TIM_ChanIDType chanID = aConfig->chanID;

    if (TIM_DRV_CHANSTATE_UNINIT == TIM_DrvDev[chanID].state) {
        retVal = TIM_SP804Init(aConfig);
        if (BCM_ERR_OK == retVal) {
            TIM_DrvDev[chanID].state = TIM_DRV_CHANSTATE_INIT;
            TIM_DrvDev[chanID].chanMode = aConfig->chanMode;
            TIM_DrvDev[chanID].width = aConfig->width;
            TIM_DrvDev[chanID].cb = aConfig->cb;
            TIM_DrvDev[chanID].sysTimeEn = aConfig->sysTimeEn;
            switch (aConfig->prescale) {
            case TIM_PRESCALE_DIV_1:
                divisor = 1UL;
                break;
            case TIM_PRESCALE_DIV_16:
                divisor = 16UL;
                break;
            default:
                divisor = 256UL;
                break;
            }
            TIM_DrvDev[chanID].freq = TIM_CLOCK / divisor;
            if (TIM_CHAN_MODE_FREE_RUNNING == aConfig->chanMode) {
                /* Disable Timer module */
                TIM_SP804_Regs->tim_chan_regs[chanID].timer1control &= ~TIM_CONTROL_TIMER_EN_MASK;
                /* Load the value */
                TIM_SP804_Regs->tim_chan_regs[chanID].timer1load = TIM_MAX_VALUE(aConfig->width);
                /* Clear the interrupt */
                TIM_SP804_Regs->tim_chan_regs[chanID].timer1intclr |= TIM_CONTROL_INT_CLR_MASK;
                /* Enable Timer module */
                TIM_SP804_Regs->tim_chan_regs[chanID].timer1control |= TIM_CONTROL_TIMER_EN_MASK;
                TIM_DrvDev[chanID].state = TIM_DRV_CHANSTATE_RUNNING;
            }
        }
    }
    if (BCM_ERR_OK != retVal) {
        TIM_DrvReportError(chanID, BRCM_SWDSGN_TIM_DRV_INIT_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_TIM_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_TIM_KERNEL_HANDLER
*/
typedef union _TIM_SVCIOType {
    uint8_t     *data;
    TIM_IOType  *io;
} TIM_SVCIOType;

/**
    @trace #BRCM_SWARCH_TIM_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_TIM_KERNEL_HANDLER

    @code{.c}
    if aSysIO.timIO is not NULL
        if aMagicID is SVC_MAGIC_SPT_ID
            timIO->retVal = TIM_CmdHandler(aCmd, timIO)
        else
            timIO->retVal = BCM_ERR_INVAL_MAGIC
    @endcode
*/
void TIM_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    TIM_SVCIOType tim;
    tim.data = aSysIO;
    int32_t retVal = BCM_ERR_OK;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_SPT_ID) {
            if (TIM_CMD_INIT == aCmd) {
                TIM_DrvInit(tim.io->config);
            } else if (TIM_CMD_GET_TIME == aCmd) {
                tim.io->sysTime = TIM_GetSystemTime();
            } else if (TIM_MAX_CHANNELS > tim.io->chanID) {
                switch (aCmd) {
                case TIM_CMD_DEINIT:
                    TIM_DrvDeInit(tim.io->chanID);
                    break;
                case TIM_CMD_GET_TIME_ELAPSED:
                    tim.io->elapsedTime = TIM_DrvGetTimeElapsed(tim.io->chanID);
                    break;
                case TIM_CMD_GET_TIME_REMAINING:
                    tim.io->remainingTime = TIM_DrvGetTimeRemaining(tim.io->chanID);
                    break;
                case TIM_CMD_START_TIMER:
                    TIM_DrvStartTimer(tim.io->chanID, tim.io->targetTime);
                    break;
                case TIM_CMD_STOP_TIMER:
                    TIM_DrvStopTimer(tim.io->chanID);
                    break;
                case TIM_CMD_ENABLE_NOTIFICATION:
                    TIM_DrvEnableNotification(tim.io->chanID);
                    break;
                case TIM_CMD_DISABLE_NOTIFICATION:
                    TIM_DrvDisableNotification(tim.io->chanID);
                    break;
                case TIM_CMD_GET_FREE_RUNNING_TIME:
                    tim.io->elapsedTime = TIM_DrvGetFreeRunningTime(tim.io->chanID);
                    break;
                default:
                    retVal = BCM_ERR_INVAL_PARAMS;
                    break;
                }
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        tim.io->retVal = retVal;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_SPT_ID, (uint8_t)0xFFU, BRCM_SWARCH_TIM_SYSCMDHANDLER_PROC, retVal,
                        4UL, values);
    }
}

/**
    @trace #BRCM_SWARCH_TIM_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_TIM_KERNEL_HANDLER
*/
void TIM_IRQHandler(TIM_ChanIDType aChanID)
{
    if (TIM_MAX_CHANNELS > aChanID) {
        TIM_SP804IRQHandler(aChanID);
    }
}

/** @} */
