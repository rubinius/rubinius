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
#include "cinvoke.h"
#include "cinvoke-private.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifndef _WIN32
#define CDECL
#endif

void context_set_error(CInvContext *context, int code, char *str, int needsfree) {
	if (context->needsfree)
		arch_free_errstr(context->errormsg);
	context->errorcode = code;
	context->errormsg = str;
	context->needsfree = needsfree;
}
void context_set_nomem(CInvContext *context) {
	context_set_error(context, CINV_E_NOMEM, CINV_S_NOMEM, CINV_NOMEM_NEEDSFREE);
}
void context_clear_error(CInvContext *context) {
	context_set_error(context, 0, "call succeeded", 0);
}

CInvContext *cinv_context_create() {
	CInvContext *context = malloc(sizeof(CInvContext));
	if (!context)
		return NULL;

	context->needsfree = 0;
	context_clear_error(context);

	return context;
}
cinv_status_t cinv_context_delete(CInvContext *context) {
	if (context->needsfree)
		arch_free_errstr(context->errormsg);
	free(context);
	return CINV_SUCCESS;
}

const char *cinv_context_geterrormsg(CInvContext *context) {
	return context->errormsg;
}
cinv_int32_t cinv_context_geterrorcode(CInvContext *context) {
	return context->errorcode;
}

CInvLibrary *cinv_library_create(CInvContext *context, const char *path) {
	CInvLibrary *library = malloc(sizeof(CInvLibrary));
	if (!library) {
		context_set_nomem(context);
		return NULL;
	}

	if (!arch_library_create(context, path, &library->arch)) {
		free(library);
		return NULL;
	}

	context_clear_error(context);
	return library;
}
void *cinv_library_load_entrypoint(CInvContext *context,
	CInvLibrary *library, const char *name) {
	void *ep;

	if (!arch_library_get_entrypoint(context, &library->arch, name,
		&ep)) {
		return NULL;
	}

	context_clear_error(context);
	return ep;
}
cinv_status_t cinv_library_delete(CInvContext *context, CInvLibrary *library) {
	if (!arch_library_delete(context, &library->arch))
		return CINV_ERROR;
	
	free(library);
	context_clear_error(context);
	return CINV_SUCCESS;
}

static char _typeformats[CINV_NUM_TYPES] = {
	'c', 's', 'i', 'l', 'e', 'f', 'd', 'p'
};

cinv_status_t parse_type(char fmt, cinv_type_t *typeout) {
	int i;
	char lower = tolower(fmt);
	if (lower == '2') {
		*typeout = CINV_T_2BYTE;
		return CINV_SUCCESS;
	}
	if (lower == '4') {
		*typeout = CINV_T_4BYTE;
		return CINV_SUCCESS;
	}
	if (lower == '8') {
		*typeout = CINV_T_8BYTE;
		return CINV_SUCCESS;
	}
	for (i = 0; i < CINV_NUM_TYPES; i++) {
		if (_typeformats[i] == lower) {
			*typeout = (cinv_type_t)i;
			return CINV_SUCCESS;
		}
	}
	return CINV_ERROR;
}

typedef void (*sizefunc)(int *, int *, int *);

static sizefunc _sizefuncs[CINV_NUM_TYPES] = {
	arch_size_char,
	arch_size_short,
	arch_size_int,
	arch_size_long,
	arch_size_extralong,
	arch_size_float,
	arch_size_double,
	arch_size_ptr
};

void get_size(cinv_type_t type, int *stacksize, int *structsize,
	int *structalign) {
	sizefunc func;
	*structalign = 1;
	if (type < 0 || type > CINV_NUM_TYPES - 1)
		fprintf(stderr, "cinvoke developer error: unknown type %d\n", type);
	func = _sizefuncs[type];
	func(stacksize, structsize, structalign);
}

typedef void (*pullfunc)(ArchRetValue*, void*);

static pullfunc _pullfuncs[CINV_NUM_TYPES] = {
	(pullfunc)arch_getval_char,
	(pullfunc)arch_getval_short,
	(pullfunc)arch_getval_int,
	(pullfunc)arch_getval_long,
	(pullfunc)arch_getval_extralong,
	(pullfunc)arch_getval_float,
	(pullfunc)arch_getval_double,
	(pullfunc)arch_getval_ptr
};

void pull_value(ArchRetValue *val, cinv_type_t type, void *valout) {
	pullfunc func;
	if (type < 0 || type > CINV_NUM_TYPES - 1)
		fprintf(stderr, "cinvoke developer error: unknown type %d\n", type);
	func = _pullfuncs[type];
	func(val, valout);
}

