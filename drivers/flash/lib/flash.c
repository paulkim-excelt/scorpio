/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
    @defgroup grp_flashdrv_impl Flash Driver
    @ingroup grp_flashdrv

    @addtogroup grp_flashdrv_impl
    @{
    @file flash.c
    @brief Flash driver implementation.
    This source file implements flash driver.

*/
#include <string.h>
#include <bcm_err.h>
#include <interrupt.h>
#include <system.h>
#include <utils.h>
#include <flash.h>
#include <flash_osil.h>

/**
  @name Implementation IDs
  @{
  @brief FLASH internal functions IDs for error reporting
*/
#define BRCM_SWDSGN_FLASH_FLAGSTATUS_TYPE                       () /**< @brief #FLASH_FlagStatusType */
#define BRCM_SWDSGN_FLASH_STATUSREG_TYPE                        () /**< @brief #FLASH_StatusRegType */
#define BRCM_SWDSGN_FLASH_OPS_TYPE                              () /**< @brief #FLASH_OpsType */
#define BRCM_SWDSGN_FLASH_OPSSTATE_TYPE                         () /**< @brief #FLASH_OpsStateType */
#define BRCM_SWDSGN_FLASH_SUBOPSSTATE_TYPE                      () /**< @brief #FLASH_SubOpsStateType */
#define BRCM_SWDSGN_FLASH_ERRFLAG_TYPE                          () /**< @brief #FLASH_ErrFlagType */
#define BRCM_SWDSGN_FLASH_INDFLAG_TYPE                          () /**< @brief #FLASH_IndFlagType */
#define BRCM_SWDSGN_FLASH_ADDRESS23_16_MACRO                    () /**< @brief #FLASH_ADDRESS23_16 */
#define BRCM_SWDSGN_FLASH_ADDRESS15_8_MACRO                     () /**< @brief #FLASH_ADDRESS15_8 */
#define BRCM_SWDSGN_FLASH_ADDRESS7_0_MACRO                      () /**< @brief #FLASH_ADDRESS7_0 */
#define BRCM_SWDSGN_FLASH_DUMMY_DATA_MACRO                      () /**< @brief #FLASH_DUMMY_DATA */
#define BRCM_SWDSGN_FLASH_STATUS_REG_LEN_MACRO                  () /**< @brief #FLASH_STATUS_REG_LEN */
#define BRCM_SWDSGN_FLASH_READ_STATUS_LOOP_COUNT_MACRO          () /**< @brief #FLASH_READ_STATUS_LOOP_COUNT */
#define BRCM_SWDSGN_FLASH_CMD_TYPE                              () /**< @brief #FLASH_CmdType */
#define BRCM_SWDSGN_FLASH_FLASHABLE_SECTOR_LIST_SZ_MACRO        () /**< @brief #FLASH_FLASHABLE_SECTOR_LIST_SZ */
#define BRCM_SWDSGN_FLASH_OPSINFO_TYPE                          () /**< @brief #FLASH_OpsInfoType */
#define BRCM_SWDSGN_FLASH_RWDEV_TYPE                            () /**< @brief #FLASH_RWDevType */
#define BRCM_SWDSGN_FLASH_RWDEV_GLOBAL                          () /**< @brief #FLASH_RWDev */
#define BRCM_SWDSGN_FLASH_FLSABLESECLIST_GLOBAL                 () /**< @brief #FLASH_FlsableSecList */
#define BRCM_SWDSGN_FLASH_FLSIDTBL_GLOBAL                       () /**< @brief #FLASH_FlsIDTbl */
#define BRCM_SWDSGN_FLASH_FLSIDTBLSZ_GLOBAL                     () /**< @brief #FLASH_FlsIDTblSz */
#define BRCM_SWDSGN_FLASH_REPORT_ERROR_PROC                     (0x90U) /**< @brief #FLASH_ReportError */
#define BRCM_SWDSGN_FLASH_UPDATELANEBIT_PROC                    (0x91U) /**< @brief #FLASH_UpdateLaneBit */
#define BRCM_SWDSGN_FLASH_UPDATESTATUSVAL_PROC                  (0x92U) /**< @brief #FLASH_UpdateStatusVal */
#define BRCM_SWDSGN_FLASH_CMDWRITEENABLE_PROC                   (0x93U) /**< @brief #FLASH_CmdWriteEnable */
#define BRCM_SWDSGN_FLASH_CMDREADSTATUS_PROC                    (0x94U) /**< @brief #FLASH_CmdReadStatus */
#define BRCM_SWDSGN_FLASH_CMDREADREG_PROC                       (0x95U) /**< @brief #FLASH_CmdReadReg */
#define BRCM_SWDSGN_FLASH_CMDWRITESTATUS_PROC                   (0x96U) /**< @brief #FLASH_CmdWriteStatus */
#define BRCM_SWDSGN_FLASH_CMDPAGEPROG_PROC                      (0x97U) /**< @brief #FLASH_CmdPageProg */
#define BRCM_SWDSGN_FLASH_CMDWRITEDATA_PROC                     (0x98U) /**< @brief #FLASH_CmdWriteData */
#define BRCM_SWDSGN_FLASH_CMDSECTORERASE_PROC                   (0x99U) /**< @brief #FLASH_CmdSectorErase */
#define BRCM_SWDSGN_FLASH_CMDSUBSECTORERASE_PROC                (0x9AU) /**< @brief #FLASH_CmdSubSectorErase */
#define BRCM_SWDSGN_FLASH_CMDREADID_PROC                        (0x9BU) /**< @brief #FLASH_CmdReadID */
#define BRCM_SWDSGN_FLASH_CMDREGREADWRITE_PROC                  (0x9CU) /**< @brief #FLASH_CmdRegReadWrite */
#define BRCM_SWDSGN_FLASH_SUBOPSWRITEENABLE_PROC                (0x9DU) /**< @brief #FLASH_SubOpsWriteEnable */
#define BRCM_SWDSGN_FLASH_SUBOPSSECTORERASE_PROC                (0x9EU) /**< @brief #FLASH_SubOpsSectorErase */
#define BRCM_SWDSGN_FLASH_SUBOPSWRITESTATUS_PROC                (0x9FU) /**< @brief #FLASH_SubOpsWriteStatus */
#define BRCM_SWDSGN_FLASH_SUBOPSWRITE_PROC                      (0xA0U) /**< @brief #FLASH_SubOpsWrite */
#define BRCM_SWDSGN_FLASH_UPDATESTATEDONE_PROC                  (0xA1U) /**< @brief #FLASH_UpdateStateDone */
#define BRCM_SWDSGN_FLASH_UPDATESTATEERROR_PROC                 (0xA2U) /**< @brief #FLASH_UpdateStateError */
#define BRCM_SWDSGN_FLASH_ERASESTATEHANDLER_PROC                (0xA3U) /**< @brief #FLASH_EraseStateHandler */
#define BRCM_SWDSGN_FLASH_WRITESTATEHANDLER_PROC                (0xA4U) /**< @brief #FLASH_WriteStateHandler */
#define BRCM_SWDSGN_FLASH_READSTATEHANDLER_PROC                 (0xA5U) /**< @brief #FLASH_ReadStateHandler */
#define BRCM_SWDSGN_FLASH_COMPARESTATEHANDLER_PROC              (0xA6U) /**< @brief #FLASH_CompareStateHandler */
#define BRCM_SWDSGN_FLASH_CFGREADLANESTATEHANDLER_PROC          (0xA7U) /**< @brief #FLASH_CfgReadLaneStateHandler */
#define BRCM_SWDSGN_FLASH_READIDSTATEHANDLER_PROC               (0xA8U) /**< @brief #FLASH_ReadIDStateHandler */
#define BRCM_SWDSGN_FLASH_REGREADWRITESTATEHANDLER_PROC         (0xA9U) /**< @brief #FLASH_RegReadWriteStateHandler */
#define BRCM_SWDSGN_FLASH_ISADDRFLASHABLE_PROC                  (0xAAU) /**< @brief #FLASH_IsAddrFlashable */
#define BRCM_SWDSGN_FLASH_OPSINIT_PROC                          (0xABU) /**< @brief #FLASH_OpsInit */
#define BRCM_SWDSGN_FLASH_OPSDEINIT_PROC                        (0xACU) /**< @brief #FLASH_OpsDeInit */
#define BRCM_SWDSGN_FLASH_OPSCANCEL_PROC                        (0xADU) /**< @brief #FLASH_OpsCancel */
#define BRCM_SWDSGN_FLASH_OPSERASE_PROC                         (0xAEU) /**< @brief #FLASH_OpsErase */
#define BRCM_SWDSGN_FLASH_OPSWRITE_PROC                         (0xAFU) /**< @brief #FLASH_OpsWrite */
#define BRCM_SWDSGN_FLASH_OPSREAD_PROC                          (0xB0U) /**< @brief #FLASH_OpsRead */
#define BRCM_SWDSGN_FLASH_OPSCOMPARE_PROC                       (0xB1U) /**< @brief #FLASH_OpsCompare */
#define BRCM_SWDSGN_FLASH_OPSCFGREADLANE_PROC                   (0xB2U) /**< @brief #FLASH_OpsCfgReadLane */
#define BRCM_SWDSGN_FLASH_OPSREADID_PROC                        (0xB3U) /**< @brief #FLASH_OpsReadID */
#define BRCM_SWDSGN_FLASH_OPSREGREADWRITE_PROC                  (0xB4U) /**< @brief #FLASH_OpsRegReadWrite */
#define BRCM_SWDSGN_FLASH_OPSHANDLER_PROC                       (0xB5U) /**< @brief #FLASH_OpsHandler */
#define BRCM_SWDSGN_FLASH_QSPISTATEHANDLER_PROC                 (0xB6U) /**< @brief #FLASH_QSPIStateHandler */
#define BRCM_SWDSGN_FLASH_CHECKSTATUS_PROC                      (0xB7U) /**< @brief #FLASH_CheckStatus */
#define BRCM_SWDSGN_FLASH_INIT_PROC                             (0xB8U) /**< @brief #FLASH_Init */
#define BRCM_SWDSGN_FLASH_DEINIT_PROC                           (0xB9U) /**< @brief #FLASH_DeInit */
#define BRCM_SWDSGN_FLASH_CANCEL_PROC                           (0xBAU) /**< @brief #FLASH_Cancel */
#define BRCM_SWDSGN_FLASH_GETSTATUS_PROC                        (0xBBU) /**< @brief #FLASH_GetStatus */
#define BRCM_SWDSGN_FLASH_GETOPSRESULT_PROC                     (0xBCU) /**< @brief #FLASH_GetOpsResult */
#define BRCM_SWDSGN_FLASH_ERASE_PROC                            (0xBDU) /**< @brief #FLASH_Erase */
#define BRCM_SWDSGN_FLASH_WRITE_PROC                            (0xBEU) /**< @brief #FLASH_Write */
#define BRCM_SWDSGN_FLASH_READ_PROC                             (0xBFU) /**< @brief #FLASH_Read */
#define BRCM_SWDSGN_FLASH_COMPARE_PROC                          (0xC0U) /**< @brief #FLASH_Compare */
#define BRCM_SWDSGN_FLASH_BLANKCHECK_PROC                       (0xC1U) /**< @brief #FLASH_BlankCheck */
#define BRCM_SWDSGN_FLASH_CFGREADLANE_PROC                      (0xC2U) /**< @brief #FLASH_CfgReadLane */
#define BRCM_SWDSGN_FLASH_READID_PROC                           (0xC3U) /**< @brief #FLASH_ReadID */
#define BRCM_SWDSGN_FLASH_REGREADWRITE_PROC                     (0xC4U) /**< @brief #FLASH_RegReadWrite */
#define BRCM_SWDSGN_FLASH_CMDHANDLER_PROC                       (0xC5U) /**< @brief #FLASH_CmdHandler */
#define BRCM_SWDSGN_FLASH_STATEHANDLER_PROC                     (0xC6U) /**< @brief #FLASH_StateHandler */
#define BRCM_SWDSGN_FLASH_STATEHANDLER_INTERNAL_PROC            (0xC7U) /**< @brief #FLASH_StateHandlerInternal */
#define BRCM_SWDSGN_FLASH_BUS_XFER_PROC                         (0xC8U) /**< @brief #FLASH_BusXfer */
#define BRCM_SWDSGN_FLASH_BUS_READ_PROC                         (0xC9U) /**< @brief #FLASH_BusRead */
#define BRCM_SWDSGN_FLASH_BUS_STATEHANDLER_PROC                 (0xCAU) /**< @brief #FLASH_BusStateHandler */
#define BRCM_SWDSGN_FLASH_BUS_ISBUSY_PROC                       (0xCBU) /**< @brief #FLASH_IsBusBusy */
/** @} */

/**
    @{
    @brief Flag status register bit mask
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
  */
typedef uint8_t FLASH_FlagStatusType;
#define FLASH_FLAGSTATUS_PROG_ERASE_CONTR_MASK   (0x80U) /**< @brief Flag status - Program erase control mask  */
#define FLASH_FLAGSTATUS_PROTECTION_ERR_MASK     (0x01U) /**< @brief Flag status - Protection error mask */
#define FLASH_FLAGSTATUS_ERASE_ERR_MASK          (0x20U) /**< @brief Flag status - Erase error mask */
#define FLASH_FLAGSTATUS_PROG_ERR_MASK           (0x10U) /**< @brief Flag status - Programing error mask */
#define FLASH_FLAGSTATUS_ERR_MASK    \
    (FLASH_FLAGSTATUS_PROTECTION_ERR_MASK |\
     FLASH_FLAGSTATUS_ERASE_ERR_MASK |\
     FLASH_FLAGSTATUS_PROG_ERR_MASK)
/** @} */

/**
    @{
    @brief Flash status register bit fields
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
  */
typedef uint8_t FLASH_StatusRegType;
#define FLASH_STATUSREG_WIP_MASK       (0x1U)   /**< @brief WIP Bit mask */
#define FLASH_STATUSREG_WEL_MASK       (0x2U)   /**< @brief WEL Bit mask */
/** @} */

/**
  @{
  @brief FLASH operation type
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_PROC
  @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
  @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
  @trace #BRCM_SWARCH_FLASH_GET_STATUS_PROC
  @trace #BRCM_SWARCH_FLASH_GET_OPS_RESULT_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
  @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef uint32_t FLASH_OpsType;
#define FLASH_OPS_NONE              (0UL)   /**< @brief No operation */
#define FLASH_OPS_INIT              (1UL)   /**< @brief Init operation */
#define FLASH_OPS_DEINIT            (3UL)   /**< @brief DeInit operation */
#define FLASH_OPS_ERASE             (4UL)   /**< @brief Erase operation */
#define FLASH_OPS_WRITE             (5UL)   /**< @brief Write operation */
#define FLASH_OPS_READ              (6UL)   /**< @brief Read operation */
#define FLASH_OPS_CANCEL            (7UL)   /**< @brief Cancel operation */
#define FLASH_OPS_COMPARE           (8UL)   /**< @brief Compare operation */
#define FLASH_OPS_GET_STATUS        (9UL)   /**< @brief Get status operation */
#define FLASH_OPS_GET_OPS_RESULT    (10UL)  /**< @brief Get ops result operation */
#define FLASH_OPS_CFG_READ_LANE     (11UL)  /**< @brief Configure read lane operation */
#define FLASH_OPS_READID            (12UL)  /**< @brief Read ID operation */
#define FLASH_OPS_REG_READ_WRITE    (13UL)  /**< @brief Raw transfer operation */
/** @} */

/**
 @{
 @brief FLASH operation state type
 @trace #BRCM_SWARCH_FLASH_ERASE_PROC
 @trace #BRCM_SWARCH_FLASH_WRITE_PROC
 @trace #BRCM_SWARCH_FLASH_READ_PROC
 @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
 @trace #BRCM_SWARCH_FLASH_GET_STATUS_PROC
 @trace #BRCM_SWARCH_FLASH_GET_OPS_RESULT_PROC
 @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
 @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
 @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
 @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef uint32_t FLASH_OpsStateType;
#define FLASH_OPSSTATE_ERASE_STATE_WE            (40UL) /**< @brief Erase operation state write enable */
#define FLASH_OPSSTATE_ERASE_STATE_SE            (41UL) /**< @brief Erase operation state sector erase */
#define FLASH_OPSSTATE_ERASE_STATE_DONE          (42UL) /**< @brief Erase operation state sector erase */
#define FLASH_OPSSTATE_ERASE_STATE_ERROR         (43UL) /**< @brief Erase operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_ERASE_STATE_EXIT          (44UL) /**< @brief Erase operation state exit */
#define FLASH_OPSSTATE_WRITE_STATE_WE            (50UL) /**< @brief Write operation state write enable */
#define FLASH_OPSSTATE_WRITE_STATE_WR            (51UL) /**< @brief Write operation state write data */
#define FLASH_OPSSTATE_WRITE_STATE_DONE          (52UL) /**< @brief Write operation state done */
#define FLASH_OPSSTATE_WRITE_STATE_ERROR         (53UL) /**< @brief Write operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_WRITE_STATE_EXIT          (54UL) /**< @brief Write operation state error */

#define FLASH_OPSSTATE_READ_STATE_READ           (60UL) /**< @brief Read operation state read */
#define FLASH_OPSSTATE_READ_STATE_DONE           (61UL) /**< @brief Read operation state done */
#define FLASH_OPSSTATE_READ_STATE_ERROR          (62UL) /**< @brief Read operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_READ_STATE_EXIT           (63UL) /**< @brief Read operation state exit */

#define FLASH_OPSSTATE_COMP_STATE_RD             (80UL) /**< @brief Compare operation state read */
#define FLASH_OPSSTATE_COMP_STATE_CMP            (81UL) /**< @brief Compare operation state compare */
#define FLASH_OPSSTATE_COMP_STATE_DONE           (82UL) /**< @brief Compare operation state done */
#define FLASH_OPSSTATE_COMP_STATE_ERROR          (83UL) /**< @brief Compare operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_COMP_STATE_EXIT           (84UL) /**< @brief Compare operation state exit */

#define FLASH_OPSSTATE_CFGLAN_STATE_WE           (110UL) /**< @brief Configure Read Lane operation state Write enable */
#define FLASH_OPSSTATE_CFGLAN_STATE_WS           (111UL) /**< @brief Configure Read Lane operation state Write status */
#define FLASH_OPSSTATE_CFGLAN_STATE_DONE         (112UL) /**< @brief Configure Read Lane operation state done */
#define FLASH_OPSSTATE_CFGLAN_STATE_ERROR        (113UL) /**< @brief Configure Read Lane operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_CFGLAN_STATE_EXIT         (114UL) /**< @brief Configure Read Lane operation state exit */

#define FLASH_OPSSTATE_READID_STATE_READ         (120UL) /**< @brief Read ID operation state read */
#define FLASH_OPSSTATE_READID_STATE_DONE         (121UL) /**< @brief Read ID operation state done */
#define FLASH_OPSSTATE_READID_STATE_ERROR        (122UL) /**< @brief Read ID operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_READID_STATE_EXIT         (123UL) /**< @brief Read ID operation state exit */

#define FLASH_OPSSTATE_REG_RD_WR_STATE_TRANSFER  (130UL) /**< @brief Reg read write operation state transfer */
#define FLASH_OPSSTATE_REG_RD_WR_STATE_DONE      (131UL) /**< @brief Reg read write operation state done */
#define FLASH_OPSSTATE_REG_RD_WR_STATE_ERROR     (132UL) /**< @brief Reg read write operation state error */
/*
    Note: This is the last state & state machine will
          enter default case for this state. This is added
          to catch execution of state machine with wrong
          state.
*/
#define FLASH_OPSSTATE_REG_RD_WR_STATE_EXIT      (133UL) /**< @brief Reg read write operation state exit */
/** @} */

/**
  @{
  @brief FLASH sub-operation state type
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
  @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef uint32_t FLASH_SubOpsStateType;
#define FLASH_SUBOPSSTATE_NONE     (0UL) /**< @brief Sub-operation state none */
#define FLASH_SUBOPSSTATE_WE       (1UL) /**< @brief Sub-operation state Write enable */
#define FLASH_SUBOPSSTATE_SE       (2UL) /**< @brief Sub-operation state sector erase */
#define FLASH_SUBOPSSTATE_RS       (3UL) /**< @brief Sub-operation state read status */
#define FLASH_SUBOPSSTATE_VER_WEL  (4UL) /**< @brief Sub-operation state verify WEL bit in read status */
#define FLASH_SUBOPSSTATE_VER_WIP  (5UL) /**< @brief Sub-operation state verify WIP bit read status */
#define FLASH_SUBOPSSTATE_PP       (6UL) /**< @brief Sub-operation state page program */
#define FLASH_SUBOPSSTATE_WR       (7UL) /**< @brief Sub-operation state write data */
#define FLASH_SUBOPSSTATE_WS       (8UL) /**< @brief Sub-operation state write status */
#define FLASH_SUBOPSSTATE_RR       (9UL) /**< @brief Sub-operation state read register */
#define FLASH_SUBOPSSTATE_VER_PES  (10UL)/**< @brief Sub-operation state verify Program or Erase bit in Flag status regiser */
/** @} */

/**
 FLASH error flag type

 @trace #BRCM_SWARCH_FLASH_INIT_PROC
 @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
 @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
 @trace #BRCM_SWARCH_FLASH_WRITE_PROC
 @trace #BRCM_SWARCH_FLASH_ERASE_PROC
 @trace #BRCM_SWARCH_FLASH_READ_PROC
 @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
 @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
 @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
 @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
 @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
 @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
 @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef uint32_t FLASH_ErrFlagType;

/**
 @{
 @brief FLASH operation indication flag type
 @trace #BRCM_SWARCH_FLASH_ERASE_PROC
 @trace #BRCM_SWARCH_FLASH_WRITE_PROC
 @trace #BRCM_SWARCH_FLASH_READ_PROC
 @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
 @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
 @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
 @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
 @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
 @trace #BRCM_SWREQ_FLASH_RESULT_INDICATION
 */
typedef uint32_t FLASH_IndFlagType;
#define FLASH_INDFLAG_NO_IND        (0UL) /**< @brief Flag for no indication */
#define FLASH_INDFLAG_DONE_IND      (1UL) /**< @brief Flag for done indication */
#define FLASH_INDFLAG_ERR_IND       (2UL) /**< @brief Flag for error indication */
/** @} */

/**
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_ADDRESS23_16(flashAddr)     (((flashAddr) >> 16UL) & 0xFFUL)

/**
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_ADDRESS15_8(flashAddr)      (((flashAddr) >>  8UL) & 0xFFUL)

/**
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_ADDRESS7_0(flashAddr)       ((flashAddr) & 0xFFUL)

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_DUMMY_DATA            (0x0)

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_STATUS_REG_LEN        (4UL) /**< @brief Flash status register
                                            length. Most of the flashes have
                                            only 2bytes status register. But
                                            kept 4byte for future extensions */

/**
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
#define FLASH_READ_STATUS_LOOP_COUNT      (10UL) /**< @brief Read status loop count */

/**
  @{
  @brief FLASH device commands
   Common flash commands followed by many flash manufacturer.
   NOTE: Apart for these commands, there are extended commands
   which differs among manufacturers.

  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef uint8_t FLASH_CmdType;
#define FLASH_CMD_READ              (0x03)  /**< @brief Read */
#define FLASH_CMD_FAST_READ         (0x0B)  /**< @brief Fast read */
#define FLASH_CMD_RDID              (0x9F)  /**< @brief Read ID */
#define FLASH_CMD_WREN              (0x06)  /**< @brief Write enable */
#define FLASH_CMD_WRDI              (0x04)  /**< @brief Write disable */
#define FLASH_CMD_RDSR              (0x05)  /**< @brief Read status */
#define FLASH_CMD_RCFR              (0x35)  /**< @brief Read configuration */
#define FLASH_CMD_WRSR              (0x01)  /**< @brief Write status */
#define FLASH_CMD_PP                (0x02)  /**< @brief Page program */
#define FLASH_CMD_SSE               (0x20)  /**< @brief Sub-sector erase */
#define FLASH_CMD_SE                (0xD8)  /**< @brief Sector erase */
#define FLASH_CMD_BE                (0xC7)  /**< @brief Bulk erase */
#define FLASH_CMD_RES               (0xAB)  /**< @brief Read electronic signature */
#define FLASH_CMD_DOFR              (0x3B)  /**< @brief Dual output fast read */
#define FLASH_CMD_DIOFR             (0xBB)  /**< @brief Dual input/ouput fast read */
#define FLASH_CMD_QOFR              (0x6B)  /**< @brief Quad output fast read */
#define FLASH_CMD_QIOFR             (0xEB)  /**< @brief Quad input/output fast read*/
#define FLASH_CMD_READ_FS_REG       (0x70)  /**< @brief Read flag status register */
/** @} */

/**
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWREQ_FLASH_INIT
 */
#define FLASH_FLASHABLE_SECTOR_LIST_SZ      (64UL)  /**< @brief FLashable sector list size */

/**
  FLASH operation info structure
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
  @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_PROC
  @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
  @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
  @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_GET_STATUS_PROC
  @trace #BRCM_SWARCH_FLASH_GET_OPS_RESULT_PROC
  @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef struct _FLASH_OpsInfoType {
    FLASH_AddrType      opsAddr;       /**< @brief Flash address */
    const uint8_t *     opsTxData;     /**< @brief Write data buffer pointer */
    uint8_t *           opsRxData;     /**< @brief Read data buffer pointer */
    FLASH_LenType       opsLen;        /**< @brief Length */
    FLASH_AddrType      tmpAddr;       /**< @brief Intermediate flash address */
    const uint8_t *     tmpTxData;     /**< @brief Intermediate Tx buffer pointer */
    uint8_t *           tmpRxData;     /**< @brief Intermeiate Rx buffer pointer */
    FLASH_LenType       tmpLen;        /**< @brief Intermediate length */
    FLASH_LenType       tmpLenRem;     /**< @brief Intermediate length remaining */
    FLASH_LenType       tmpRxLen;      /**< @brief Intermediate Read length */
    FLASH_LenType       tmpTxLen;      /**< @brief Intermediate Trasnmit length */
    FLASH_LenType       prevRxLen;     /**< @brief Previous read length */
    FLASH_LenType       prevTxLen;     /**< @brief Previous transmit length */
    uint32_t            readStatusCnt; /**< @brief Read status loop count */
    FLASH_IndFlagType   indFlag;       /**< @brief Flag to identify indication type */
    uint32_t            isWIPBitSet;   /**< @brief Flag to indicate WIP state */
    FLASH_FlsIDType *   flsID;         /**< @brief Flash device ID */
} FLASH_OpsInfoType;

