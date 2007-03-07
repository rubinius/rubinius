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
	/*if (dlclose(library->dl)) {
		context_set_error(context, -1, strdup(dlerror()), 1);
		return CINV_ERROR;
	}*/ // darwin returns an error if we do this

	return CINV_SUCCESS;
}

int isflt(cinv_type_t t) {
	return (t == CINV_T_FLOAT) || (t == CINV_T_DOUBLE);
}

const static int LEN = 4096;

char *arch_callback_stub(void *functionp, void *param,
	short stacksize, cinv_callconv_t cc, cinv_type_t types[], int numparams) {
	short functionphi = ((int)functionp >> 16);
	short functionplo = ((int)functionp & 0xFFFF);
	short paramhi = ((int)param >> 16);
	short paramlo = ((int)param & 0xFFFF);

	int needscopy = 0, i, numints = 0;

	for (i = 0; i < numparams; i++) {
		if (isflt(types[i])) {
			if (types[i] == CINV_T_FLOAT)
				numints++;
			else
				numints += 2;
		} else {
			if (types[i] == CINV_T_EXTRALONG)
				numints += 2;
			else
				numints ++;
		}

		if (numints == 9 && types[i] == CINV_T_EXTRALONG) {
			needscopy = 1;
			break;
		}
	}

	char *ret = mmap(0, LEN, PROT_EXEC|PROT_READ|PROT_WRITE,
		MAP_ANON|MAP_PRIVATE, -1, 0);
	if (ret == MAP_FAILED)
		return NULL;
	
	if (!needscopy) {
		//void f() {
		//	__asm("mr r2, r3" ::: "r2");
		//	((void (*)(void *))0xAAAABBBB)((void *)0xCCCCDDDD);
		//}
		memcpy(ret,
			"\x7c\x08\x02\xa6\xbf\xc1\xff\xf8"
			"\x90\x01\x00\x08\x94\x21\xff\xb0"
			"\x7c\x3e\x0b\x78\x7c\x62\x1b\x78"
			"\x3c\x00\xaa\xaa\x60\x00\xbb\xbb"
			"\x3c\x60\xcc\xcc\x60\x63\xdd\xdd"
			"\x7c\x0c\x03\x78\x7d\x89\x03\xa6"
			"\x4e\x80\x04\x21\x80\x21\x00\x00"
			"\x80\x01\x00\x08\x7c\x08\x03\xa6"
			"\xbb\xc1\xff\xf8\x4e\x80\x00\x20",
			72);

		memcpy(ret + 26, &functionphi, 2);
		memcpy(ret + 30, &functionplo, 2);
		memcpy(ret + 34, &paramhi, 2);
		memcpy(ret + 38, &paramlo, 2);
	} else {
		short offset = 80 + ARCH_STACK_SKIPTOP + (7 * 4);
		offset &= 0x7FF;
		
		//void f2() {
		//  __asm("lwz r10 3822(r1)"); // 0xEEE
		//	__asm("mr r2, r3" ::: "r2");
		//	((void (*)(void *))0xAAAABBBB)((void *)0xCCCCDDDD);
		//}
		memcpy(ret,
			"\x7c\x08\x02\xa6\xbf\xc1\xff\xf8"
			"\x90\x01\x00\x08\x94\x21\xff\xb0"
			"\x7c\x3e\x0b\x78\x91\x41\x0e\xee"
			"\x7c\x62\x1b\x78\x3c\x00\xaa\xaa"
			"\x60\x00\xbb\xbb\x3c\x60\xcc\xcc"
			"\x60\x63\xdd\xdd\x7c\x0c\x03\x78"
			"\x7d\x89\x03\xa6\x4e\x80\x04\x21"
			"\x80\x21\x00\x00\x80\x01\x00\x08"
			"\x7c\x08\x03\xa6\xbb\xc1\xff\xf8"
			"\x4e\x80\x00\x20",
			76);

		memcpy(ret + 22, &offset, 2);
		memcpy(ret + 30, &functionphi, 2);
		memcpy(ret + 34, &functionplo, 2);
		memcpy(ret + 38, &paramhi, 2);
		memcpy(ret + 42, &paramlo, 2);
	}
	
	// the following block is a ppc oddity which is neccessary due to
	// the instruction cache and data cache not being linked.  in
	// order to modify and execute code on ppc we have to flush it out
	// of the data cache, then invalidate the instruction cache.  this
	// will ensure that when the code is executed it is running our fresh
	// copy from memory.  Much praise to H D Moore for pointing this out @
	// http://www.uninformed.org/?v=1&a=1&t=pdf.
	int *c = (int *)ret;
	for (i = 0; i < 76; i++) {
		__asm("lwz r2, %0" :: "m" (c) : "r2");
		__asm("dcbf 0, r2");
		__asm("sync");
		__asm("icbi 0, r2");
		__asm("isync");
		c++;
	}


	return ret;
}
void arch_free_stub(char *stub) {
	munmap(stub, LEN);
}

