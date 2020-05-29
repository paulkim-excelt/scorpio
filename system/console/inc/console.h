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
/**
 * @file console.h
 *
 * @brief Console APIs.
 *
 * This header file describes the console APIs used by shell
 */


#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdlib.h>
#include <inttypes.h>
#include <compiler.h>

/* Console API IDs for error reporting */
#define CONSOLE_CMD_HANDLER_API_ID  (0x00U) /**< Console_CmdHandler API ID */
#define CONSOLE_RCV_IND_API_ID      (0x01U) /**< Console_RcvInd API ID */
#define CONSOLE_WRITE_API_ID        (0x02U) /**< Console_Write API ID */
#define CONSOLE_PROCESS_API_ID      (0x03U) /**< Console_Process API ID */
#define SPLIT_LINE_API_ID           (0x04U) /**< SplitLine API ID */

/* Console IDs  */
#define CONSOLE_UART            (0UL)   /**< UART Console ID    */
#define CONSOLE_MAX_NUM         (1UL)   /**< Max Console ID     */

/* Console SVC Commands */
#define CONSOLE_INIT            (0UL)   /**< Command to initialise console */
#define CONSOLE_WRITE           (1UL)   /**< Command to write to the console */
#define CONSOLE_READ            (2UL)   /**< Command to read from the console */

/* Result values for Quality Report */
#define CONSOLE_QUAL_PASS     (1UL)    /**< Quality report status value for pass test case */
#define CONSOLE_QUAL_FAIL     (0UL)    /**< Quality report status value for fail test case */

/** @brief Handles the shell command requested from the console.
 *
 * Handles the shell command requested from the console. These functions are to
 * be defined by each component or driver and hooked on to the shell.
 *
 * @param[in]   aconsoleID      Console from which the command was requested.
 * @param[in]   aArgString      Argument String received for the command.
 * @param[in]   aArgStringLen   Size of argument string in bytes.
 *
 * @return      void
 */
typedef void (*CmdHandler)(uint32_t aConsoleID, char *aArgString,
                            uint32_t aArgStringLen);

/** @brief Callback from the console on detecting a complete command
 *
 * This callback will be invoked from the console on detecting a complete
 * command (i.e. carriage return). This needs to be provided by the application
 * that uses console.
 *
 * @param[in]   aConsoleID      Console on which the complete command was
 *                              detected.
 * @param[in]   aCmdString      Command received from the console.
 * @param[in]   aArgString      Argument string received from the console.
 * @param[in]   aArgStringLen   Length of the argument string in bytes.
 *
 * @return      void
 */
extern void CONSOLE_ExecuteCmd(uint32_t aConsoleID, char *aCmdString,
                        char *aArgString, uint32_t aArgStringLen);

/** @brief Callback from the console to indicate data arrival on the console
 *
 * This API is triggered from the console to the application whenever there is
 * data received on the console. This callback is to be provided by the
 * application.
 *
 * @param[in]   aConsoleID      ID of console on which data arrived.
 *
 * @return      void
 */
extern void CONSOLE_RcvInd(uint32_t aConsoleID);

void CONSOLE_SetUlogID(uint32_t aConsoleID);

void CONSOLE_SetProfileID(uint32_t aConsoleID);
/**
 * Structure for Console IO
 */
typedef struct _CONSOLE_IOType {
    int32_t response;           /**< SVC Response */
    const char * buffer;        /**< Pointer to buffer */
    uint32_t bufSize;           /**< Size of the buffer */
    uint32_t numRead;           /**< Number of characters read from the console */
} CONSOLE_IOType;


/** @brief API to write to a console
 *
 * Write the contents of the buffer to the given console
 *
 * @param[in]   aConsoleID      Console to which the buffer is to be written to.
 * @param[in]   aBuffer         Pointer to the buffer.
 *
 * @return      void
 */
extern void CONSOLE_Write(uint32_t aConsoleID, const char * const aBuffer);

extern void CONSOLE_Read(uint32_t aConsoleID);
/** @brief API to process the RX Event for the given console
 *
 * API to process the RX Event for the given console. This API will fill the
 * corressponding console's Rx buffer and on detecting a complete command (i.e.
 * a carriage return), calls the provided callback to handle the command.
 *
 * @param[in]   aConsoleID      Console which needs to process its Rx event.
 *
 * @return      void
 */
extern void CONSOLE_Process(uint32_t aConsoleID);

/** @brief Split the string to get the first word
 *
 * This API parses through the given string to get the first word (i.e. till the
 * first blank space). The blank space so found is replaced by \0 and *remains
 * will point to rest of the string.
 *
 * @param[inout]   aString          Input string. The first blank space in the
 *                                  string is replaced by a \0
 * @param[in]       aSize           Size of the string in bytes
 * @param[out]      aRemains        *aRemains will point to the next location
 *                                  after the first blank space
 * @param[out]      aRemainLen      Number of characters after the first blank
 *                                  space
 *
 * @return          void
 */
extern void CONSOLE_SplitLine(char * const aString, uint32_t aSize, char **aRemains,
                      uint32_t * const aRemainLen);

/** @brief Print formatted string to Buffer
*/
void CONSOLE_PrintToBuffer(char* aLogBuf, uint32_t aLogLen, const char *const aFmt, ...);

void CONSOLE_QualReport(uint32_t aConsoleID,
                        uint16_t aCompId,
                        uint32_t aTestSeq,
                        uint32_t aTestCase,
                        uint32_t aStatus);

#endif /* CONSOLE_H */