/**
  FLASH read-write device structure
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
  @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_PROC
  @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
  @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
  @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_GET_STATUS_PROC
  @trace #BRCM_SWARCH_FLASH_GET_OPS_RESULT_PROC
  @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
typedef struct _FLASH_RWDevType {
    FLASH_StatusType            status;         /**< @brief Current controller status */
    FLASH_OpsType               currOps;        /**< @brief Operation being processed */
    FLASH_OpsStateType          currOpsState;   /**< @brief Current state for current operation */
    FLASH_SubOpsStateType    subOpsState;    /**< @brief Current state for current sub-operation */
    FLASH_CfgType               config;         /**< @brief Pointer to config for runtime usage */
    FLASH_OpsResultType         opsRes;         /**< @brief Result of last ops processed */
    FLASH_OpsInfoType           opsInfo;        /**< @brief Operation information */
    FLASH_ErrFlagType           errFlag;        /**< @brief Error flag */
    uint8_t                     xferTxBuf[FLASH_LEN_PAGE]; /**< @brief Xfer tranmit buffer */
    uint8_t                     xferRxBuf[FLASH_LEN_PAGE]; /**< @brief Xfer receive buffer */
    uint32_t                    isLaneCfgSup;   /**< @brief Flag to indicate flash read lane configuration support */
    uint32_t                    isLaneCfgDone;  /**< @brief Flag to indicate flash read lane configuration done */
    uint32_t                    flsID;          /**< @brief Flash Identification */
    uint32_t                    quadEnByteOffset; /**< @brief Quad Enable Bit Mask and byte offset */
    uint8_t                     quadEnBitMask;
} FLASH_RWDevType;

