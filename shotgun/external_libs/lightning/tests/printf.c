/******************************** -*- C -*- ****************************
 *
 *	Sample call to printf using GNU lightning
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "lightning.h"

static char codeBuffer[1024];

typedef void (*pvfi)(int);	/* Pointer to Void Function of Int */

static void display_message (char *msg, int value)
{
  printf (msg, value);
}

int main()
{
  pvfi		myFunction;		/* ptr to generated code */
  char		*start, *end;		/* a couple of labels */
  int		ofs;			/* to get the argument */

  myFunction = (pvfi) (jit_set_ip(codeBuffer).vptr);
  start = jit_get_ip().ptr;
  jit_prolog(1);
  ofs = jit_arg_i();
  jit_movi_p(JIT_R0, "looks like %d bytes sufficed\n");
  jit_getarg_i(JIT_R1, ofs);
  jit_prepare_i(2);
    jit_pusharg_i(JIT_R1);		/* push in reverse order */
    jit_pusharg_p(JIT_R0);
  jit_finish(display_message);
  jit_ret();
  end = jit_get_ip().ptr;

  jit_flush_code(codeBuffer, end);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, codeBuffer, end);
#endif
#ifndef LIGHTNING_CROSS
  /* call the generated code, passing its size as argument */
  myFunction(sizeof(codeBuffer));
#endif
  return 0;
}
