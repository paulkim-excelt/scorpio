#
# Copyright 2017-2019 Broadcom Limited.  All rights reserved.
#
# This program is the proprietary software of Broadcom Limited and/or its
# licensors, and may only be used, duplicated, modified or distributed pursuant
# to the terms and conditions of a separate, written license agreement executed
# between you and Broadcom (an "Authorized License").
#
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
# LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
# IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
# 1. This program, including its structure, sequence and organization,
#    constitutes the valuable trade secrets of Broadcom, and you shall use all
#    reasonable efforts to protect the confidentiality thereof, and to use this
#    information only in connection with your use of Broadcom integrated
#    circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
#    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
#    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
#    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
#    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
#    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
#    SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
#    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
#    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################


##  @defgroup grp_lwq Light Weight Queues
#   @ingroup grp_system_sw
#
#   @addtogroup grp_lwq
#   @{
#   @section sec_lwq_overview Overview
#   This Module contains requirements, Interface, Design
#   and implementation details of Light Weight Queues.
#   <BR>Light Weight Queues are intented to be used by
#   media framework and media components extensively.
#
#   Light Weight Queue provides the following features:
#       -# Macros to define and initialize a LWQ
#       -# APIs to get number of indices in LWQ
#       -# APIs to push/pop to/from the LWQ
#       -# API to peek into the LWQ
#       .
#   @todo update state machine and sequence diagram
#   @limitations Maximum number of objects in the queue is limited to 15.
#   @trace #BRCM_SWREQ_LWQ
#
#   @}
#
#   @defgroup grp_msgq Message Queue
#   @ingroup grp_system_sw
#
#
#   @addtogroup grp_msgq
#   @{
#   @section sec_msgq_overview Overview
#   This Module contains requirements, Interface, Design
#   and implementation details of Message Queues.
#   <BR>Message Queues are intented to be used for Inter-Process
#   communications. They form basic blocks for Client/Server
#   communication. They are intented to be used by Media Framework,
#   Media Components, and System services like Flash Manager, PTM Server etc.
#   @image html msgq_overview.jpg "Message Queue Overview"
#   Message Queue provides a FIFO based message passing between different
#   components. Each queue acts as a communication channel between one server
#   and multiple clients. Each message queue is owned by the corresponding
#   server (service provider). Requester (client) posts a message to the queue
#   (which internally triggers the service provider). Service provider (server)
#   can statically configure queue to notify the client. When client
#   notifications are enabled, service provider signals the requester after
#   processing the request.
#
#   @image html msgq_layout.jpg "Message Queue Layout"
#
#   @limitations Message Queue supports a depth of 15 messages
#   @limitations It is recommended that the Message Queue Server Task has a
#   higher priority than the Client Tasks. If the Server Task has a lower
#   priority than a Client Task and the client uses #MSGQ_TryCancelMsg API,
#   Integrator shall ensure that the Server Task gets an opportunity to execute
#   before client task posts another message.
#
#   @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
#
#   @}
#
#   @file system/queue/comp.mk
#   @brief Makefile for Queues
#   @version 0.2 Added groups for Message Queues
#   @version 0.1 Initial version
#

BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += inc/lw_queue.h
ifeq ($(ENABLE_MSG_QUEUE), TRUE)
BRCM_CONFIGURATION_ITEM += inc/msg_queue.h
BRCM_CONFIGURATION_ITEM += inc/osil/msg_queue_osil.h
endif
ifneq ($(cust),1)
BRCM_CONFIGURATION_ITEM += doc/lw_queue_req.c
BRCM_CONFIGURATION_ITEM += lib/lw_queue.c
ifeq ($(ENABLE_MSG_QUEUE), TRUE)
BRCM_CONFIGURATION_ITEM += doc/msg_queue_req.c
BRCM_CONFIGURATION_ITEM += lib/msg_queue.c
BRCM_CONFIGURATION_ITEM += lib/msg_queue_priv.c
BRCM_CONFIGURATION_ITEM += os/erika/msg_queue_osil.c
endif
endif

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

QUEUE_DIR := $(BRCM_SDK_ROOT)/system/queue

BRCM_INC += $(BRCM_SDK_ROOT)/drivers/include

ifeq ($(ENABLE_MSG_QUEUE), TRUE)
    BRCM_SRC += $(BRCM_SDK_ROOT)/system/queue/lib/msg_queue_priv.c
    BRCM_SRC += $(BRCM_SDK_ROOT)/system/queue/lib/msg_queue.c
endif

BRCM_SRC += $(BRCM_SDK_ROOT)/system/queue/lib/lw_queue.c

BRCM_COMP_NAME := queue
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

