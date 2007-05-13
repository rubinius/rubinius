/******************************** -*- C -*- ****************************
 *
 *	Floating-point function invocation using GNU lightning
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
#include "lightning.h"

#ifdef JIT_FPR
static jit_insn codeBuffer[300];

typedef int (*intFunc) (int, int);
typedef double (*dblFunc) (double, double);
typedef float (*floatFunc) (float, float);


/* Generate a function that computes and returns the sum of 
   its two double arguments (return an int)
   i.e., double foo(double x,double y) { return x + y;} */
dblFunc
makeDblFunc ()
{
  dblFunc retVal;
  int dbl1, dbl2;
  retVal = (dblFunc) jit_get_ip ().iptr;
  jit_prolog (2);
  dbl1 = jit_arg_d ();
  dbl2 = jit_arg_d ();
  jit_getarg_d (JIT_FPR0, dbl1);
  jit_getarg_d (JIT_FPR1, dbl2);
  jit_addr_d (JIT_FPR0, JIT_FPR0, JIT_FPR1);
  jit_movr_d (JIT_FPRET, JIT_FPR0);
  jit_ret ();
  jit_flush_code ((char *) retVal, jit_get_ip ().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble (stderr, (char *) retVal, jit_get_ip ().ptr);
#endif

  return retVal;
}


/* Generate a function that computes and returns the sum of 
   its two double arguments (return an int)
   i.e., double foo(double x,double y) { return x + y;} */
floatFunc
makeFloatFunc ()
{
  floatFunc retVal;
  int dbl1, dbl2;
  retVal = (floatFunc) jit_get_ip ().iptr;
  jit_prolog (2);
  dbl1 = jit_arg_f ();
  dbl2 = jit_arg_f ();
  jit_getarg_f (JIT_FPR0, dbl1);
  jit_getarg_f (JIT_FPR1, dbl2);
  jit_addr_f (JIT_FPR0, JIT_FPR0, JIT_FPR1);
  jit_movr_f (JIT_FPRET, JIT_FPR0);
  jit_ret ();
  jit_flush_code ((char *) retVal, jit_get_ip ().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble (stderr, (char *) retVal, jit_get_ip ().ptr);
#endif

  return retVal;
}

dblFunc
makeCallFunc (dblFunc theFunc)
{
  dblFunc retVal;
  int dbl1, dbl2;
  retVal = (dblFunc) jit_get_ip ().iptr;
  jit_prolog (2);
  dbl1 = jit_arg_d ();
  dbl2 = jit_arg_d ();

  jit_prepare_d (2);
  jit_getarg_d (JIT_FPR0, dbl1);
  jit_getarg_d (JIT_FPR1, dbl2);
  jit_mulr_d (JIT_FPR1, JIT_FPR1, JIT_FPR0);
  jit_pusharg_d (JIT_FPR1);
  jit_pusharg_d (JIT_FPR0);
  jit_finish ((void *) theFunc);
  jit_retval_d (JIT_FPRET);
  jit_ret ();
  jit_flush_code ((char *) retVal, jit_get_ip ().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble (stderr, (char *) retVal, jit_get_ip ().ptr);
#endif

  return retVal;
}

floatFunc
makeCallFloatFunc (floatFunc theFunc)
{
  floatFunc retVal;
  int dbl1, dbl2;
  retVal = (floatFunc) jit_get_ip ().iptr;
  jit_prolog (2);
  dbl1 = jit_arg_f ();
  dbl2 = jit_arg_f ();

  jit_prepare_f (2);
  jit_getarg_f (JIT_FPR0, dbl1);
  jit_getarg_f (JIT_FPR1, dbl2);
  jit_mulr_f (JIT_FPR1, JIT_FPR1, JIT_FPR0);
  jit_pusharg_f (JIT_FPR1);
  jit_pusharg_f (JIT_FPR0);
  jit_finish ((void *) theFunc);
  jit_retval_f (JIT_FPRET);
  jit_ret ();
  jit_flush_code ((char *) retVal, jit_get_ip ().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble (stderr, (char *) retVal, jit_get_ip ().ptr);
#endif

  return retVal;
}


int
main (int argc, char *argv[])
{
  dblFunc myFunc2, callIt1;
  floatFunc myFunc3, callIt2;
  double y;
  float a, b, z;

  jit_set_ip (codeBuffer);
  myFunc2 = makeDblFunc ();
  myFunc3 = makeFloatFunc ();
  callIt1 = makeCallFunc (myFunc2);
  callIt2 = makeCallFloatFunc (myFunc3);
#ifndef LIGHTNING_CROSS
  y = callIt1 (10.5, 15.3);
  a = 1.5;
  b = 10.5;
  z = callIt2 (a, b);
  printf ("result is %.5g\t %.5g\n", y, z);
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