void set_value(ArchRetValue *archval, cinv_type_t type, void *val) {
	switch (type) {
	case CINV_T_CHAR:
		arch_setval_char(archval, *(char *)val);
		break;
	case CINV_T_SHORT:
		arch_setval_short(archval, *(short *)val);
		break;
	case CINV_T_INT:
		arch_setval_int(archval, *(int *)val);
		break;
	case CINV_T_LONG:
		arch_setval_long(archval, *(long *)val);
		break;
	case CINV_T_EXTRALONG:
		arch_setval_extralong(archval, *(long long int *)val);
		break;
	case CINV_T_FLOAT:
		arch_setval_float(archval, *(float *)val);
		break;
	case CINV_T_DOUBLE:
		arch_setval_double(archval, *(double *)val);
		break;
	case CINV_T_PTR:
		arch_setval_ptr(archval, *(void **)val);
		break;
	default:
		fprintf(stderr, "cinvoke developer error: unknown type %d\n", type);
		break;
	}
}

CInvFunction *cinv_function_create(CInvContext *context,
	cinv_callconv_t callingconvention, const char *returnformat,
	const char *parameterformat) {
	int index;
	CInvFunction *function = malloc(sizeof(CInvFunction));
	if (!function) {
		context_set_nomem(context);
		return NULL;
	}

	function->callconv = callingconvention;
	function->stacksize = 0;
	
	if (!(*returnformat))
		function->hasreturn = 0;
	else {
		function->hasreturn = 1;
		if (!parse_type(*returnformat, &function->rettype)) {
			free(function);
			context_set_error(context, CINV_E_INVAL,
				"The return value format was not recognized", 0);
			return NULL;
		}
	}

	function->numparms = (int)strlen(parameterformat);
	if (function->numparms < 0) {
		free(function);
		context_set_error(context, CINV_E_INVAL,
			"Too many parameters", 0);
		return NULL;
	}
	function->parmstacksizes = NULL;
	function->parmmemsizes = NULL;
	function->parmtypes = NULL;
	function->regparms = NULL;
	if (function->numparms > 0) {
		function->parmstacksizes = malloc(function->numparms * sizeof(int));
		function->parmmemsizes = malloc(function->numparms * sizeof(int));
		function->parmtypes = malloc(function->numparms * sizeof(cinv_type_t));
		function->regparms = malloc(function->numparms * sizeof(int));

		if (!function->parmstacksizes || !function->parmmemsizes
			|| !function->parmtypes) {
			free(function->parmstacksizes);
			free(function->parmmemsizes);
			free(function->parmtypes);
			free(function->regparms);
			free(function);
			context_set_nomem(context);
			return NULL;
		}

		for (index = 0; index < function->numparms; index++) {
			int sz, memsize, dummy;
			cinv_type_t type;
			if (!parse_type(parameterformat[index], &type)) {
				free(function->parmstacksizes);
				free(function->parmmemsizes);
				free(function->parmtypes);
				free(function->regparms);
				free(function);
				context_set_error(context, CINV_E_INVAL,
					"The parameter format was not recognized", 0);
				return NULL;
			}

			get_size(type, &sz, &memsize, &dummy);
			function->parmstacksizes[index] = sz;
			function->parmmemsizes[index] = memsize;
			function->parmtypes[index] = type;
		}

		for (index = 0; index < function->numparms; index++) {
			function->regparms[index] = arch_is_register_parm(
				function->callconv, index,
				function->numparms, function->parmtypes);
#if !ARCH_REGPARMS_IN_STACKSIZE
			if (function->regparms[index])
				continue;
#endif

			function->stacksize += function->parmstacksizes[index];

			if (function->stacksize < 0) {
				free(function->parmstacksizes);
				free(function->parmmemsizes);
				free(function->parmtypes);
				free(function->regparms);
				free(function);
				context_set_error(context, CINV_E_INVAL,
					"Too many parameters", 0);
				return NULL;
			}
		}
	}

	context_clear_error(context);
	return function;
}
cinv_status_t cinv_function_delete(CInvContext *context,
	CInvFunction *function) {
	free(function->parmstacksizes);
	free(function->parmmemsizes);
	free(function->parmtypes);
	free(function->regparms);
	free(function);
	context_clear_error(context);
	return CINV_SUCCESS;
}

#define MEMCPY_MACRO(dest, src, len) \
while (len) { \
	*(dest) = *(src); \
	(dest)++; \
	(src)++; \
	(len)--; \
}

#define MEMZERO_MACRO(dest, len) \
while (len) { \
	*(dest) = '\0'; \
	(dest)++; \
	(len)--; \
}

