/******************************** -*- C -*- ****************************
 *
 *	lightning disassembling support
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

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "dis-asm.h"

#define LIGHTNING_I386_32 0
#define LIGHTNING_I386_64 1
#define LIGHTNING_PPC 2
#define LIGHTNING_SPARC 3

void disassemble(stream, from, to)
     FILE *stream;
     char *from, *to;
{
  disassemble_info info;
  bfd_vma pc = (bfd_vma) from;
  bfd_vma end = (bfd_vma) to;

  INIT_DISASSEMBLE_INFO(info, stream, fprintf);
  info.buffer = NULL;
  info.buffer_vma = 0;
  info.buffer_length = end;

  while (pc < end) {
    fprintf_vma(stream, pc);
    putc('\t', stream);
#if LIGHTNING_TARGET == LIGHTNING_I386_32
    pc += print_insn_i386(pc, &info);
#elif LIGHTNING_TARGET == LIGHTNING_PPC
    pc += print_insn_big_powerpc(pc, &info);
#elif LIGHTNING_TARGET == LIGHTNING_SPARC
    pc += print_insn_sparc(pc, &info);
#else
#   error disassembling not yet supported for your architecture
#endif
    putc('\n', stream);
  }
}

/* Panic on failing malloc */
PTR
xmalloc(size)
  size_t size;
{
  PTR ret = malloc(size ? size : 1);
  if (!ret) {
    fprintf(stderr, "Couldn't allocate memory\n");
    exit(1);
  }
  return ret;
}

