/******************************** -*- C -*- ****************************
 *
 *	Simple example of recursion and forward references
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2004 Free Software Foundation, Inc.
 * Written by Paolo Bonzini and Laurent Michel.
 *
 * This file is part of GNU lightning.
 *
 * GNU lightning is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 * 
 * GNU lightning is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GNU lightning; see the file COPYING.LESSER; if not, write to the
 * Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ***********************************************************************/

#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lightning.h"

static jit_insn codeBuffer[1024];

typedef int (*pifi)(int);	/* Pointer to Int Function of Int */

int main()
{
  pifi      nfibs = (pifi) (jit_set_ip(codeBuffer).iptr);
  int	    in;				/* offset of the argument */
  jit_insn  *ref;			/* to patch the forward reference */
  jit_insn  *mref;                     /* ref of move to backpatch */
  jit_insn  *tp;                       /* location to patch */

        jit_prolog   (1);
  in =  jit_arg_ui   ();
        jit_getarg_ui(JIT_V0, in);              /* V0 = n */
  mref= jit_movi_p(JIT_V2,jit_forward ());      /* Generate a dumb movi */
        jit_jmpr(JIT_V2);
        /* generate some dump filler that will never be executed!*/
        jit_addi_ui(JIT_V0,JIT_V0,1);
        jit_addi_ui(JIT_V0,JIT_V0,1);        
        jit_addi_ui(JIT_V0,JIT_V0,1);        
        jit_addi_ui(JIT_V0,JIT_V0,1);        
  tp  = jit_get_label ();
  ref = jit_blti_ui  (jit_forward(), JIT_V0, 2);
        jit_subi_ui  (JIT_V1, JIT_V0, 1);       /* V1 = n-1 */
        jit_subi_ui  (JIT_V2, JIT_V0, 2);       /* V2 = n-2 */
        jit_prepare  (1);
          jit_pusharg_ui(JIT_V1);
        jit_finish(nfibs);
        jit_retval(JIT_V1);                     /* V1 = nfibs(n-1) */
        jit_prepare(1);
          jit_pusharg_ui(JIT_V2);
        jit_finish(nfibs);
        jit_retval(JIT_V2);                     /* V2 = nfibs(n-2) */
        jit_addi_ui(JIT_V1,  JIT_V1,  1);
        jit_addr_ui(JIT_RET, JIT_V1, JIT_V2);   /* RET = V1 + V2 + 1 */
        jit_ret();

  jit_patch(ref);                               /* patch jump */
        jit_movi_i(JIT_RET, 1);                 /* RET = 1 */
        jit_ret();

  jit_patch_movi(mref,tp);                      /* Ok. Do the back-patching */

  /* call the generated code, passing 32 as an argument */
  jit_flush_code(codeBuffer, jit_get_ip().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  printf("nfibs(%d) = %d\n", 32, nfibs(32));
#endif
  return 0;
}