/**
  @brief Flash drv read-write device data
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
  @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_READ_PROC
  @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
  @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
  @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
  @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
  @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
  @trace #BRCM_SWARCH_FLASH_GET_STATUS_PROC
  @trace #BRCM_SWARCH_FLASH_GET_OPS_RESULT_PROC
  @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
  @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static FLASH_RWDevType FLASH_RWDev[FLASH_MAX_HW_ID] COMP_SECTION(".data.flsmgr")  =
    {
        #if (FLASH_MAX_HW_ID == 0)
        #error "FLASH_MAX_HW_ID == 0"
        #endif
        {
            .status = FLASH_STATUS_UNINIT,
            .currOps = FLASH_OPS_NONE,
            .opsRes = FLASH_OPS_RESULT_OK,
            .errFlag = FALSE,
            .isLaneCfgSup = FALSE,
            .isLaneCfgDone = FALSE,
            .flsID = 0x0UL,
            .quadEnByteOffset = 0x0UL,
            .quadEnBitMask = (uint8_t)0,
        },
        #if (FLASH_MAX_HW_ID > 1)
        {
            .status = FLASH_STATUS_UNINIT,
            .currOps = FLASH_OPS_NONE,
            .opsRes = FLASH_OPS_RESULT_OK,
            .errFlag = FALSE,
            .isLaneCfgSup = FALSE,
            .isLaneCfgDone = FALSE,
            .flsID = 0x0UL,
            .quadEnByteOffset = 0x0UL,
            .quadEnBitMask = (uint8_t)0,
        },
        #endif
        #if (FLASH_MAX_HW_ID > 2)
        #error "FLASH_MAX_HW_ID > 2 is not supported by driver"
        #endif
    };

/**
  @brief Flash ID table
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWREQ_FLASH_INIT
*/
static const FLASH_FlsIDType FLASH_FlsIDTbl[] = {
                FLASH_FLSID_WINBOND_W25Q16,
                FLASH_FLSID_WINBOND_W25Q64CV,
                FLASH_FLSID_MICRON_N25Q512A,
                FLASH_FLSID_MACRONIX_MX25R32,
                FLASH_FLSID_MACRONIX_MX25L32,
                FLASH_FLSID_MICRON_M25P16
                };

/**
  @brief Flash ID table size
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWREQ_FLASH_INIT
 */
static const uint32_t FLASH_FlsIDTblSz = sizeof(FLASH_FlsIDTbl)/sizeof(uint32_t);

/**
  @brief Flashable sector list for each flash device
  @trace #BRCM_SWARCH_FLASH_INIT_PROC
  @trace #BRCM_SWARCH_FLASH_ERASE_PROC
  @trace #BRCM_SWARCH_FLASH_WRITE_PROC
  @trace #BRCM_SWREQ_FLASH_INIT
 */
static FLASH_FlsableSecType FLASH_FlsableSecList[FLASH_MAX_HW_ID][FLASH_FLASHABLE_SECTOR_LIST_SZ] COMP_SECTION(".data.flsmgr");

