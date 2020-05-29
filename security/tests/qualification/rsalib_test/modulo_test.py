#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2018 Broadcom Limited.
# This program is the proprietary software of Broadcom Limited
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#
import time
import sys
import numpy as np

def getNibbleCount(x):
    lenx = np.floor(np.log2(x)/4) + 1
    lenx[np.where(lenx == -np.inf)] = 0
    return lenx.astype(int)

def compare(x,y,lenx,leny):
    out = np.zeros(x.shape)
    diff = np.minimum(lenx, leny)
    tmpx = np.right_shift(x, (lenx - diff)*4)
    tmpy = np.right_shift(y, (leny - diff)*4)
    out[np.where(tmpx > tmpy)] = 1
    out[np.where(tmpx < tmpy)] = -1
    return out

def get_modulus(a1,b,verbose=False):
    a = a1.copy()
    lena = getNibbleCount(a)
    lenb = getNibbleCount(np.array([b]))
    if verbose:
        print 'Get Modulus :A'
        print a
        print 'Get Modulus :B'
        print b
    while (lena > lenb).any():
        prev_out_while = a.copy()
        if verbose:
            print 'First Loop Begin :A'
            print a
            print lena
            print lenb
        if (a < 0).any():
            print 'a has become negative!'
            sys.exit()
        flags = ((compare(a,b,lena,lenb) >= 0) & (lena > lenb))
        while flags.any():
            #subtract
            if verbose:
                print 'Second while:A'
                print a
                print 'Second while:flags'
                print flags
                print 'Second while:lena'
                print lena
                print 'Second while:lenb'
                print lenb
            tempDiff = (lena - lenb)
            tempDiff[tempDiff < 0] = 0
            tmpb = (b *(16 ** tempDiff))
            tmpb[np.where(np.logical_not(flags))] = 0
            if verbose:
                print 'Second while before subtract:A'
                print a
                print tmpb
            a = a - tmpb
            lena = getNibbleCount(a)
            flags = ((compare(a,b,lena,lenb) >= 0) & (lena > lenb))
            if verbose:
                print 'Second while after subtract:A'
                print a
        if (lena <= lenb).all():
            break
        q = np.right_shift(a, (lena - 2)*4)/np.right_shift(b, (lenb - 1)*4)

        flags = np.array(np.right_shift(a, (lena - 1)*4) == np.right_shift(b, (lenb - 1)*4)) & np.array(lena > lenb)
        q[np.where(flags)] = 15

        if (np.array(lena > lenb) & np.array(q>15)).any():
            print 'failed'
            sys.exit(1)

        tmp = q * b
        lentmp = getNibbleCount(tmp)
        if verbose:
            print 'Q'
            print q
            print 'condition'
            print np.array(compare(tmp,a,lentmp,lena) > 0) & np.array(q > 1)
        while (np.array(compare(tmp,a,lentmp,lena) > 0) & np.array(q > 1)).any():
            flag = (np.array(compare(tmp,a,lentmp,lena) > 0) & np.array(q > 1))
            if verbose:
                print 'Inside Third loop Q:'
                print q
                print 'Inside Third loop conditions:'
                print flag
            tmp[np.where(flag)] -= b
            lentmp = getNibbleCount(tmp)
            q[np.where(flag)] -= 1

        diff = lena - lentmp
        diff[diff < 0] = 0
        tmp1 = np.left_shift(tmp ,diff * 4)
        if verbose:
            print 'tmp1'
            print tmp1
        tmpflag = a >= tmp1
        a[np.where(tmpflag)] = a[np.where(tmpflag)] - tmp1[np.where(tmpflag)]

        diff = lena - lentmp - 1
        diff[diff < 0] = 0
        tmp1 = np.left_shift(tmp ,diff * 4)
        if verbose:
            print 'tmp'
            print tmp1
        tmpflag1 = a >= tmp1
        a[np.where(tmpflag1 & ~tmpflag)] = a[np.where(tmpflag1 & ~tmpflag)] - tmp1[np.where(tmpflag1 & ~tmpflag)]
        lena = getNibbleCount(a)
        if verbose:
            print 'First loop end :A'
            print a
            print b
        if (a == prev_out_while).all():
            break
    while (a >= b).any():
        flag = (a >= b)
        a[flag] = a[flag] - b
    return a

np.set_printoptions(formatter={'int':hex})
j = np.array(range(0xFFFFFF))
j = 0xFFFFFF - j
for i in range(0xFFF,0x800-1,-1):
    t = get_modulus(j,i,False)
    print 'completed ' + str(j[0]) + ' with  ' + str(i)
    if (t != j%i).any():
        new_j = np.extract(t != j%i,j)
        print new_j.shape
        t = get_modulus(new_j[0:1],i,True)
        print 'failed'
        sys.exit()
