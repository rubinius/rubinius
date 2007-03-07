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

#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

void arch_free_errstr(char *str) {
	free(str);
}

cinv_status_t arch_library_create(CInvContext *context, const char *path,
	ArchLibrary *library_out) {
	void *dl = dlopen(path, RTLD_LAZY);
	if (!dl) {
		context_set_error(context, -1, strdup(dlerror()), 1);
		return CINV_ERROR;
	}
		
	library_out->dl = dl;

	return CINV_SUCCESS;
}
cinv_status_t arch_library_get_entrypoint(CInvContext *context,
	ArchLibrary *library, const char *name, void **entrypoint_out) {
	void *sym = dlsym(library->dl, name);
	if (!sym) {
		context_set_error(context, -1, strdup(dlerror()), 1);
		return CINV_ERROR;
	}

	*entrypoint_out = sym;

	return CINV_SUCCESS;
}
cinv_status_t arch_library_delete(CInvContext *context, ArchLibrary *library) {
	if (dlclose(library->dl)) {
		context_set_error(context, -1, strdup(dlerror()), 1);
		return CINV_ERROR;
	}

	return CINV_SUCCESS;
}

const static int LEN = 4096;

char *arch_callback_stub(void *functionp, void *param,
	short stacksize, cinv_callconv_t cc, cinv_type_t types[], int numparams) {
	char *ret = mmap(0, LEN, PROT_EXEC|PROT_READ|PROT_WRITE,
		MAP_ANON|MAP_PRIVATE, -1, 0);
	if (ret == MAP_FAILED)
		return NULL;
	
	// void f() { ((void (*)(void *))0xAAAAAAAA)((void *)0xBBBBBBBB); }
	memcpy(ret,
		"\x55\x89\xe5\x83\xec\x08\x83\xec\x0c\x68"
		"\xbb\xbb\xbb\xbb\xb8\xaa\xaa\xaa\xaa\xff"
		"\xd0\x83\xc4\x10\xc9\xc3",
		26);

	memcpy(ret + 10, &param, 4);
	memcpy(ret + 15, &functionp, 4);
	
	return ret;
}
void arch_free_stub(char *stub) {
	munmap(stub, LEN);
}

int arch_is_register_parm(cinv_callconv_t callingconvention, int index,
	int num_params, cinv_type_t types[]) { return 0; }
void arch_set_register_parms(ArchRegParms *regparms, 
	cinv_callconv_t callingconvention, int num_params, void *parameters[], 
	cinv_type_t types[]) {}
void arch_get_register_parms(ArchRegParms *regparms,
	cinv_callconv_t callingconvention, int num_params, void *parameters_out[],
	cinv_type_t types[]) {}

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
	archval->ivalhigh = (val >> 32);
	archval->ivallow = val;
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
	*structalign_out = 4;
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
	*structalign_out = 4;
}
void arch_size_ptr(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 4;
	*structalign_out = 4;
}