/**
    @brief Flash drv layer report error function

    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE
 */
static void FLASH_ReportError(uint16_t aCompID, uint8_t aInstanceID,
        uint8_t aApiID, int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE
*/
static int32_t FLASH_BusXfer(FLASH_HwIDType aID, const uint8_t * const aTxData,
                uint8_t * const aRxData, uint32_t aLen,
                FLASH_ChipSelectAssertType aFlag)
{

    FLASH_IOType flashIO;

    flashIO.retVal = BCM_ERR_UNKNOWN;
    flashIO.hwID = aID;
    flashIO.txBuf = aTxData;
    flashIO.rxBuf = aRxData;
    flashIO.len = aLen;
    flashIO.flag = aFlag;

    return FLASH_SysCmdReq(FLASH_BUS_CMD_XFER, &flashIO);
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static int32_t FLASH_BusRead(FLASH_HwIDType aID, uint32_t aAddr,
                uint8_t * const aData, uint32_t aLen)
{

    FLASH_IOType flashIO;

    flashIO.retVal = BCM_ERR_UNKNOWN;
    flashIO.hwID = aID;
    flashIO.addr = aAddr;
    flashIO.rxBuf = aData;
    flashIO.len = aLen;

    return FLASH_SysCmdReq(FLASH_BUS_CMD_READ, &flashIO);
}

/**
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static int32_t FLASH_BusStateHandler(FLASH_HwIDType aID, uint32_t aIsErr)
{
    FLASH_IOType flashIO;

    flashIO.retVal = BCM_ERR_UNKNOWN;
    flashIO.hwID = aID;
    flashIO.isErr = aIsErr;

    return FLASH_SysCmdReq(FLASH_BUS_CMD_STATEHANDLER, &flashIO);
}

/**
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static uint32_t FLASH_IsBusBusy(FLASH_HwIDType aID)
{
    uint32_t retVal;
    int32_t err;
    FLASH_IOType flashIO;

    flashIO.retVal = BCM_ERR_UNKNOWN;
    flashIO.hwID = aID;

    err = FLASH_SysCmdReq(FLASH_BUS_CMD_GETSTATUS, &flashIO);

    if (BCM_ERR_OK == err) {
        retVal = FALSE;
    } else {
        retVal = TRUE;
    }

    return retVal;
}


/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_UpdateLaneBit(FLASH_HwIDType aID)
{
    uint32_t i;
    uint8_t quadEnBitMask[FLASH_STATUS_REG_LEN] = {(uint8_t)0};
    uint32_t quadEnByteOffset = FLASH_RWDev[aID].quadEnByteOffset;

    if (quadEnByteOffset >= FLASH_STATUS_REG_LEN) {
        FLASH_ReportError(BCM_QSP_ID, (uint8_t)aID,
                BRCM_SWDSGN_FLASH_UPDATELANEBIT_PROC, BCM_ERR_CUSTOM, quadEnByteOffset,
                0UL, 0UL, 0UL);
        goto err;
    }

    quadEnBitMask[quadEnByteOffset] =
                            (uint8_t)FLASH_RWDev[aID].quadEnBitMask;

    /* Set Quad Enable bit for Quad lane read and clear Quad Enable Bit for
       single/dual lane read mode */
    if (FLASH_READ_LANE_QUAD != FLASH_RWDev[aID].config.readLane) {
        quadEnBitMask[quadEnByteOffset] &= ~quadEnBitMask[quadEnByteOffset];
    }

    for (i = 0UL; i < 2UL; i++) {
        FLASH_RWDev[aID].xferTxBuf[i+1UL] = quadEnBitMask[i];
    }
err:
    return ;
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_UpdateStatusVal(FLASH_HwIDType aID)
{
        FLASH_UpdateLaneBit(aID);
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdWriteEnable(FLASH_HwIDType aID)
{
    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_WREN;

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    NULL, 1UL, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdReadStatus(FLASH_HwIDType aID)
{
    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_RDSR;
    FLASH_RWDev[aID].xferTxBuf[1] = (uint8_t)FLASH_DUMMY_DATA;

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    &(FLASH_RWDev[aID].xferRxBuf[0]), 2UL, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdReadReg(FLASH_HwIDType aID, uint8_t aReg)
{
    FLASH_RWDev[aID].xferTxBuf[0] = aReg;
    FLASH_RWDev[aID].xferTxBuf[1] = (uint8_t)FLASH_DUMMY_DATA;

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    &(FLASH_RWDev[aID].xferRxBuf[0]), 2UL, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdWriteStatus(FLASH_HwIDType aID)
{
    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_WRSR;
    FLASH_UpdateStatusVal(aID);

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    NULL, 3UL, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdPageProg(FLASH_HwIDType aID)
{
    uint32_t addr = FLASH_RWDev[aID].opsInfo.tmpAddr;

    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_PP;
    FLASH_RWDev[aID].xferTxBuf[1] = (uint8_t)FLASH_ADDRESS23_16(addr);
    FLASH_RWDev[aID].xferTxBuf[2] = (uint8_t)FLASH_ADDRESS15_8(addr);
    FLASH_RWDev[aID].xferTxBuf[3] = (uint8_t)FLASH_ADDRESS7_0(addr);

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    NULL, 4UL, FLASH_CS_ASSERT_CONTINUE);
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdWriteData(FLASH_HwIDType aID)
{
    const uint8_t * txData = FLASH_RWDev[aID].opsInfo.tmpTxData;
    uint32_t wrLen = FLASH_RWDev[aID].config.pageSize;

    return FLASH_BusXfer(aID, txData, NULL, wrLen, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdSectorErase(FLASH_HwIDType aID)
{
    uint32_t addr = FLASH_RWDev[aID].opsInfo.tmpAddr;

    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_SE;
    FLASH_RWDev[aID].xferTxBuf[1] = (uint8_t)FLASH_ADDRESS23_16(addr);
    FLASH_RWDev[aID].xferTxBuf[2] = (uint8_t)FLASH_ADDRESS15_8(addr);
    FLASH_RWDev[aID].xferTxBuf[3] = (uint8_t)FLASH_ADDRESS7_0(addr);

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    NULL, 4UL, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdSubSectorErase(FLASH_HwIDType aID)
{
    uint32_t addr = FLASH_RWDev[aID].opsInfo.tmpAddr;

    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_SSE;
    FLASH_RWDev[aID].xferTxBuf[1] = (uint8_t)FLASH_ADDRESS23_16(addr);
    FLASH_RWDev[aID].xferTxBuf[2] = (uint8_t)FLASH_ADDRESS15_8(addr);
    FLASH_RWDev[aID].xferTxBuf[3] = (uint8_t)FLASH_ADDRESS7_0(addr);

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
                    NULL, 4UL, FLASH_CS_ASSERT_STOP);
}


/**
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdReadID(FLASH_HwIDType aID)
{
    FLASH_RWDev[aID].xferTxBuf[0] = (uint8_t)FLASH_CMD_RDID;
    FLASH_RWDev[aID].xferTxBuf[1] = (uint8_t)FLASH_DUMMY_DATA;
    FLASH_RWDev[aID].xferTxBuf[2] = (uint8_t)FLASH_DUMMY_DATA;
    FLASH_RWDev[aID].xferTxBuf[3] = (uint8_t)FLASH_DUMMY_DATA;

    return FLASH_BusXfer(aID, &(FLASH_RWDev[aID].xferTxBuf[0]),
            &(FLASH_RWDev[aID].xferRxBuf[0]), 4UL,
            FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CmdRegReadWrite(FLASH_HwIDType aID)
{
    uint8_t *rxBuf;

    if (NULL == FLASH_RWDev[aID].opsInfo.opsRxData) {
        rxBuf = FLASH_RWDev[aID].xferRxBuf;
    } else {
        rxBuf = FLASH_RWDev[aID].opsInfo.opsRxData;
    }
    return FLASH_BusXfer(aID, FLASH_RWDev[aID].opsInfo.opsTxData, rxBuf,
            FLASH_RWDev[aID].opsInfo.opsLen, FLASH_CS_ASSERT_STOP);
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_SubOpsWriteEnable(FLASH_HwIDType aID,
                                    uint32_t * const aIsSubStateEnd,
                                    uint32_t * const aIsTrigAgain)
{
    int32_t retVal;

    *aIsSubStateEnd = FALSE;
    *aIsTrigAgain = FALSE;

    switch(FLASH_RWDev[aID].subOpsState) {
    case FLASH_SUBOPSSTATE_WE: /* case 0: */
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
        FLASH_RWDev[aID].opsInfo.readStatusCnt = 0UL;
        retVal = FLASH_CmdWriteEnable(aID);
        break;
    case FLASH_SUBOPSSTATE_RS: /* case 1: */
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_VER_WEL;
        retVal = FLASH_CmdReadStatus(aID);
        break;
    case FLASH_SUBOPSSTATE_VER_WEL: /* case 2: */
        if (FLASH_READ_STATUS_LOOP_COUNT
                > FLASH_RWDev[aID].opsInfo.readStatusCnt) {
            if ((FLASH_RWDev[aID].xferRxBuf[1] & FLASH_STATUSREG_WEL_MASK)
                    != FLASH_STATUSREG_WEL_MASK) {
                /* Write enable failure & trigger sub state machine again */
                FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
                (FLASH_RWDev[aID].opsInfo.readStatusCnt)++;
                *aIsTrigAgain = TRUE;
            } else {
                /* Write enable success */
                *aIsSubStateEnd = TRUE;
                FLASH_RWDev[aID].opsInfo.readStatusCnt = 0UL;
            }
            retVal = BCM_ERR_OK;
        } else {
            *aIsTrigAgain = FALSE;
            *aIsSubStateEnd = TRUE;
            retVal = BCM_ERR_TIME_OUT;
        }
        break;
    default:
        /* Shall never reach here */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_SubOpsSectorErase(FLASH_HwIDType aID,
                                    uint32_t * const aIsSubStateEnd,
                                    uint32_t * const aIsTrigAgain)
{
    int32_t retVal;

    *aIsSubStateEnd = FALSE;
    *aIsTrigAgain = FALSE;

    switch(FLASH_RWDev[aID].subOpsState) {
    case FLASH_SUBOPSSTATE_SE:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
        if (FLASH_RWDev[aID].config.subSectorSize > 0UL) {
            retVal = FLASH_CmdSubSectorErase(aID);
        } else {
            retVal = FLASH_CmdSectorErase(aID);
        }
        break;
    case FLASH_SUBOPSSTATE_RS:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_VER_WIP;
        retVal = FLASH_CmdReadStatus(aID);
        break;
    case FLASH_SUBOPSSTATE_VER_WIP:
        if ((FLASH_RWDev[aID].xferRxBuf[1] & FLASH_STATUSREG_WIP_MASK) ==
                FLASH_STATUSREG_WIP_MASK) {
            FLASH_RWDev[aID].opsInfo.isWIPBitSet = TRUE;
        } else {
            FLASH_RWDev[aID].opsInfo.isWIPBitSet = FALSE;
        }

        if (FLASH_FLSID_MICRON_N25Q512A == FLASH_RWDev[aID].flsID) {
            /* Command in progress and trigger sub state again */
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RR;
            *aIsTrigAgain = TRUE;
        } else {
            if (TRUE == FLASH_RWDev[aID].opsInfo.isWIPBitSet){
                /* Command in progress and trigger sub state again */
                FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
                *aIsTrigAgain = TRUE;
            } else {
                /* Command complete */
                *aIsSubStateEnd = TRUE;
            }
        }
        retVal = BCM_ERR_OK;
        break;
    case FLASH_SUBOPSSTATE_RR:
        /* Read Flag Status Register */
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_VER_PES;
        retVal = FLASH_CmdReadReg(aID, (uint8_t)FLASH_CMD_READ_FS_REG);
        break;
    case FLASH_SUBOPSSTATE_VER_PES:
        /* Verify state of program and erase status bit */
        if (((FLASH_RWDev[aID].xferRxBuf[1] & FLASH_FLAGSTATUS_PROG_ERASE_CONTR_MASK)
                    == FLASH_FLAGSTATUS_PROG_ERASE_CONTR_MASK)
                && (FALSE == FLASH_RWDev[aID].opsInfo.isWIPBitSet)) {
            /* Command complete */
            *aIsSubStateEnd = TRUE;
        } else {
            /* Command in progress and trigger sub state again */
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
            *aIsTrigAgain = TRUE;
        }
        if ((FLASH_RWDev[aID].xferRxBuf[1]
                    & FLASH_FLAGSTATUS_ERR_MASK) != (uint8_t)0) {
            retVal = BCM_ERR_CUSTOM;
            FLASH_ReportError(BCM_QSP_ID, (uint8_t)aID,
                    BRCM_SWDSGN_FLASH_SUBOPSSECTORERASE_PROC, retVal,
                    FLASH_RWDev[aID].xferRxBuf[1], 0UL, 0UL, 0UL);
        } else {
            retVal = BCM_ERR_OK;
        }
        break;
    default:
        /* Shall never reach here */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_SubOpsWriteStatus(FLASH_HwIDType aID,
                                    uint32_t * const aIsSubStateEnd,
                                    uint32_t * const aIsTrigAgain)
{
    int32_t retVal;

    *aIsSubStateEnd = FALSE;
    *aIsTrigAgain = FALSE;

    switch(FLASH_RWDev[aID].subOpsState) {
    case FLASH_SUBOPSSTATE_WS:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
        retVal = FLASH_CmdWriteStatus(aID);
        break;
    case FLASH_SUBOPSSTATE_RS:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_VER_WIP;
        retVal = FLASH_CmdReadStatus(aID);
        break;
    case FLASH_SUBOPSSTATE_VER_WIP:
        if ((FLASH_RWDev[aID].xferRxBuf[1] & FLASH_STATUSREG_WIP_MASK) ==
                                                    FLASH_STATUSREG_WIP_MASK) {
            /* Command in progress and trigger sub state again */
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
            *aIsTrigAgain = TRUE;
        } else {
            /* Command complete */
            *aIsSubStateEnd = TRUE;
        }
        retVal = BCM_ERR_OK;
        break;
    default:
        /* Shall never reach here */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_SubOpsWrite(FLASH_HwIDType aID,
                                uint32_t * const aIsSubStateEnd,
                                uint32_t * const aIsTrigAgain)
{
    int32_t retVal;

    *aIsTrigAgain = FALSE;
    *aIsSubStateEnd = FALSE;

    switch(FLASH_RWDev[aID].subOpsState) {
    case FLASH_SUBOPSSTATE_PP:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WR;
        retVal = FLASH_CmdPageProg(aID);
        break;
    case FLASH_SUBOPSSTATE_WR:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
        retVal = FLASH_CmdWriteData(aID);
        break;
    case FLASH_SUBOPSSTATE_RS:
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_VER_WIP;
        retVal = FLASH_CmdReadStatus(aID);
        break;
    case FLASH_SUBOPSSTATE_VER_WIP:
        if ((FLASH_RWDev[aID].xferRxBuf[1] & FLASH_STATUSREG_WIP_MASK) ==
                FLASH_STATUSREG_WIP_MASK) {
            FLASH_RWDev[aID].opsInfo.isWIPBitSet = TRUE;
        } else {
            FLASH_RWDev[aID].opsInfo.isWIPBitSet = FALSE;
        }

        if (FLASH_FLSID_MICRON_N25Q512A == FLASH_RWDev[aID].flsID) {
            /* Command in progress and trigger sub state again */
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RR;
            *aIsTrigAgain = TRUE;
        } else {
            if (TRUE == FLASH_RWDev[aID].opsInfo.isWIPBitSet){
                /* Command in progress and trigger sub state again */
                FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
                *aIsTrigAgain = TRUE;
            } else {
                /* Command complete */
                *aIsSubStateEnd = TRUE;
            }
        }
        retVal = BCM_ERR_OK;
        break;
    case FLASH_SUBOPSSTATE_RR:
        /* Read Flag Status Register */
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_VER_PES;
        retVal = FLASH_CmdReadReg(aID, (uint8_t)FLASH_CMD_READ_FS_REG);
        break;
    case FLASH_SUBOPSSTATE_VER_PES:
        /* Verify state of program and erase status bit */
        if (((FLASH_RWDev[aID].xferRxBuf[1]
                        & FLASH_FLAGSTATUS_PROG_ERASE_CONTR_MASK)
                    == FLASH_FLAGSTATUS_PROG_ERASE_CONTR_MASK)
                && (FALSE == FLASH_RWDev[aID].opsInfo.isWIPBitSet)) {
            /* Command complete */
            *aIsSubStateEnd = TRUE;
        } else {
            /* Command in progress and trigger sub state again */
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_RS;
            *aIsTrigAgain = TRUE;
        }
        if ((FLASH_RWDev[aID].xferRxBuf[1]
                    & FLASH_FLAGSTATUS_ERR_MASK) != (uint8_t)0) {
            retVal = BCM_ERR_CUSTOM;
            FLASH_ReportError(BCM_QSP_ID, (uint8_t)aID,
                    BRCM_SWDSGN_FLASH_SUBOPSWRITE_PROC, retVal,
                    FLASH_RWDev[aID].xferRxBuf[1], 0UL, 0UL, 0UL);
        } else {
            retVal = BCM_ERR_OK;
        }
        break;
    default:
        /* Shall never reach here */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_UpdateStateDone(FLASH_HwIDType aID,
                                FLASH_OpsStateType aOpsState)
{
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_OK;
    FLASH_RWDev[aID].currOps = FLASH_OPS_NONE;
    FLASH_RWDev[aID].currOpsState = aOpsState;
    FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_NONE;
    FLASH_RWDev[aID].status = FLASH_STATUS_IDLE;
    FLASH_RWDev[aID].opsInfo.indFlag = FLASH_INDFLAG_DONE_IND;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static void FLASH_UpdateStateError(FLASH_HwIDType aID,
                                FLASH_OpsStateType aOpsState)
{
    FLASH_RWDev[aID].errFlag = FALSE;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_FAIL;
    FLASH_RWDev[aID].currOps = FLASH_OPS_NONE;
    FLASH_RWDev[aID].currOpsState = aOpsState;
    FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_NONE;
    FLASH_RWDev[aID].status = FLASH_STATUS_IDLE;
    FLASH_RWDev[aID].opsInfo.indFlag = FLASH_INDFLAG_ERR_IND;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_EraseStateHandler(FLASH_HwIDType aID,
                            uint32_t * const isTrigAgain)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t isSubStateEnd;
    uint32_t isSubStateTrigAgain;

    *isTrigAgain = FALSE;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_ERASE_STATE_ERROR;
    }

    switch(FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_ERASE_STATE_WE:
        retVal = FLASH_SubOpsWriteEnable(aID, &isSubStateEnd,
                                        &isSubStateTrigAgain);
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateEnd)) {
            FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_ERASE_STATE_SE;
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_SE;
            *isTrigAgain = TRUE;
        }
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateTrigAgain)) {
            *isTrigAgain = TRUE;
        }
        break;
    case FLASH_OPSSTATE_ERASE_STATE_SE:
        retVal = FLASH_SubOpsSectorErase(aID, &isSubStateEnd,
                                        &isSubStateTrigAgain);
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateEnd)) {
            if (FLASH_RWDev[aID].config.subSectorSize > 0UL) {
                FLASH_RWDev[aID].opsInfo.tmpAddr +=
                    FLASH_RWDev[aID].config.subSectorSize;
                FLASH_RWDev[aID].opsInfo.tmpLen -=
                    FLASH_RWDev[aID].config.subSectorSize;
            } else {
                FLASH_RWDev[aID].opsInfo.tmpAddr +=
                    FLASH_RWDev[aID].config.sectorSize;
                FLASH_RWDev[aID].opsInfo.tmpLen -=
                    FLASH_RWDev[aID].config.sectorSize;
            }
            if (0UL == FLASH_RWDev[aID].opsInfo.tmpLen) {
                FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_ERASE_STATE_DONE;
            } else {
                FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_ERASE_STATE_WE;
                FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WE;
            }
            *isTrigAgain = TRUE;
        }
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateTrigAgain)) {
            *isTrigAgain = TRUE;
        }
        break;
    case FLASH_OPSSTATE_ERASE_STATE_DONE:
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_ERASE_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_ERASE_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_ERASE_STATE_EXIT);
        break;
    default:
        /* Wrong current operation state */
        /* TODO: Add abort/crash */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    };

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_WriteStateHandler(FLASH_HwIDType aID,
                                uint32_t * const isTrigAgain)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t isSubStateEnd;
    uint32_t isSubStateTrigAgain;

    *isTrigAgain = FALSE;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_WRITE_STATE_ERROR;
    }

    switch(FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_WRITE_STATE_WE:
        retVal = FLASH_SubOpsWriteEnable(aID, &isSubStateEnd,
                                        &isSubStateTrigAgain);
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateEnd)) {
            FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_WRITE_STATE_WR;
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_PP;
            *isTrigAgain = TRUE;
        }
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateTrigAgain)) {
            *isTrigAgain = TRUE;
        }
        break;
    case FLASH_OPSSTATE_WRITE_STATE_WR:
        retVal = FLASH_SubOpsWrite(aID, &isSubStateEnd, &isSubStateTrigAgain);
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateEnd)) {
            FLASH_RWDev[aID].opsInfo.tmpLen -=
                                    FLASH_RWDev[aID].config.pageSize;
            FLASH_RWDev[aID].opsInfo.tmpAddr +=
                                    FLASH_RWDev[aID].config.pageSize;
            FLASH_RWDev[aID].opsInfo.tmpTxData =
                                &(FLASH_RWDev[aID].opsInfo.tmpTxData[\
                                    FLASH_RWDev[aID].config.pageSize]);
            if (0UL == FLASH_RWDev[aID].opsInfo.tmpLen) {
                FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_WRITE_STATE_DONE;
            } else {
                FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_WRITE_STATE_WE;
                FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WE;
            }
            *isTrigAgain = TRUE;
        }
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateTrigAgain)) {
            *isTrigAgain = TRUE;
        }
        break;
    case FLASH_OPSSTATE_WRITE_STATE_DONE:
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_WRITE_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_WRITE_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_WRITE_STATE_EXIT);
        break;
    default:
        /* Wrong current operation state */
        /* TODO: Add abort/crash */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    };

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_ReadStateHandler(FLASH_HwIDType aID,
                                    uint32_t * const isTrigAgain)
{
    int32_t retVal = BCM_ERR_OK;

    *isTrigAgain = FALSE;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_READ_STATE_ERROR;
    }

    switch(FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_READ_STATE_READ:
        retVal = FLASH_BusRead(aID, FLASH_RWDev[aID].opsInfo.opsAddr,
                        FLASH_RWDev[aID].opsInfo.opsRxData,
                        FLASH_RWDev[aID].opsInfo.opsLen);
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_READ_STATE_DONE;
        break;
    case FLASH_OPSSTATE_READ_STATE_DONE:
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_READ_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_READ_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_READ_STATE_EXIT);
        break;
    default:
        /* Wrong current operation state */
        /* TODO: Add core abort/crash */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    };

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CompareStateHandler(FLASH_HwIDType aID,
                                uint32_t *const aIsTrigAgain)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t tmpOpLen = 0UL;
    const uint8_t * txBuf = NULL;

    *aIsTrigAgain = FALSE;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_COMP_STATE_ERROR;
    }

    switch(FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_COMP_STATE_RD:
        if (0UL < FLASH_RWDev[aID].opsInfo.tmpLen) {
            tmpOpLen = FLASH_RWDev[aID].opsInfo.tmpLen;
            if (FLASH_LEN_PAGE < tmpOpLen) {
                tmpOpLen = FLASH_LEN_PAGE;
            }
        }
        FLASH_RWDev[aID].opsInfo.tmpLen -= tmpOpLen;
        FLASH_RWDev[aID].opsInfo.prevRxLen = tmpOpLen;
        retVal = FLASH_BusRead(aID, FLASH_RWDev[aID].opsInfo.tmpAddr,
                        FLASH_RWDev[aID].xferRxBuf, tmpOpLen);
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_COMP_STATE_CMP;
        break;
    case FLASH_OPSSTATE_COMP_STATE_CMP:
        if (NULL != FLASH_RWDev[aID].opsInfo.opsTxData) {
            tmpOpLen =  FLASH_RWDev[aID].opsInfo.opsLen -
                (FLASH_RWDev[aID].opsInfo.tmpLen +
                 FLASH_RWDev[aID].opsInfo.prevRxLen);
            txBuf = &(FLASH_RWDev[aID].opsInfo.opsTxData[tmpOpLen]);
        } else {
            /* Blank check case */
            BCM_MemSet(FLASH_RWDev[aID].xferTxBuf, 0xFF, FLASH_LEN_PAGE);
            txBuf = (const uint8_t *)FLASH_RWDev[aID].xferTxBuf;
        }
        retVal = memcmp(FLASH_RWDev[aID].xferRxBuf, txBuf,
                FLASH_RWDev[aID].opsInfo.prevRxLen);
        FLASH_RWDev[aID].opsInfo.tmpAddr += FLASH_RWDev[aID].opsInfo.prevRxLen;
        if (retVal == 0L) {
            if (0UL < FLASH_RWDev[aID].opsInfo.tmpLen) {
                FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_COMP_STATE_RD;
            } else {
                FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_COMP_STATE_DONE;
            }
        } else {
            FLASH_RWDev[aID].errFlag = TRUE;
        }
        retVal = BCM_ERR_OK;
        *aIsTrigAgain = TRUE;
        break;
    case FLASH_OPSSTATE_COMP_STATE_DONE:
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_COMP_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_COMP_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_COMP_STATE_EXIT);
        break;
    default:
        /* Wrong current operation state */
        /* TODO: Add core abort/crash */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    };

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_CfgReadLaneStateHandler(FLASH_HwIDType aID,
                                             uint32_t *const aIsTrigAgain)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t isSubStateEnd;
    uint32_t isSubStateTrigAgain;

    *aIsTrigAgain = FALSE;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_CFGLAN_STATE_ERROR;
    }

    switch(FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_CFGLAN_STATE_WE:
        retVal = FLASH_SubOpsWriteEnable(aID, &isSubStateEnd,
                &isSubStateTrigAgain);
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateEnd)) {
            FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_CFGLAN_STATE_WS;
            FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WS;
            *aIsTrigAgain = TRUE;
        }
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateTrigAgain)) {
            *aIsTrigAgain = TRUE;
        }
        break;
    case FLASH_OPSSTATE_CFGLAN_STATE_WS:
        retVal = FLASH_SubOpsWriteStatus(aID, &isSubStateEnd,
                &isSubStateTrigAgain);
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateEnd)) {
            *aIsTrigAgain = TRUE;
            FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_CFGLAN_STATE_DONE;
            FLASH_RWDev[aID].isLaneCfgDone = TRUE;
        }
        if ((BCM_ERR_OK == retVal) && (TRUE == isSubStateTrigAgain)) {
            *aIsTrigAgain = TRUE;
        }
        break;
    case FLASH_OPSSTATE_CFGLAN_STATE_DONE:
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_CFGLAN_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_CFGLAN_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_CFGLAN_STATE_EXIT);
        break;
    default:
        retVal = BCM_ERR_INVAL_STATE;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_ReadIDStateHandler(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_READID_STATE_ERROR;
    }

    switch(FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_READID_STATE_READ:
        retVal = FLASH_CmdReadID(aID);
        if (BCM_ERR_OK == retVal) {
            FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_READID_STATE_DONE;
        }
        break;
    case FLASH_OPSSTATE_READID_STATE_DONE:
        *(FLASH_RWDev[aID].opsInfo.flsID) =
            (((uint32_t)FLASH_RWDev[aID].xferRxBuf[1] << 16UL) |
             ((uint32_t)FLASH_RWDev[aID].xferRxBuf[2] << 8UL) |
             (uint32_t)FLASH_RWDev[aID].xferRxBuf[3]);
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_READID_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_READID_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_READID_STATE_EXIT);
        break;
    default:
        /* Wrong current operation state */
        /* TODO: Add core abort/crash */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    };

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
 */
