/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer inline functions (Sparc)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
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

#if !defined(__GNUC__) && !defined(__GNUG__)
#error Go get GNU C, I do not know how to flush the cache
#error with this compiler.
#else
/* Why doesn't this compile?!?
 * static void
 * jit_flush_code(start, end)
 *      void 	*start;
 *      void	*end;
 */

static void
jit_flush_code(void* start, void* end)
{
#ifndef LIGHTNING_CROSS
  register char *dest;

  __asm__ __volatile__ ("stbar");
  for (dest = (char *)start; dest <= (char *)end; dest += 4) {
    __asm__ __volatile__ ("flush %0"::"r"(dest));
  }

  /* [SPARC Architecture Manual v8, page 139, implementation note #5] */
  __asm__ __volatile__ ("nop; nop; nop; nop; nop");
#endif
}
#endif

#endif /* __lightning_core_h */