int arch_is_register_parm(cinv_callconv_t callingconvention, int index,
	int num_params, cinv_type_t types[]) {
	int numints = 0, numflts = 0;
	int i;
	for (i = 0; i <= index; i++) {
		if (isflt(types[i])) {
			numflts++;
			if (types[i] == CINV_T_FLOAT)
				numints++;
			else
				numints += 2;
		} else if (types[i] == CINV_T_EXTRALONG)
			numints += 2;
		else
			numints++;
	}
	return (isflt(types[index]) && numflts <= 13) || (numints <= 8);
}
void set_flt(ArchRegParms *regparms, int index, void *p, cinv_type_t type) {
	double *toset[] = {
		&(regparms->f1), &(regparms->f2), &(regparms->f3),
		&(regparms->f4), &(regparms->f5), &(regparms->f6),
		&(regparms->f7), &(regparms->f8), &(regparms->f9),
		&(regparms->f10), &(regparms->f11), &(regparms->f12),
		&(regparms->f13)
	};
	
	if (type == CINV_T_FLOAT)
		*toset[index] = *(float *)p;
	else
		*toset[index] = *(double *)p;
}
void set_int(ArchRegParms *regparms, int index, void *p, cinv_type_t type) {
	int *toset[] = {
		&(regparms->r3), &(regparms->r4), &(regparms->r5), 
		&(regparms->r6), &(regparms->r7), &(regparms->r8),
		&(regparms->r9), &(regparms->r10)
	};
	
	if (type == CINV_T_CHAR)
		*(toset[index]) = *(char *)p;
	else if (type == CINV_T_SHORT)
		*(toset[index]) = *(short *)p;
	else if (type == CINV_T_INT)
		*(toset[index]) = *(int *)p;
	else if (type == CINV_T_LONG)
		*(toset[index]) = (int)*(long *)p;
	else if (type == CINV_T_EXTRALONG) {
		*(toset[index]) = (int)((*(long long *)p) >> 32);
		if (index < 7)
			*(toset[index + 1]) = (int)((*(long long *)p) & 0xFFFFFFFF);
	} else
		*(toset[index]) = (int)*(void **)p;
}

void arch_set_register_parms(ArchRegParms *regparms, 
	cinv_callconv_t callingconvention, int num_params, void *parameters[], 
	cinv_type_t types[]) {
	int numints = 0, numflts = 0;
	int i;
	for (i = 0; i < num_params; i++) {
		if (isflt(types[i])) {
			if (numflts < 13)
				set_flt(regparms, numflts, parameters[i], types[i]);
			numflts++;
			if (types[i] == CINV_T_FLOAT)
				numints++;
			else
				numints += 2;
		} else {
			if (numints < 8)
				set_int(regparms, numints, parameters[i], types[i]);
			if (types[i] == CINV_T_EXTRALONG)
				numints += 2;
			else
				numints++;
		}
	}
}

