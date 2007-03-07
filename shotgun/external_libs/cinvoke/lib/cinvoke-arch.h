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
#ifndef _ARCH_H
#define _ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* \file cinvoke-arch.h
* Architecture-Specific Types
*/

/** Indicates whether a function call succeeded or failed.
*/
typedef enum _cinv_status_t {
	CINV_ERROR = 0, /**< Indicates that an error occurred. */
	CINV_SUCCESS  = 1 /**< Indicates that no error occurred. */
} cinv_status_t;

/** Represents the simple C types which can be marshaled by C/Invoke.
*/
typedef enum _cinv_type_t {
	CINV_T_CHAR = 0, /**< Represents the \b char C type */
	CINV_T_SHORT = 1, /**< Represents the \b short C type */
	CINV_T_INT = 2, /**< Represents the \b int C type */
	CINV_T_LONG = 3, /**< Represents the \b long C type */
	CINV_T_EXTRALONG = 4, /**< Represents the <b>long long</b> C type */
	CINV_T_FLOAT = 5, /**< Represents the \b float C type */
	CINV_T_DOUBLE = 6, /**< Represents the \b double C type */
	CINV_T_PTR = 7, /**< Represents any pointer type */

	CINV_NUM_TYPES = 8
} cinv_type_t;

/** Indicates the calling convention being used for a function.
*/
typedef enum _cinv_callconv_t {
	CINV_CC_CDECL = 0, /**< The cdecl calling convention, the most common convention on x86. */
	CINV_CC_STDCALL = 1, /**< The stdcall calling convention, the default convention for the Windows API. */
	CINV_CC_FASTCALL = 2 /**< Yet another, rarely used, Windows calling convention */
} cinv_callconv_t;

#ifdef ARCH_GCC_X86_UNIX
#include "arch/gcc_x86_unix.h"
#endif
#ifdef ARCH_GCC_X64_UNIX
#include "arch/gcc_x64_unix.h"
#endif
#ifdef ARCH_CL_X86_WIN
#include "arch/cl_x86_win.h"
#endif
#ifdef ARCH_GCC_PPC_OSX
#include "arch/gcc_ppc_osx.h"
#endif
#ifndef CINVOKE_BUILD
#include "cinvoke-archspec.h"
#endif

typedef struct _CInvContext {
	cinv_int32_t errorcode;
	char *errormsg;
	int needsfree;
} CInvContext;

void arch_free_errstr(char *str);

cinv_status_t arch_library_create(CInvContext *context, const char *path,
	ArchLibrary *library_out);
cinv_status_t arch_library_get_entrypoint(CInvContext *context,
	ArchLibrary *library, const char *name, void **entrypoint_out);
cinv_status_t arch_library_delete(CInvContext *context, ArchLibrary *library);

char *arch_callback_stub(void *functionp, void *param, short stacksize,
						 cinv_callconv_t cc, cinv_type_t types[], int numparams);
void arch_free_stub(char *stub);

int arch_is_register_parm(cinv_callconv_t callingconvention, int index,
	int num_params, cinv_type_t types[]);
void arch_set_register_parms(ArchRegParms *regparms, 
	cinv_callconv_t callingconvention, int num_params, void *parameters[], 
	cinv_type_t types[]);
void arch_get_register_parms(ArchRegParms *regparms,
	cinv_callconv_t callingconvention, int num_params, void *parameters_out[],
	cinv_type_t types[]);

void arch_getval_char(ArchRetValue *archval, char *outval);
void arch_getval_short(ArchRetValue *archval, short *outval);
void arch_getval_int(ArchRetValue *archval, int *outval);
void arch_getval_long(ArchRetValue *archval, long int *outval);
void arch_getval_extralong(ArchRetValue *archval, long long int *outval);
void arch_getval_float(ArchRetValue *archval, float *outval);
void arch_getval_double(ArchRetValue *archval, double *outval);
void arch_getval_ptr(ArchRetValue *archval, void **outval);

void arch_setval_char(ArchRetValue *archval, char val);
void arch_setval_short(ArchRetValue *archval, short val);
void arch_setval_int(ArchRetValue *archval, int val);
void arch_setval_long(ArchRetValue *archval, long int val);
void arch_setval_extralong(ArchRetValue *archval, long long int val);
void arch_setval_float(ArchRetValue *archval, float val);
void arch_setval_double(ArchRetValue *archval, double val);
void arch_setval_ptr(ArchRetValue *archval, void *val);

void arch_size_char(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_short(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_int(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_long(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_extralong(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_float(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_double(int *stacksize_out, int *structsize_out,
	int *structalign_out);
void arch_size_ptr(int *stacksize_out, int *structsize_out,
	int *structalign_out);

#ifdef __cplusplus
}
#endif

#endif
