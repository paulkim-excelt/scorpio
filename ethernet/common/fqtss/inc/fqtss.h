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
    @defgroup grp_fqtss_ifc Interface
    @ingroup grp_fqtss

    @addtogroup grp_fqtss_ifc
    @{
    @file fqtss.h
    @brief FQTSS APIs
    This file specifies credits shaper interface
    @version 0.1 Initial Version
*/

#ifndef FQTSS_H
#define FQTSS_H

/**
    @name FQTSS Architecture IDs
    @{
    @brief Architecture IDs for NIF
*/
#define BRCM_SWARCH_FQTSS_GETBYTES_PROC       (0x00U) /**< @brief #FQTSS_GetBytes */
#define BRCM_SWARCH_FQTSS_CONSUMEBYTES_PROC   (0x01U) /**< @brief #FQTSS_ConsumeBytes */
#define BRCM_SWARCH_FQTSS_ENABLESTREAM_PROC   (0x02U) /**< @brief #FQTSS_EnableStream */
#define BRCM_SWARCH_FQTSS_DISABLESTREAM_PROC  (0x03U) /**< @brief #FQTSS_DisableStream */
#define BRCM_SWARCH_FQTSS_UPDATECREDITS_PROC  (0x04U) /**< @brief #FQTSS_UpdateCredits */
#define BRCM_SWARCH_FQTSS_MAX_STREAMS_MACRO   (0x05U) /**< @brief #FQTSS_MAX_STREAMS */
#define BRCM_SWARCH_FQTSS_AVB_BANDWIDTH_MACRO (0x06U) /**< @brief #FQTSS_MAX_AVB_BANDWIDTH */
/** @} */

/**
    @brief Maximum streams supported

    @trace #BRCM_SWREQ_FQTSS
*/
#define FQTSS_MAX_STREAMS         (8UL)

/**
    @brief Maximum AVB reservation

    @trace #BRCM_SWREQ_FQTSS
*/
#define FQTSS_MAX_AVB_BANDWIDTH   (70U)

/**
    @brief Request bytes from credit shaper

    Request bytes from FQTSS. It responds with the available credits (could be 0)

    @pre none

    @behavior Sync, Reentrant

    @param[in]    aStreamIdx         Stream index
    @param[inout] aBytesInOut        Length of the requested bytes (in),
                                     Length of the available credits (out)
    @return   #BCM_ERR_OK            Available bytes returned
    @return   #BCM_ERR_INVAL_PARAMS  aStreamIdx is invalid or aBytesInOut is NULL
                                     or stream has no reservation

    @post none

    @trace  #BRCM_SWREQ_FQTSS

    @limitations none

 */
int32_t FQTSS_GetBytes(uint32_t aStreamIdx,
                       uint32_t *const aBytesInOut);


/**
    @brief Consume bytes on transmission

    Inform FQTSS of number of bytes that were transmitted. Along with this the
    originally requested number is also passed so that the shaper can reuse
    unused credit

    @pre none

    @behavior Sync, Reentrant

    @param[in]    aStreamIdx         Stream index
    @param[in]    aRequested         Number of bytes originally requested
    @param[in]    aSent              Number of bytes transmitted

    @return   #BCM_ERR_OK            Bytes consumed successfully
    @return   #BCM_ERR_INVAL_PARAMS  aStreamIdx is invalid

    @post none

    @trace  #BRCM_SWREQ_FQTSS

    @limitations none
 */
int32_t FQTSS_ConsumeBytes(uint32_t aStreamIdx,
                           uint32_t aRequested,
                           uint32_t aSent);

/**
    @brief Enable stream for shaping

    Enable shaping on the stream as per the provided rate and quanta

    @pre none

    @behavior Sync, Reentrant

    @param[in]    aStreamIdx         Stream index
    @param[in]    aPeriod            Shaper time interval
    @param[in]    aReservation       Percentage of link bandwidth reserved for
                                     this stream
    @return   #BCM_ERR_OK            Stream enabled
    @return   #BCM_ERR_INVAL_PARAMS  StreamIdx is invalid or
                                     aReservation exceeds bound or
                                     aPeriod is 0

    @post none

    @trace  #BRCM_SWREQ_FQTSS

    @limitations none
 */
int32_t FQTSS_EnableStream(uint32_t aStreamIdx,
                           uint32_t aPeriod,
                           uint32_t aReservation);

/**
    @brief Disable stream shaping

    Disable shaping on the stream and update timer if required

    @pre none

    @behavior Sync, Reentrant

    @param[in]    aStreamIdx         Stream index
    @param[out]   aReservation       Reserved bandwidth of disabled stream
    @param[out]   aPeriod            Fastest period amongst remaining streams

    @return   #BCM_ERR_OK            Stream disabled
    @return   #BCM_ERR_INVAL_PARAMS  StreamIdx is invalid or
                                    aReservation is NULL or
                                    aPeriod is NULL

    @post none

    @trace  #BRCM_SWREQ_FQTSS

    @limitations none
 */
int32_t FQTSS_DisableStream(uint32_t aStreamIdx,
                            uint32_t* const aReservation,
                            uint32_t* const aPeriod);

/**
    @brief Replenish shaper credits

    Invoked once every class interval to replenish shaper credits

    @pre none

    @behavior Sync, Non-reentrant

    @return      void

    @post none

    @trace  #BRCM_SWREQ_FQTSS

    @limitations none
 */
void FQTSS_UpdateCredits(void);
#endif /* FQTSS_H */
/** @} */
