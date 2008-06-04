#include <string.h>
#include <dlfcn.h>
#include "ltdl.h"

#include <stdio.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/primitive_indexes.h"

#include "shotgun/lib/subtend/library.h"
#include "shotgun/lib/subtend/nmethod.h"
#include "shotgun/lib/subtend/nmc.h"
#include "shotgun/lib/subtend/ffi.h"

#include "shotgun/lib/strlcpy.h"
#include "shotgun/lib/strlcat.h"


/* Accessor for the default library handle */
extern const ffi_dlhandle_t* const ffi_this_process;


/*
 * Uses dynamic loading to obtain a handle to the symbol from the
 * named shared library. For Mach-O, both .dylib and .bundle are
 * supported. Windows has .dll and any other platform is assumed
 * to use .so.
 *
 * The library name or path should never have a file extension and
 * it is always assumed to be no longer than FFI_MAX_PATH.
 */
static void* ffi_load_function_from_dso(STATE, const OBJECT library, const OBJECT symbol)
{
  const ffi_dlhandle_t* lib = ffi_this_process;

  /*
   * If no library name/path is given, we already have the correct
   * default "handle." Otherwise attempt to load the named library.
   */
  if(!NIL_P(library)) {
    char        dso_name[FFI_MAX_PATH];
    const char* c_library = rbx_string_as_cstr(state, library);
    const char* suffix    = (const char*) FFI_DSO_EXT;

  #ifdef __APPLE_CC__
    try_other_suffix:
  #endif

    bzero(dso_name, FFI_MAX_PATH);

    (void) strlcpy(dso_name, c_library, sizeof(dso_name));
    (void) strlcat(dso_name, suffix, sizeof(dso_name));

    const ffi_dlhandle_t l = ffi_dlopen(dso_name);
    lib = &l;

    /* This name not found or unable to open. Error in ffi_dlerror(). */
    if(!lib) {
      #ifdef __APPLE_CC__
        if(suffix != FFI_DSO_ALT_EXT) {
          suffix = FFI_DSO_ALT_EXT;
          goto try_other_suffix;
        }
      #endif
      return NULL;
    }
  }

  /* Now locate the symbol in the library (or this process.)
   * We may return NULL if not found, user can check ffi_dlerror().
   */
  return((void*) ffi_dlsym(*lib, rbx_string_as_cstr(state, symbol)));
}


void Init_ffi(STATE) {
  OBJECT mod;
  BASIC_CLASS(ffi_ptr) = rbs_class_new(state, "MemoryPointer", 0, BASIC_CLASS(bytearray));
  class_set_object_type(BASIC_CLASS(ffi_ptr), I2N(MemPtrType));

  BASIC_CLASS(ffi_func) = rbs_class_new(state, "NativeFunction", 0, BASIC_CLASS(object));

  mod = rbs_module_new(state, "FFI", BASIC_CLASS(object));
  rbs_const_set(state, mod, "TYPE_OBJECT",   I2N(RBX_FFI_TYPE_OBJECT));
  rbs_const_set(state, mod, "TYPE_CHAR",     I2N(RBX_FFI_TYPE_CHAR));
  rbs_const_set(state, mod, "TYPE_UCHAR",    I2N(RBX_FFI_TYPE_UCHAR));
  rbs_const_set(state, mod, "TYPE_SHORT",    I2N(RBX_FFI_TYPE_SHORT));
  rbs_const_set(state, mod, "TYPE_USHORT",   I2N(RBX_FFI_TYPE_USHORT));
  rbs_const_set(state, mod, "TYPE_INT",      I2N(RBX_FFI_TYPE_INT));
  rbs_const_set(state, mod, "TYPE_UINT",     I2N(RBX_FFI_TYPE_UINT));
  rbs_const_set(state, mod, "TYPE_LONG",     I2N(RBX_FFI_TYPE_LONG));
  rbs_const_set(state, mod, "TYPE_ULONG",    I2N(RBX_FFI_TYPE_ULONG));
  rbs_const_set(state, mod, "TYPE_LL",       I2N(RBX_FFI_TYPE_LL));
  rbs_const_set(state, mod, "TYPE_ULL",      I2N(RBX_FFI_TYPE_ULL));
  rbs_const_set(state, mod, "TYPE_FLOAT",    I2N(RBX_FFI_TYPE_FLOAT));
  rbs_const_set(state, mod, "TYPE_DOUBLE",   I2N(RBX_FFI_TYPE_DOUBLE));
  rbs_const_set(state, mod, "TYPE_PTR",      I2N(RBX_FFI_TYPE_PTR));
  rbs_const_set(state, mod, "TYPE_VOID",     I2N(RBX_FFI_TYPE_VOID));
  rbs_const_set(state, mod, "TYPE_STRING",   I2N(RBX_FFI_TYPE_STRING));
  rbs_const_set(state, mod, "TYPE_STATE",    I2N(RBX_FFI_TYPE_STATE));
  rbs_const_set(state, mod, "TYPE_STRPTR",   I2N(RBX_FFI_TYPE_STRPTR));
  rbs_const_set(state, mod, "TYPE_CHARARR",  I2N(RBX_FFI_TYPE_CHARARR));
}

