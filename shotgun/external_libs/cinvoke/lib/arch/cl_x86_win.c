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
#ifdef CINVOKE_BUILD
#include "../cinvoke.h"
#include "../cinvoke-private.h"
#else
#include "cinvoke.h"
#include "cinvoke-private.h"
#endif

char *GetWin32ErrMsg(DWORD err) {
	char *str;
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&str, 0, NULL)) {
			str = NULL;
	}
	return str;
}

void context_set_win32_error(CInvContext *context) {
	DWORD err = GetLastError();
	char *str = GetWin32ErrMsg(err);

	context_set_error(context, err, str, 1);
}

void arch_free_errstr(char *str) {
	LocalFree(str);
}

cinv_status_t arch_library_create(CInvContext *context, const char *path,
	ArchLibrary *library_out) {
	HANDLE lib = LoadLibrary(path);
	if (!lib) {
		context_set_win32_error(context);
		return CINV_ERROR;
	}
		
	library_out->dl = lib;

	return CINV_SUCCESS;
}
cinv_status_t arch_library_get_entrypoint(CInvContext *context,
	ArchLibrary *library, const char *name, void **entrypoint_out) {
	void *sym = GetProcAddress(library->dl, name);
	if (!sym) {
		context_set_win32_error(context);
		return CINV_ERROR;
	}

	*entrypoint_out = sym;

	return CINV_SUCCESS;
}
cinv_status_t arch_library_delete(CInvContext *context, ArchLibrary *library) {
	if (!FreeLibrary(library->dl)) {
		context_set_win32_error(context);
		return CINV_ERROR;
	}

	return CINV_SUCCESS;
}
const static int LEN = 4096;

char *arch_callback_stub(void *functionp, void *param,
	short stacksize, cinv_callconv_t cc, cinv_type_t types[], int numparams) {
	int codesize;
	const char *code;
	char *ret;

	if (cc != CINV_CC_STDCALL && cc != CINV_CC_FASTCALL) {
		// void f() { ((void (__cdecl *)(void *))0xAAAAAAAA)((void *)0xBBBBBBBB); }
		codesize = 31;
		code = "\x55\x8b\xec\x83\xec\x40\x53\x56\x57\x68"
				"\xbb\xbb\xbb\xbb\xb8\xaa\xaa\xaa\xaa\xff"
				"\xd0\x83\xc4\x04\x5f\x5e\x5b\x8b\xe5\x5d"
				"\xc3";
	} else {
		// same thing except with an additional stack-cleanup argument placeholder
		// note that the below code does not modify ecx or edx, so we handle
		// fastcall correctly
		codesize = 33;
		code = "\x55\x8b\xec\x83\xec\x40\x53\x56\x57\x68"
				"\xbb\xbb\xbb\xbb\xb8\xaa\xaa\xaa\xaa\xff"
				"\xd0\x83\xc4\x04\x5f\x5e\x5b\x8b\xe5\x5d"
				"\xc2\xdd\xdd"; 
	}

	ret = VirtualAlloc(NULL, LEN, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!ret)
		return NULL;
	
	memcpy(ret, code, codesize);

	memcpy(ret + 10, &param, 4);
	memcpy(ret + 15, &functionp, 4);

	if (cc == CINV_CC_STDCALL || cc == CINV_CC_FASTCALL)
		memcpy(ret + 31, &stacksize, 2);
	
	return ret;
}
void arch_free_stub(char *stub) {
	VirtualFree(stub, 0, MEM_RELEASE);
}

int is_ok_register(cinv_type_t type) {
	return (type != CINV_T_FLOAT) &&
		(type != CINV_T_DOUBLE) &&
		(type != CINV_T_EXTRALONG);
}

