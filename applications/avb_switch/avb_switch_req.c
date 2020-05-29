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

/** @defgroup grp_avb_switch_req Requirements
    @ingroup grp_avb_switch
    @addtogroup grp_avb_switch_req
    @{
    @section sec_avb_switch_req_overview Overview
    AVB Switch application shall support the following AVB and L2 functionalities
    for all participating ports of a switch.
    -# 802.1AS (Timing and Synchronization for Time-Sensitive Applications in
       Bridged Local Area Networks) protocol in SLAVE mode as per AVnu specification
       (by default).
        -# Other modes of operation(GM/MASTER) can be supported by modifying the
           configuration.
    -# 802.1Qav (Forwarding and Queuing Enhancements for Time-Sensitive Streams)
       and 802.1Qat (Stream Reservation Protocol).
    -# L2 functionalities : VLAN management, ARL table configuration and
       traffic management.
    -# L2 functionalities : Stream Policing, CFP configuration.
    -# Light Stacking mode of operation.


    @file avb_switch_req.c
    @brief Requirements for AVB Switch application

    @version 0.1 Initial version
*/

/**
    @brief Establishing AVB domain on a switch with all participating ports.

    For a bridge to be called AVB capable:
    -# All participating ports shall be capable of supporting gPTP.
    -# All participating ports shall be capable of supporting at least one
       stream reservation, associated with a traffic class that supports FQTSS.
    -# All Bridges shall support SR class B.

    @rationale
    Consumer demand has driven a large increase in audio and video
    features and options in the automobile. Once only found in luxury cars,
    features such as movie playback, backup cameras and navigation have
    become commonplace options in many mainstream automobiles. Rear Seat
    Entertainment (RSE) units are growing in sophistication with more
    sources and choices. Each of these options has added to the need
    and desire for a common networking architecture in the automobile.

    AVB is an enhancement to the Ethernet suite of open standards.
    It provides quality of service(QoS) guarantees, a network time
    synchronization service and a related transport protocol for transmission
    of time-sensitive traffic that together allow a network to handle
    audio-visual (AV) data.

    Automotive applications, such as infotainment and drivers assistance,
    can deploy these specifications for an in-vehicle high bandwidth,
    and synchronized network. To do this, AVB uses a number of important
    concepts.
    -# priority: To specify that some data streams are time-sensitive
       and distinct from ordinary traffic that gets carried on a
       best-effort basis.
    -# reservation: The network can set aside a certain amount of
       guaranteed bandwidth to handle this high-priority traffic.
    -# Network time: A set of latency specifications, enables synchronized
       AV playback through simultaneous packet delivery.
    -# Traffic Shaping: Ensure that the end-points do not have jittery
       presentation of the media.

    By establishing an AVB domain for a bridge, all the above requirements
    can be met.

    @analysis
    To get an AVB Switch application, the following things needs to be done:
    -# BR-Ports needs to be configured in master/slave mode.
    -# Uplink ports needs to be configured in SGMII/RGMII/RvMII mode.
    -# All ports needs to be linked up before 100 msec from power up.
    -# System initialization including peripheral drivers.
    -# Ethernet sub-system initialization.
    -# Valid MAC address for the switch.
    -# Switch needs to be initialized. The switch initialization consists of:
        -# Programming any user specific VLAN's
        -# Programming/Configuring the ARL table
        -# Programming the CFP block to trap 802.1AS packets to the ARM
        -# Programming the PCP-TC and TC-PCP mapping
        -# Mechanism to reserve the bandwidth for a port.
        -# Mechanism for stream policing.
        -# Configuring ports for light stacking mode.
        -# gPTP initialization consisting of:
    -# Configuring the gPTP role as either MASTER/SLAVE/GM/STACKING
    -# Configuring the signalling intervals.
    -# Configuring the peer delay.

    When the switch is powered up, the transceiver ports are configured with
    the appropriate configuration. This configuration shall be stored in external
    flash. A successful configuration of the ports results in link-up
    (if the peer is connected and configured).
    The switch needs to be configured before the applcation can start. The switch
    is configured statically by loading the respective configuration from the flash.
    After the switch is initialized, the gPTP module is initialized. This is required
    to set the port roles, signalling intervals and other AVnu parameters as required
    by the gPTP module. These parameters are read from the flash and passed to the
    module for initialization.
    After the above modules are initialized, the application is initialized.
    Exchange of gPTP messages indicates a successful application initialization.

    The AVB application shall consume 200KB of memory (including code and data).

    @todo Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment.

    @verification
    -# Packets going out of the switch can be monitored to see if they are correct
       from syntax and semantic point of view for an L2 packet.
    -# A successful establishment of AVB domain shall result in a common notion
       of time across all devices. This can be verified by snooping the packets.
    -# End points which are connected to the switch can send their AVB media
       data from talkers to listeners within the time as specified by AVB spec.
    -# Correction factors on downstream ports shall be linearly increasing without
       any negative jump with respect to previous packet.
    -# Switch can be configured to send reserved bandwidth for AVB traffic.
    -# Switch can be configured to send AVB data on respective queues.
    -# Switch can be configured to send gPTP packets at different intervals
       based on the configuration.
    -# Switch MIBS can be dumped to check if there are any packet loss on AVB port.

    @dependencies #BRCM_SWREQ_BL_EXEC
    @dependencies #BRCM_SWREQ_SYSTEM_INIT
    @dependencies #BRCM_SWREQ_ETS_CONFIG
    @dependencies #BRCM_SWREQ_SWITCH_CONFIG
    @dependencies #BRCM_SWREQ_SHELL_HOOKS

*/

#define BRCM_SWREQ_AVBSWITCH

/** @} */
