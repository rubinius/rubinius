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
This is a skeleton arch .c file, with helpful comments.
If more specific examples are needed, see the other
files in this directory.
*/

#ifdef CINVOKE_BUILD
#include "../cinvoke.h"
#include "../cinvoke-private.h"
#else
#include "cinvoke.h"
#include "cinvoke-private.h"
#endif

void arch_free_errstr(char *str) {
	// TODO: free the given error string.  Note that some archs
	// don't allocate errors on the heap and can leave this blank.
}

cinv_status_t arch_library_create(CInvContext *context, const char *path,
	ArchLibrary *library_out) {
	// TODO: load the shared library given by 'path' and store in
	// the library_out structure.  The below example is for
	// UNIX.  Note that the error is set in the context
	// before returning CINV_ERROR.
	/*void *dl = dlopen(path, RTLD_LAZY);
	if (!dl) {
		context_set_error(context, -1, (char*)dlerror(), 0);
		return CINV_ERROR;
	}
		
	library_out->dl = dl;

	return CINV_SUCCESS;*/
}
cinv_status_t arch_library_get_entrypoint(CInvContext *context,
	ArchLibrary *library, const char *name, void **entrypoint_out) {
	// TODO: load an entrypoint from the given shared library.  Again,
	// example below from UNIX.

	/*void *sym = dlsym(library->dl, name);
	if (!sym) {
		context_set_error(context, -1, (char*)dlerror(), 0);
		return CINV_ERROR;
	}

	*entrypoint_out = sym;

	return CINV_SUCCESS;*/
}
cinv_status_t arch_library_delete(CInvContext *context, ArchLibrary *library) {
	// TODO: delete the library handle and any entrypoints associated with
	// it.
	/*if (dlclose(library->dl)) {
		context_set_error(context, -1, (char*)dlerror(), 0);
		return CINV_ERROR;
	}

	return CINV_SUCCESS;*/
}

//const static int LEN = 4096;

char *arch_callback_stub(void *functionp, void *param,
	short stacksize, cinv_callconv_t cc, cinv_type_t types[], int numparams) {
	// TODO: this function returns a chunk of executable memory
	// which contains a compiled function.  This compiled function
	// performs one task: call the function pointed to by functionp,
	// passing it the argument param.  The cc parameter specifies
	// the calling convention that the compiled function will need
	// to follow.  The stacksize parameter indicates the size of the
	// "true" stack being passed to the function, which is used by
	// calling conventions (such as Windows stdcall) to clean up 
	// their own stacks.

	// in the example below, the function:
	// void f() { ((void (*)(void *))0xAAAAAAAA)((void *)0xBBBBBBBB); }
	// was compiled, loaded in gdb, and then memory dumped to produce
	// the base string value given below.  This string value is then
	// modified to replace the placeholder values with the given
	// parameters.  
	
/*	char *ret = mmap(0, LEN, PROT_EXEC|PROT_READ|PROT_WRITE,
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
	
	return ret;*/
}
void arch_free_stub(char *stub) {
	// TODO: "free" the memory returned by arch_callback_stub
	/*munmap(stub, LEN);*/
}

int arch_is_register_parm(cinv_callconv_t callingconvention, int index,
	int num_params, cinv_type_t types[]) {
	// TODO: given the calling convention, the index (zero-based) of
	// a parameter, the total number of parameters and the types of
	// all the parameters, return 1 if the parameter at said index needs
	// to be passed in a register in the given calling convention, or 0
	// if it should be passed on the stack.
	/*return 0;*/
}
void arch_set_register_parms(ArchRegParms *regparms, 
	cinv_callconv_t callingconvention, int num_params, void *parameters[], 
	cinv_type_t types[]) {
	// TODO: given the parameter values pointed to by the pointers in
	// the 'parameters' array, copy the ones which need to be passed in
	// registers into the regparms structure.
}
void arch_get_register_parms(ArchRegParms *regparms,
	cinv_callconv_t callingconvention, int num_params, void *parameters_out[],
	cinv_type_t types[]) {
	// TODO: this does the opposite of arch_get_register_parms; copy the
	// values in the regparms structure into the memory pointed to by the
	// pointers in the parameters_out array.  The array will already contain
	// pointers to memory areas which are allocated with a size large enough
	// to contain the corresponding types from the 'types' array.  
}

// TODO: the following eight functions marshal return values
// from the given structure and return them for different
// types.  The x86 implementation is provided for reference
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

// TODO: These eight functions do the opposite; set a return
// of a specific type in the archval structure.
// Again, the x86 implementation is provided for reference
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

// TODO: Finally, these eight functions give the sizes
// and alignments for different types.  Two sizes and
// are given, one for the stack when passing
// parameters, and one for values inside a structure
// (for an automatic or heap variable), as well as the alignment
// in a struct.  It is important to differentiate these;
// for example, on x86, a char takes up 1 byte in a struct but
// is pushed on the stack as 4 bytes when passed as an argument.
// Also, gcc for x86 aligns 8-byte integers on a 4-byte boundary
// in structures, but Microsoft cl aligns them on an 8-byte
// boundary.  If you do not set structalign_out, then
// it is implcitly assumed to be 1.
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
