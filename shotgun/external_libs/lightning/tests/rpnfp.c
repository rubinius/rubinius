/******************************** -*- C -*- ****************************
 *
 *	Sample RPN calculator using GNU lightning and floating-point
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2004 Free Software Foundation, Inc.
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
#include <stdlib.h>
#include "lightning.h"

#ifdef JIT_FPR
static jit_insn codeBuffer[1024];

typedef double (*pdfd) (double);	/* Pointer to Double Function of Double */

static int regs[6] = { JIT_FPR0, JIT_FPR1, JIT_FPR2, JIT_FPR3, JIT_FPR4, JIT_FPR5 };

pdfd
compile_rpn (char *expr)
{
  pdfd fn;
  int ofs, sp = 1;

  fn = (pdfd) (jit_get_ip ().dptr);
  jit_leaf (1);
  ofs = jit_arg_d ();
  jit_getarg_d (regs[0], ofs);

  while (*expr)
    {
      char buf[32];
      int n;

      /* This scanner is much less advanced than the one in rpn.c.  */
      if (sscanf (expr, "%[0-9]%n", buf, &n))
	{
	  double d = strtod (buf, NULL);
	  expr += n - 1;
	  jit_movi_d (regs[sp], d);
	  sp++;
	}
      else if (*expr == '+')
	{
	  jit_addr_d (regs[sp - 2], regs[sp - 2], regs[sp - 1]);
	  sp--;
	}
      else if (*expr == '-')
	{
	  jit_subr_d (regs[sp - 2], regs[sp - 2], regs[sp - 1]);
	  sp--;
	}
      else if (*expr == '*')
	{
	  jit_mulr_d (regs[sp - 2], regs[sp - 2], regs[sp - 1]);
	  sp--;
	}
      else if (*expr == '/')
	{
	  jit_divr_d (regs[sp - 2], regs[sp - 2], regs[sp - 1]);
	  sp--;
	}
      else
	{
	  fprintf (stderr, "cannot compile: %s\n", expr);
	  abort ();
	}
      ++expr;
    }
  jit_movr_d (JIT_FPRET, regs[0]);
  jit_ret ();

  jit_flush_code ((char *) fn, jit_get_ip ().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble (stderr, (char *) fn, jit_get_ip ().ptr);
#endif
  return fn;
}


int
main ()
{
  pdfd c2f, f2c;
  double i;

  jit_set_ip (codeBuffer);
  c2f = compile_rpn ("9*5/32+");
  f2c = compile_rpn ("32-5*9/");

#ifndef LIGHTNING_CROSS
  printf ("\nC:");
  for (i = 0; i <= 100; i += 10)
    printf ("%6.1f", i);
  printf ("\nF:");
  for (i = 0; i <= 100; i += 10)
    printf ("%6.1f", c2f (i));
  printf ("\n");

  printf ("\nF:");
  for (i = 32; i <= 212; i += 10)
    printf ("%6.1f", i);
  printf ("\nC:");
  for (i = 32; i <= 212; i += 10)
    printf ("%6.1f", f2c (i));
  printf ("\n");
#endif
  return 0;
}
#else
int
main()
{       
	  return (77);
} 
#endif