int ffi_type_size(int type) {
  switch(type) {
    case RBX_FFI_TYPE_CHAR:
    case RBX_FFI_TYPE_UCHAR:
    return sizeof(char);

    case RBX_FFI_TYPE_SHORT:
    case RBX_FFI_TYPE_USHORT:
    return sizeof(short);

    case RBX_FFI_TYPE_INT:
    case RBX_FFI_TYPE_UINT:
    return sizeof(int);

    case RBX_FFI_TYPE_LONG:
    case RBX_FFI_TYPE_ULONG:
    return sizeof(long);

    case RBX_FFI_TYPE_LL:
    case RBX_FFI_TYPE_ULL:
    return sizeof(long long);

    case RBX_FFI_TYPE_FLOAT:
    return sizeof(float);

    case RBX_FFI_TYPE_DOUBLE:
    return sizeof(double);

    case RBX_FFI_TYPE_PTR:
    case RBX_FFI_TYPE_STRING:
    case RBX_FFI_TYPE_STATE:
    case RBX_FFI_TYPE_STRPTR:
    case RBX_FFI_TYPE_OBJECT:
    return sizeof(void*);

    default:

    return -1;
  }
}

OBJECT ffi_function_new(STATE, OBJECT ptr, OBJECT name, int args) {
  OBJECT nf;
  nf = nfunc_allocate(state);
  cmethod_set_primitive(nf, I2N(CPU_PRIMITIVE_NFUNC_CALL));
  cmethod_set_required(nf, I2N(args));
  cmethod_set_serial(nf, I2N(0));
  cmethod_set_name(nf, name);
  cmethod_set_file(nf, symtbl_lookup_cstr(state, state->global->symbols, "<system>"));
  nmethod_set_data(nf, ptr);

  return nf;
}

OBJECT ffi_libffi_generate(STATE, int arg_count, int *arg_types, int ret_type, void *func);


/* The main interface function, handles looking up the pointer in the library,
 * generating the stub, wrapping it up and attaching it to the module.
 */
OBJECT ffi_function_create(STATE, OBJECT library, OBJECT name, OBJECT args, OBJECT ret) {
  void *handle;
  int *arg_types;
  int ret_type;
  int i, tot, arg_count;
  OBJECT ptr, func, sym, type; /* meths; */

  handle = (void*) ffi_load_function_from_dso(state, library, name);

  if(!handle) return Qnil;

  tot = N2I(array_get_total(args));
  arg_count = tot;
  if(tot > 0) {
    arg_types = ALLOC_N(int, tot);

    for(i = 0; i < tot; i++) {
      type = array_get(state, args, i);
      if(!FIXNUM_P(type)) return Qnil;
      arg_types[i] = N2I(type);

      /* State can only be passed as the first arg, and it's invisible,
         ie doesn't get seen as in onbound arg by ruby. But it can ONLY
         be the first arg. */
      if(arg_types[i] == RBX_FFI_TYPE_STATE) {
        if(i == 0) {
          arg_count--;
        } else {
          XFREE(arg_types);
          printf("Invalid arg types.\n");
          return Qnil;
        }
      }
    }
  } else {
    arg_types = NULL;
  }

  ret_type = N2I(ret);

  ptr = ffi_libffi_generate(state, tot, arg_types, ret_type, handle);

  sym = string_to_sym(state, name);
  func = ffi_function_new(state, ptr, sym, arg_count);

  return func;
}

OBJECT ffi_get_field(char *ptr, int offset, int type) {
  OBJECT ret;
  STATE = current_machine->s;

  ptr += offset;

#define READ(type) (*((type*)(ptr)))

  switch(type) {
  case RBX_FFI_TYPE_CHAR:
    ret = I2N(READ(char));
    break;
  case RBX_FFI_TYPE_UCHAR:
    ret = I2N(READ(unsigned char));
    break;
  case RBX_FFI_TYPE_SHORT:
    ret = I2N(READ(short));
    break;
  case RBX_FFI_TYPE_USHORT:
    ret = I2N(READ(unsigned short));
    break;
  case RBX_FFI_TYPE_INT:
    ret = I2N(READ(int));
    break;
  case RBX_FFI_TYPE_UINT:
    ret = I2N(READ(unsigned int));
    break;
  case RBX_FFI_TYPE_LONG:
    ret = I2N(READ(long));
    break;
  case RBX_FFI_TYPE_ULONG:
    ret = I2N(READ(unsigned long));
    break;
  case RBX_FFI_TYPE_FLOAT:
    ret = float_new(state, (double)READ(float));
    break;
  case RBX_FFI_TYPE_DOUBLE:
    ret = float_new(state, READ(double));
    break;
  case RBX_FFI_TYPE_LL:
    ret = LL2N(READ(long long));
    break;
  case RBX_FFI_TYPE_ULL:
    ret = ULL2N(READ(unsigned long long));
    break;
  case RBX_FFI_TYPE_OBJECT:
    ret = READ(OBJECT);
    break;
  case RBX_FFI_TYPE_PTR: {
    void *lptr = READ(void*);
    if(!lptr) {
      ret = Qnil;
    } else {
      ret = ffi_new_pointer(state, lptr);
    }
    break;
  }
  case RBX_FFI_TYPE_STRING: {
    char* result = READ(char*);
    if(result == NULL) {
      ret = Qnil;
    } else {
      ret = string_new(state, result);
    }
    break;
  }
  case RBX_FFI_TYPE_STRPTR: {
    char* result;
    OBJECT s, p;

    result = READ(char*);

    if(result == NULL) {
      s = p = Qnil;
    } else {
      p = ffi_new_pointer(state, result);
      s = string_new(state, result);
    }

    ret = array_new(state, 2);
    array_set(state, ret, 0, s);
    array_set(state, ret, 1, p);
    break;
  }
  default:
  case RBX_FFI_TYPE_VOID:
    ret = Qnil;
    break;
  }

  return ret;
}

