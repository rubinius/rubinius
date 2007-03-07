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
#ifndef _ARCH_CL_X86_WIN_H
#define _ARCH_CL_X86_WIN_H

#include <windows.h>

char *GetWin32ErrMsg(DWORD err);

typedef struct _ArchLibrary {
	HANDLE dl;
} ArchLibrary;

typedef struct _ArchRetValue {
	int ivallow;
	int ivalhigh;
	double dval;
} ArchRetValue;

typedef struct _ArchRegParms {
	int first;
	int second;
} ArchRegParms;

typedef char cinv_int8_t;
typedef short cinv_int16_t;
typedef int cinv_int32_t;
typedef long long cinv_int64_t;

#define CINV_E_NOMEM ((cinv_int32_t)ERROR_NOT_ENOUGH_MEMORY)
#define CINV_S_NOMEM (GetWin32ErrMsg(ERROR_NOT_ENOUGH_MEMORY))
#define CINV_NOMEM_NEEDSFREE 1
#define CINV_E_INVAL ((cinv_int32_t)ERROR_INVALID_PARAMETER)

// setting this to stdcall makes the win32 api work, but user-compiled
// dlls break; you can't win
#define CINV_CC_DEFAULT CINV_CC_CDECL
#define CINV_T_2BYTE CINV_T_SHORT
#define CINV_T_4BYTE CINV_T_INT
#define CINV_T_8BYTE CINV_T_EXTRALONG

/////////////////////////////////////
// macros
/////////////////////////////////////
#define ARCH_SAVE_REGPARMS(regparms) \
	__asm mov [(regparms).first], ecx \
	__asm mov [(regparms).second], edx

#define ARCH_CALL(regparms, ep) \
	__asm mov ecx, [(regparms).first] \
	__asm mov edx, [(regparms).second] \
	__asm call dword ptr [ep]

#define ARCH_SAVE_RETURN(archvalue) \
	__asm mov [(archvalue).ivallow], eax \
	__asm mov [(archvalue).ivalhigh], edx \
	__asm fstp [(archvalue).dval]

#define ARCH_SET_RETURN(archvalue) \
	__asm mov eax, [(archvalue).ivallow] \
	__asm mov edx, [(archvalue).ivalhigh] \
	__asm fld [(archvalue).dval]

#define ARCH_PUT_STACK_BYTES(bcount) \
	__asm sub esp, bcount;

#define ARCH_REMOVE_STACK_BYTES(bcount) \
	__asm add esp, bcount;

#define ARCH_GET_STACK(stackp) \
	__asm mov stackp, esp;

#define ARCH_GET_FRAME_PTR(framep) \
	__asm mov framep, ebp;

#define ARCH_CALLBACK_ARG_OFFSET (96)

#define ARCH_BIG_ENDIAN 0

#define ARCH_STACK_GROWS_DOWN 1

#define ARCH_STACK_SKIPTOP 0

#define ARCH_REGPARMS_IN_STACKSIZE 0

#define ARCH_CLAMP_NONFIRST_STRUCTALIGN 0

#endif
