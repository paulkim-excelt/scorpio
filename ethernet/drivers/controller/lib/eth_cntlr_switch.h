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
/**
    @defgroup grp_eth_cntlrdrv_switch_ifc_impl Ethernet Controller Switch Integration
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_switch_ifc_impl
    @{
*/

#ifndef ETH_CNTLR_SWITCH_H
#define ETH_CNTLR_SWITCH_H

#include <system.h>
#include <eth_switch.h>

/**
    @name Ethernet Switch Driver interface IDs
    @{
    @brief API IDs for thernet Switch Driver
*/
#define BRCM_SWARCH_ETHER_CNTLRSWTTXADAPTBUFFER_PROC              (0x30U) /**< @brief #ETHER_CntlrSwtTxAdaptBuffer */
#define BRCM_SWARCH_ETHER_CNTLRSWTTXPROCESSFRAME_PROC             (0x31U) /**< @brief #ETHER_CntlrSwtTxProcessFrame */
#define BRCM_SWARCH_ETHER_CNTLRSWTTXDONEIND_PROC                  (0x32U) /**< @brief #ETHER_CntlrSwtTxDoneInd */
#define BRCM_SWARCH_ETHER_CNTLRSWTRXPROCESSFRAME_PROC             (0x33U) /**< @brief #ETHER_CntlrSwtRxProcessFrame */
#define BRCM_SWARCH_ETHER_CNTLRSWTRXDONEIND_PROC                  (0x34U) /**< @brief #ETHER_CntlrSwtRxDoneInd */
/** @} */

/** @brief Adapt the transmit buffer

    This API adapts the transmit buffer and prepare for addition of management
    information.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aDataInOut  - In: Pointer to the position of the EthType in
                                  common Ethernet frame
                                - Out: Pointer to the position of the EthType in
                                  management frame
    @param[inout]   aLenInOut   - In: Size of the Ethernet frame without
                                  management information
                                - Out: Size of the Ethernet frame with
                                  management information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully adapted transmit buffer
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut points to a NULL pointer
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut is NULL
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_SWITCH_INTGERATION

    @limitations None
*/
extern int32_t ETHER_CntlrSwtTxAdaptBuffer(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t **const aDataInOut,
        uint32_t * const aLenInOut);

/** @brief Process transmit frame

    This API processes transmit frame to add management information.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aDataInOut  - In: Pointer to the position of the EthType in
                                  common Ethernet frame
                                - Out: Pointer to the position of the EthType in
                                  management frame
    @param[inout]   aLenInOut   - In: Size of the Ethernet frame without
                                  management information
                                - Out: Size of the Ethernet frame with
                                  management information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully processed transmit
                                            frame
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid aBufIdx
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Data buffer pointer pointed by
                                            aDataInOut is  NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Value pointed by aLenInOut is zero
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_SWITCH_INTGERATION

    @limitations None
*/
extern int32_t ETHER_CntlrSwtTxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut);

/** @brief Callback indication from the Ethernet driver when a management
    enabled packet transmission is completed

    This callback indication is called from the Ethernet driver when a
    management enabled packet transmission is completed and there is nothing
    more to be done on that packet. (This indication comes from the Ethernet
    driver when all the required processing for the frame is completed including
    the TX confirmation callback to the client.)

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      bufIdx      Adapted Ethernet buffer index


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_SWITCH_INTGERATION

    @limitations None
*/
extern int32_t ETHER_CntlrSwtTxDoneInd (ETHER_HwIDType aCntrlID,
        uint32_t bufIdx);

/** @brief RX process frame callback

    This RX process frame callback is called from the Ethernet driver when a
    packet arrives to extract management information.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aDataInOut  - In: Pointer to the EtherType in common
                                  Ethernet frame
                                - Out: Pointer to EtherType in management frame
    @param[inout]   aLenInOut   - In: Length of the frame received
                                - Out: Length of the frame deceased by the
                                  management information
    @param[out]     isMgmtFrame Set to #TRUE if its management frame only
                                (consumed by Ethernet switch driver)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully processed
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut points to a NULL pointer
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut in NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut points to zero
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized
    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_SWITCH_INTGERATION

    @limitations None
*/
extern int32_t ETHER_CntlrSwtRxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut,
        uint32_t * const isMgmtFrame);

/** @brief RX finished callback indication

    Rx finished callback indication from the Ethernet driver. This interface
    shall raise the Rx timestamp callback to the client.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Adapted Ethernet buffer index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_SWITCH_INTGERATION

    @limitations any None
*/
extern int32_t ETHER_CntlrSwtRxDoneInd(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx);

#endif /* ETH_CNTLR_SWITCH_H */
/** @} */
