/* Main header file for the bfd library -- portable access to object files.
   Copyright 1990, 91, 92, 93, 94, 95, 1996 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.
(Simplified and modified for GNU lightning)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* bfd.h -- The only header file required by users of the bfd library 

The bfd.h file is generated from bfd-in.h and various .c files; if you
change it, your changes will probably be lost.

All the prototypes and definitions following the comment "THE FOLLOWING
IS EXTRACTED FROM THE SOURCE" are extracted from the source files for
BFD.  If you change it, someone oneday will extract it from the source
again, and your changes will be lost.  To save yourself from this bind,
change the definitions in the source in the bfd directory.  Type "make
docs" and then "make headers" in that directory, and magically this file
will change to reflect your changes.

If you don't have the tools to perform the extraction, then you are
safe from someone on your system trampling over your header files.
You should still maintain the equivalence between the source and this
file though; every change you make to the .c file should be reflected
here.  */

#ifndef __BFD_H_SEEN__
#define __BFD_H_SEEN__

#include "ansidecl.h"

#ifndef INLINE
#if __GNUC__ >= 2
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

/* To squelch erroneous compiler warnings ("illegal pointer
   combination") from the SVR3 compiler, we would like to typedef
   boolean to int (it doesn't like functions which return boolean.
   Making sure they are never implicitly declared to return int
   doesn't seem to help).  But this file is not configured based on
   the host.  */
/* General rules: functions which are boolean return true on success
   and false on failure (unless they're a predicate).   -- bfd.doc */
/* I'm sure this is going to break something and someone is going to
   force me to change it. */
/* typedef enum boolean {false, true} boolean; */
/* Yup, SVR4 has a "typedef enum boolean" in <sys/types.h>  -fnf */
/* It gets worse if the host also defines a true/false enum... -sts */
/* And even worse if your compiler has built-in boolean types... -law */
#if defined (__GNUG__) && (__GNUC_MINOR__ > 5)
#define TRUE_FALSE_ALREADY_DEFINED
#endif
#ifdef MPW
/* Pre-emptive strike - get the file with the enum. */
#include <Types.h>
#define TRUE_FALSE_ALREADY_DEFINED
#endif /* MPW */
#ifndef TRUE_FALSE_ALREADY_DEFINED
typedef enum bfd_boolean {false, true} boolean;
#define BFD_TRUE_FALSE
#else
/* Use enum names that will appear nowhere else.  */
typedef enum bfd_boolean {bfd_fffalse, bfd_tttrue} boolean;
#endif

/* A pointer to a position in a file.  */
/* FIXME:  This should be using off_t from <sys/types.h>.
   For now, try to avoid breaking stuff by not including <sys/types.h> here.
   This will break on systems with 64-bit file offsets (e.g. 4.4BSD).
   Probably the best long-term answer is to avoid using file_ptr AND off_t 
   in this header file, and to handle this in the BFD implementation
   rather than in its interface.  */
/* typedef off_t	file_ptr; */
typedef long int file_ptr;

/* Represent a target address.  Also used as a generic unsigned type
   which is guaranteed to be big enough to hold any arithmetic types
   we need to deal with.  */
typedef unsigned long bfd_vma;

/* A generic signed type which is guaranteed to be big enough to hold any
   arithmetic types we need to deal with.  Can be assumed to be compatible
   with bfd_vma in the same way that signed and unsigned ints are compatible
   (as parameters, in assignment, etc).  */
typedef long bfd_signed_vma;

typedef unsigned long symvalue;
typedef unsigned long bfd_size_type;

/* Print a bfd_vma x on stream s.  */
#define fprintf_vma(s,x) fprintf(s, "%08lx", x)
#define sprintf_vma(s,x) sprintf(s, "%08lx", x)
#define printf_vma(x) fprintf_vma(stdout,x)

typedef unsigned int flagword;	/* 32 bits of flags */
typedef unsigned char bfd_byte;

enum bfd_architecture 
{
  bfd_arch_unknown,    /* File arch not known */
  bfd_arch_obscure,    /* Arch known, not one of these */
  bfd_arch_m68k,       /* Motorola 68xxx */
  bfd_arch_vax,        /* DEC Vax */   
  bfd_arch_i960,       /* Intel 960 */
     /* The order of the following is important.
       lower number indicates a machine type that 
       only accepts a subset of the instructions
       available to machines with higher numbers.
       The exception is the "ca", which is
       incompatible with all other machines except 
       "core". */

#define bfd_mach_i960_core      1
#define bfd_mach_i960_ka_sa     2
#define bfd_mach_i960_kb_sb     3
#define bfd_mach_i960_mc        4
#define bfd_mach_i960_xa        5
#define bfd_mach_i960_ca        6
#define bfd_mach_i960_jx	 7
#define bfd_mach_i960_hx        8

  bfd_arch_a29k,       /* AMD 29000 */
  bfd_arch_sparc,      /* SPARC */
#define bfd_mach_sparc			1
 /* The difference between v8plus and v9 is that v9 is a true 64 bit env.  */
#define bfd_mach_sparc_v8plus		2
#define bfd_mach_sparc_v8plusa		3  /* with ultrasparc add'ns */
#define bfd_mach_sparc_v9		4
#define bfd_mach_sparc_v9a		5  /* with ultrasparc add'ns */
 /* Nonzero if MACH has the v9 instruction set.  */
#define bfd_mach_sparc_v9_p(mach) ((mach) != bfd_mach_sparc)
  bfd_arch_mips,       /* MIPS Rxxxx */
  bfd_arch_i386,       /* Intel 386 */
  bfd_arch_we32k,      /* AT&T WE32xxx */
  bfd_arch_tahoe,      /* CCI/Harris Tahoe */
  bfd_arch_i860,       /* Intel 860 */
  bfd_arch_romp,       /* IBM ROMP PC/RT */
  bfd_arch_alliant,    /* Alliant */
  bfd_arch_convex,     /* Convex */
  bfd_arch_m88k,       /* Motorola 88xxx */
  bfd_arch_pyramid,    /* Pyramid Technology */
  bfd_arch_h8300,      /* Hitachi H8/300 */
#define bfd_mach_h8300   1
#define bfd_mach_h8300h  2
  bfd_arch_powerpc,    /* PowerPC */
  bfd_arch_rs6000,     /* IBM RS/6000 */
  bfd_arch_hppa,       /* HP PA RISC */
  bfd_arch_z8k,        /* Zilog Z8000 */
#define bfd_mach_z8001		1
#define bfd_mach_z8002		2
  bfd_arch_h8500,      /* Hitachi H8/500 */
  bfd_arch_sh,         /* Hitachi SH */
  bfd_arch_alpha,      /* Dec Alpha */
  bfd_arch_arm,        /* Advanced Risc Machines ARM */
  bfd_arch_ns32k,      /* National Semiconductors ns32000 */
  bfd_arch_w65,        /* WDC 65816 */
  bfd_arch_last
  };

enum bfd_endian { BFD_ENDIAN_UNKNOWN };

typedef struct bfd bfd;

#define bfd_getb32(x) *((int *)(x))
#define bfd_getl32(x) *((int *)(x))

#endif
