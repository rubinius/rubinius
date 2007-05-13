/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer inline functions (PowerPC)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2004, 2006 Free Software Foundation, Inc.
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



#ifndef __lightning_funcs_h
#define __lightning_funcs_h

#include <string.h>

#if !defined(__GNUC__) && !defined(__GNUG__)
#error Go get GNU C, I do not know how to flush the cache
#error with this compiler.
#else
static void
jit_flush_code(void *start, void *end)
{
#ifndef LIGHTNING_CROSS
  register char *ddest, *idest;

  static int cache_line_size;
  if (cache_line_size == 0) {
    char buffer[8192];
    int i, probe;

    /* Find out the size of a cache line by zeroing one */
    memset(buffer, 0xFF, 8192);
    __asm__ __volatile__ ("dcbz 0,%0" : : "r"(buffer + 4096));

    /* Probe for the beginning of the cache line. */
    for(i = 0, probe = 4096; probe; probe >>= 1)
      if (buffer[i | probe] != 0x00)
        i |= probe;

    /* i is now just before the start of the cache line */
    i++;
    for(cache_line_size = 1; i + cache_line_size < 8192; cache_line_size <<= 1)
      if (buffer[i + cache_line_size] != 0x00)
        break;
  }

  start -= ((long) start) & (cache_line_size - 1);
  end -= ((long) end) & (cache_line_size - 1);

  /* Force data cache write-backs */
  for (ddest = (char *) start; ddest <= (char *) end; ddest += cache_line_size) {
    __asm__ __volatile__ ("dcbst 0,%0" : : "r"(ddest));
  }
  __asm__ __volatile__ ("sync" : : );

  /* Now invalidate the instruction cache */
  for (idest = (char *) start; idest <= (char *) end; idest += cache_line_size) {
    __asm__ __volatile__ ("icbi 0,%0" : : "r"(idest));
  }
  __asm__ __volatile__ ("isync" : : );
#endif /* !LIGHTNING_CROSS */
}
#endif /* __GNUC__ || __GNUG__ */

#define _jit (*jit)

static void
_jit_epilog(jit_state *jit)
{
  int n = _jitl.nbArgs;
  int first_saved_reg = JIT_AUX - n;
  int num_saved_regs = 32 - first_saved_reg;
  int frame_size = (_jitl.frame_size + 15) & ~15;

#ifdef __APPLE__
  LWZrm(0, frame_size + 8, 1);		/* lwz   r0, x+8(r1)  (ret.addr.)  */
#else
  LWZrm(0, frame_size + 4, 1);		/* lwz   r0, x+4(r1)  (ret.addr.)  */
#endif
  MTLRr(0);				/* mtspr LR, r0			   */

  LMWrm(first_saved_reg, 24 + 32, 1);	/* lmw   rI, ofs(r1)		   */
  ADDIrri(1, 1, frame_size);		/* addi  r1, r1, x		   */
  BLR();				/* blr				   */
}

/* Emit a prolog for a function.
  
   The +32 in frame_size computation is to accound for the parameter area of
   a function frame. 

   On PPC the frame must have space to host the arguments of any callee.
   However, as it currently stands, the argument to jit_trampoline (n) is
   the number of arguments of the caller we generate. Therefore, the
   callee can overwrite a part of the stack (saved register area) when it
   flushes its own parameter on the stack. The addition of a constant 
   offset = 32 is enough to hold eight 4 bytes arguments.  This is less
   than perfect but is a reasonable work around for now. 
   Better solution must be investigated.  */
static void
_jit_prolog(jit_state *jit, int n)
{
  int orig_frame_size, frame_size;
  int i;
  int first_saved_reg = JIT_AUX - n;
  int num_saved_regs = 32 - first_saved_reg;

  _jitl.nextarg_geti = JIT_AUX - 1;
  _jitl.nextarg_getd = 1;
  _jitl.nbArgs = n;

  MFLRr(0);

#ifdef __APPLE__
  STWrm(0, 8, 1);			/* stw   r0, 8(r1)	   */
#else
  STWrm(0, 4, 1);			/* stw   r0, 4(r1)	   */
#endif

  /* 0..55 -> frame data
     56..frame_size -> saved registers

     The STMW instruction is patched by jit_allocai, thus leaving
     the space for the allocai above the 56 bytes.  jit_allocai is
     also able to reuse the slack space needed to keep the stack
     quadword-aligned.  */

  _jitl.frame_size = 24 + 32 + num_saved_regs * 4;	/* r27..r31 + args */

  /* The stack must be quad-word aligned.  */
  frame_size = (_jitl.frame_size + 15) & ~15;
  _jitl.slack = frame_size - _jitl.frame_size;
  _jitl.stwu = _jit.x.pc;
  STWUrm(1, -frame_size, 1);		/* stwu  r1, -x(r1)	   */

  STMWrm(first_saved_reg, 24 + 32, 1);		/* stmw  rI, ofs(r1)	   */
  for (i = 0; i < n; i++)
    MRrr(JIT_AUX-1-i, 3+i);		/* save parameters below r24	   */
}

#undef _jit

#endif /* __lightning_funcs_h */
