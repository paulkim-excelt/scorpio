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
    @defgroup grp_bl_board Board
    @ingroup grp_bootloader

    @addtogroup grp_bl_board
    @{
    @section grp_bl_board Overview
    Based on the board, the respective board files are included

    @file bl_board.h
    @brief Board header include file

    @version 0.30 Import from Document
*/

#ifndef BL_BOARD_H
#define BL_BOARD_H

#if defined(__BCM89103_EVK__)
#include <board/bcm89103_evk/bl_board.h>
#elif defined (__BCM89106_EVK__)
#include <board/bcm89106_evk/bl_board.h>
#elif defined (__BCM89107_EVK__)
#include <board/bcm89107_evk/bl_board.h>
#elif defined (__BCM89105_DDR_EVK__)
#include <board/bcm89105_ddr_evk/bl_board.h>
#elif defined (__BCM89109_DDR_EVK__)
#include <board/bcm89109_ddr_evk/bl_board.h>
#elif defined (__BCM89531_M25P16_EVK__)
#include <board/bcm89531_m25p16_evk/bl_board.h>
#elif defined (__BCM89531_W25Q16_EVK__)
#include <board/bcm89531_w25q16_evk/bl_board.h>
#elif defined (__BCM89531_EVK__)
#include <board/bcm89531_evk/bl_board.h>
#elif defined (__BCM89541_EVK__)
#include <board/bcm89541_evk/bl_board.h>
#elif defined (__BCM89551_EVK__)
#include <board/bcm89551_evk/bl_board.h>
#elif defined (__BCM89559_EVK__)
#include <board/bcm89559_evk/bl_board.h>
#elif defined (__BCM89559G_EVK__)
#include <board/bcm89559g_evk/bl_board.h>
#elif defined (__BCM89531_LS_EVK__)
#include <board/bcm89531_ls_evk/bl_board.h>
#elif defined (__BCM89561_EVK__)
#include <board/bcm89561_evk/bl_board.h>
#elif defined (__BCM89564G_EVK__)
#include <board/bcm89564g_evk/bl_board.h>
#elif defined (__BCM89083_EVK__)
#include <board/bcm89083_evk/bl_board.h>
#else
#error "Unknown board type"
#endif

#endif /* BL_BOARD_H */

/** @} */
