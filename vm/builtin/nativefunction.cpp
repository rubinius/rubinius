/* A simple C++ wrapper around libffi. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "prelude.hpp"
#include "object.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "vmnativefunction.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/memorypointer.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "ffi.hpp"
#include "message.hpp"

#include "strlcpy.hpp"
#include "strlcat.hpp"

#ifdef _WIN32
#define LIBSUFFIX ".dll"
#else
#ifdef __APPLE_CC__
#define LIBSUFFIX ".bundle"
#define LIBSUFFIX2 ".dylib"
#else
#define LIBSUFFIX ".so"
#endif
#endif

#include "ltdl.h"

#ifdef CONFIG_USE_LTDL
#define dlhandle lt_dlhandle
#define xdlopen(name) lt_dlopen(name)
#define xdlsym lt_dlsym
#define xdlsym2(name) lt_dlsym(NULL, name)
#define xdlerror lt_dlerror
#else
#define dlhandle void*
#define xdlopen(name) dlopen(name, RTLD_NOW | RTLD_GLOBAL)
#define xdlsym dlsym
#define xdlsym2(name) dlsym(RTLD_DEFAULT, name)
#define xdlerror dlerror
#endif


#include "vm/builtin/nativefunction.hpp"

namespace rubinius {

  void NativeFunction::init(STATE) {
    GO(native_function).set(state->new_class("NativeFunction", G(executable),
          NativeFunction::fields));
    G(native_function)->set_object_type(state, NativeFuncType);

    G(rubinius)->set_const(state, "LIBSUFFIX", String::create(state, LIBSUFFIX));
  }

  size_t NativeFunction::type_size(size_t type) {
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

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG:
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

  FIXNUM NativeFunction::type_size_prim(STATE, FIXNUM type) {
    return Fixnum::from(NativeFunction::type_size(type->to_native()));
  }

  void* NativeFunction::find_symbol(STATE, OBJECT opath, String* name) {
    dlhandle lib;
    void *ep;
    char sys_name[128];

    if(opath->nil_p()) {
      lib = xdlopen(NULL);
      if(!lib) return NULL;
    } else {
      String* path = as<String>(opath);

      /* path is a string like 'ext/gzip', we turn that into 'ext/gzip.so'
         or whatever the library suffix is. */
      memset(sys_name, 0, 128);
      strlcpy(sys_name, path->byte_address(), sizeof(sys_name));
      strlcat(sys_name, LIBSUFFIX, sizeof(sys_name));

      /* Open it up. If this fails, then we just pretend like
         the library isn't there. */
      lib = xdlopen(sys_name);
      if(!lib) {
#ifdef LIBSUFFIX2
        memset(sys_name, 0, 128);
        strlcpy(sys_name, path->byte_address(), sizeof(sys_name));
        strlcat(sys_name, LIBSUFFIX2, sizeof(sys_name));

        lib = xdlopen(sys_name);
        if(!lib) return NULL;
#else
          return NULL;
#endif
      }
    }

    ep = xdlsym(lib, name->byte_address());
    if(!ep) {
      ep = xdlsym2(name->byte_address());
    }
    return ep;
  }

  NativeFunction* NativeFunction::create(STATE, OBJECT name, int args) {
    NativeFunction* nf = (NativeFunction*)state->new_object(G(native_function));
    nf->primitive(state, state->symbol("nativefunction_call"));
    nf->required(state, Fixnum::from(args));
    nf->serial(state, Fixnum::from(0));
    nf->name(state, name);
    nf->file(state, state->symbol("<system>"));
    nf->data(state, (MemoryPointer*)Qnil);

    nf->executable = VMNativeFunction::create(state); // HACK stupid

    return nf;
  }

  void NativeFunction::bind(STATE, int arg_count, int *arg_types,
      int ret_type, void *func) {

    ffi_type **types;
    ffi_status status;
    ffi_type *rtype;
    struct ffi_stub *stub;
    int i;

    types = ALLOC_N(ffi_type*, arg_count);

    for(i = 0; i < arg_count; i++) {
      switch(arg_types[i]) {
      case RBX_FFI_TYPE_CHAR:
        types[i] = &ffi_type_schar;
        break;
      case RBX_FFI_TYPE_UCHAR:
        types[i] = &ffi_type_uchar;
        break;
      case RBX_FFI_TYPE_SHORT:
        types[i] = &ffi_type_sshort;
        break;
      case RBX_FFI_TYPE_USHORT:
        types[i] = &ffi_type_ushort;
        break;
      case RBX_FFI_TYPE_INT:
        types[i] = &ffi_type_sint;
        break;
      case RBX_FFI_TYPE_UINT:
        types[i] = &ffi_type_uint;
        break;
      case RBX_FFI_TYPE_LONG:
        types[i] = &ffi_type_slong;
        break;
      case RBX_FFI_TYPE_ULONG:
        types[i] = &ffi_type_ulong;
        break;
      case RBX_FFI_TYPE_FLOAT:
        types[i] = &ffi_type_float;
        break;
      case RBX_FFI_TYPE_DOUBLE:
        types[i] = &ffi_type_double;
        break;
      case RBX_FFI_TYPE_LONG_LONG:
        types[i] = &ffi_type_sint64;
        break;
      case RBX_FFI_TYPE_ULONG_LONG:
        types[i] = &ffi_type_uint64;
        break;
      case RBX_FFI_TYPE_OBJECT:
      case RBX_FFI_TYPE_PTR:
      case RBX_FFI_TYPE_STRING:
      case RBX_FFI_TYPE_STATE:
        types[i] = &ffi_type_pointer;
        break;
      }
    }

    switch(ret_type) {
    case RBX_FFI_TYPE_CHAR:
      rtype = &ffi_type_schar;
      break;
    case RBX_FFI_TYPE_UCHAR:
      rtype = &ffi_type_uchar;
      break;
    case RBX_FFI_TYPE_SHORT:
      rtype = &ffi_type_sshort;
      break;
    case RBX_FFI_TYPE_USHORT:
      rtype = &ffi_type_ushort;
      break;
    case RBX_FFI_TYPE_INT:
      rtype = &ffi_type_sint;
      break;
    case RBX_FFI_TYPE_UINT:
      rtype = &ffi_type_uint;
      break;
    case RBX_FFI_TYPE_LONG:
      rtype = &ffi_type_slong;
      break;
    case RBX_FFI_TYPE_ULONG:
      rtype = &ffi_type_ulong;
      break;
    case RBX_FFI_TYPE_FLOAT:
      rtype = &ffi_type_float;
      break;
    case RBX_FFI_TYPE_DOUBLE:
      rtype = &ffi_type_double;
      break;
    case RBX_FFI_TYPE_LONG_LONG:
      rtype = &ffi_type_sint64;
      break;
    case RBX_FFI_TYPE_ULONG_LONG:
      rtype = &ffi_type_uint64;
      break;
    case RBX_FFI_TYPE_OBJECT:
    case RBX_FFI_TYPE_PTR:
    case RBX_FFI_TYPE_STRING:
    case RBX_FFI_TYPE_STATE:
    case RBX_FFI_TYPE_STRPTR:
      rtype = &ffi_type_pointer;
      break;
    default:
    case RBX_FFI_TYPE_VOID:
      rtype = &ffi_type_void;
      break;
    }

    stub = ALLOC_N(struct ffi_stub, 1);
    stub->ret_type = ret_type;
    stub->ep = func;
    stub->arg_count = arg_count;
    stub->arg_types = ALLOC_N(int, arg_count);
    memcpy(stub->arg_types, arg_types, sizeof(int) * arg_count);
    status = ffi_prep_cif(&stub->cif, FFI_DEFAULT_ABI, arg_count, rtype, types);

    if(status != FFI_OK) {
      XFREE(stub);
      sassert(status == FFI_OK);
    }

    data(state, MemoryPointer::create(state, (void*)stub));
  }
  
  /* The main interface function, handles looking up the pointer in the library,
   * generating the stub, wrapping it up and attaching it to the module.
   */
  NativeFunction* NativeFunction::bind(STATE, OBJECT library, String* name,
      Array* args, OBJECT ret) {
    void *ep;
    int *arg_types;
    int ret_type;
    int i, tot, arg_count;
    OBJECT type; /* meths; */

    ep = NativeFunction::find_symbol(state, library, name);
    if(!ep) return (NativeFunction*)Qnil;

    tot = args->size();
    arg_count = tot;
    if(tot > 0) {
      arg_types = ALLOC_N(int, tot);

      for(i = 0; i < tot; i++) {
        type = args->get(state, i);
        if(!type->fixnum_p()) return (NativeFunction*)Qnil;
        arg_types[i] = as<Integer>(type)->to_native();

        /* State can only be passed as the first arg, and it's invisible,
           ie doesn't get seen as in onbound arg by ruby. But it can ONLY
           be the first arg. */
        if(arg_types[i] == RBX_FFI_TYPE_STATE) {
          if(i == 0) {
            arg_count--;
          } else {
            XFREE(arg_types);
            printf("Invalid arg types.\n");
            return (NativeFunction*)Qnil;
          }
        }
      }
    } else {
      arg_types = NULL;
    }

    ret_type = as<Integer>(ret)->to_native();

    NativeFunction* func = NativeFunction::create(state, name->to_sym(state), arg_count);
    func->bind(state, tot, arg_types, ret_type, ep);

    return func;
  }

  void **NativeFunction::marshal_arguments(STATE, Message* msg) {
    void **values;
    OBJECT obj;
    struct ffi_stub *stub = (struct ffi_stub*)data_->pointer;

    values = ALLOC_N(void*, stub->arg_count);

    for(size_t i = 0; i < stub->arg_count; i++) {
      switch(stub->arg_types[i]) {
      case RBX_FFI_TYPE_CHAR: {
        char *tmp = (char*)malloc(sizeof(char));
        obj = msg->get_argument(i);
        type_assert(obj, FixnumType, "converting to char");
        *tmp = (char)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UCHAR: {
        unsigned char *tmp = (unsigned char*)malloc(sizeof(char));
        obj = msg->get_argument(i);
        type_assert(obj, FixnumType, "converting to char");
        *tmp = (unsigned char)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_SHORT: {
        short *tmp = (short*)malloc(sizeof(short));
        obj = msg->get_argument(i);
        type_assert(obj, FixnumType, "converting to char");
        *tmp = (short)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_USHORT: {
        unsigned short *tmp = (unsigned short*)malloc(sizeof(short));
        obj = msg->get_argument(i);
        type_assert(obj, FixnumType, "converting to char");
        *tmp = (unsigned short)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_INT: {
        int *tmp = (int*)malloc(sizeof(int));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = as<Fixnum>(obj)->to_int();
        } else {
          type_assert(obj, BignumType, "converting to int");
          *tmp = as<Bignum>(obj)->to_int();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UINT: {
        unsigned int *tmp = (unsigned int*)malloc(sizeof(unsigned int));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = as<Fixnum>(obj)->to_uint();
        } else {
          type_assert(obj, BignumType, "converting to unsigned int");
          *tmp = as<Bignum>(obj)->to_uint();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG: {
        long *tmp = (long*)malloc(sizeof(long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = as<Fixnum>(obj)->to_long();
        } else {
          type_assert(obj, BignumType, "converting to long");
          *tmp = as<Bignum>(obj)->to_long();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG: {
        unsigned long *tmp = (unsigned long*)malloc(sizeof(long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = as<Fixnum>(obj)->to_ulong();
        } else {
          type_assert(obj, BignumType, "converting to unsigned long");
          *tmp = as<Bignum>(obj)->to_ulong();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_FLOAT: {
        float *tmp = (float*)malloc(sizeof(float));
        obj = msg->get_argument(i);
        type_assert(obj, FloatType, "converting to float");
        *tmp = (float)as<Float>(obj)->to_double(state);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_DOUBLE: {
        double *tmp = (double*)malloc(sizeof(double));
        obj = msg->get_argument(i);
        type_assert(obj, FloatType, "converting to double");
        *tmp = as<Float>(obj)->to_double(state);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG_LONG: {
        long long *tmp = (long long*)malloc(sizeof(long long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = as<Fixnum>(obj)->to_long_long();
        } else {
          type_assert(obj, BignumType, "converting to long long");
          *tmp = as<Bignum>(obj)->to_long_long();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG_LONG: {
        unsigned long long *tmp = (unsigned long long*)malloc(sizeof(long long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = as<Fixnum>(obj)->to_ulong_long();
        } else {
          type_assert(obj, BignumType, "converting to unsigned long long");
          *tmp = as<Bignum>(obj)->to_ulong_long();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_STATE: {
        VM **tmp = (VM**)malloc(sizeof(VM*));
        *tmp = state;
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_OBJECT: {
        OBJECT *tmp = (OBJECT*)malloc(sizeof(OBJECT));
        obj = msg->get_argument(i);
        *tmp = obj;
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_PTR: {
        void **tmp = (void**)malloc(sizeof(void*));
        obj = msg->get_argument(i);
        if(NIL_P(obj)) {
          *tmp = NULL;
        } else {
          MemoryPointer *mp = as<MemoryPointer>(obj);
          type_assert(obj, MemPtrType, "converting to pointer");
          *tmp = mp->pointer;
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_STRING: {
        char **tmp = (char**)malloc(sizeof(char*));
        obj = msg->get_argument(i);
        if(NIL_P(obj)) {
          *tmp = NULL;
        } else {
          String *so = as<String>(obj);
          *tmp = so->c_str();
        }
        values[i] = tmp;
        break;
      }
      }
    }

    return values;
  }

  OBJECT NativeFunction::call(STATE, Message* msg) {
    OBJECT ret;

    struct ffi_stub *stub = (struct ffi_stub*)data_->pointer;

    void **values = marshal_arguments(state, msg);

    switch(stub->ret_type) {
    case RBX_FFI_TYPE_CHAR: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_UCHAR: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_SHORT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_USHORT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_INT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Integer::from(state, (native_int)result);
      break;
    }
    case RBX_FFI_TYPE_UINT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Integer::from(state, (unsigned int)result);
      break;
    }
    case RBX_FFI_TYPE_LONG: {
      long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG: {
      unsigned long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_FLOAT: {
      float result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Float::create(state, (double)result); 
      break;
    }
    case RBX_FFI_TYPE_DOUBLE: {
      double result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Float::create(state, result);
      break;
    }
    case RBX_FFI_TYPE_LONG_LONG: {
      long long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG_LONG: {
      unsigned long long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_OBJECT: {
      ffi_call(&stub->cif, FFI_FN(stub->ep), &ret, values);
      break;
    }
    case RBX_FFI_TYPE_PTR: {
      void *result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      if(result == NULL) {
        ret = Qnil;
      } else {
        ret = MemoryPointer::create(state, result);
      }
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char* result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      if(result == NULL) {
        ret = Qnil;
      } else {
        ret = String::create(state, result);
      }
      break;
    }
    case RBX_FFI_TYPE_STRPTR: {
      char* result;
      OBJECT s, p;

      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);

      if(result == NULL) {
        s = p = Qnil;
      } else {
        s = String::create(state, result);
        p = MemoryPointer::create(state, result);
      }

      Array* ary = Array::create(state, 2);
      ary->set(state, 0, s);
      ary->set(state, 1, p);
      ret = ary;
      break;
    }
    default:
    case RBX_FFI_TYPE_VOID: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Qnil;
      break;
    }
    }

    /* TODO: fix marshal_argumets to not malloc memory for every arg.
     * use a single block. */
    /* Free the memory used to store the args */
    for(size_t i = 0; i < stub->arg_count; i++) {
      free(values[i]);
    }

    free(values);

    return ret;
  }

}
