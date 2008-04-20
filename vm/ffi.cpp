#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include "ffi.hpp"
#include "message.hpp"

#include "strlcpy.h"
#include "strlcat.h"

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

namespace rubinius {

  void VM::init_ffi() {
    globals.ffi_ptr = new_class("MemoryPointer", globals.bytearray, 0);
    globals.ffi_ptr->set_object_type(MemPtrType);

    globals.ffi_func = new_class("NativeFunction", globals.executable, NativeFunction::fields);

    Module* mod = new_module("FFI");
    mod->set_const(this, "TYPE_OBJECT",   Object::i2n(this, RBX_FFI_TYPE_OBJECT));
    mod->set_const(this, "TYPE_CHAR",     Object::i2n(this, RBX_FFI_TYPE_CHAR));
    mod->set_const(this, "TYPE_UCHAR",    Object::i2n(this, RBX_FFI_TYPE_UCHAR));
    mod->set_const(this, "TYPE_SHORT",    Object::i2n(this, RBX_FFI_TYPE_SHORT));
    mod->set_const(this, "TYPE_USHORT",   Object::i2n(this, RBX_FFI_TYPE_USHORT));
    mod->set_const(this, "TYPE_INT",      Object::i2n(this, RBX_FFI_TYPE_INT));
    mod->set_const(this, "TYPE_UINT",     Object::i2n(this, RBX_FFI_TYPE_UINT));
    mod->set_const(this, "TYPE_LONG",     Object::i2n(this, RBX_FFI_TYPE_LONG));
    mod->set_const(this, "TYPE_ULONG",    Object::i2n(this, RBX_FFI_TYPE_ULONG));
    mod->set_const(this, "TYPE_LL",       Object::i2n(this, RBX_FFI_TYPE_LL));
    mod->set_const(this, "TYPE_ULL",      Object::i2n(this, RBX_FFI_TYPE_ULL));
    mod->set_const(this, "TYPE_FLOAT",    Object::i2n(this, RBX_FFI_TYPE_FLOAT));
    mod->set_const(this, "TYPE_DOUBLE",   Object::i2n(this, RBX_FFI_TYPE_DOUBLE));
    mod->set_const(this, "TYPE_PTR",      Object::i2n(this, RBX_FFI_TYPE_PTR));
    mod->set_const(this, "TYPE_VOID",     Object::i2n(this, RBX_FFI_TYPE_VOID));
    mod->set_const(this, "TYPE_STRING",   Object::i2n(this, RBX_FFI_TYPE_STRING));
    mod->set_const(this, "TYPE_STATE",    Object::i2n(this, RBX_FFI_TYPE_STATE));
    mod->set_const(this, "TYPE_STRPTR",   Object::i2n(this, RBX_FFI_TYPE_STRPTR));
    mod->set_const(this, "TYPE_CHARARR",  Object::i2n(this, RBX_FFI_TYPE_CHARARR));
  }

  MemoryPointer* MemoryPointer::create(STATE, void* ptr) {
    MemoryPointer* obj = (MemoryPointer*)state->new_struct(G(ffi_ptr), sizeof(MemoryPointer));
    obj->pointer = ptr;
    return obj;
  }
  
