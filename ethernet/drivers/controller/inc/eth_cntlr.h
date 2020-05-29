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
    @defgroup grp_eth_cntlrdrv_cntlr_ifc Ethernet Controller Interface
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_cntlr_ifc
    @{

    @file ethernet/drivers/controller/inc/eth_cntlr.h
    @brief Ethernet Driver interface
    This header file contains the interface functions for Ethernet Controller

    @version 1.26 Imported from docx
*/

#ifndef ETH_CNTLR_H
#define ETH_CNTLR_H

#include <ethernet.h>

/**
    @name Ethernet Controller  API IDs
    @{
    @brief API IDs for Ethernet controller driver
*/
#define BRCM_SWARCH_ETHER_STATE_TYPE                         ()    /**< @brief #ETHER_StateType                     */
#define BRCM_SWARCH_ETHER_CHANSTATE_TYPE                     ()    /**< @brief #ETHER_ChanStateType                 */
#define BRCM_SWARCH_ETHER_ALIGN_MACRO                        ()    /**< @brief #ETHER_ALIGN                         */
#define BRCM_SWARCH_ETHER_MTU_MACRO                          ()    /**< @brief #ETHER_MTU                           */
#define BRCM_SWARCH_ETHER_MINFRMSIZENOCRC_MACRO              ()    /**< @brief #ETHER_MINFRMSIZENOCRC               */
#define BRCM_SWARCH_ETHER_MINFRMSIZE_MACRO                   ()    /**< @brief #ETHER_MINFRMSIZE                    */
#define BRCM_SWARCH_ETHER_CRCSIZE_MACRO                      ()    /**< @brief #ETHER_CRCSIZE                       */
#define BRCM_SWARCH_ETHER_VLANTAGSIZE_MACRO                  ()    /**< @brief #ETHER_VLANTAGSIZE                   */
#define BRCM_SWARCH_ETHER_FRMSIZENOCRC_MACRO                 ()    /**< @brief #ETHER_FRMSIZENOCRC                  */
#define BRCM_SWARCH_ETHER_FRAMESIZE_MACRO                    ()    /**< @brief #ETHER_FRAMESIZE                     */
#define BRCM_SWARCH_ETHER_CLIENTBUFIDX2CHANIDXMASK_MACRO     ()    /**< @brief #ETHER_CLIENTBUFIDX2CHANIDXMASK      */
#define BRCM_SWARCH_ETHER_CLIENTBUFIDX2CHANIDXSHIFT_MACRO    ()    /**< @brief #ETHER_CLIENTBUFIDX2CHANIDXSHIFT     */
#define BRCM_SWARCH_ETHER_CLIENTBUFIDX2CHANBUFIDXMASK_MACRO  ()    /**< @brief #ETHER_CLIENTBUFIDX2CHANBUFIDXMASK   */
#define BRCM_SWARCH_ETHER_CLIENTBUFIDX2CHANBUFIDXSHIFT_MACRO ()    /**< @brief #ETHER_CLIENTBUFIDX2CHANBUFIDXSHIFT  */
#define BRCM_SWARCH_ETHER_CLIENTBUFIDX2CHANIDX_MACRO         ()    /**< @brief #ETHER_CLIENTBUFIDX2CHANIDX          */
#define BRCM_SWARCH_ETHER_CLIENTBUFIDX2CHANBUFIDX_MACRO      ()    /**< @brief #ETHER_CLIENTBUFIDX2CHANBUFIDX       */
#define BRCM_SWARCH_ETHER_GETCLIENTBUFIDX_MACRO              ()    /**< @brief #ETHER_GETCLIENTBUFIDX               */
#define BRCM_SWARCH_ETHER_TIMEGETEGRESSTS_PROC               ()    /**< @brief #ETHER_TimeGetEgressTS               */
#define BRCM_SWARCH_ETHER_TIMEGETINGRESSTS_PROC              ()    /**< @brief #ETHER_TimeGetIngressTS              */
/** @} */

/**
    @name ETHER_StateType
    @{
    @brief States of Ethernet driver

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
typedef uint32_t ETHER_StateType; /**< @brief Ethernet state type */
#define ETHER_STATE_UNINIT  (0UL) /**< @brief Driver uninitialised */
#define ETHER_STATE_INIT    (1UL) /**< @brief Driver initialised */
/** @} */

/**
    @name ETHER_ChanStateType
    @{
    @brief States of Ethernet driver channels

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
typedef uint32_t ETHER_ChanStateType;   /**< @brief Ethernet channel state type */
#define ETHER_CHANSTATE_UNINIT          (0UL)   /**< @brief Uninitialised state */
#define ETHER_CHANSTATE_INIT            (1UL)   /**< @brief Initialised State */
/** @} */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_ALIGN(x, y)   ((((uint32_t)(x)) + (((uint32_t)(y)) - 1)) \
                            & (~(uint32_t)((y) - 1))) /**< @brief Ethernet buffer alignment macro */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_MTU               (1500UL)/**< @brief Ethernet MTU size */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_MINFRMSIZENOCRC   (60UL)  /**< @brief Ethernet minimum size frame without CRC */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_MINFRMSIZE        (64UL)  /**< @brief Ethernet minimum size frame with CRC */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CRCSIZE           (4UL)   /**< @brief CRC byte-length */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_VLANTAGSIZE       (4UL)   /**< @brief VLAN tag byte-length */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_FRMSIZENOCRC      (ETHER_MTU + ETHER_HDR_SIZE + ETHER_VLANTAGSIZE) /**< @brief Ethernet maximum size frame without CRC */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_FRAMESIZE         (ETHER_MTU + ETHER_HDR_SIZE \
                                + ETHER_VLANTAGSIZE \
                                + ETHER_CRCSIZE) /**< @brief Ethernet maximum size frame with CRC */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CLIENTBUFIDX2CHANIDXMASK      (0x000000F0UL) /**< @brief Ethernet Client buffer index to channel index mask */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CLIENTBUFIDX2CHANIDXSHIFT     (4UL)          /**< @brief Ethernet Client buffer index to channel index shift */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CLIENTBUFIDX2CHANBUFIDXMASK   (0x0000000FUL) /**< @brief Ethernet Client buffer index to channel buffer index mask */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CLIENTBUFIDX2CHANBUFIDXSHIFT  (0UL)          /**< @brief Ethernet Client buffer index to channel buffer index shift */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CLIENTBUFIDX2CHANIDX(aIdx)        \
    (((aIdx) & ETHER_CLIENTBUFIDX2CHANIDXMASK)  \
     >> ETHER_CLIENTBUFIDX2CHANIDXSHIFT)        /**< @brief Ethernet client buffer index to channel index */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_CLIENTBUFIDX2CHANBUFIDX(aIdx)             \
    (((aIdx) & ETHER_CLIENTBUFIDX2CHANBUFIDXMASK)       \
     >> ETHER_CLIENTBUFIDX2CHANBUFIDXSHIFT)     /**< @brief Ethernet Client buffer index to channel buffer index */

/**
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_GETCLIENTBUFIDX(aChan, aBufIdx)           \
    ((((aChan) << ETHER_CLIENTBUFIDX2CHANIDXSHIFT)      \
      & ETHER_CLIENTBUFIDX2CHANIDXMASK)                \
      | (((aBufIdx) & ETHER_CLIENTBUFIDX2CHANBUFIDXMASK))) /**< @brief Ethernet get client buffer index */

/**
    @trace #BRCM_SWREQ_ETHER_TIME
*/
extern int32_t ETHER_TimeGetEgressTS(uint32_t aID,
                                   uint32_t aPortIdx,
                                   uint32_t aBufIdx,
                                   ETHER_TimestampType* const aTS,
                                   ETHER_TimestampQualType* const aTSQual);

/**
    @trace #BRCM_SWREQ_ETHER_TIME
*/
extern int32_t ETHER_TimeGetIngressTS(uint32_t aID,
                                    uint32_t aPortIdx,
                                    const uint8_t* const aPktBuf,
                                    ETHER_TimestampType* const aTS,
                                    ETHER_TimestampQualType* const aTSQual);


#endif /* ETH_CNTLR_H */
/** @} */
