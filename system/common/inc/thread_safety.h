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

/**
        @defgroup grp_thread_safety Thread Safety
        @ingroup grp_safety

        @addtogroup grp_thread_safety
        @{
        @file thread_safety.h
        @brief Thread Safety interface
        This header file implements thread safety interface

*/

#ifndef THREAD_SAFETY_H
#define THREAD_SAFETY_H

/**
  @name BCM Thread Safety API IDs
  @{
  @brief API IDs for Thread Safety
*/
#define BRCM_SWARCH_BCM_THREAD_STACK_PROC                  (0x01U) /**< @brief #BCM_MPUAddInitThreadStack */
#define BRCM_SWARCH_BCM_ADDTASK_MPUENTRY_PROC              (0x02U) /**< @brief #BCM_AddTaskMPUEntry */

/** @} */

#if defined(ENABLE_THREAD_PROTECTION)
/**
    @brief Add Task Info in MPU

    This API protect task memory space.

    @behavior Sync, Re-entrant

    @pre Previous is pre-empted and new task (with task id aTaskID)
         is getting resumed

    @param[in]      aTaskID         Task ID

    Return values are documented in reverse-chronological order
    @return     void

    @trace #BRCM_SWREQ_BCM_THREAD_PROTECTION

    @post MPU settings for new task, with aTaskID, is setup (stack, data and bss) .

    @limitations None
 */
extern void BCM_AddTaskMPUEntry(uint32_t aTaskID);

/**
    @brief Initialize MPU along with Task Attributes for Init thread (early init)

    This API Initialize and Enable MPU

    @behavior Sync, Re-entrant

    @pre CPU should be using MSP

    @param[in]      void

    Return values are documented in reverse-chronological order
    @return     void

    @trace #BRCM_SWREQ_BCM_THREAD_PROTECTION

    @post MPU is setup with init thread stack so that
          Init thread can start using PSP in unprivileged mode

    @limitations This should be used only during Early Init before any task starts
 */
extern void BCM_MPUAddInitThreadStack(void);
#endif
#endif /* THREAD_SAFETY_H */
/** @} */
