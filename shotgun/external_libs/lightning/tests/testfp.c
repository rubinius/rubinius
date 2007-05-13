/******************************** -*- C -*- ****************************
 *
 *	Floating-point miscellanea using GNU lightning
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2002, 2004 Free Software Foundation, Inc.
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
static double a;

void
int_test(char *what, jit_code code, double b, double c, double d, double e, double f)
{
  a = b; printf("%s\t\t%d ", what, code.iptr ());
  a = c; printf("%d ", code.iptr());
  a = d; printf("%d ", code.iptr());
  a = e; printf("%d ", code.iptr());
  a = f; printf("%d\n", code.iptr());
}

int
main()
{
  jit_code code;
  volatile double x = 0.0;
  code.ptr = (char *) codeBuffer;

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jit_ldi_d(JIT_FPR0, &a);
  jit_movi_d(JIT_FPR1, 0.0);
  jit_gtr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
  jit_ltr_d(JIT_R1, JIT_FPR0, JIT_FPR1);
  jit_subr_i(JIT_RET, JIT_R0, JIT_R1);	/* [greater] - [less] = -1/0/1 */
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  int_test("compare", code, -2.6, -2.4, 0, 2.4, 2.6);
#endif

#ifdef __GNUC__
  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jit_ldi_d(JIT_FPR0, &a);
  jit_movi_d(JIT_FPR1, 0.0);
  jit_eqr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
  jit_ltgtr_d(JIT_R1, JIT_FPR0, JIT_FPR1);
  jit_lshi_i(JIT_R1, JIT_R1, 1);
  jit_orr_i(JIT_RET, JIT_R0, JIT_R1);
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  int_test("nans", code, x / x, 1 / (a - a), -1 / (a - a), 0.0, -2.0);
#endif
#else
  printf ("nans\t\t1 3 3 0 3\n");
#endif

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jit_ldi_d(JIT_FPR0, &a);
  jit_truncr_d_i(JIT_RET, JIT_FPR0);
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  int_test("trunc", code, -2.6, -2.4, 0, 2.4, 2.6);
  int_test("trunc", code, -3, -2, 0, 2, 3);
#endif

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jit_ldi_d(JIT_FPR0, &a);
  jit_ceilr_d_i(JIT_RET, JIT_FPR0);
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  int_test("ceil", code, -2.6, -2.4, 0, 2.4, 2.6);
  int_test("ceil", code, -3, -2, 0, 2, 3);
#endif

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jit_ldi_d(JIT_FPR0, &a);
  jit_floorr_d_i(JIT_RET, JIT_FPR0);
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  int_test("floor", code, -2.6, -2.4, 0, 2.4, 2.6);
  int_test("floor", code, -3, -2, 0, 2, 3);
#endif

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jit_ldi_d(JIT_FPR0, &a);
  jit_roundr_d_i(JIT_RET, JIT_FPR0);
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  int_test("round", code, -2.6, -2.4, 0, 2.4, 2.6);
  int_test("round", code, -3, -2, 0, 2, 3);
#endif

#if 0 && defined JIT_TRANSCENDENTAL
  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jitfp_sti_d(&a,
    jitfp_log(
      jitfp_exp(jitfp_imm(1.0))
    )
  );
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
  code.vptr();
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  printf("log e = \t%f\n", a);
#endif

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jitfp_sti_d(&a,
    jitfp_atn(
      jitfp_imm(1.732050807657)
    )
  );
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
  code.vptr();
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  printf("pi =         \t%f\n", a*3);
#endif

  jit_set_ip(codeBuffer);
  jit_leaf(0);
  jitfp_sti_d(&a,
    jitfp_tan(
      jitfp_ldi_d(&a)
    )
  );
  jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);
  code.vptr();
#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, (char *)codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  printf("tan^2 pi/3 = \t%f\n", a*a);
#endif

#endif /* JIT_TRANSCEDENTAL */

  return (0);
}
#else
int
main()
{
  return (77);
}
#endif
