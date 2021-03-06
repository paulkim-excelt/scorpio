/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Author: 2003 Francesco Bertozzi
 * CVS: $Id: ee_queue_TM.c,v 1.2 2005/07/17 13:58:36 pj Exp $
 */

#include "com/com/inc/ee_comprv.h"

/* This function insert a new monitored ipdu in minimum
 * interarrival time queue. 
 * The queue is organized in a relative way. 
 * It's forbidden to insert an already present element.
 ******
 *  IN
 *    num : the position number of ipdu in EE_com_ipdu[] 
 */
#ifndef __PRIVATE_COM_INSERT_TM__
void EE_com_insert_TM (MessageIdentifier ipdu_num, EE_UINT8 reload)
{
  EE_UINT8 cont;
  MessageIdentifier ipdu_prev, ipdu_curr;
  
  cont = reload;
  ipdu_curr = EE_com_sys.first_TM;
      
  if (ipdu_curr == EE_COM_NULL) 
  { /* The ipdu become the first into the queue */
    EE_com_sys.first_TM = ipdu_num;
    EE_com_ipdu_RAM[ipdu_num]->cont_TM = cont;
  }
  else 
    if (cont < EE_com_ipdu_RAM[ipdu_curr]->cont_TM)
    { /* The ipdu however become the first into the queue */
      EE_com_ipdu_RAM[ipdu_curr]->cont_TM -= cont;
      EE_com_ipdu_RAM[ipdu_num]->cont_TM = cont;
      EE_com_sys.first_TM = ipdu_num; 
    }
    else
    { /* I have to search the right place */
      do { 
        cont -= EE_com_ipdu_RAM[ipdu_curr]->cont_TM;
        ipdu_prev = ipdu_curr;       
        ipdu_curr = EE_com_ipdu_RAM[ipdu_curr]->next_TM;
      } while ((ipdu_curr != EE_COM_NULL) && 
            (cont > EE_com_ipdu_RAM[ipdu_curr]->cont_TM));
      
      if (ipdu_curr != EE_COM_NULL)
        EE_com_ipdu_RAM[ipdu_curr]->cont_TM -=  cont;
      
      EE_com_ipdu_RAM[ipdu_prev]->next_TM = ipdu_num;
    }
    
  EE_com_ipdu_RAM[ipdu_num]->next_TM = ipdu_curr;
          
}
#endif

/* This function is used to remove an element from the deadline 
 * monitor queue.
 * This element must be in the queue.
 ******
 *  IN
 *    num : the position number of ipdu in EE_com_ipdu[] 
 */
#ifndef __PRIVATE_COM_CANCEL_TM__
void EE_com_remove_TM (MessageIdentifier ipdu_num)
{
  MessageIdentifier ipdu_prev, ipdu_curr;
    
  ipdu_curr = EE_com_sys.first_TM;
        
  if (ipdu_curr == ipdu_num)
  { /* The element that I have to be extract is in the first position */
    EE_com_sys.first_TM = EE_com_ipdu_RAM[ipdu_num]->next_TM;
  }
  else
  {
    do {
      ipdu_prev = ipdu_curr;
      ipdu_curr = EE_com_ipdu_RAM[ipdu_curr]->next_TM;
    } while (EE_com_ipdu_ROM[ipdu_curr]->name != ipdu_num);
    
    EE_com_ipdu_RAM[ipdu_prev]->next_TM = 
          EE_com_ipdu_RAM[ipdu_curr]->next_TM;
  }
    
  EE_com_ipdu_RAM[EE_com_ipdu_RAM[ipdu_curr]->next_TM]->cont_TM += 
        EE_com_ipdu_RAM[ipdu_curr]->cont_TM;
   
}
#endif
