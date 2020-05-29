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
    @defgroup grp_autotest Test Automation
    @ingroup grp_system_sw

    @addtogroup grp_autotest
    @{
    @section sec_comp_overview Overview
    This header describes interfaces for test automation

    @todo update autotest header
    @section sec_comp_fsm State Machine
    TBD

    @section sec_comp_seq Sequence Diagrams
    TBD

    @section sec_comp_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @limitations None


    @file bcm_test.h
    @brief Interfaces for Automated testing
    This header file contains the interface functions for autotest
    @version 1.0 Initial version
*/

#ifndef BCM_TEST_H
#define BCM_TEST_H

#include <stdint.h>
/**
    @name Autotest API IDs
    @{
    @brief API IDs for Autotest
*/
#define BRCM_ARCH_AUTOTEST_RETVAL_MACRO     (0UL)   /**< @brief #BCM_AT_NOT_AVAILABLE */
#define BRCM_ARCH_AUTOTEST_SETTING_MACRO    (1UL)   /**< @brief #COMP_SETTING0 */
#define BRCM_ARCH_AUTOTEST_EXECUTE_PROC     (5UL)   /**< @brief #BCM_ExecuteAT  */
#define BRCM_ARCH_AUTOTEST_GETRESULT_PROC   (5UL)   /**< @brief #BCM_GetResultAT  */
/** @} */

/**
    @name Autotest error codes
    @{
    @brief Error codes for Autotest

    @trace #BRCM_ARCH_AUTOTEST_RETVAL_MACRO #BCM_REQ_AUTOTEST
*/
#define BCM_AT_NOT_AVAILABLE    (-1)   /**< Auto test not available */
#define BCM_AT_NOT_STARTED      (-2)   /**< Auto test not started */
#define BCM_AT_EXECUTING        (-3)   /**< Auto test running */
/** @} */

/** @brief Execute an automated test

    @behavior Async, Non-reentrant

    @pre None

    @param[in]      aIndex      32-bit Test Index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Test execution started
    @retval     Others                  As in Autotest error codes

    @post None

    @trace  #BRCM_ARCH_AUTOTEST_EXECUTE_PROC  #BCM_REQ_AUTOTEST

    @limitations None
*/
int32_t BCM_ExecuteAT(uint32_t aIndex);

/** @brief Get test result

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aIndex      32-bit Test Index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Test execution successful
    @retval     #BCM_AT_EXECUTING       Test is executing
    @retval     #BCM_AT_NOT_STARTED     If test is not started
    @retval     Others                  Test completed with this error

    @post None

    @trace  #BRCM_ARCH_AUTOTEST_GETRESULT_PROC  #BCM_REQ_AUTOTEST

    @limitations None
*/
int32_t BCM_GetResultAT(uint32_t aIndex);

#endif /* BCM_TEST_H */

