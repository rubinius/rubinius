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
#ifndef _CINVOKE_H
#define _CINVOKE_H

#define CINVOKE_VERSION_MAJOR 0
#define CINVOKE_VERSION_MINOR 9

#include "cinvoke-arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**

\mainpage C/Invoke Reference Documentation

\section general General Information

To use C/Invoke in an application such as a language interpreter, #include
<cinvoke.h> in your source code, and link with the cinvoke.[a|lib] static
library.  No dynamic library is available at this time to avoid potential
binary incompatibility issues should the API change in the future.

C/Invoke uses certain conventions throughout its API.  All structures in
C/Invoke are considered opaque; their members should never be accessed
directly.  Types are generally allocated on the heap with a *_create function,
and each type allocated this way has a corresponding *_delete function.  All
functions return an error status indicator.  This is usually in the form of a
cinv_status_t code, which can be either CINV_SUCCESS (1) or CINV_ERROR (0).
However, if a function returns a pointer, the function will return a NULL
value to indicate errors, in order to avoid redundant parameters.

The C/Invoke API is broken down into several groups: contexts, libraries,
functions, structures, and callbacks.

\section contexts Context Functions

The cinv_context_* functions create, manipulate and destroy CInvContext*
objects.  A C/Invoke Context is an opaque data structure which is required
for all C/Invoke functions to operate.  You could consider a call to
cinv_context_create as being analagous to initializing the library, except
that this function returns a new value each time it is called, allowing
the library to be thread-safe.

Contexts can be used to fetch the last error message and code associated with
a failed function call.

\section libraries Library Functions

The cinv_library_* functions provide a cross-platform method of loading
shared libraries.  A library once loaded can be used to load entrypoints
(otherwise known as symbols or function pointers).

\section functions Function (Prototype) Functions

In C/Invoke, a Function object represents the information needed to call a
C function, similar to a prototype in a header file.  Once being created,
a function can be passed to cinv_function_invoke, along with an entrypoint and
the requisite parameters, to make a call through that entrypoint at runtime.

\section structures Structure Functions

The cinv_structure_* functions provide a cross-platform interface for
manipulating C structure values at runtime.  After building a description of
the members of a structure, instances of the structure can be allocated and
its values set so that it can be passed as a parameter to an invoked function.

\section callbacks Callback Functions

Some C library functions take function pointers as arguments, which they then
"callback" at a later time.  Using the cinv_callback_* functions, an object
can be created which can be used to provide a function pointer for this
purpose.  Creating a callback requires a C/Invoke Function object for a
description, as well as a pointer to a generic function which will be
called in place of the "fake" callback.

\section more More Information

For more detailed information, see the "Files" link above to browse the
available functions in the API, or visit
http://savannah.nongnu.org/projects/cinvoke for information on joining our
mailing list.
*/

/**
* \file cinvoke.h
* Main C/Invoke Header
*/

typedef struct _CInvLibrary {
	ArchLibrary arch;
} CInvLibrary;

typedef struct _CInvFunction {
	cinv_callconv_t callconv;
	int hasreturn;
	cinv_type_t rettype;
	int numparms;
	int *parmstacksizes;
	int *parmmemsizes;
	cinv_type_t *parmtypes;
	int *regparms;
	short stacksize;
} CInvFunction;

/** Function type which must be implemented when invoking interfaces
* which use callbacks.
* \param[in] f The function object which describes the prototype of
* the callback.
* \param[in] parameters An array of pointers to the callback arguments.
* \param[out] returnout A pointer to a value which should be set to the
* return value.  Should be ignored if there is no return value.
* \param[in] userdata A user-defined pointer to arbitrary data.
*/
typedef void (*cinv_cbfunc_t)(CInvFunction *f, void *parameters[],
	void *returnout, void *userdata);

struct _CInvStructure;
struct hashtable;

typedef struct _CInvStructMember {
	struct _CInvStructure *structtype;
	cinv_type_t type;
	int offset;
} CInvStructMember;

typedef struct _CInvStructure {
	struct hashtable *members;
	int nextoffset;
	int finished;
	int alignment;
} CInvStructure;

typedef struct _CInvCallback {
	CInvFunction *prototype;
	void *userdata;
	cinv_cbfunc_t cbfunc;
	char *stub;
} CInvCallback;

