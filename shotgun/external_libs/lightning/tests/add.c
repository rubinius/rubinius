/******************************** -*- C -*- ****************************
 *
 *	Sample call for using arguments in GNU lightning
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

typedef int (*pifii)(int, int);	/* Pointer to Int Function of Int, Int */

int main()
{
  pifii myFunction= (pifii) (jit_set_ip(codeBuffer).iptr);
  int	ofs;				/* offset of the argument */

  jit_leaf(2);
  ofs = jit_arg_i();
  jit_getarg_i(JIT_R0, ofs);
  ofs = jit_arg_i();
  jit_getarg_i(JIT_R1, ofs);
  jit_addr_i(JIT_RET, JIT_R0, JIT_R1);
  jit_ret();
  jit_flush_code(codeBuffer, jit_get_ip().ptr);

  /* call the generated code, passing its size as argument */
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  printf("%d + %d = %d\n", 5, 4, myFunction(5, 4));
#endif
  return 0;
}
