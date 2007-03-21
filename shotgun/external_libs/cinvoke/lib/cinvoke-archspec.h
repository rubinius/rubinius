/*
C/Invoke Source Code File

Copyright (c) 2006 Will Weisser

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
   3. The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _ARCH_GCC_PPC_OSX_H
#define _ARCH_GCC_PPC_OSX_H

#include <errno.h>

typedef struct _ArchLibrary {
	void *dl;
} ArchLibrary;

typedef struct _ArchRetValue {
	int ivallo;
	int ivalhi;
	double dval;
} ArchRetValue;

typedef struct _ArchRegParms {
	int r3;
	int r4;
	int r5;
	int r6;
	int r7;
	int r8;
	int r9;
	int r10;
	double f1;
	double f2;
	double f3;
	double f4;
	double f5;
	double f6;
	double f7;
	double f8;
	double f9;
	double f10;
	double f11;
	double f12;
	double f13;
} ArchRegParms;

typedef char cinv_int8_t;
typedef short cinv_int16_t;
typedef int cinv_int32_t;
typedef long long cinv_int64_t;

#define CINV_E_NOMEM ((cinv_int32_t)ENOMEM)
#define CINV_S_NOMEM (strerror(ENOMEM))
#define CINV_NOMEM_NEEDSFREE 0
#define CINV_E_INVAL ((cinv_int32_t)EINVAL)

#define CINV_CC_DEFAULT CINV_CC_CDECL
#define CINV_T_2BYTE CINV_T_SHORT
#define CINV_T_4BYTE CINV_T_INT
#define CINV_T_8BYTE CINV_T_EXTRALONG

/////////////////////////////////////
// macros
/////////////////////////////////////
#define ARCH_SAVE_REGPARMS(regparms) \
	__asm__("stw r2, %0\n \
		stw r4, %1\n \
		stw r5, %2\n \
		stw r6, %3\n \
		stw r7, %4\n \
		stw r8, %5\n \
		stw r9, %6\n \
		stw r10, %7\n \
		stfd f1, %8\n \
		stfd f2, %9\n \
		stfd f3, %10\n \
		stfd f4, %11\n \
		stfd f5, %12\n \
		stfd f6, %13\n \
		stfd f7, %14\n \
		stfd f8, %15\n \
		stfd f9, %16\n \
		stfd f10, %17\n \
		stfd f11, %18\n \
		stfd f12, %19\n \
		stfd f13, %20\n" : \
			"=m" ((regparms).r3), \
			"=m" ((regparms).r4), \
			"=m" ((regparms).r5), \
			"=m" ((regparms).r6), \
			"=m" ((regparms).r7), \
			"=m" ((regparms).r8), \
			"=m" ((regparms).r9), \
			"=m" ((regparms).r10), \
			"=m" ((regparms).f1), \
			"=m" ((regparms).f2), \
			"=m" ((regparms).f3), \
			"=m" ((regparms).f4), \
			"=m" ((regparms).f5), \
			"=m" ((regparms).f6), \
			"=m" ((regparms).f7), \
			"=m" ((regparms).f8), \
			"=m" ((regparms).f9), \
			"=m" ((regparms).f10), \
			"=m" ((regparms).f11), \
			"=m" ((regparms).f12), \
			"=m" ((regparms).f13));

#define ARCH_CALL(regparms, ep) \
	__asm__("lwz r3, %0\n \
		lwz r4, %1\n \
		lwz r5, %2\n \
		lwz r6, %3\n \
		lwz r7, %4\n \
		lwz r8, %5\n \
		lwz r9, %6\n \
		lwz r10, %7\n \
		lfd f1, %8\n \
		lfd f2, %9\n \
		lfd f3, %10\n \
		lfd f4, %11\n \
		lfd f5, %12\n \
		lfd f6, %13\n \
		lfd f7, %14\n \
		lfd f8, %15\n \
		lfd f9, %16\n \
		lfd f10, %17\n \
		lfd f11, %18\n \
		lfd f12, %19\n \
		lfd f13, %20\n \
		lwz r0, %21\n \
		mtctr r0\n \
		bctrl\n" :: \
			"m" ((regparms).r3), \
			"m" ((regparms).r4), \
			"m" ((regparms).r5), \
			"m" ((regparms).r6), \
			"m" ((regparms).r7), \
			"m" ((regparms).r8), \
			"m" ((regparms).r9), \
			"m" ((regparms).r10), \
			"m" ((regparms).f1), \
			"m" ((regparms).f2), \
			"m" ((regparms).f3), \
			"m" ((regparms).f4), \
			"m" ((regparms).f5), \
			"m" ((regparms).f6), \
			"m" ((regparms).f7), \
			"m" ((regparms).f8), \
			"m" ((regparms).f9), \
			"m" ((regparms).f10), \
			"m" ((regparms).f11), \
			"m" ((regparms).f12), \
			"m" ((regparms).f13), \
			"m" (ep) : \
			"r1", "r2", "r3", "r4", "r5", \
			"r6", "r7", "r8", "r9", "r10", \
			"f1", "f2", "f3", "f4", "f5", \
			"f6", "f7", "f8", "f9", "f10", \
			"f11", "f12", "f13", \
			"r0", "ctr");

#define ARCH_SAVE_RETURN(archvalue) \
	__asm__("stw r3, %0\n \
		stw r4, %1\n \
		stfd f1, %2\n" : \
			"=m" ((archvalue).ivallo), \
			"=m" ((archvalue).ivalhi), \
			"=m" ((archvalue).dval));

#define ARCH_SET_RETURN(archvalue) \
	__asm__("lwz r3, %0\n \
		lwz r4, %1\n \
		lfd f1, %2\n" :: \
			"m" ((archvalue).ivallo), \
			"m" ((archvalue).ivalhi), \
			"m" ((archvalue).dval) : \
			"r3", "r4", "f1");
#define ARCH_PUT_STACK_BYTES(bcount) \
{ \
	int bc = bcount + 24; \
	if (bc <= 32) \
		__asm__("stwu r1, -32(r1)" ::: "r1"); \
	else if (bc <= 48) \
		__asm__("stwu r1, -48(r1)" ::: "r1"); \
	else if (bc <= 64) \
		__asm__("stwu r1, -64(r1)" ::: "r1"); \
	else if (bc <= 128) \
		__asm__("stwu r1, -128(r1)" ::: "r1"); \
	else if (bc <= 256) \
		__asm__("stwu r1, -256(r1)" ::: "r1"); \
	else \
		__asm__("stwu r1, -512(r1)" ::: "r1"); \
}
#define ARCH_REMOVE_STACK_BYTES(bcount) \
	__asm__("lwz r1, 0(r1)" ::: "r1");

#define ARCH_GET_STACK(sp) \
	__asm__("stw r1, %0" : "=m" (sp));

#define ARCH_GET_FRAME_PTR(fp) \
	__asm__("stw r30, %0" : "=m" (fp));

#define ARCH_CALLBACK_ARG_OFFSET (392)

#define ARCH_BIG_ENDIAN 1

#define ARCH_STACK_GROWS_DOWN 1

#define ARCH_STACK_SKIPTOP 24

#define ARCH_REGPARMS_IN_STACKSIZE 1

#define ARCH_CLAMP_NONFIRST_STRUCTALIGN 4

#endif