  OBJECT MemoryPointer::get_field(STATE, int offset, int type) {
    OBJECT ret;
    char* ptr = (char*)pointer;

    ptr += offset;

#define READ(type) (*((type*)(ptr)))

    switch(type) {
    case RBX_FFI_TYPE_CHAR:
      ret = Object::i2n(state, READ(char));
      break;
    case RBX_FFI_TYPE_UCHAR:
      ret = Object::i2n(state, READ(unsigned char));
      break;
    case RBX_FFI_TYPE_SHORT:
      ret = Object::i2n(state, READ(short));
      break;
    case RBX_FFI_TYPE_USHORT:
      ret = Object::i2n(state, READ(unsigned short));
      break;
    case RBX_FFI_TYPE_INT:
      ret = Object::i2n(state, READ(int));
      break;
    case RBX_FFI_TYPE_UINT:
      ret = Object::i2n(state, READ(unsigned int));
      break;
    case RBX_FFI_TYPE_LONG:
      ret = Object::i2n(state, READ(long));
      break;
    case RBX_FFI_TYPE_ULONG:
      ret = Object::i2n(state, READ(unsigned long));
      break;
    case RBX_FFI_TYPE_FLOAT:
      ret = Float::create(state, (double)READ(float));
      break;
    case RBX_FFI_TYPE_DOUBLE:
      ret = Float::create(state, READ(double));
      break;
    case RBX_FFI_TYPE_LL:
      ret = Object::ll2n(state, READ(long long));
      break;
    case RBX_FFI_TYPE_ULL:
      ret = Object::ull2n(state, READ(unsigned long long));
      break;
    case RBX_FFI_TYPE_OBJECT:
      ret = READ(OBJECT);
      break;
    case RBX_FFI_TYPE_PTR: {
      void *lptr = READ(void*);
      if(!lptr) {
        ret = Qnil;
      } else {
        ret = MemoryPointer::create(state, lptr);
      }
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char* result = READ(char*);
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

      result = READ(char*);

      if(result == NULL) {
        s = p = Qnil;
      } else {
        p = MemoryPointer::create(state, result);
        s = String::create(state, result);
      }

      Array* ary = Array::create(state, 2);
      ary->set(state, 0, s);
      ary->set(state, 1, p);
      ret = ary;
      break;
    }
    default:
    case RBX_FFI_TYPE_VOID:
      ret = Qnil;
      break;
    }

    return ret;
  }

  void MemoryPointer::set_field(STATE, int offset, int type, OBJECT val) {
    char* ptr = (char*)pointer;

    ptr += offset;

#define WRITE(type, val) *((type*)ptr) = (type)val

    switch(type) {
    case RBX_FFI_TYPE_CHAR:
      type_assert(val, FixnumType, "converting to char");
      WRITE(char, val->n2i());
      break;
    case RBX_FFI_TYPE_UCHAR:
      type_assert(val, FixnumType, "converting to unsigned char");
      WRITE(unsigned char, val->n2i());
      break;
    case RBX_FFI_TYPE_SHORT:
      type_assert(val, FixnumType, "converting to short");
      WRITE(short, val->n2i());
      break;
    case RBX_FFI_TYPE_USHORT:
      type_assert(val, FixnumType, "converting to unsigned short");
      WRITE(unsigned short, val->n2i());
      break;
    case RBX_FFI_TYPE_INT:
      if(FIXNUM_P(val)) {
        WRITE(int, val->n2i());
      } else {
        Bignum *big = as<Bignum>(val);
        type_assert(val, BignumType, "converting to int");
        WRITE(int, big->to_i(state));
      }
      break;
    case RBX_FFI_TYPE_UINT:
      if(FIXNUM_P(val)) {
        WRITE(unsigned int, val->n2i());
      } else {
        Bignum *big = as<Bignum>(val);
        type_assert(val, BignumType, "converting to unsigned int");
        WRITE(unsigned int, big->to_i(state));
      }
      break;
    case RBX_FFI_TYPE_LONG:
      if(FIXNUM_P(val)) {
        WRITE(long, val->n2i());
      } else {
        Bignum *big = as<Bignum>(val);
        type_assert(val, BignumType, "converting to long");
        if(sizeof(long) == sizeof(long long)) {
          WRITE(long, big->to_ll(state));
        } else {
          WRITE(long, big->to_i(state));
        }
      }
      break;
    case RBX_FFI_TYPE_ULONG:
      if(FIXNUM_P(val)) {
        WRITE(unsigned long, val->n2i());
      } else {
        Bignum *big = as<Bignum>(val);
        type_assert(val, BignumType, "converting to long");
        if(sizeof(long) == sizeof(long long)) {
          WRITE(unsigned long, big->to_ll(state));
        } else {
          WRITE(unsigned long, big->to_i(state));
        }
      }
      break;
    case RBX_FFI_TYPE_FLOAT: {
      Float* flt = as<Float>(val);
      type_assert(val, FloatType, "converting to float");
      WRITE(float, flt->to_double(state));
      break;
    }
    case RBX_FFI_TYPE_DOUBLE: {
      Float* flt = as<Float>(val);
      type_assert(val, FloatType, "converting to double");
      WRITE(double, flt->to_double(state));
      break;
    }
    case RBX_FFI_TYPE_LL:
      if(FIXNUM_P(val)) {
        WRITE(long long, val->n2i());
      } else {
        Bignum *big = as<Bignum>(val);
        type_assert(val, BignumType, "converting to long long");
        WRITE(long long, big->to_ll(state));
      }
      break;
    case RBX_FFI_TYPE_ULL:
      if(FIXNUM_P(val)) {
        WRITE(unsigned long long, val->n2i());
      } else {
        Bignum *big = as<Bignum>(val);
        type_assert(val, BignumType, "converting to unsigned long long");
        WRITE(unsigned long long, big->to_ll(state));
      }
      break;
    case RBX_FFI_TYPE_OBJECT:
      WRITE(OBJECT, val);
      break;
    case RBX_FFI_TYPE_PTR:
      if(NIL_P(val)) {
        WRITE(void*, NULL);
      } else {
        MemoryPointer *mp = as<MemoryPointer>(val);
        type_assert(val, MemPtrType, "converting to pointer");
        WRITE(void*, mp->pointer);
      }
      break;
    case RBX_FFI_TYPE_STRING: {
      char* result;
      if(NIL_P(val)) {
        result = NULL;
      } else {
        String* str = as<String>(val);
        /* TODO this is probably not correct. Saving away an 
         * internal pointer to the string means that when the string
         * moves, the data will point at the wrong place. Probably need to
         * copy the string data instead */
        result = str->byte_address(state);
      }
      WRITE(char*, result);
      break;
    }
    default:
      sassert(0);
    }
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
      strlcpy(sys_name, *path, sizeof(sys_name));
      strlcat(sys_name, LIBSUFFIX, sizeof(sys_name));

      /* Open it up. If this fails, then we just pretend like
         the library isn't there. */
      lib = xdlopen(sys_name);
      if(!lib) {
#ifdef LIBSUFFIX2
        memset(sys_name, 0, 128);
        strlcpy(sys_name, *path, sizeof(sys_name));
        strlcat(sys_name, LIBSUFFIX2, sizeof(sys_name));

        lib = xdlopen(sys_name);
        if(!lib) return NULL;
#else
          return NULL;
#endif
      }
    }

