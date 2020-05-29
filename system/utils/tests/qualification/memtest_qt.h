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
    @defgroup grp_memtest_qt BCM memory Test
    @ingroup grp_memtest

    @addtogroup grp_memtest_qt
    @{

    @file memtest_qt.h
    @brief This header file contains internal interfaces for
    the BCM memory tests.
    @version 0.1 Initial Version
*/

#ifndef MEMTEST_QT_H
#define MEMTEST_QT_H

/**
    @brief Test BCM_MemTest API

    @pre None

    @test Invoke BCM_MemTest API on some scratch memory location

    @post Every byte in the memory region being tested on has been
          touched by the API. This is validated by checking the
          'garbage' value stored at that memory location.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_BCM_MEM_TEST
*/
#define BRCM_SWQTST_BCM_MEMTEST                 (1UL)

/**
    @brief Test BCM_MemTestNoStack API

    @pre None

    @test Invoke BCM_MemTestNoStack API on some scratch memory location

    @post Every byte in the memory region being tested on has been
          touched by the API. This is validated by checking the
          'garbage' value stored at that memory location.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_BCM_MEM_TEST
*/
#define BRCM_SWQTST_BCM_MEMTESTNOSTACK          (2UL)

/** @brief Memory Test sequence

    Detailed description of BRCM_SWQTSEQ_MEMORY_SEQ0

    @code{.c}

        BCM_MemTest() return value should be 0           #BRCM_SWQTST_BCM_MEMTEST

        BCM_MemTestNoStack() return value should be 0    #BRCM_SWQTST_BCM_MEMTESTNOSTACK

    @endcode

    @board board1
    @board board2

    @auto           Yes

    @type           Sanity

    @testproc #BRCM_SWQTST_BCM_MEMTEST
    @testproc #BRCM_SWQTST_BCM_MEMTESTNOSTACK

*/
#define BRCM_SWQTSEQ_MEMORY_SEQ0   (1UL)

#endif /* MEMTEST_QT_H */

/** @} */