cinv_status_t cinv_function_invoke(CInvContext *context,
	CInvFunction *function, void *entrypoint,
	void *returnvalout, void **parameters) {
	ArchRetValue retval;
	ArchRegParms regparms;
	int i;
	int stacksize = function->stacksize;
	char *stackptr;

	arch_set_register_parms(&regparms, function->callconv,
		function->numparms, parameters, function->parmtypes);
	
	if (stacksize) {
		ARCH_PUT_STACK_BYTES(stacksize);
		ARCH_GET_STACK(stackptr);

#if ARCH_STACK_SKIPTOP
	#if ARCH_STACK_GROWS_DOWN
		stackptr += ARCH_STACK_SKIPTOP;
	#else
		stackptr -= ARCH_STACK_SKIPTOP;
	#endif
#endif

		for (i = 0; i < function->numparms; i++) {
			int len = function->parmstacksizes[i];
			int memsize = function->parmmemsizes[i];
			int regparm = function->regparms[i];
			int diff = len - memsize;
			char *param = parameters[i];
	
#if !ARCH_REGPARMS_IN_STACKSIZE
			if (regparm)
				continue;
#endif

#if ARCH_REGPARMS_IN_STACKSIZE
			if (regparm) {
	#if ARCH_STACK_GROWS_DOWN
				stackptr += len;
	#else
				stackptr -= len;
	#endif
				continue;
			}
#endif

#if !ARCH_STACK_GROWS_DOWN
			stackptr -= len;
#endif
#if ARCH_BIG_ENDIAN
			MEMZERO_MACRO(stackptr, diff)
			MEMCPY_MACRO(stackptr, param, memsize)
#else
			MEMCPY_MACRO(stackptr, param, memsize)
			MEMZERO_MACRO(stackptr, diff)
#endif
#if !ARCH_STACK_GROWS_DOWN
			stackptr -= len;
#endif
		}
	}

	ARCH_CALL(regparms, entrypoint)
	ARCH_SAVE_RETURN(retval)

	if (function->callconv != CINV_CC_STDCALL && stacksize != 0) {
		ARCH_REMOVE_STACK_BYTES(stacksize)
	}

	if (function->hasreturn)
		pull_value(&retval, function->rettype, returnvalout);
	
	context_clear_error(context);
	return CINV_SUCCESS;
}

void CDECL cinv_cbthunk(CInvCallback *cb) {
	long long int returnptr;
	ArchRetValue retval;
	void **parameters;
	char *frameptr;
	int i;
	ArchRegParms regparms;

	ARCH_SAVE_REGPARMS(regparms);

	ARCH_GET_FRAME_PTR(frameptr)

	parameters = NULL;
	
#if ARCH_STACK_GROWS_DOWN
		frameptr += ARCH_CALLBACK_ARG_OFFSET;
#else
		frameptr -= ARCH_CALLBACK_ARG_OFFSET;
#endif

	if (cb->prototype->numparms) {
		parameters = malloc(sizeof(void*) * cb->prototype->numparms);
		if (!parameters)
			return;
		for (i = 0; i < cb->prototype->numparms; i++) {
			int memsize = cb->prototype->parmmemsizes[i];
			int stacksize = cb->prototype->parmstacksizes[i];
			int diff = stacksize - memsize;
			diff = diff;

			parameters[i] = malloc(memsize);
			if (!parameters[i]) {
				int j;
				for (j = 0; j < i; j++)
					free(parameters[j]);
				free(parameters);
				return;
			}

#if ARCH_REGPARMS_IN_STACKSIZE
			if (cb->prototype->regparms[i]) {
	#if ARCH_STACK_GROWS_DOWN
				frameptr += stacksize;
	#else
				frameptr -= stacksize;
	#endif
				continue;
			}
#else
			if (cb->prototype->regparms[i])
				continue;
#endif

#if ARCH_STACK_GROWS_DOWN
	#if ARCH_BIG_ENDIAN
			frameptr += diff;
			memcpy(parameters[i], frameptr, memsize);
			frameptr += memsize;
	#else
			memcpy(parameters[i], frameptr, memsize);
			frameptr += stacksize;
	#endif
#else
			frameptr -= stacksize;
	#if ARCH_BIG_ENDIAN
			frameptr += diff;
			memcpy(parameters[i], frameptr, memsize);
			frameptr -= diff;
	#else
			memcpy(parameters[i], frameptr, memsize);
	#endif
#endif
		}

		arch_get_register_parms(&regparms, cb->prototype->callconv,
			cb->prototype->numparms, parameters, cb->prototype->parmtypes);
	}

	cb->cbfunc(cb->prototype, parameters, &returnptr, cb->userdata);
	
	if (cb->prototype->hasreturn) {
		set_value(&retval, cb->prototype->rettype, &returnptr);
		ARCH_SET_RETURN(retval)
	}
}

CInvCallback *cinv_callback_create(CInvContext *context,
	CInvFunction *prototype, void *userdata, cinv_cbfunc_t cbfunc) {
	CInvCallback *ret = malloc(sizeof(CInvCallback));
	if (!ret) {
		context_set_nomem(context);
		return NULL;
	}
	ret->stub = arch_callback_stub(cinv_cbthunk, ret, prototype->stacksize,
		prototype->callconv, prototype->parmtypes, prototype->numparms);
	if (!ret->stub) {
		free(ret);
		context_set_nomem(context);
		return NULL;
	}

	ret->prototype = prototype;
	ret->userdata = userdata;
	ret->cbfunc = cbfunc;
	
	context_clear_error(context);
	return ret;
}
void *cinv_callback_getentrypoint(CInvContext *context,
	CInvCallback *callback) {
	context_clear_error(context);
	return callback->stub;
}
cinv_status_t cinv_callback_delete(CInvContext *context,
	CInvCallback *callback) {
	arch_free_stub(callback->stub);
	free(callback);
	context_clear_error(context);
	return CINV_SUCCESS;
}
