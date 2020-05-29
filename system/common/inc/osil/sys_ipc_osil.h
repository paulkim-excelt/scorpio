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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/** @defgroup grp_sys_ipc_il System Commands Integration Guide
    @ingroup grp_sys_ipc

    @addtogroup grp_sys_ipc_il
    @{

    @file sys_ipc_osil.h
    @brief Integration for System Commands

    @version 0.1 Initial version
*/

#ifndef IPC_SYSTEM_OSIL_H
#define IPC_SYSTEM_OSIL_H

#include <rpc_cmds.h>
#include <sys_ipc.h>

/**
    @name IPC Commands for System Integration Interface IDs
    @{
    @brief Integration Interface IDs for IPC System Commands

*/
#define BRCM_SWARCH_SYS_PROCESS_CMDS_PROC    (0x81U)   /**< @brief #SYS_ProcessCmds  */
/** @} */


/** @brief Function to process commands in System Commands Queue

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void (for void functions)

    @post None

    @note To be implemented/used.

    @trace  #BRCM_SWREQ_COMP_FUNCTIONALITY

    @limitations None

*/
void SYS_ProcessCmds(void);


/** @brief Get Pending Reboot request

    @behavior Sync, Non-reentrant

    @pre None

    @param[out]      aReboot   Reboot Command/Request valid only when return value is BCM_ERR_OK

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command status retrieved successfully
    @retval     #BCM_ERR_NOT_FOUND      Pending Reboot command not found
    @retval     #BCM_ERR_INVAL_PARAMS   aReboot is NULL

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_COMP_FUNCTIONALITY

    @limitations None

*/
int32_t SYS_GetRebootRequest(SYS_RebootCmdType *aReboot);

#endif /* IPC_SYSTEM_OSIL_H */

/** @} */