    ep = xdlsym(lib, *name);
    if(!ep) {
      ep = xdlsym2(*name);
    }
    return ep;
  }

  NativeFunction* NativeFunction::create(STATE, OBJECT name, int args) {
    NativeFunction* nf = (NativeFunction*)state->new_object(G(ffi_func));
    SET(nf, primitive, state->symbol("nfunc_call"));
    SET(nf, required, Object::i2n(state, args));
    SET(nf, serial, Object::i2n(0));
    SET(nf, name,   name);
    SET(nf, file,   state->symbol("<system>"));
    SET(nf, data,   Qnil);
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
      case RBX_FFI_TYPE_LL:
        types[i] = &ffi_type_sint64;
        break;
      case RBX_FFI_TYPE_ULL:
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
    case RBX_FFI_TYPE_LL:
      rtype = &ffi_type_sint64;
      break;
    case RBX_FFI_TYPE_ULL:
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

    data = MemoryPointer::create(state, (void*)stub);
  }
  
  /* The main interface function, handles looking up the pointer in the library,
   * generating the stub, wrapping it up and attaching it to the module.
   */
  NativeFunction* NativeFunction::bind(STATE, String* library, String* name, 
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
        arg_types[i] = type->n2i();

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

    ret_type = ret->n2i();

    NativeFunction* func = NativeFunction::create(state, name->to_sym(state), arg_count);
    func->bind(state, tot, arg_types, ret_type, ep);
    
    return func;
  }

  void **NativeFunction::marshal_arguments(STATE, Message* msg) {
    void **values;
    OBJECT obj;
    struct ffi_stub *stub = (struct ffi_stub*)data->pointer;

    values = ALLOC_N(void*, stub->arg_count);
    
    for(size_t i = 0; i < stub->arg_count; i++) {
      switch(stub->arg_types[i]) {
      case RBX_FFI_TYPE_CHAR: {
        char *tmp = (char*)alloca(sizeof(char));
        obj = msg->get_argument(i);
        type_assert(obj, FixnumType, "converting to char");
        *tmp = (char)obj->n2i();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UCHAR: {
        unsigned char *tmp = (unsigned char*)alloca(sizeof(char));
        obj = msg->get_argument(i);
        type_assert(obj, FixnumType, "converting to unsigned char");
        *tmp = (unsigned char)obj->n2i();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_SHORT: {
        short *tmp = (short*)alloca(sizeof(short));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (short)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to short");
          *tmp = as<Bignum>(obj)->to_i(state);
        }
        *tmp = (short)obj->n2i();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_USHORT: {
        unsigned short *tmp = (unsigned short*)alloca(sizeof(short));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (unsigned short)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to unsigned short");
          *tmp = (unsigned short)as<Bignum>(obj)->to_i(state);
        }
        *tmp = (unsigned short)obj->n2i();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_INT: {
        int *tmp = (int*)alloca(sizeof(int));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (int)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to int");
          *tmp = as<Bignum>(obj)->to_i(state);
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UINT: {
        unsigned int *tmp = (unsigned int*)alloca(sizeof(unsigned int));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (int)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to int");
          *tmp = as<Bignum>(obj)->to_i(state);
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG: {
        long *tmp = (long*)alloca(sizeof(long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (long)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to long");
          if(sizeof(long) == sizeof(long long)) {
            *tmp = (long)as<Bignum>(obj)->to_ll(state);
          } else {
            *tmp = as<Bignum>(obj)->to_i(state);
          }
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG: {
        unsigned long *tmp = (unsigned long*)alloca(sizeof(long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (unsigned long)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to unsigned long");
          if(sizeof(long) == sizeof(long long)) {
            *tmp = (unsigned long)as<Bignum>(obj)->to_ll(state);
          } else {
            *tmp = as<Bignum>(obj)->to_i(state);
          }
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_FLOAT: {
        float *tmp = (float*)alloca(sizeof(float));
        obj = msg->get_argument(i);
        type_assert(obj, FloatType, "converting to float");
        *tmp = (float)as<Float>(obj)->to_double(state);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_DOUBLE: {
        double *tmp = (double*)alloca(sizeof(double));
        obj = msg->get_argument(i);
        type_assert(obj, FloatType, "converting to double");
        *tmp = as<Float>(obj)->to_double(state);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LL: {
        long long *tmp = (long long*)alloca(sizeof(long long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (long long)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to unsigned long");
          *tmp = (long long)as<Bignum>(obj)->to_ll(state);
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULL: {
        unsigned long long *tmp = (unsigned long long*)alloca(sizeof(long long));
        obj = msg->get_argument(i);
        if(FIXNUM_P(obj)) {
          *tmp = (unsigned long long)obj->n2i();
        } else {
          type_assert(obj, BignumType, "converting to unsigned long");
          *tmp = (unsigned long long)as<Bignum>(obj)->to_ll(state);
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_STATE:
        values[i] = &state;
        break;
      case RBX_FFI_TYPE_OBJECT: {
        OBJECT *tmp = (OBJECT*)alloca(sizeof(OBJECT));
        obj = msg->get_argument(i);
        *tmp = obj;
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_PTR: {
        void **tmp = (void**)alloca(sizeof(void*));
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
        char **tmp = (char**)alloca(sizeof(char*));
        obj = msg->get_argument(i);
        if(NIL_P(obj)) {
          *tmp = NULL;
        } else {
          String *so = as<String>(obj);
          *tmp = so->byte_address(state);
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

    struct ffi_stub *stub = (struct ffi_stub*)data->pointer;

    void **values = marshal_arguments(state, msg);

    switch(stub->ret_type) {
    case RBX_FFI_TYPE_CHAR: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::i2n(state, (char)result);
      break;
    }
    case RBX_FFI_TYPE_UCHAR: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::ui2n(state, (unsigned char)result);
      break;
    }
    case RBX_FFI_TYPE_SHORT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::i2n(state, (short)result);
      break;
    }
    case RBX_FFI_TYPE_USHORT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::ui2n(state, (unsigned short)result);
      break;
    }
    case RBX_FFI_TYPE_INT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::i2n(state, (int)result);
      break;
    }
    case RBX_FFI_TYPE_UINT: {
      ffi_arg result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::ui2n(state, (unsigned int)result);
      break;
    }
    case RBX_FFI_TYPE_LONG: {
      long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::i2n(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG: {
      unsigned long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::ui2n(state, result);
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
    case RBX_FFI_TYPE_LL: {
      long long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::ll2n(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULL: {
      unsigned long long result;
      ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
      ret = Object::ull2n(state, result);
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
        p = MemoryPointer::create(state, result);
        s = String::create(state, result);
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

    return ret;
  }


};
