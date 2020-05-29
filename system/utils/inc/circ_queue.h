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
    @defgroup grp_circq_ifc Interface
    @ingroup grp_circq

    @addtogroup grp_circq_ifc
    @{
    @file circ_queue.h
    @brief Circular queue interface

    @version 1.3 Export comments from docx
*/
#ifndef CIRC_QUEUE_H
#define CIRC_QUEUE_H

#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>

/**
    @name Circular Queue Architecture IDs
    @{
    @brief Architecture IDs for Component
*/
#define BRCM_SWARCH_CIRCQ_OVERWRITE_TYPE        (00U)   /**< @brief #CIRCQ_OverwriteType  */
#define BRCM_SWARCH_CIRCQ_INDEX_TYPE            (01U)   /**< @brief #CIRCQ_IndexType      */
#define BRCM_SWARCH_CIRCQ_TYPE                  (02U)   /**< @brief #CIRCQ_Type           */
#define BRCM_SWARCH_CIRCQ_INDEX_INIT_MACRO      (03U)   /**< @brief #CIRCQ_INDEX_INIT     */
#define BRCM_SWARCH_CIRCQ_INDEX_DEFINE_MACRO    (04U)   /**< @brief #CIRCQ_INDEX_DEFINE   */
#define BRCM_SWARCH_CIRCQ_INIT_MACRO            (05U)   /**< @brief #CIRCQ_INIT           */
#define BRCM_SWARCH_CIRCQ_DEFINE_MACRO          (06U)   /**< @brief #CIRCQ_DEFINE         */
#define BRCM_SWARCH_CIRCQ_V2_MACRO              (07U)   /**< @brief #CIRCQ_V2_DEFINE      */
#define BRCM_SWARCH_CIRCQ_RESET_PROC            (08U)   /**< @brief #CIRCQ_Reset          */
#define BRCM_SWARCH_CIRCQ_ISEMPTY_PROC          (09U)   /**< @brief #CIRCQ_IsEmpty        */
#define BRCM_SWARCH_CIRCQ_ISFULL_PROC           (10U)   /**< @brief #CIRCQ_IsFull         */
#define BRCM_SWARCH_CIRCQ_GETFILLEDSIZE_PROC    (11U)   /**< @brief #CIRCQ_GetFilledSize  */
#define BRCM_SWARCH_CIRCQ_PUSH_PROC             (12U)   /**< @brief #CIRCQ_Push           */
#define BRCM_SWARCH_CIRCQ_POP_PROC              (13U)   /**< @brief #CIRCQ_Pop            */
#define BRCM_SWARCH_CIRCQ_PEEK_PROC             (14U)   /**< @brief #CIRCQ_Peek           */
/** @} */

/**
    @name CIRCQ_Overwrite
    @{
    @brief Circular Queue Overwrite enable/disable

    @trace #BRCM_SWREQ_CIRCQ_CONFIGURATION
*/
typedef uint32_t CIRCQ_OverwriteType;       /**< @brief Circular Queue Overwrite Mode */
#define CIRCQ_OVERWRITE_DISABLE     (0UL)   /**< @brief Disable queue overwrite */
#define CIRCQ_OVERWRITE_ENABLE      (1UL)   /**< @brief Enable queue overwrite */
/** @} */

/**
    @brief Circular Queue Index

    @trace #BRCM_SWREQ_CIRCQ_CONFIGURATION
*/
typedef struct _CIRCQ_IndexType {
    volatile uint32_t qIndex; /**< @brief Read/write index.
                                  <BR>[31:16] buffer read index.
                                  <BR>[15:00] buffer write index.*/
    volatile uint32_t qLock;  /**< @brief Buffer write access lock.
                                  <BR>Incremented by every push call at entry.
                                  <BR>Decremented by every push call before exit. */
} CIRCQ_IndexType;

/**
    @brief Circular Queue Structure

    @trace #BRCM_SWREQ_CIRCQ_CONFIGURATION
*/
typedef struct _CIRCQ_Type {
    CIRCQ_IndexType  *qIndex;   /**< @brief Circular queue index */
    uint32_t    count;          /**< @brief Maximum number of objects in the queue */
    uint32_t    objSize;        /**< @brief Size of objects in the circular queue */
    char*       buffer;         /**< @brief Circular queue buffer */
    CIRCQ_OverwriteType overwrite;  /**< @brief Allow overwrite on queue overflow */
} CIRCQ_Type;

/** @brief Initialize Circular queue index

    @param[in]   cnt     Maximum number of objects in the circular queue

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION
*/
#define CIRCQ_INDEX_INIT(cnt)        \
{                                    \
    .qIndex = ((cnt) - 1UL) << 16UL, \
    .qLock = 0UL,                    \
}

/** @brief Define and initialize circular queue index

    This macro defines static circular queue index with "name" as its instance name.

    @param[in]   name    Name of this CIRCQ_Index instance.
    @param[in]   cnt     Maximum number of objects in the circular queue.

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION
*/
#define CIRCQ_INDEX_DEFINE(name, cnt)                   \
    static CIRCQ_IndexType (name) = CIRCQ_INDEX_INIT(cnt)

/** @brief Initialise a CIRCQ instance.

    Client is responsible for allocating global memory for cnt * objSz bytes.

    @param[in]   circQIdx    CIRCQ_Index instance.
    @param[in]   cnt         Maximum number of objects in the circular queue.
    @param[in]   objSz       Size of an object in the circular queue.
    @param[in]   ptr         Buffer pointer to cnt number of objects.
    @param[in]   ov          Enable circular queue overwrite

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION
*/
#define CIRCQ_INIT(circQIdx, cnt, objSz, ptr, ov)  \
{                                                  \
    .qIndex = (circQIdx),                          \
    .count = (cnt),                                \
    .objSize = (objSz),                            \
    .buffer = (char*)(ptr),                        \
    .overwrite = (ov),                             \
}

/** @brief Define and initialize a CIRCQ instance.

    Client is responsible for allocating global memory for cnt * objSz bytes.

    @param[in]   name        Name of CIRCQ instance.
    @param[in]   circQIdx    CIRCQ_Index instance.
    @param[in]   cnt         Maximum number of objects in the circular queue.
    @param[in]   objSz       Size of an object in the circular queue.
    @param[in]   ptr         Buffer pointer.
    @param[in]   ov          Enable circular queue overwrite.

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION
*/
#define CIRCQ_DEFINE(name, circQIdx, cnt, objSz, ptr, ov)      \
   const CIRCQ_Type (name) = CIRCQ_INIT((circQIdx), (cnt), (objSz), (ptr), (ov));

/** @brief Define and initialize a CIRCQ instance (v2).

    @param[in]   name        Name of CIRCQ instance.
    @param[in]   log2cnt     Log2 of max number of objects in the queue.
    @param[in]   objType     C Type of an object in the queue.
    @param[in]   bufName     Name of Queue Buffer.
    @param[in]   idxName     Name of CIRCQ_Index instance.
    @param[in]   ov          Enable circular queue overwrite.
    @param[in]   secName     Name of the section to place bufName and idxName.

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION
*/
#define CIRCQ_V2_DEFINE(name, log2cnt, objType, bufName, idxName, ov, secName)\
static COMP_SECTION(secName) CIRCQ_IndexType (idxName) =  {                   \
                        .qIndex = ((1UL<<(log2cnt)) - 1UL) << 16UL,           \
                        .qLock = 0UL, };                                      \
static COMP_SECTION(secName) objType (bufName)[1UL<<(log2cnt)];               \
static const CIRCQ_Type (name) =  {                                           \
                                .qIndex = &(idxName),                         \
                                .count = (1UL<<(log2cnt)),                    \
                                .objSize = sizeof(objType),                   \
                                .buffer = (char*)(bufName),                   \
                                .overwrite = (ov),};

/** @brief Reset buffer Q.

    This API initializes circular buffer queue.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]   aQ          Pointer to the queue structure

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Success.
    @retval      BCM_ERR_INVAL_PARAMS    aQ is NULL.

    @post None

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION

    @limitations None
*/
extern int32_t CIRCQ_Reset(const CIRCQ_Type *const aQ);

/** @brief check if queue is empty.

    This API check is queue is empty.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aQ                      Pointer to the queue structure.
    @param[out]  aIsEmpty                Set to 1 if Q is empty else set to 0.

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Success.
    @retval      BCM_ERR_INVAL_PARAMS    (aQ is NULL) or
                                         (aIsEmpty is NULL)

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS

    @limitations None
*/
extern int32_t CIRCQ_IsEmpty(const CIRCQ_Type *const aQ,
                          uint32_t *const aIsEmpty);

/** @brief check if queue is full.

    This API check is queue is full.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aQ                      Pointer to the queue structure.
    @param[out]  aIsFull                Set to 1 if Q is full else set to 0.

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Success.
    @retval      BCM_ERR_INVAL_PARAMS    (aQ is NULL) or
                                         (aIsFull is NULL)

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS

    @limitations None
*/
extern int32_t CIRCQ_IsFull(const CIRCQ_Type *const aQ,
                         uint32_t *const aIsFull);

/** @brief Get the filled size of the queue

    This API retrieves the filled size of the circular queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aQ     Pointer to the queue structure
    @param[out]  aSize  Pointer to hold filled size of the circular queue

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Success.
    @retval      BCM_ERR_INVAL_PARAMS    (aQ is NULL) or
                                         (aSize is NULL)

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS

    @limitations None
*/
extern int32_t CIRCQ_GetFilledSize(const CIRCQ_Type *const aQ,
                                   uint32_t *const aSize);

/** @brief Push objects to Queue.

    This API pushes objects to queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aQ              Pointer to the queue structure.
    @param[in]   aData           Data to push.
    @param[in]   aCnt            Number of objects to be pushed.

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Data is pushed to the queue successfully.
    @retval      BCM_ERR_NOMEM           Queue is full
    @retval      BCM_ERR_INVAL_PARAMS    (aQ is NULL ) or
                                         (aData is NULL)

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS

    @limitations If the input elements count is more than the size left in the
                 queue, it pushes the allowed number of items and discards the
                 rest. It returns BCM_ERR_OK in this case.
*/
extern int32_t CIRCQ_Push(const CIRCQ_Type *const aQ,
                         const char *const aData,
                         uint32_t aCnt);

/** @brief Pop objects to Queue.

    This API pops objects to queue.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aQ              Pointer to the queue structure.
    @param[in]   aData           Pointer to store the objects popped from queue.
    @param[in]   aCnt            Number of objects to be popped.

    Return values are documented in reverse-chronological order
    @retval      Number of objects actually popped

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS

    @limitations None
*/
extern uint32_t CIRCQ_Pop(const CIRCQ_Type *const aQ,
                         char *const aData,
                         uint32_t aCnt);

/** @brief peeks into objects from Queue.

    @behavior Sync, Non-reentrant

    @pre None

    This API peeks into objects in the circular queue. Read index is not altered.

    @param[in]       aQ          Pointer to the queue structure.
    @param[out]      aData       Pointer to store the objects peeked on queue.
    @param[in]       aCnt        Number of objects to be peeked on.

    Return values are documented in reverse-chronological order
    @retval      Number of objects actually peeked

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS

    @limitations None
*/
extern uint32_t CIRCQ_Peek(const CIRCQ_Type *const aQ,
                          char *const aData,
                          uint32_t aCnt);

#endif /* CIRC_QUEUE_H */
/** @} */
