#ifndef __RBX_FFI_H__
#define __RBX_FFI_H__

#include "ltdl.h"
#include <dlfcn.h>

int ffi_type_size(int type);
void Init_ffi(STATE);
OBJECT ffi_function_create(STATE, OBJECT library, OBJECT name, OBJECT args, OBJECT ret);

OBJECT ffi_new_pointer(STATE, void *ptr);
void ffi_autorelease(OBJECT ptr, int ar);
#define ffi_pointer(ptr) (*DATA_STRUCT(ptr, void**))

#define RBX_FFI_TYPE_OBJECT  0
#define RBX_FFI_TYPE_CHAR    1
#define RBX_FFI_TYPE_UCHAR   2
#define RBX_FFI_TYPE_SHORT   3
#define RBX_FFI_TYPE_USHORT  4
#define RBX_FFI_TYPE_INT     5
#define RBX_FFI_TYPE_UINT    6
#define RBX_FFI_TYPE_LONG    7
#define RBX_FFI_TYPE_ULONG   8
#define RBX_FFI_TYPE_LL      9
#define RBX_FFI_TYPE_ULL     10
#define RBX_FFI_TYPE_FLOAT   11
#define RBX_FFI_TYPE_DOUBLE  12
#define RBX_FFI_TYPE_PTR     13
#define RBX_FFI_TYPE_VOID    14
#define RBX_FFI_TYPE_STRING  15
#define RBX_FFI_TYPE_STATE   16
#define RBX_FFI_TYPE_STRPTR  17
#define RBX_FFI_TYPE_CHARARR 18

#define NFUNC_FIELDS 7

#define nfunc_allocate(st) (object_memory_new_object_mature(st->om, st->global->ffi_func, NFUNC_FIELDS))
#define nfunc_get_data(obj) cmethod_get_bytecodes(obj)
#define nfunc_set_data(obj, data) cmethod_set_bytecodes(obj, data)

/*
 * On OS X both .bundle and .dylib are supported for FFI.
 * Subtend only supports .bundle so we reimplement this
 * here. If a bug is found, please also cross-check it.
 * shotgun/lib/subtend/library.c
 */
#ifdef _WIN32
  #define FFI_DSO_EXT ".dll"
#else
  #ifdef __APPLE_CC__
    #define FFI_DSO_EXT  ".dylib"
    #define FFI_DSO_ALT_EXT ".bundle"
  #else  /* POSIX */
    #define FFI_DSO_EXT ".so"
  #endif
#endif

/* Fake an API. FBSD uses dlfcn.h directly, elsewhere we can rely on libltdl. */
#ifdef __FreeBSD__
  #define ffi_dlhandle_t      void*

  #define ffi_dlinit()        /* No expansion */
  #define ffi_dldefault()     RTLD_DEFAULT
  #define ffi_dlopen(lib)     dlopen((lib), RTLD_NOW | RTLD_GLOBAL)
  #define ffi_dlsym(lib, sym) dlsym((ffi_dlhandle_t)(lib), (sym))
  #define ffi_dlclose(lib)    dlclose((lib))
  #define ffi_dlerror()       dlerror()

#else
  #define ffi_dlhandle_t      lt_dlhandle

  #define ffi_dlinit()        lt_dlinit()
  #define ffi_dldefault()     lt_dlopen(NULL)
  #define ffi_dlopen(lib)     lt_dlopen((lib))
  #define ffi_dlsym(lib, sym) lt_dlsym((ffi_dlhandle_t)(lib), (sym))
  #define ffi_dlclose(lib)    lt_dlclose((lib))
  #define ffi_dlerror()       lt_dlerror()
#endif

/* Internal defs */
#define FFI_MAX_PATH 128

int ffi_get_arg_count(OBJECT ptr);

#endif                      /* __RBX_FFI_H__ */
