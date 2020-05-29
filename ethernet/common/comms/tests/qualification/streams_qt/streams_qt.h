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
    @defgroup grp_streams_rt Streams Qualification Tests
    @ingroup grp_comms_sw

    @addtogroup grp_streams_rt
    @{

    @file streams_qt.h
    @brief Streams Qualification Test cases
    This source file contains the test cases for Streams functionality
    @version 0.1 Initial version
*/

#ifndef STREAMS_QT_H
#define STREAMS_QT_H

/**
    @brief Stream creation

    @pre COMMS subsystem should be ready. VLAN 6 should be created with portMask 0xF
    and ARL with MAC address 00:11:22:33:44:55, VLAN 6 and portMask 0x1

    @test Invoke the #ETHERSWT_CMDID_STREAMPOLICER_ADD command with the following
    parameters:
    -# MAC address: 00:11:22:33:44:55
    -# vlan: 6
    -# rate: 30000 Kbps
    -# burstSize: 1000 bytes
    -# dropThreshold: 5
    -# monitoringInterval: 100
    -# portMask: 0x2
    -# action: having report and block actions enabled

    @post The command should succeed and stream index 0 should be returned. Should be able to
    retrieve the stream using the #ETHERSWT_CMDID_STREAMPOLICER_GET command using stream index 0

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
#define BRCM_SWQTST_COMMS_STREAMS_CASE0   (0UL)

/**
    @brief Stream deletion

    @pre #BRCM_SWQTST_STREAMS_CASE0 should have executed and passed

    @test Invoke the #ETHERSWT_CMDID_STREAMPOLICER_DEL command with stream index 0

    @post The command should succeed. Invoking the #ETHERSWT_CMDID_STREAMPOLICER_GET
    command using stream index 0 should return an error

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
#define BRCM_SWQTST_COMMS_STREAMS_CASE1   (1UL)

/** @brief Addition/Deletion of a stream


    @code{.c}
    Check Expected Result for BRCM_SWQTST_COMMS_STREAMS_CASE0
    Check Expected Result for BRCM_SWQTST_COMMS_STREAMS_CASE1
    @endcode

    @board board1
    @board board2

    @auto           Yes/No/Semi

    @type           Sanity/ShortRun/LongRun

    @testproc #BRCM_SWQTST_COMMS_STREAMS_CASE0
    @testproc #BRCM_SWQTST_COMMS_STREAMS_CASE1

*/
#define BRCM_SWQTSEQ_COMMS_STREAMS_SEQ0   (1UL)


#endif /* STREAMS_QT_H */

/** @} */
