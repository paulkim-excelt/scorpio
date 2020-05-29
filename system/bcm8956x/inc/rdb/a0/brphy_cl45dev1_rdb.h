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
    @file brphy_cl45dev1_rdb.h
    @brief RDB File for BRPHY_CL45DEV1

    @version 2019Jan21_rdb
*/

#ifndef BRPHY_CL45DEV1_RDB_H
#define BRPHY_CL45DEV1_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t BRPHY_CL45DEV1_PMD_IEEE_CTL1_TYPE;
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_RESET_MASK (0x8000U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_RESET_SHIFT (15U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_SPEED_SEL_1_MASK (0x2000U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_SPEED_SEL_1_SHIFT (13U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_LOW_PWR_MASK (0x800U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_LOW_PWR_SHIFT (11U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_SPEED_SEL_0_MASK (0x40U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_SPEED_SEL_0_SHIFT (6U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_SPEED_SEL_10G_MASK (0x3cU)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_SPEED_SEL_10G_SHIFT (2U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_LPBK_MASK (0x1U)
#define BRPHY_CL45DEV1_PMD_IEEE_CTL1_LPBK_SHIFT (0U)




typedef uint8_t BRPHY_CL45DEV1_RESERVED_TYPE;




typedef volatile struct COMP_PACKED _BRPHY_CL45DEV1_RDBType {
    BRPHY_CL45DEV1_PMD_IEEE_CTL1_TYPE ctl1; /* OFFSET: 0x0 */
    BRPHY_CL45DEV1_RESERVED_TYPE rsvd0[13846]; /* OFFSET: 0x2 */
} BRPHY_CL45DEV1_RDBType;


#define BRPHY1_CL45DEV1_BASE            (0x49020000UL)

#define BRPHY2_CL45DEV1_BASE            (0x49420000UL)

#define BRPHY3_CL45DEV1_BASE            (0x49820000UL)

#define BRPHY4_CL45DEV1_BASE            (0x49C20000UL)



#define BRPHY_CL45DEV1_MAX_HW_ID        (4UL)


#define BRPHY0_CL45DEV1_CTRL_BASE       (BRPHY1_CL45DEV1_BASE)


#define BRPHY1_CL45DEV1_CTRL_BASE       (BRPHY2_CL45DEV1_BASE)


#define BRPHY2_CL45DEV1_CTRL_BASE       (BRPHY3_CL45DEV1_BASE)


#define BRPHY3_CL45DEV1_CTRL_BASE       (BRPHY4_CL45DEV1_BASE)


#define BRPHY4_CL45DEV1_CTRL_BASE       (UNDEFINED)

#endif /* BRPHY_CL45DEV1_RDB_H */