///////////////////
// public functions
///////////////////
/** Creates a new C/Invoke context
* \return A new C/Invoke context object, or NULL if an error occurs.
*/
CInvContext *cinv_context_create();
/** Gets the error message corresponding to the error which occurred
when a C/Invoke function fails.
* \param[in] context A C/Invoke context.
* \return An error message.
*/
const char *cinv_context_geterrormsg(CInvContext *context);
/** Gets the error code corresponding to the error which occurred when
* a C/Invoke function fails, or 0 if the last call succeeded.
* The code returned will be platform-specific.
* \param[in] context A C/Invoke context.
* \return An error code, or 0 if no error occurred.
*/
cinv_int32_t cinv_context_geterrorcode(CInvContext *context);
/** Deletes a C/Invoke context
* \param[in] context A C/Invoke context.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_context_delete(CInvContext *context);

/** Loads a shared library and creates a library object.
* \param[in] context A C/Invoke context.
* \param[in] path A filesystem path to load. The semantics of how
* libraries are searched for are system-specific.  For maximum
* portability, always specify a fully-qualified path.
* \return A new library object, or NULL if an error occurred.
*/
CInvLibrary *cinv_library_create(CInvContext *context, const char *path);
/** Loads an entrypoint (function pointer) from a shared library.
* \param[in] context A C/Invoke context.
* \param[in] library The library to load from.
* \param[in] name The name of the symbol (function) to load.
* \return A pointer to the entrypoint, or NULL if an error occurred.
*/
void *cinv_library_load_entrypoint(CInvContext *context,
	CInvLibrary *library, const char *name);
/** Deletes a library object
* \param[in] context A C/Invoke context.
* \param[in] library The library to delete.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_library_delete(CInvContext *context, CInvLibrary *library);

/** Creates a function object, which descibes the prototype of a function
* which can be invoked.
* \param[in] context A C/Invoke context.
* \param[in] callingconvention A calling convention to use. The value
* CINV_CC_DEFAULT is aliased to the default calling convention for the
* current platform.
* \param[in] returnformat A string containing a character type code, or an
* empty string for functions returning 'void'. See the parameterformat
* documentation below for a list of type codes.
* \param[in] parameterformat A string containing zero or more characters,
* each character representing the type of a parameter to the function. The
* available type codes are:
* \li \b c - char type.
* \li \b s - short type.
* \li \b i - int type.
* \li \b l - long type.
* \li \b e - long long type.
* \li \b f - float type.
* \li \b d - double type.
* \li \b p - any pointer type.
* \li \b 2 - the integer type which uses 2 bytes on the current platform.
* \li \b 4 - the integer type which uses 4 bytes on the current platform.
* \li \b 8 - the integer type which uses 8 bytes on the current platform.
*
* Note that functions accepting or returning structures are not supported,
* only pointers to structures.
* \par Example
* A function with the following prototype:
* \code
* int myfunction(char *ptr, float f);
* \endcode
* Could be captured using the following call:
* \code
* cinv_function_create(ctx, CINV_CC_DEFAULT, "i", "pf");
* \endcode
* \return A new function object, or NULL if an error occurred.
*/
CInvFunction *cinv_function_create(CInvContext *context,
	cinv_callconv_t callingconvention, const char *returnformat,
	const char *parameterformat);
