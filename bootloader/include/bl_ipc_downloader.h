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

#ifndef BL_IPC_DOWNLOADER_H
#define BL_IPC_DOWNLOADER_H


/* Target side */
#define BL_IPC_DWNLD_BL_READY_MASK             (0x0001U)
#define BL_IPC_DWNLD_BL_READY_SHIFT            (0U)
#define BL_IPC_DWNLD_FLASHING_PERCENT_MASK     (0x00FEU)
#define BL_IPC_DWNLD_FLASHING_PERCENT_SHIFT    (1U)
#define BL_IPC_DWNLD_STATUS_MASK               (0x0300U)
#define BL_IPC_DWNLD_STATUS_SHIFT              (8U)

#define BL_IPC_DWNLD_STATUS_SUCCESS            (0U)
#define BL_IPC_DWNLD_STATUS_FAILURE            (1U)
#define BL_IPC_DWNLD_STATUS_BUSY               (2U)


#define BL_PING_MODE_BL                    (0x424C4452UL) /* 'B' 'L' 'D' 'R' */

typedef uint32_t BL_IPC_BootModeType;
#define BL_IPC_BOOTMODE_BROM      (0x42524F4DUL) /* 'B' 'R' 'O' 'M' */
#define BL_IPC_BOOTMODE_BL        (0x424C4452UL) /* 'B' 'L' 'D' 'R' */
#define BL_IPC_BOOTMODE_FW        (0x464D5752UL) /* 'F' 'M' 'W' 'R' */
#define BL_IPC_BOOTMODE_DEFAULT   (0x44464C54UL) /* 'D' 'F' 'L' 'T' */

typedef uint32_t BL_IPC_ResetModeType;
#define BL_IPC_RESETMODE_CHIP     (0x00000001UL)
#define BL_IPC_RESETMODE_CPU      (0x00000002UL)

#pragma pack(push, 4)
/* Note: The size of this struct should be aligned to 4bytes.
 * This is required to transfer this struct completely to
 * BCM8953x.
 */

typedef struct _BL_IPC_RebootType {
    BL_IPC_ResetModeType resetMode;    /* Chip/CPU reset */
    BL_IPC_BootModeType bootMode;    /* Reboot into default mode or BL */
    uint32_t delayMs;                /* delay in milliseconds before rebooting */
} BL_IPC_RebootType;

#pragma pack(pop)

#endif /* BL_IPC_DOWNLOADER_H */
