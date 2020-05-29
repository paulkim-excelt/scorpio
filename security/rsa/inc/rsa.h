/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
/**
 *  @file rsa.h
 *  @brief rsa API header
 *
 *  This file contains the interface function declarations for rsa
 *
 */
#ifndef RSA_H
#define RSA_H

#include <stdint.h>

#define RSA_S_MODULUS       2048UL                          /* Bits in a key modulus     */
#define RSA_S_MODULUS8      (RSA_S_MODULUS / 8UL)                  /* bytes in a key modulus     */
#define SHA256_S_DIGEST     256UL                           /* Bits in a sha-256 digest  */
#define SHA256_S_DIGEST8    (uint32_t)(SHA256_S_DIGEST/8UL)  /* Bytes in a sha-256 digest */

typedef uint32_t RSA_Status;
#define RSA_SUCCESS           0UL  /* The function has completed successfully */
#define RSA_ERR_BADMESLEN     1UL  /* The expected message is larger than we can sign using SHA-256 */
#define RSA_ERR_BADSIGLEN     2UL  /* The signature block provided is the wrong size */
#define RSA_ERR_BADDIGLEN     3UL  /* The SHA-256 digest we were passed was the wrong size */
#define RSA_ERR_BADPUBLEN     4UL  /* The public key modulus provided is the wrong size */
#define RSA_ERR_BADSEEDLEN    5UL  /* The seed length is not the length of a SHA-256 digest */
#define RSA_ERR_BADMASKLEN    6UL  /* We cannot generate a mask of the requested length */
#define RSA_ERR_TSHORT        7UL  /* The message representative is too short to hold the required information  */
#define RSA_ERR_TENDNOTBC     8UL  /* The message representative did not end with the required byte 0xbc */
#define RSA_ERR_BADLEADER     9UL  /* The recovered message MB' is not null (but should have been) */
#define RSA_ERR_MISMATCH      10UL /* The computed hash does not match the recovered hash */


/** @brief Verify the RSASSA-PSS signature for an object
 *
 * @details Given a signature, the modulus of the key alleged to have been used for
 *          signing and the object to which the signature is alleged to correspond, check
 *          that the signature is indeed the result of signing the object with the key
 *          using the RSASSA-PSS algorithm. We expect the signature and key modulus to be
 *          SEC_S_SIGNATURE and SEC_S_MODULUS bits respectively.Some 2048 bytes of stack
 *          memory shall be used for computation.
 *
 * @param [in] aSignature     the signature
 * @param [in] aSiglen        the length of the signature in bytes
 * @param [in] aPublicKey     the modulus of the key used to create the signature
 * @param [in] aPubLen        the length of the modulus in bytes
 * @param [in] aMsg            the non-recoverable (appended) object that was signed
 * @param [in] aMsgLen         the length of the signed object in bytes
 * @param [out] aStatus       RSA_SUCCESS if the signature was verified or the error
 *                            status mentioned in RSA_Status based on the error
 * @retval                    BCM_ERR_OK for success: BCM_ERR_AUTH_FAILED for errors
 */

extern int32_t BCM_RSAVerify(const uint32_t* const aSignature, uint32_t aSiglen,
                      const  uint32_t* const aPublicKey, uint32_t aPubLen,
                      const uint8_t* const aMsg, uint32_t aMsgLen,
                      RSA_Status * const aStatus);

extern RSA_Status Sec_verifyDigest(const uint32_t* const aSignature, uint32_t aSigLen, const uint32_t* const aPublicKey, uint32_t aPubLen, const uint8_t* const H2, uint32_t H2len);

#endif /* RSA_H */
