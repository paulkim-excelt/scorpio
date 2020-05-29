/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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

#ifndef BL_UDP_H
#define BL_UDP_H

/** @file bl_udp.h
    @brief UDP API of Internet Stack

    This file provides the interface/APIs which can be used to communicate
    with a remote peer using the UDP protocol
*/

/* Includes */
#include <bl_inet.h>

/** UDP interface functions IDs for error reporting */
#define BL_UDP_SEND_API_ID        (0x01U)

/**********************************************************************
                     Interface Functions
**********************************************************************/
/** @brief  Send data using UDP
 *
 *  This API is used to make a request to the Internet stack to transmit
 *  the data provided in the buffer using UDP protocol (as configured in
 *  the stack configuration) to the remote end. In case the provided buffer
 *  is larger than the L2 MTU, it would not be transmitted and API would
 *  return BCM_ERR_NOMEM.
 *  On successful return from this call, the complete payload will be
 *  transmitted .
 *
 * @param[in]   aPayloadId            Payload identifier used to identify the
 *                                    client of the end point
 * @param[in]   aBuf                  Buffer containing the UDP payload
 * @param[in]   aLen                  Length of the UDP payload
 *
 * @retval      BL_BCM_ERR_OK            The complete data has been transmitted.
 *                                       The client is free to reuse the buffer
 * @retval      BL_BCM_ERR_INVAL_PARAMS  Returned in the following cases:
 *                                       1. aBuf is NULL or aLen is 0
 *                                       2. no UDP end point allocated for payload
 *                                       3. end point does not belong to a valid
 *                                          interface
 *                                       4. end point does not support UDP
 * @retval      BL_BCM_ERR_NOMEM         Transmission has not completed (due to
 *                                       unavailability of L2 buffers).  The client
 *                                      should retry later.
 * @retval      BL_BCM_ERR_NOT_FOUND     Next hop's MAC address is unresolved. The
 *                                      client is free to drop the packet or retry
 *                                      later
 */
int32_t BL_UDP_Send(const uint32_t aPayloadId,
                    const uint8_t* const aBuf,
                    const uint16_t aLen);

#endif /* BL_UDP_H */