void get_flt(void *po, int index, ArchRegParms *regparms, cinv_type_t type) {
	double *toget[] = {
		&(regparms->f1), &(regparms->f2), &(regparms->f3),
		&(regparms->f4), &(regparms->f5), &(regparms->f6),
		&(regparms->f7), &(regparms->f8), &(regparms->f9),
		&(regparms->f10), &(regparms->f11), &(regparms->f12),
		&(regparms->f13)
	};
	
	if (type == CINV_T_FLOAT)
		*(float *)po = *toget[index];
	else
		*(double *)po = *toget[index];
}
void get_int(void *po, int index, ArchRegParms *regparms, cinv_type_t type) {
	int *toget[] = {
		&(regparms->r3), &(regparms->r4), &(regparms->r5), 
		&(regparms->r6), &(regparms->r7), &(regparms->r8),
		&(regparms->r9), &(regparms->r10)
	};
	
	if (type == CINV_T_CHAR)
		*(char *)po = *toget[index];
	else if (type == CINV_T_SHORT)
		*(short *)po = *toget[index];
	else if (type == CINV_T_INT)
		*(int *)po = *toget[index];
	else if (type == CINV_T_LONG)
		*(long *)po = *toget[index];
	else if (type == CINV_T_EXTRALONG) {
		if (index < 7) {
			*(long long *)po = ((long long)*toget[index]) << 32;
			*(long long *)po |= *toget[index + 1];
		}
	} else
		*(void* *)po = (void *)*toget[index];
}

void arch_get_register_parms(ArchRegParms *regparms,
	cinv_callconv_t callingconvention, int num_params, void *parameters_out[],
	cinv_type_t types[]) {
	int numints = 0, numflts = 0;
	int i;
	for (i = 0; i < num_params; i++) {
		if (isflt(types[i])) {
			if (numflts < 13)
				get_flt(parameters_out[i], numflts, regparms, types[i]);
			numflts++;
			if (types[i] == CINV_T_FLOAT)
				numints++;
			else
				numints += 2;
		} else {
			if (numints < 8)
				get_int(parameters_out[i], numints, regparms, types[i]);
			if (types[i] == CINV_T_EXTRALONG)
				numints += 2;
			else
				numints++;
		}
	}
}

void arch_getval_char(ArchRetValue *archval, char *outval) {
	*outval = archval->ivallo;
}
void arch_getval_short(ArchRetValue *archval, short *outval) {
	*outval = archval->ivallo;
}
void arch_getval_int(ArchRetValue *archval, int *outval) {
	*outval = archval->ivallo;
}
void arch_getval_long(ArchRetValue *archval, long int *outval) {
	*outval = archval->ivallo;
}
void arch_getval_extralong(ArchRetValue *archval, long long int *outval) {
	*outval = archval->ivalhi;
	*outval |= (((long long)archval->ivallo) << 32);
}
void arch_getval_float(ArchRetValue *archval, float *outval) {
	*outval = archval->dval;
}
void arch_getval_double(ArchRetValue *archval, double *outval) {
	*outval = archval->dval;
}
void arch_getval_ptr(ArchRetValue *archval, void **outval) {
	*outval = (void *)archval->ivallo;
}

void arch_setval_char(ArchRetValue *archval, char val) {
	archval->ivallo = val;
}
void arch_setval_short(ArchRetValue *archval, short val) {
	archval->ivallo = val;
}
void arch_setval_int(ArchRetValue *archval, int val) {
	archval->ivallo = val;
}
void arch_setval_long(ArchRetValue *archval, long int val) {
	archval->ivallo = val;
}
void arch_setval_extralong(ArchRetValue *archval, long long int val) {
	archval->ivalhi = (val & 0xFFFFFFFF);
	archval->ivallo = val >> 32;
}
void arch_setval_float(ArchRetValue *archval, float val) {
	archval->dval = val;
}
void arch_setval_double(ArchRetValue *archval, double val) {
	archval->dval = val;
}
void arch_setval_ptr(ArchRetValue *archval, void *val) {
	archval->ivallo = (int)val;
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
	*structalign_out = 8;
}
void arch_size_ptr(int *stacksize_out, int *structsize_out,
	int *structalign_out) {
	*stacksize_out = 4;
	*structsize_out = 4;
	*structalign_out = 4;
}