void ffi_set_field(char *ptr, int offset, int type, OBJECT val) {
  STATE = current_machine->s;

  ptr += offset;

#define WRITE(type, val) *((type*)ptr) = (type)val

  switch(type) {
  case RBX_FFI_TYPE_CHAR:
    type_assert(val, FixnumType, "converting to char");
    WRITE(char, N2I(val));
    break;
  case RBX_FFI_TYPE_UCHAR:
    type_assert(val, FixnumType, "converting to unsigned char");
    WRITE(unsigned char, N2I(val));
    break;
  case RBX_FFI_TYPE_SHORT:
    type_assert(val, FixnumType, "converting to short");
    WRITE(short, N2I(val));
    break;
  case RBX_FFI_TYPE_USHORT:
    type_assert(val, FixnumType, "converting to unsigned short");
    WRITE(unsigned short, N2I(val));
    break;
  case RBX_FFI_TYPE_INT:
    if(FIXNUM_P(val)) {
      WRITE(int, N2I(val));
    } else {
      type_assert(val, BignumType, "converting to int");
      WRITE(int, bignum_to_i(state, val));
    }
    break;
  case RBX_FFI_TYPE_UINT:
    if(FIXNUM_P(val)) {
      WRITE(unsigned int, N2I(val));
    } else {
      type_assert(val, BignumType, "converting to unsigned int");
      WRITE(unsigned int, bignum_to_i(state, val));
    }
    break;
  case RBX_FFI_TYPE_LONG:
    if(FIXNUM_P(val)) {
      WRITE(long, N2I(val));
    } else {
      type_assert(val, BignumType, "converting to long");
      if(sizeof(long) == sizeof(long long)) {
        WRITE(long, bignum_to_ll(state, val));
      } else {
        WRITE(long, bignum_to_i(state, val));
      }
    }
    break;
  case RBX_FFI_TYPE_ULONG:
    if(FIXNUM_P(val)) {
      WRITE(unsigned long, N2I(val));
    } else {
      type_assert(val, BignumType, "converting to long");
      if(sizeof(long) == sizeof(long long)) {
        WRITE(unsigned long, bignum_to_ll(state, val));
      } else {
        WRITE(unsigned long, bignum_to_i(state, val));
      }
    }
    break;
  case RBX_FFI_TYPE_FLOAT:
    type_assert(val, FloatType, "converting to float");
    WRITE(float, FLOAT_TO_DOUBLE(val));
    break;
  case RBX_FFI_TYPE_DOUBLE:
    type_assert(val, FloatType, "converting to double");
    WRITE(double, FLOAT_TO_DOUBLE(val));
    break;
  case RBX_FFI_TYPE_LL:
    if(FIXNUM_P(val)) {
      WRITE(long long, N2I(val));
    } else {
      type_assert(val, BignumType, "converting to long long");
      WRITE(long long, bignum_to_ll(state, val));
    }
    break;
  case RBX_FFI_TYPE_ULL:
    if(FIXNUM_P(val)) {
      WRITE(unsigned long long, N2I(val));
    } else {
      type_assert(val, BignumType, "converting to unsigned long long");
      WRITE(unsigned long long, bignum_to_ll(state, val));
    }
    break;
  case RBX_FFI_TYPE_OBJECT:
    WRITE(OBJECT, val);
    break;
  case RBX_FFI_TYPE_PTR:
    if(NIL_P(val)) {
      WRITE(void*, NULL);
    } else {
      type_assert(val, MemPtrType, "converting to pointer");
      WRITE(void*, *DATA_STRUCT(val, void**));
    }
    break;
  case RBX_FFI_TYPE_STRING: {
    char* result;
    if(NIL_P(val)) {
      result = NULL;
    } else {
      type_assert(val, StringType, "converting to C string");
      result = rbx_string_as_cstr(state, val);
    }
    WRITE(char*, result);
    break;
  }
  default:
    sassert(0);
  }
}
