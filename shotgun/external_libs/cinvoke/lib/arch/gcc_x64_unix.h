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
#ifndef _ARCH_GCC_X64_UNIX_H
#define _ARCH_GCC_X64_UNIX_H

#include <errno.h>

typedef struct _ArchLibrary {
	void *dl;
} ArchLibrary;

typedef struct _ArchRetValue {
	long ival;
	long dval;
} ArchRetValue;

typedef struct _ArchRegParms {
	long rdi;
	long rsi;
	long rdx;
	long rcx;
	long r8;
	long r9;
	long xmm0;
	long xmm1;
	long xmm2;
	long xmm3;
	long xmm4;
	long xmm5;
	long xmm6;
	long xmm7;
} ArchRegParms;

typedef char cinv_int8_t;
typedef short cinv_int16_t;
typedef int cinv_int32_t;
typedef long cinv_int64_t;

#define CINV_E_NOMEM ((cinv_int32_t)ENOMEM)
#define CINV_S_NOMEM (strerror(ENOMEM))
#define CINV_NOMEM_NEEDSFREE 0
#define CINV_E_INVAL ((cinv_int32_t)EINVAL)

#define CINV_CC_DEFAULT CINV_CC_CDECL
#define CINV_T_2BYTE CINV_T_SHORT
#define CINV_T_4BYTE CINV_T_INT
#define CINV_T_8BYTE CINV_T_LONG

/////////////////////////////////////
// macros
/////////////////////////////////////
// note we use r11 instead of rdi, this is because we
// saved this value in the callback stub
#define ARCH_SAVE_REGPARMS(regparms) \
	__asm__("movq %%r11, %0; \
		movq %%rsi, %1; \
		movq %%rdx, %2; \
		movq %%rcx, %3; \
		movq %%r8, %4; \
		movq %%r9, %5; \
		movsd %%xmm0, %6; \
		movsd %%xmm1, %7; \
		movsd %%xmm2, %8; \
		movsd %%xmm3, %9; \
		movsd %%xmm4, %10; \
		movsd %%xmm5, %11; \
		movsd %%xmm6, %12; \
		movsd %%xmm7, %13" : \
			"=m" ((regparms).rdi), \
			"=m" ((regparms).rsi), \
			"=m" ((regparms).rdx), \
			"=m" ((regparms).rcx), \
			"=m" ((regparms).r8), \
			"=m" ((regparms).r9), \
			"=m" ((regparms).xmm0), \
			"=m" ((regparms).xmm1), \
			"=m" ((regparms).xmm2), \
			"=m" ((regparms).xmm3), \
			"=m" ((regparms).xmm4), \
			"=m" ((regparms).xmm5), \
			"=m" ((regparms).xmm6), \
			"=m" ((regparms).xmm7));

#define ARCH_CALL(regparms, ep) \
	__asm__("movq %0, %%rdi; \
		movq %1, %%rsi; \
		movq %2, %%rdx; \
		movq %3, %%rcx; \
		movq %4, %%r8; \
		movq %5, %%r9; \
		movsd %6, %%xmm0; \
		movsd %7, %%xmm1; \
		movsd %8, %%xmm2; \
		movsd %9, %%xmm3; \
		movsd %10, %%xmm4; \
		movsd %11, %%xmm5; \
		movsd %12, %%xmm6; \
		movsd %13, %%xmm7; \
		call *%14" :: \
			"m" ((regparms).rdi), \
			"m" ((regparms).rsi), \
			"m" ((regparms).rdx), \
			"m" ((regparms).rcx), \
			"m" ((regparms).r8), \
			"m" ((regparms).r9), \
			"m" ((regparms).xmm0), \
			"m" ((regparms).xmm1), \
			"m" ((regparms).xmm2), \
			"m" ((regparms).xmm3), \
			"m" ((regparms).xmm4), \
			"m" ((regparms).xmm5), \
			"m" ((regparms).xmm6), \
			"m" ((regparms).xmm7), \
			"m" (ep) : \
			"%rdi", "%rsi", "%rdx", "%rcx", \
			"%r8", "%r9", "%xmm0", "%xmm1", \
			"%xmm2", "%xmm3", "%xmm4", "%xmm5", \
			"%xmm6", "%xmm7");

#define ARCH_SAVE_RETURN(archvalue) \
	__asm__("movq %%rax, %0; \
		movsd %%xmm0, %1" : \
			"=m" ((archvalue).ival), \
			"=m" ((archvalue).dval));

#define ARCH_SET_RETURN(archvalue) \
	__asm__("movq %0, %%rax; \
		movsd %1, %%xmm0" :: \
			"m" ((archvalue).ival), \
			"m" ((archvalue).dval) : \
			"%rax", "%xmm0");

#define ARCH_PUT_STACK_BYTES(bcount) \
{ \
	long bc = bcount; \
	if ((bc % 16) != 0) bc += (16 - (bc % 16)); \
	__asm__("subq %0, %%rsp" :: "m" (bc) : "%rsp"); \
}
#define ARCH_REMOVE_STACK_BYTES(bcount) \
{ \
	long bc = bcount; \
	if ((bc % 16) != 0) bc += (16 - (bc % 16)); \
	__asm__("addq %0, %%rsp" :: "m" (bc) : "%rsp"); \
}
#define ARCH_GET_STACK(sp) \
	__asm__("movq %%rsp, %0" : "=m" (sp));

#define ARCH_GET_FRAME_PTR(fp) \
	__asm__("movq %%rbp, %0" : "=m" (fp));

#define ARCH_CALLBACK_ARG_OFFSET (32)

#define ARCH_BIG_ENDIAN 0

#define ARCH_STACK_GROWS_DOWN 1

#define ARCH_STACK_SKIPTOP 0

#define ARCH_REGPARMS_IN_STACKSIZE 0

#define ARCH_CLAMP_NONFIRST_STRUCTALIGN 0

#endif
