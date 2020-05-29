/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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
 * @file shell.h
 *
 * @brief Shell Callbacks.
 *
 * This header file describes the shell callback functions used by shell
 */


#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <inttypes.h>
#include <console.h>

/** @brief Application Init Callback called by Shell.
 *
 * Initialization function to be implemented by application for
 * application specific initialization. Function is called by Shell
 * during system late init
 *
 * @param[in]   void
 *
 * @return      void
 */
extern void ApplicationInit(void);

/** @brief Callback from the shell to handle "app" command on console
 *
 * This callback will be invoked from shell when "app" command is invoked
 * on consolde. This needs to be provided by the application
 *
 * @param[in]   aConsoleID      Console on which the complete command was
 *                              detected.
 * @param[in]   aArgString      Argument string received from the console.
 * @param[in]   aArgStringLen   Length of the argument string in bytes.
 *
 * @return      void
 */
extern void Shell_App(uint32_t aConsoleID, char *aArgString, uint32_t aArgStringLen);

/** @brief Callback from the shell to handle "test" command on console
 *
 * This callback will be invoked from shell when "test" command is invoked
 * on consolde. This needs to be provided by the application
 *
 * @param[in]   aConsoleID      Console on which the complete command was
 *                              detected.
 * @param[in]   aArgString      Argument string received from the console.
 * @param[in]   aArgStringLen   Length of the argument string in bytes.
 *
 * @return      void
 */
extern void Shell_TestFunc(uint32_t aConsoleID, char *aArgString, uint32_t aArgStringLen);

#endif /* SHELL_H */
