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
#ifndef _ARCH_GCC_X86_UNIX_H
#define _ARCH_GCC_X86_UNIX_H

#include <errno.h>

typedef struct _ArchLibrary {
	void *dl;
} ArchLibrary;

typedef struct _ArchRetValue {
	int ivallow;
	int ivalhigh;
	double dval;
} ArchRetValue;

typedef struct _ArchRegParms {} ArchRegParms;

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
#define ARCH_SAVE_REGPARMS(regparms)

#define ARCH_CALL(regparms, ep) ((void (*)())ep)();

#define ARCH_SAVE_RETURN(archvalue) \
	__asm__("movl %%eax, %0; \
			movl %%edx, %1; \
			fstpl %2" : \
			"=m" ((archvalue).ivallow), \
			"=m" ((archvalue).ivalhigh), \
			"=m" ((archvalue).dval));

#define ARCH_SET_RETURN(archvalue) \
	__asm__("movl %0, %%eax; \
			movl %1, %%edx; \
			fldl %2" :: \
			"m" ((archvalue).ivallow), \
			"m" ((archvalue).ivalhigh), \
			"m" ((archvalue).dval) : \
			"%eax", "%edx", "%st");

#define ARCH_PUT_STACK_BYTES(bcount) \
	__asm__("subl %0, %%esp" :: "m" (bcount) : "%esp");

#define ARCH_REMOVE_STACK_BYTES(bcount) \
	__asm__("addl %0, %%esp" :: "m" (bcount) : "%esp");

#define ARCH_GET_STACK(sp) \
	__asm__("movl %%esp, %0" : "=m" (sp));

#define ARCH_GET_FRAME_PTR(fp) \
	__asm__("movl %%ebp, %0" : "=m" (fp));

#define ARCH_CALLBACK_ARG_OFFSET (4*10)

#define ARCH_BIG_ENDIAN 0

#define ARCH_STACK_GROWS_DOWN 1

#define ARCH_STACK_SKIPTOP 0

#define ARCH_REGPARMS_IN_STACKSIZE 0

#define ARCH_CLAMP_NONFIRST_STRUCTALIGN 0

#endif
