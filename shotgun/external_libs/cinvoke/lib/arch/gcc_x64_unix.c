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
	
	//void f() {
	//	__asm("movq %%rdi, %%r11" ::: "%r11");
	//	((void (*)(void *))0xAAAAAAAAAAAAAAAA)(
	//		(void *)0xBBBBBBBBBBBBBBBB);
	//}
	memcpy(ret,
		"\x55\x48\x89\xe5\x49\x89\xfb\x48\xb8"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\x48\xbf\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb"
		"\xff\xd0\xc9\xc3",
		31);

	memcpy(ret + 9, &functionp, 8);
	memcpy(ret + 19, &param, 8);
	
	return ret;
}
void arch_free_stub(char *stub) {
	munmap(stub, LEN);
}

int isflt(cinv_type_t t) {
	return (t == CINV_T_FLOAT) || (t == CINV_T_DOUBLE);
}
int arch_is_register_parm(cinv_callconv_t callingconvention, int index,
	int num_params, cinv_type_t types[]) {
	int numints = 0, numflts = 0;
	int i;
	for (i = 0; i < index; i++) {
		if (isflt(types[i]))
			numflts++;
		else
			numints++;
	}
	return (isflt(types[index]) && numflts < 8) || (numints < 6);
}

void set_flt(ArchRegParms *regparms, int index, void *p, cinv_type_t type) {
	long *toset[] = {
		&(regparms->xmm0), &(regparms->xmm1), &(regparms->xmm2),
		&(regparms->xmm3), &(regparms->xmm4), &(regparms->xmm5),
		&(regparms->xmm6), &(regparms->xmm7)
	};
	
	if (type == CINV_T_FLOAT)
		*((float *)toset[index]) = *(float *)p;
	else
		*((double *)toset[index]) = *(double *)p;
}
void set_int(ArchRegParms *regparms, int index, void *p, cinv_type_t type) {
	long *toset[] = {
		&(regparms->rdi), &(regparms->rsi), &(regparms->rdx), 
		&(regparms->rcx), &(regparms->r8), &(regparms->r9) 
	};
	
	if (type == CINV_T_CHAR)
		*(toset[index]) = *(char *)p;
	else if (type == CINV_T_SHORT)
		*(toset[index]) = *(short *)p;
	else if (type == CINV_T_INT)
		*(toset[index]) = *(int *)p;
	else if (type == CINV_T_LONG)
		*(toset[index]) = *(long *)p;
	else if (type == CINV_T_EXTRALONG)
		*(toset[index]) = *(long long *)p;
	else
		*(toset[index]) = (long)*(void **)p;
}

void arch_set_register_parms(ArchRegParms *regparms, 
	cinv_callconv_t callingconvention, int num_params, void *parameters[], 
	cinv_type_t types[]) {
	int numints = 0, numflts = 0;
	int i;
	for (i = 0; i < num_params; i++) {
		if (isflt(types[i])) {
			if (numflts < 8)
				set_flt(regparms, numflts, parameters[i], types[i]);
			numflts++;
		} else {
			if (numints < 6)
				set_int(regparms, numints, parameters[i], types[i]);
			numints++;
		}
	}
}
void get_flt(void *po, int index, ArchRegParms *regparms, cinv_type_t type) {
	long *toget[] = {
		&(regparms->xmm0), &(regparms->xmm1), &(regparms->xmm2),
		&(regparms->xmm3), &(regparms->xmm4), &(regparms->xmm5),
		&(regparms->xmm6), &(regparms->xmm7)
	};
	
	if (type == CINV_T_FLOAT)
		*(float *)po = *((float *)toget[index]);
	else
		*(double *)po = *((double *)toget[index]);
}
void get_int(void *po, int index, ArchRegParms *regparms, cinv_type_t type) {
	long *toget[] = {
		&(regparms->rdi), &(regparms->rsi), &(regparms->rdx), 
		&(regparms->rcx), &(regparms->r8), &(regparms->r9) 
	};
	
	if (type == CINV_T_CHAR)
		*(char *)po = (char)*(toget[index]);
	else if (type == CINV_T_SHORT)
		*(short *)po = (short)*(toget[index]);
	else if (type == CINV_T_INT)
		*(int *)po = (int)*(toget[index]);
	else if (type == CINV_T_LONG)
		*(long *)po = *(toget[index]);
	else if (type == CINV_T_EXTRALONG)
		*(long long *)po = (long long)*(toget[index]);
	else
		*(void **)po = (void *)*(toget[index]);
}

void arch_get_register_parms(ArchRegParms *regparms,
	cinv_callconv_t callingconvention, int num_params, void *parameters_out[],
	cinv_type_t types[]) {
	int numints = 0, numflts = 0;
	int i;
	for (i = 0; i < num_params; i++) {
		if (isflt(types[i])) {
			if (numflts < 8)
				get_flt(parameters_out[i], numflts, regparms, types[i]);
			numflts++;
		} else {
			if (numints < 6)
				get_int(parameters_out[i], numints, regparms, types[i]);
			numints++;
		}
	}
}

void arch_getval_char(ArchRetValue *archval, char *outval) {
	*outval = archval->ival;
}
void arch_getval_short(ArchRetValue *archval, short *outval) {
	*outval = archval->ival;
}
void arch_getval_int(ArchRetValue *archval, int *outval) {
	*outval = archval->ival;
}
void arch_getval_long(ArchRetValue *archval, long int *outval) {
	*outval = archval->ival;
}
void arch_getval_extralong(ArchRetValue *archval, long long int *outval) {
	*outval = archval->ival;
}
void arch_getval_float(ArchRetValue *archval, float *outval) {
	float f = *(float *)&archval->dval;
	*outval = f;
}
void arch_getval_double(ArchRetValue *archval, double *outval) {
	double d = *(double *)&archval->dval;
	*outval = d;
}
void arch_getval_ptr(ArchRetValue *archval, void **outval) {
	*outval = (void *)archval->ival;
}

void arch_setval_char(ArchRetValue *archval, char val) {
	archval->ival = val;
}
void arch_setval_short(ArchRetValue *archval, short val) {
	archval->ival = val;
}
void arch_setval_int(ArchRetValue *archval, int val) {
	archval->ival = val;
}
void arch_setval_long(ArchRetValue *archval, long int val) {
	archval->ival = val;
}
void arch_setval_extralong(ArchRetValue *archval, long long int val) {
	archval->ival = val;
}
void arch_setval_float(ArchRetValue *archval, float val) {
	*((float *)&archval->dval) = val;
}
void arch_setval_double(ArchRetValue *archval, double val) {
	*((double *)&archval->dval) = val;
}
void arch_setval_ptr(ArchRetValue *archval, void *val) {
	archval->ival = (long)val;
}

void arch_size_char(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 8;
	*structsize_out = 1;
}
void arch_size_short(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 8;
	*structsize_out = 2;
	*structalign_out = 2;
}
void arch_size_int(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 8;
	*structsize_out = 4;
	*structalign_out = 4;
}
void arch_size_long(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 8;
	*structsize_out = 8;
	*structalign_out = 8;
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
	*stacksize_out = 8;
	*structsize_out = 8;
	*structalign_out = 8;
}
