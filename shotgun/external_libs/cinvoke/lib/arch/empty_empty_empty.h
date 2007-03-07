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

/*
This is a skeleton arch .h file, with helpful comments.
If more specific examples are needed, see the other
files in this directory.
*/

// TODO: replace these with symbols for your architecture
#ifndef _EMPTY_EMPTY_EMPTY_H
#define _EMPTY_EMPTY_EMPTY_H

// This structure represents the arch-specific portion of
// a shared library.  Most architectures will only need to
// store a void* here.
typedef struct _ArchLibrary {
	// TODO
} ArchLibrary;

// This structure holds values which will be or have been
// returned from a function call.  For example, on x86,
// return values can be in the eax, eax and edx, or st(0)
// registers.  This structure on that architecture contains
// two integers and a double, so that the values of all
// registers can be placed into it without clobbering
// each other.
typedef struct _ArchRetValue {
	// TODO
} ArchRetValue;

// This structure holds parameters which can be passed in
// registers.  For example if an arch's calling convention
// specified a maximum of seven input registers to pass
// arguments in in addition to the stack, then this struct
// should have seven ints
typedef struct _ArchRegParms {
	// TODO
} ArchRegParms;

// TODO: define these to be correct for your arch. The
// examples given here are for most 32-bit machines.
typedef char cinv_int8_t;
typedef short cinv_int16_t;
typedef int cinv_int32_t;
typedef long long cinv_int64_t; 

// TODO: these are constants used to provide error
// messages/codes. The CINV_NOMEM_NEEDSFREE value determines
// if C/Invoke will call arch_free_errstr on the returned
// CINV_S_NOMEM string value.  In the examples given below
// from UNIX, the answer is 0 since strerror returns a
// constant.
#define CINV_E_NOMEM (ENOMEM)
#define CINV_S_NOMEM (strerror(ENOMEM))
#define CINV_NOMEM_NEEDSFREE 0
#define CINV_E_INVAL (EINVAL)

// TODO: this sets the default calling convention.  most
// archs only have one, so you can either add a new one to
// the cinv_callconv_t enumeration, or just use "cdecl"
#define CINV_CC_DEFAULT CINV_CC_CDECL 

// TODO: set these similarly to the cinv_int*_t types above
#define CINV_T_2BYTE CINV_T_SHORT
#define CINV_T_4BYTE CINV_T_INT
#define CINV_T_8BYTE CINV_T_EXTRALONG

/////////////////////////////////////
// macros
/////////////////////////////////////

// the following examples are given
// for x86 in MASM syntax.  Some of these
// macros might not need to be implemented
// in inline assembly, depending on the arch,
// but likely most of them will.

// this macro stores the values in the input
// registers in the ArchRegParms structure passed as 'regparms'.
// In the example below we store ecx and edx because
// they are used in the fastcall convention to pass
// parameters.
#define ARCH_SAVE_REGPARMS(regparms) \
	__asm mov [(regparms).first], ecx \
	__asm mov [(regparms).second], edx // TODO: fix this for your arch

// this macro does two things: copies the values
// stored in regparms into the input registers,
// and calls the function pointed to by the pointer
// ep.
#define ARCH_CALL(regparms, ep) \
	__asm mov ecx, [(regparms).first] \
	__asm mov edx, [(regparms).second] \
	__asm call dword ptr [ep] // TODO: fix this for your arch

// saves any possible return values in the ArchRetValue
// structure given by archvalue.
#define ARCH_SAVE_RETURN(archvalue) \
	__asm mov [(archvalue).ivallow], eax \
	__asm mov [(archvalue).ivalhigh], edx \
	__asm fstp [(archvalue).dval] // TODO: fix this for your arch

// stores the return values in the ArchRetValue structure
// into any place where a caller might expect to find them
#define ARCH_SET_RETURN(archvalue) \
	__asm mov eax, [(archvalue).ivallow] \
	__asm mov edx, [(archvalue).ivalhigh] \
	__asm fld [(archvalue).dval] // TODO: fix this for your arch

// increases the stack size by bcount bytes
#define ARCH_PUT_STACK_BYTES(bcount) \
	__asm sub esp, bcount; // TODO: fix this for your arch

// decreases the stack size by bcount bytes
#define ARCH_REMOVE_STACK_BYTES(bcount) \
	__asm add esp, bcount; // TODO: fix this for your arch

// copies the current stack pointer into the pointer variable
// stackp
#define ARCH_GET_STACK(stackp) \
	__asm mov stackp, esp; // TODO: fix this for your arch

// copies the current frame pointer into the pointer variable
// framep
#define ARCH_GET_FRAME_PTR(framep) \
	__asm mov framep, ebp; // TODO: fix this for your arch

// TODO: This is the number of bytes up the stack
// from the frame pointer at which cinv_cbthunk can
// start looking for argument values.  The easiest
// way to determine this value is to run the test
// program under a debugger, set a breakpoint in
// cinv_cbthunk, look at the memory at the frame
// pointer, then start counting bytes upwards until
// you see where the first argument is stored.
#define ARCH_CALLBACK_ARG_OFFSET (96)

// TODO: Set to 1 if CPU puts most signifigant byte
// first in integers
#define ARCH_BIG_ENDIAN 0

// TODO: Whether the stack grows larger as the
// stack pointer gets numerically smaller.  Set to 1 on
// x86.
#define ARCH_STACK_GROWS_DOWN 1

// TODO: Set this to the number of bytes to leave blank at the top
// of the stack after the copied arguments.  Usually 0.  Note if you
// set this to a value other than 0, you must allocate additional
// space in ARCH_PUT_STACK_BYTES (and de-allocate additional space in
// ARCH_REMOVE_STACK_BYTES!).
#define ARCH_STACK_SKIPTOP 0

// TODO: Set this to 1 if args being passed in registers are
// still allocated space on the stack, despite not being copied
// there.  Usually 0.
#define ARCH_REGPARMS_IN_STACKSIZE 0

// TODO: If there is a maximum alignment size for structure members
// other than the first, set it here.  AFAIK the only arch to need
// this is OSX/PPC
#define ARCH_CLAMP_NONFIRST_STRUCTALIGN 0

#endif