static int32_t FLASH_RegReadWriteStateHandler(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    if (TRUE == FLASH_RWDev[aID].errFlag) {
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_REG_RD_WR_STATE_ERROR;
    }
    switch (FLASH_RWDev[aID].currOpsState) {
    case FLASH_OPSSTATE_REG_RD_WR_STATE_TRANSFER:
        retVal = FLASH_CmdRegReadWrite(aID);
        if (BCM_ERR_OK == retVal) {
            FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_REG_RD_WR_STATE_DONE;
        }
        break;
    case FLASH_OPSSTATE_REG_RD_WR_STATE_DONE:
        FLASH_UpdateStateDone(aID, FLASH_OPSSTATE_REG_RD_WR_STATE_EXIT);
        break;
    case FLASH_OPSSTATE_REG_RD_WR_STATE_ERROR:
        FLASH_UpdateStateError(aID, FLASH_OPSSTATE_REG_RD_WR_STATE_EXIT);
        break;
    default:
        /* Wrong current operation state */
        /* TODO: Add core abort/crash */
        retVal = BCM_ERR_INVAL_STATE;
        break;
    };

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
 */
static uint32_t FLASH_IsAddrFlashable(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    FLASH_FlsableSecType *addrList;
    uint32_t addrListSize;
    uint32_t i;
    uint32_t isFlsable = FALSE;
    FLASH_AddrType listEndAddr;
    FLASH_AddrType startAddr = aAddr;
    FLASH_AddrType endAddr = (aAddr + aLen - 1UL);
    FLASH_AddrType sectorSize = FLASH_RWDev[aID].config.sectorSize;

    addrList = &(FLASH_FlsableSecList[aID][0UL]);
    addrListSize = FLASH_RWDev[aID].config.flsableSecListSize;

    for (i = 0UL; i < addrListSize; i++) {
        listEndAddr = (addrList[i].startAddr) +
                    ((addrList[i].numSectors * sectorSize) - 1UL);
        if ((endAddr <= listEndAddr) &&
            (startAddr >= addrList[i].startAddr)) {
            isFlsable = TRUE;
            break;
        }
    }

    return isFlsable;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWREQ_FLASH_INIT
 */
static int32_t FLASH_OpsInit(FLASH_HwIDType aID,
                            const FLASH_CfgType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    FLASH_IOType flashIO;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_INIT;
    FLASH_RWDev[aID].flsID = aConfig->flsID;

    switch(FLASH_RWDev[aID].flsID) {
    case FLASH_FLSID_WINBOND_W25Q16:
        FLASH_RWDev[aID].isLaneCfgSup = TRUE;
        FLASH_RWDev[aID].quadEnByteOffset = 0x1UL;
        FLASH_RWDev[aID].quadEnBitMask = (uint8_t)0x2;
        break;
    case FLASH_FLSID_WINBOND_W25Q64CV:
        FLASH_RWDev[aID].isLaneCfgSup = TRUE;
        FLASH_RWDev[aID].quadEnByteOffset = 0x1UL;
        FLASH_RWDev[aID].quadEnBitMask = (uint8_t)0x2;
        break;
    case FLASH_FLSID_MICRON_N25Q512A:
        FLASH_RWDev[aID].isLaneCfgSup = FALSE;
        break;
    case FLASH_FLSID_MACRONIX_MX25R32:
        FLASH_RWDev[aID].isLaneCfgSup = FALSE;
        break;
    case FLASH_FLSID_MACRONIX_MX25L32:
        FLASH_RWDev[aID].isLaneCfgSup = FALSE;
        break;
    case FLASH_FLSID_MICRON_M25P16:
        FLASH_RWDev[aID].isLaneCfgSup = FALSE;
        break;
    default:
        retVal = BCM_ERR_NOSUPPORT;
        break;
    }

    if (BCM_ERR_OK != retVal) {
      goto err;
    }

    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    BCM_MemSet(&(FLASH_FlsableSecList), 0x0, sizeof(FLASH_FlsableSecList));

    /* Keep config for runtime usage */
    BCM_MemCpy(&(FLASH_RWDev[aID].config), aConfig, sizeof(FLASH_CfgType));

    /* Copy the flashable sector list */
    if ((aConfig->flsableSecListSize > 0UL) &&
            (NULL != aConfig->flsableSecList)) {
        BCM_MemCpy(&(FLASH_FlsableSecList[aID][0UL]), aConfig->flsableSecList,
                (aConfig->flsableSecListSize * sizeof(FLASH_FlsableSecType)));
    } else {
        FLASH_RWDev[aID].config.flsableSecListSize = 0UL;
    }

    /* Initialise Flash Bus */
    flashIO.retVal = BCM_ERR_UNKNOWN;
    flashIO.hwID = aID;
    flashIO.config = aConfig;

    retVal = FLASH_SysCmdReq(FLASH_BUS_CMD_INIT, &flashIO);
    if (BCM_ERR_OK == retVal) {
        FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_OK;
        FLASH_RWDev[aID].currOps = FLASH_OPS_NONE;
        FLASH_RWDev[aID].status = FLASH_STATUS_IDLE;
    } else {
        FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_FAIL;
        FLASH_RWDev[aID].currOps = FLASH_OPS_NONE;
        FLASH_RWDev[aID].status = FLASH_STATUS_UNINIT;
    }

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWREQ_FLASH_INIT
 */
static int32_t FLASH_OpsDeInit(FLASH_HwIDType aID)
{
    /* Reset the variables */
    FLASH_RWDev[aID].currOps = FLASH_OPS_NONE;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_OK;
    FLASH_RWDev[aID].errFlag = FALSE;
    FLASH_RWDev[aID].flsID = 0x0UL;
    if (TRUE == FLASH_RWDev[aID].isLaneCfgSup) {
        FLASH_RWDev[aID].isLaneCfgDone = FALSE;
        FLASH_RWDev[aID].quadEnByteOffset = 0x0UL;
        FLASH_RWDev[aID].quadEnBitMask = (uint8_t)0x0;
    }

    /* Set driver state to unitialised */
    FLASH_RWDev[aID].status = FLASH_STATUS_UNINIT;

    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsCancel(FLASH_HwIDType aID)
{
    /* Set current operation to none */
    FLASH_RWDev[aID].currOps = FLASH_OPS_NONE;

    if (FLASH_OPS_RESULT_PENDING == FLASH_RWDev[aID].opsRes) {
        FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_CANCELLED;
    }

    /* Set driver state to IDLE */
    FLASH_RWDev[aID].status = FLASH_STATUS_IDLE;

    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsErase(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;
    uint32_t isTrigAgain;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_ERASE;
    FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_ERASE_STATE_WE;
    FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WE;

    /* Set Operation Info */
    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    FLASH_RWDev[aID].opsInfo.opsAddr = aAddr;
    FLASH_RWDev[aID].opsInfo.opsLen = aLen;
    FLASH_RWDev[aID].opsInfo.tmpLen = aLen;
    FLASH_RWDev[aID].opsInfo.tmpAddr = aAddr;

    /* Set errFlag to FALSE */
    FLASH_RWDev[aID].errFlag = FALSE;

    /* Call FLASH_Erase State Machine */
    do {
        retVal = FLASH_EraseStateHandler(aID, &isTrigAgain);
    } while((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsWrite(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    const uint8_t * const aBuf, FLASH_LenType aLen)
{
    int32_t retVal;
    uint32_t isTrigAgain;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_WRITE;
    FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_WRITE_STATE_WE;
    FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WE;

    /* Set Operation Info */
    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    FLASH_RWDev[aID].opsInfo.opsAddr = aAddr;
    FLASH_RWDev[aID].opsInfo.opsTxData = aBuf;
    FLASH_RWDev[aID].opsInfo.opsLen = aLen;
    FLASH_RWDev[aID].opsInfo.tmpLen = aLen;
    FLASH_RWDev[aID].opsInfo.tmpAddr = aAddr;
    FLASH_RWDev[aID].opsInfo.tmpTxData = aBuf;

    /* Set errFlag to FALSE */
    FLASH_RWDev[aID].errFlag = FALSE;

    /* Call FLASH_Write State Machine handler */
    do {
        retVal = FLASH_WriteStateHandler(aID, &isTrigAgain);
    } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsRead(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    uint8_t * const aBuf, FLASH_LenType aLen)
{
    int32_t retVal;
    uint32_t isTrigAgain;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_READ;
    FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_READ_STATE_READ;
    FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_NONE;

    /* Set Operation Info */
    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    FLASH_RWDev[aID].opsInfo.opsAddr = aAddr;
    FLASH_RWDev[aID].opsInfo.opsRxData = aBuf;
    FLASH_RWDev[aID].opsInfo.opsLen = aLen;

    /* Set errFlag to FALSE */
    FLASH_RWDev[aID].errFlag = FALSE;

    /* Call FLASH_Read State Machine handler */
    do {
        retVal = FLASH_ReadStateHandler(aID, &isTrigAgain);
    } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsCompare(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    const uint8_t * const aBuf, FLASH_LenType aLen)
{
    int32_t retVal;
    uint32_t isTrigAgain;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_COMPARE;
    FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_COMP_STATE_RD;
    FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_NONE;

    /* Set Operation Info */
    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    FLASH_RWDev[aID].opsInfo.opsAddr = aAddr;
    FLASH_RWDev[aID].opsInfo.opsTxData = aBuf;
    FLASH_RWDev[aID].opsInfo.opsLen = aLen;
    FLASH_RWDev[aID].opsInfo.tmpLen = aLen;
    FLASH_RWDev[aID].opsInfo.tmpLenRem = aLen;
    FLASH_RWDev[aID].opsInfo.tmpAddr = aAddr;
    FLASH_RWDev[aID].opsInfo.tmpTxData = aBuf;

    /* Set errFlag to FALSE */
    FLASH_RWDev[aID].errFlag = FALSE;

    /* Call FLASH_Compare State Machine handler */
    do {
        retVal = FLASH_CompareStateHandler(aID, &isTrigAgain);
    } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsCfgReadLane(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t isTrigAgain;
    FLASH_IOType flashIO;

    flashIO.retVal = BCM_ERR_UNKNOWN;
    flashIO.hwID = aID;
    flashIO.readLane = FLASH_RWDev[aID].config.readLane;

    retVal = FLASH_SysCmdReq(FLASH_BUS_CMD_CFG_READ_LANE, &flashIO);
    if (BCM_ERR_OK == retVal) {
        FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
        FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
        FLASH_RWDev[aID].currOps = FLASH_OPS_CFG_READ_LANE;
        FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_CFGLAN_STATE_WE;
        FLASH_RWDev[aID].subOpsState = FLASH_SUBOPSSTATE_WE;

        BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
        /* Set errFlag to FALSE */
        FLASH_RWDev[aID].errFlag = FALSE;

        do {
            retVal = FLASH_CfgReadLaneStateHandler(aID, &isTrigAgain);
        } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsReadID(FLASH_HwIDType aID,
                               FLASH_FlsIDType *const aFlsID)
{
    int32_t retVal;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_READID;
    FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_READID_STATE_READ;

    /* Set Operation Info */
    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    FLASH_RWDev[aID].opsInfo.flsID = aFlsID;

    /* Set errFlag to FALSE */
    FLASH_RWDev[aID].errFlag = FALSE;

    /* Call FLASH_Read State Machine handler */
    retVal = FLASH_ReadIDStateHandler(aID);

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_REG_READ_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_OpsRegReadWrite(FLASH_HwIDType aID,
                                     uint8_t *const aRxBuf,
                                     const uint8_t *const aTxBuf,
                                     uint32_t aLen)
{
    int32_t retVal;

    FLASH_RWDev[aID].status = FLASH_STATUS_BUSY;
    FLASH_RWDev[aID].opsRes = FLASH_OPS_RESULT_PENDING;
    FLASH_RWDev[aID].currOps = FLASH_OPS_REG_READ_WRITE;
    FLASH_RWDev[aID].currOpsState = FLASH_OPSSTATE_REG_RD_WR_STATE_TRANSFER;

    /* Set Operation Info */
    BCM_MemSet(&(FLASH_RWDev[aID].opsInfo), 0x0, sizeof(FLASH_OpsInfoType));
    FLASH_RWDev[aID].opsInfo.opsRxData = aRxBuf;
    FLASH_RWDev[aID].opsInfo.opsTxData = aTxBuf;
    FLASH_RWDev[aID].opsInfo.opsLen = aLen;

    /* Set errFlag to FALSE */
    FLASH_RWDev[aID].errFlag = FALSE;

    /* Call FLASH_Read State Machine handler */
    retVal = FLASH_RegReadWriteStateHandler(aID);

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
static int32_t FLASH_OpsHandler(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    uint32_t isTrigAgain = FALSE;

    /* Based on operation call corresponding state handlers */
    do {
        switch(FLASH_RWDev[aID].currOps) {
        case FLASH_OPS_NONE:
            /*Reach here in case, flash operation is cancelled after
              checking for currOps in this handler */
            break;
        case FLASH_OPS_ERASE:
            retVal = FLASH_EraseStateHandler(aID, &isTrigAgain);
            break;
        case FLASH_OPS_WRITE:
            retVal = FLASH_WriteStateHandler(aID, &isTrigAgain);
            break;
        case FLASH_OPS_READ:
            retVal = FLASH_ReadStateHandler(aID, &isTrigAgain);
            break;
        case FLASH_OPS_COMPARE:
            retVal = FLASH_CompareStateHandler(aID, &isTrigAgain);
            break;
        case FLASH_OPS_CFG_READ_LANE:
            retVal = FLASH_CfgReadLaneStateHandler(aID, &isTrigAgain);
            break;
        case FLASH_OPS_READID:
            retVal = FLASH_ReadIDStateHandler(aID);
            break;
        case FLASH_OPS_REG_READ_WRITE:
            retVal = FLASH_RegReadWriteStateHandler(aID);
            break;
        default:
            /* Shall never reach here */
            retVal = BCM_ERR_INVAL_STATE;
            break;
        }
    } while ((TRUE == isTrigAgain) && (BCM_ERR_OK == retVal));

    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
static int32_t FLASH_CheckStatus(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    if (FLASH_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (FLASH_STATUS_BUSY == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_INIT_PROC
    @trace #BRCM_SWREQ_FLASH_INIT
*/
void FLASH_Init(FLASH_HwIDType aID,
                const FLASH_CfgType *const aConfig)
{
    int32_t retVal;
    uint32_t flsID;
    uint32_t i;

    if ((FLASH_MAX_HW_ID <= aID) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check for Flash ID supported */
    flsID = aConfig->flsID;

    for (i = 0UL; i < FLASH_FlsIDTblSz; i++) {
        if (flsID == FLASH_FlsIDTbl[i]) {
            break;
        }
    }

    if (FLASH_FlsIDTblSz == i) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }

    /* Quad lane not supported for Micron N25Q512A flash */
    if ((FLASH_FLSID_MICRON_N25Q512A == flsID)
            && (FLASH_READ_LANE_QUAD == aConfig->readLane)) {
        retVal = BCM_ERR_NOSUPPORT;
        goto err;
    }

    /* Check the controller status */
    if (FLASH_STATUS_UNINIT != FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* Check for flashable sector list size */
    if (FLASH_FLASHABLE_SECTOR_LIST_SZ < aConfig->flsableSecListSize) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = FLASH_OpsInit(aID, aConfig);

err:
    if (BCM_ERR_OK != retVal) {
        FLASH_ReportError((BCM_QSP_ID & BCM_LOGMASK_USER), (uint8_t)aID,
                BRCM_SWARCH_FLASH_INIT_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_FLASH_DEINIT_PROC
    @trace #BRCM_SWREQ_FLASH_INIT
*/
void FLASH_DeInit(FLASH_HwIDType aID)
{
    int32_t retVal;

    if (FLASH_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    retVal = FLASH_OpsDeInit(aID);
    if (BCM_ERR_OK == retVal) {
        if (TRUE == FLASH_IsBusBusy(aID)) {
            retVal = FLASH_BusStateHandler(aID, TRUE);
        }
    }
err:
    if (BCM_ERR_OK != retVal) {
        FLASH_ReportError((BCM_QSP_ID & BCM_LOGMASK_USER), (uint8_t)aID,
                BRCM_SWARCH_FLASH_DEINIT_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_FLASH_CANCEL_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS

*/
int32_t FLASH_Cancel(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    if (FLASH_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    retVal = FLASH_OpsCancel(aID);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if (TRUE == FLASH_IsBusBusy(aID)) {
        retVal = FLASH_BusStateHandler(aID, TRUE);
    }
    FLASH_OpsErrInd(aID);
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_GET_STATUS_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
int32_t FLASH_GetStatus(FLASH_HwIDType aID,
                        FLASH_StatusType * const aStatus)
{
    int32_t retVal;

    if ((FLASH_MAX_HW_ID <= aID) || (NULL == aStatus)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    *aStatus = FLASH_RWDev[aID].status;
    retVal = BCM_ERR_OK;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_GET_OPS_RESULT_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS

*/
int32_t FLASH_GetOpsResult(FLASH_HwIDType aID,
                           FLASH_OpsResultType * const aOpsRes)
{
    int32_t retVal;

    if ((FLASH_MAX_HW_ID <= aID) || (NULL == aOpsRes)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    *aOpsRes = FLASH_RWDev[aID].opsRes;
    retVal = BCM_ERR_OK;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_ERASE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
int32_t FLASH_Erase(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;

    retVal = FLASH_CheckStatus(aID);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if ((0UL == aLen) || (FLASH_RWDev[aID].config.size < (aAddr + aLen))) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (FLASH_RWDev[aID].config.subSectorSize > 0UL) {
        if (((aAddr & (FLASH_RWDev[aID].config.subSectorSize - 1UL)) != 0UL)
                || ((aLen & (FLASH_RWDev[aID].config.subSectorSize - 1UL)) != 0UL)) {
            retVal = BCM_ERR_INVAL_PARAMS;
            goto err;
        }
    } else {
        /* Check aAddr and aLen for flash sector size alignment */
        if (((aAddr & (FLASH_RWDev[aID].config.sectorSize - 1UL)) != 0UL)
                || ((aLen & (FLASH_RWDev[aID].config.sectorSize - 1UL)) != 0UL)) {
            retVal = BCM_ERR_INVAL_PARAMS;
            goto err;
        }
    }

    /* Check address for flasable address list */
    if (FALSE == FLASH_IsAddrFlashable(aID, aAddr, aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = FLASH_OpsErase(aID, aAddr, aLen);
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_WRITE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS

*/
int32_t FLASH_Write(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    const uint8_t * const aBuf, FLASH_LenType aLen)
{
    int32_t retVal;

    retVal = FLASH_CheckStatus(aID);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if ((0UL == aLen) || (NULL == aBuf)
        || (FLASH_RWDev[aID].config.size < (aAddr + aLen))) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check aAddr and aLen for flash page size alignment */
    if (((aAddr & (FLASH_RWDev[aID].config.pageSize - 1UL)) != 0UL)
        || ((aLen & (FLASH_RWDev[aID].config.pageSize - 1UL)) != 0UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check address for flasable address list */
    if (FALSE == FLASH_IsAddrFlashable(aID, aAddr, aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = FLASH_OpsWrite(aID, aAddr, aBuf, aLen);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
 */
int32_t FLASH_Read(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                   uint8_t * const aBuf, FLASH_LenType aLen)
{
    int32_t retVal;

    retVal = FLASH_CheckStatus(aID);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if ((0UL == aLen) || (NULL == aBuf)
        || (FLASH_RWDev[aID].config.size < (aAddr + aLen))) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = FLASH_OpsRead(aID, aAddr, aBuf, aLen);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_COMPARE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
int32_t FLASH_Compare(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                      const uint8_t * const aBuf, FLASH_LenType aLen)
{
    int32_t retVal;

    retVal = FLASH_CheckStatus(aID);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if ((0UL == aLen) || (NULL == aBuf)
        || (FLASH_RWDev[aID].config.size < (aAddr + aLen))) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = FLASH_OpsCompare(aID, aAddr, aBuf, aLen);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_BLANK_CHECK_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
int32_t FLASH_BlankCheck(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;

    retVal = FLASH_CheckStatus(aID);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if ((0UL == aLen) ||(FLASH_RWDev[aID].config.size < (aAddr + aLen))) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = FLASH_OpsCompare(aID, aAddr, NULL, aLen);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_CFG_READ_LANE_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
*/
int32_t FLASH_CfgReadLane(FLASH_HwIDType aID)
{
    int32_t retVal;

    if (FLASH_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (FLASH_STATUS_BUSY == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }
    if (TRUE == FLASH_RWDev[aID].isLaneCfgSup) {
        if (TRUE == FLASH_RWDev[aID].isLaneCfgDone) {
            /* Read lane configuration is already done and re-configuration is
             * not allowed */
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            retVal = FLASH_OpsCfgReadLane(aID);
        }
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS
 */
int32_t FLASH_ReadID(FLASH_HwIDType aID,
                     FLASH_FlsIDType *const aFlsID)
{
    int32_t retVal;

    if ((FLASH_MAX_HW_ID <= aID) || (NULL == aFlsID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (FLASH_STATUS_BUSY == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }

    retVal = FLASH_OpsReadID(aID, aFlsID);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_READ_ID_PROC
    @trace #BRCM_SWREQ_FLASH_OPERATIONS

*/
int32_t FLASH_RegReadWrite(FLASH_HwIDType aID,
                           uint8_t *const aRxBuf,
                           const uint8_t *const aTxBuf,
                           FLASH_LenType aLen)
{
    int32_t retVal;

    if ((FLASH_MAX_HW_ID <= aID) || (NULL == aTxBuf) || (0UL == aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Check the controller status */
    if (FLASH_STATUS_UNINIT == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if (FLASH_STATUS_BUSY == FLASH_RWDev[aID].status) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }

    retVal = FLASH_OpsRegReadWrite(aID, aRxBuf, aTxBuf, aLen);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_FLASH_STATEHANDLER_PROC
    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER
*/
void FLASH_StateHandler(FLASH_HwIDType aID)
{
    int32_t retVal = BCM_ERR_EAGAIN;

    if (FLASH_MAX_HW_ID <= aID) {
        FLASH_ReportError((BCM_QSP_ID & BCM_LOGMASK_USER), (uint8_t)aID,
                BRCM_SWARCH_FLASH_STATEHANDLER_PROC, BCM_ERR_INVAL_PARAMS,
                __LINE__, 0UL, 0UL, 0UL);
        goto exit;
    }

    /* Check if there is any flash operation in progress */
    if (FLASH_OPS_NONE == FLASH_RWDev[aID].currOps) {
        goto err;
    }

    /* Call QSPI state handler */
    if (TRUE == FLASH_IsBusBusy(aID)) {
        retVal = FLASH_BusStateHandler(aID, FLASH_RWDev[aID].errFlag);
        if (BCM_ERR_OK != retVal) {
            FLASH_RWDev[aID].errFlag = TRUE;
        }
    }

    if (BCM_ERR_OK == retVal) {
        /* No command in-progress with QSPI */
        if (TRUE == FLASH_IsBusBusy(aID)) {
            retVal = BCM_ERR_EAGAIN;
            goto err;
        }
    }

    /* Call Flash operation handler as QSPI is not busy */
    retVal = FLASH_OpsHandler(aID);
    if (BCM_ERR_OK != retVal) {
        FLASH_RWDev[aID].errFlag = TRUE;
    }

err:
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
        FLASH_ReportError((BCM_QSP_ID & BCM_LOGMASK_USER), (uint8_t)aID,
                BRCM_SWARCH_FLASH_STATEHANDLER_PROC, retVal,
                FLASH_RWDev[aID].currOps, 0UL, 0UL, 0UL);
    }

    if (BCM_ERR_EAGAIN != retVal) {
        if (FLASH_INDFLAG_DONE_IND == FLASH_RWDev[aID].opsInfo.indFlag) {
            FLASH_OpsDoneInd(aID);
        }
        if (FLASH_INDFLAG_ERR_IND == FLASH_RWDev[aID].opsInfo.indFlag) {
            FLASH_OpsErrInd(aID);
        }
    }
exit:
    return;
}

/** @} */
