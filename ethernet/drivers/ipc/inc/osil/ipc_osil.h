/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_ipc_il Integration Guide
    @ingroup grp_ipcdrv

    @addtogroup grp_ipc_il
    @{
    @section sec_ipc_integration Integration Guide
    This section explains integration interfaces exposed by IPC.

    @section sec_ipc_il_seq Sequence Diagram
    Below figure shows a typical call flow for a synchronous call such as
    #IPC_Init invoked by the Client. The same sequence holds good for the rest
    of the synchronous APIs exposed by IPC.
    @image html ipc_seq_il_diagram_svc.jpg

    @file ipc_osil.h
    @brief IPC driver Integration Interfaces
    This header file contains the integration interface for IPC driver
    @version 0.20 Imported from docx
*/

#ifndef IPC_OSIL_H
#define IPC_OSIL_H

#include <ipc.h>

/**
    @name IPC driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for IPC driver
*/
#define BRCM_ARCH_IPC_IL_MSG_MEM_GLOBAL         (0x40U) /**<@brief #IPC_MsgMem */
#define BRCM_ARCH_IPC_IL_MSG_LOG2_COUNT_GLOBAL  (0x41U) /**<@brief #IPC_MsgLog2Count */
#define BRCM_ARCH_IPC_IL_STATE_TYPE             (0x42U) /**<@brief #IPC_CmdType */
#define BRCM_ARCH_IPC_IL_IO_TYPE                (0x43U) /**<@brief #IPCIO */
#define BRCM_ARCH_IPC_IL_SYS_CMD_REQ_PROC       (0x44U) /**<@brief #IPC_SysCmdReq */
#define BRCM_ARCH_IPC_IL_SYS_CMD_HANDLER_PROC   (0x45U) /**<@brief #IPCDrv_SysCmdHandler */
#define BRCM_ARCH_IPC_IL_CMD_HANDLER_PROC       (0x46U) /**<@brief #IPCDrv_CmdHandler */
#define BRCM_ARCH_IPC_IL_IRQ_HANDLER_PROC       (0x47U) /**<@brief #IPCDrv_IRQHandler */
/** @} */

/** @brief IPC Memory Start Address

    @trace #BRCM_ARCH_IPC_IL_MSG_MEM_GLOBAL #BRCM_REQ_IPC_IL_MSG_MEM
*/
extern uint32_t IPC_MsgMem[];

extern const IPC_ChannCfgType IPC_ChannCfg[IPC_MAX_CHANNELS];

/**
    @name IPC_CmdType
    @{
    @brief IPC SVC Commands

    @trace #BRCM_ARCH_IPC_IL_STATE_TYPE #BRCM_REQ_IPC_IL_FUNCTIONALITY
*/
typedef uint32_t IPC_CmdType;           /**< @brief IPC SVC command type  */
#define IPC_CMD_INIT            (0x1UL) /**< @brief #IPC_Init */
#define IPC_CMD_DEINIT          (0x2UL) /**< @brief #IPC_Deinit */
#define IPC_CMD_SEND            (0x3UL) /**< @brief #IPC_Send */
#define IPC_CMD_RECEIVE         (0x4UL) /**< @brief #IPC_Receive */
#define IPC_CMD_PAUSE           (0x5UL) /**< @brief #IPC_Pause */
#define IPC_CMD_RESUME          (0x6UL) /**< @brief #IPC_Resume */
#define IPC_CMD_READ_DIRECT     (0x7UL) /**< @brief #IPC_ReadDirect */
#define IPC_CMD_WRITE_DIRECT    (0x8UL) /**< @brief #IPC_WriteDirect */

/** @} */

/**
    @brief Structure to define input and output parameters

    @trace #BRCM_ARCH_IPC_IL_IO_TYPE #BRCM_REQ_IPC_IL_IO
*/
typedef struct _IPCIO {
    int32_t             status;           /**< @brief Status of system call */
    IPC_ChannIDType     channID;          /**< @brief IPC Channel ID */
    uint32_t            mode;             /**< @brief IPC Mode, Master or Slave */
    uint32_t            targetFeatures;   /**< @brief Target features */
    uint32_t            addr;             /**< @brief memory address for direct read/write */
    IPC_AccWidthType    width;            /**< @brief access witdth for direct read/write */
    uint8_t             *msg;             /**< @brief Status of system call */
    uint32_t            cmd;              /**< @brief Pointer to command */
    uint32_t            inLen;            /**< @brief max lenght of the message buffer */
    uint32_t            outLen;           /**< @brief Pointer to length of the message */
} IPCIO;

/** @brief Trigger a system command request

    This API shall trigger a system command request for a given command.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aCmd        Command to be executed in system context
    @param[inout]   aIO         IPC IO structure to exchange information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request raised successfully
    @retval     Others                  As returned by #IPCDrv_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_ARCH_IPC_IL_SYS_CMD_REQ_PROC  #BRCM_REQ_IPC_SYS_CMD_REQ

    @limitations None
*/
extern int32_t IPC_SysCmdReq(const uint32_t aCmd, IPCIO *aIO);

/** @brief Handle all IPC driver command requests

    This interface handles all the IPC diver commands requested from System
    Command Request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    Magic ID of VGC
    @param[in]      aCmd        Command to be executed in system context
    @param[inout]   aSysIO      System IO structure to exchange information

    @return     void

    @post TBD

    @note This interface shall be implemented by the itegrator and shall install
    it to SVC layer.

    @trace  #BRCM_ARCH_IPC_IL_SYS_CMD_HANDLER_PROC  #BRCM_REQ_IPC_SYS_CMD_HANDLER

    @limitations None
*/
extern void IPC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO);

/** @brief Interrupt handler

    This API shall be called by the respective channel IRQ handler

    @behavior Sync, Non-reentrant

    @param[in]  aID     IPC Channel ID

    @return     void

    @note This interface shall be called from the Interrupt Service Routine for
    IPC driver. The IPC client shall also be notified from the Interrupt Service
    Routine.

    @trace  #BRCM_ARCH_IPC_IL_IRQ_HANDLER_PROC  #BRCM_REQ_IPC_IRQ_HANDLER

    @limitations None

    @snippet ipc_osil.c Usage of IPCDrv_IRQHandler
*/
extern void IPC_IRQHandler(IPC_ChannIDType aID);

extern void IPC_IRQChann0Handler(void);
extern void IPC_IRQChann1Handler(void);
extern void IPC_IRQChann2Handler(void);

#endif /* IPC_OSIL_H */
/** @} */