int arch_is_register_parm(cinv_callconv_t callingconvention, int index,
	int num_params, cinv_type_t types[]) {
	int numeligible = 0, i;

	if (callingconvention != CINV_CC_FASTCALL) return 0;
	
	for (i = 0; i < index; i++) {
		if (is_ok_register(types[i]))
			numeligible++;
		if (numeligible == 2) break;
	}
	return (numeligible < 2 && is_ok_register(types[i]));
}
int convert_to_int(void *ptr, cinv_type_t type) {
	switch (type) {
	case CINV_T_CHAR:
		return (int)*(char *)ptr;
	case CINV_T_SHORT:
		return (int)*(short *)ptr;
	case CINV_T_INT:
		return *(int *)ptr;
	case CINV_T_LONG:
		return (int)*(long *)ptr;
	case CINV_T_PTR:
		return (int)*(void **)ptr;
	default:
		return -1;
	}
}
void set_from_int(void *ptr_out, cinv_type_t type, int val) {
	switch (type) {
	case CINV_T_CHAR:
		*(char *)ptr_out = (char)val;
		break;
	case CINV_T_SHORT:
		*(short *)ptr_out = (short)val;
		break;
	case CINV_T_INT:
		*(int *)ptr_out = val;
		break;
	case CINV_T_LONG:
		*(long *)ptr_out = (long)val;
		break;
	case CINV_T_PTR:
		*(void **)ptr_out = (void *)val;
		break;
	default:
		break;
	}
}
void arch_set_register_parms(ArchRegParms *regparms, 
	cinv_callconv_t callingconvention, int num_params, void *parameters[], 
	cinv_type_t types[]) {
	int numeligible = 0, i;

	if (callingconvention != CINV_CC_FASTCALL) return;

	for (i = 0; i < num_params; i++) {
		if (is_ok_register(types[i])) {
			if (numeligible == 0)
				regparms->first = convert_to_int(parameters[i], types[i]);
			else
				regparms->second = convert_to_int(parameters[i], types[i]);
			numeligible++;
		}
		if (numeligible == 2) break;
	}
}

void arch_get_register_parms(ArchRegParms *regparms,
	cinv_callconv_t callingconvention, int num_params, void *parameters_out[],
	cinv_type_t types[]) {
	int numeligible = 0, i;

	if (callingconvention != CINV_CC_FASTCALL) return;
	
	for (i = 0; i < num_params; i++) {
		if (is_ok_register(types[i])) {
			if (numeligible == 0)
				set_from_int(parameters_out[i], types[i], regparms->first);
			else
				set_from_int(parameters_out[i], types[i], regparms->second);
			numeligible++;
		}
		if (numeligible == 2) break;
	}
}

void arch_getval_char(ArchRetValue *archval, char *outval) {
	*outval = archval->ivallow;
}
void arch_getval_short(ArchRetValue *archval, short *outval) {
	*outval = archval->ivallow;
}
void arch_getval_int(ArchRetValue *archval, int *outval) {
	*outval = archval->ivallow;
}
void arch_getval_long(ArchRetValue *archval, long int *outval) {
	*outval = archval->ivallow;
}
void arch_getval_extralong(ArchRetValue *archval, long long int *outval) {
	*outval = archval->ivalhigh;
	*outval <<= 32;
	*outval |= archval->ivallow;
}
void arch_getval_float(ArchRetValue *archval, float *outval) {
	*outval = (float)archval->dval;
}
void arch_getval_double(ArchRetValue *archval, double *outval) {
	*outval = archval->dval;
}
void arch_getval_ptr(ArchRetValue *archval, void **outval) {
	*outval = (void *)archval->ivallow;
}

void arch_setval_char(ArchRetValue *archval, char val) {
	archval->ivallow = val;
}
void arch_setval_short(ArchRetValue *archval, short val) {
	archval->ivallow = val;
}
void arch_setval_int(ArchRetValue *archval, int val) {
	archval->ivallow = val;
}
void arch_setval_long(ArchRetValue *archval, long int val) {
	archval->ivallow = val;
}
void arch_setval_extralong(ArchRetValue *archval, long long int val) {
	archval->ivalhigh = (int)(val >> 32);
	archval->ivallow = (int)val;
}
void arch_setval_float(ArchRetValue *archval, float val) {
	archval->dval = val;
}
void arch_setval_double(ArchRetValue *archval, double val) {
	archval->dval = val;
}
void arch_setval_ptr(ArchRetValue *archval, void *val) {
	archval->ivallow = (int)val;
}

void arch_size_char(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 1;
}
void arch_size_short(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 2;
	*structalign_out = 2;
}
void arch_size_int(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 4;
	*structalign_out = 4;
}
void arch_size_long(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 4;
	*structalign_out = 4;
}
void arch_size_extralong(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 8;
	*structsize_out = 8;
	*structalign_out = 8;
}
void arch_size_float(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 4;
	*structalign_out = 4;
}
void arch_size_double(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 8;
	*structsize_out = 8;
	*structalign_out = 8;
}
void arch_size_ptr(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 4;
	*structalign_out = 4;
}