/** Calls the entrypoint given using the prototype specified.
* \param[in] context A C/Invoke context.
* \param[in] function The prototype of the function being called.
* \param[in] entrypoint The function pointer to call through.
* \param[out] returnvalout A pointer to the return value which
* will be set when the function is called.  Can be NULL if the
* called function returns 'void'.  The type of the pointer must
* match the return type and the memory must be allocated by the
* caller.
* \param[in] parameters An array of pointers to the parameters
* of the function.  Can be NULL if the function takes no parameters.
* Note that each parameter is passed via a pointer, so if the value
* being passed is for a pointer, place a pointer to the pointer
* in the array.
* The caller is responsible for allocating and de-allocating the
* memory for the parameters.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_function_invoke(CInvContext *context,
	CInvFunction *function, void *entrypoint, void *returnvalout,
	void *parameters[]);
/** Deletes a function object.
* \param[in] context A C/Invoke context.
* \param[in] function The function to delete.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_function_delete(CInvContext *context,
	CInvFunction *function);

/** Creates a new structure description.
* \param[in] context A C/Invoke context.
* \return A new structure description object, or NULL if an error occurred.
*/
CInvStructure *cinv_structure_create(CInvContext *context);
/** Adds a simple type member to a structure description.
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description to add to.
* \param[in] name The name of the member to add.
* \param[in] type The type of the member to add.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_addmember_value(CInvContext *context,
	CInvStructure *structure, const char *name, cinv_type_t type);
/** Adds a structure type member to a structure description.  Note that
* this is distinct from adding a pointer to a structure, to do that one
* would call cinv_structure_addmember_value with a CINV_T_PTR argument.
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description to add to.
* \param[in] name The name of the member to add.
* \param[in] type A structure description describing the type of the
* member to add.  This description must be finished.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_addmember_struct(CInvContext *context,
	CInvStructure *structure, const char *name, CInvStructure *type);
/** Finishes a structure, completing its description.  This function
* must be called before using the structure description object.
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description to finish.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_finish(CInvContext *context,
	CInvStructure *structure);
/** Returns the size, in bytes, of a structure.
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description.
* \param[out] size_out The number of bytes required to hold the structure
* in memory.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_getsize(CInvContext *context,
	CInvStructure *structure, int *size_out);
/** Allocates memory for a structure instance.
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description to allocate memory for.
* \return A pointer to memory which can hold the given structure.
*/
void *cinv_structure_create_instance(CInvContext *context,
	CInvStructure *structure);
/** Sets the value of a member inside of a structure.  
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description corresponding to
* the given instance.
* \param[in] instance The structure instance to set the member
* value of.
* \param[in] name The name of the member to set.
* \param[in] value_ptr A pointer to the value to set.  If the value
* being set is a pointer, you should pass a pointer to a pointer. If
* the value being set is an embedded structure, create a structure
* instance with cinv_structure_create_instance and pass the instance
* pointer.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_instance_setvalue(CInvContext *context,
	CInvStructure *structure, void *instance, const char *name,
	void *value_ptr);
/** Gets a pointer to the value of a member inside of a structure.  
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description corresponding to
* the given instance.
* \param[in] instance The structure instance to get the member
* value from.
* \param[in] name The name of the member to get.
* \return A pointer to the value, or NULL if an error occurred.
* If the value being returned is a pointer type, this will be a
* pointer to that pointer. If the value being returned is an
* embedded structure, this will be an instance pointer.
* In all cases, the pointer being returned is internal to the
* structure instance and does not need to be deallocated separately.
*/
void *cinv_structure_instance_getvalue(CInvContext *context,
	CInvStructure *structure, void *instance, const char *name);
/** Frees the memory associated with a structure instance.
* \param[in] context A C/Invoke context.
* \param[in] instance The structure instance to delete.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_delete_instance(CInvContext *context,
	void *instance);
/** Deletes a structure description.  Note that the descriptions of
* any embedded structure members are not deleted.
* \param[in] context A C/Invoke context.
* \param[in] structure The structure description to delete.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_structure_delete(CInvContext *context,
	CInvStructure *structure);

/** Creates a callback object.
* \param[in] context A C/Invoke context.
* \param[in] prototype A function object which specifies the
* prototype of the callback.
* \param[in] userdata A pointer which can be used to store
* user-defined data to be passed to the callback.
* \param[in] cbfunc The function to forward calls to.
* \return A new callback object, or NULL if an error occurred.
*/
CInvCallback *cinv_callback_create(CInvContext *context,
	CInvFunction *prototype, void *userdata, cinv_cbfunc_t cbfunc);
/** Returns a function pointer which can be passed as a callback
* to a function being invoked.
* \param[in] context A C/Invoke context.
* \param[in] callback A callback object.
* \return A function pointer.
*/
void *cinv_callback_getentrypoint(CInvContext *context,
	CInvCallback *callback);
/** Deletes a callback object.
* \param[in] context A C/Invoke context.
* \param[in] callback A callback object.
* \return A standard C/Invoke status code.
*/
cinv_status_t cinv_callback_delete(CInvContext *context,
	CInvCallback *callback);
///////////////////
///////////////////
#ifdef __cplusplus
}
#endif

#endif
