#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

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

#undef ffi_call
#include <ffi.h>

struct ffi_stub {
  ffi_cif cif;
  int arg_count;
  int *arg_types;
  int ret_type;
  void *ep;
};

OBJECT ffi_libffi_generate(STATE, int arg_count, int *arg_types,
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

  return ffi_new_pointer(state, stub);
}

void ffi_call_libffi(STATE, cpu c, OBJECT ptr) {
  struct ffi_stub *stub;
  void **values;
  OBJECT obj, ret;
  int i;

  stub = (struct ffi_stub*)(*DATA_STRUCT(ptr, void**));

  values = ALLOC_N(void*, stub->arg_count);
  for(i = 0; i < stub->arg_count; i++) {
    switch(stub->arg_types[i]) {
    case RBX_FFI_TYPE_CHAR: {
      char *tmp = (char*)alloca(sizeof(char));
      obj = stack_pop();
      type_assert(obj, FixnumType, "converting to char");
      *tmp = (char)N2I(obj);
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_UCHAR: {
      unsigned char *tmp = (unsigned char*)alloca(sizeof(char));
      obj = stack_pop();
      type_assert(obj, FixnumType, "converting to unsigned char");
      *tmp = (unsigned char)N2I(obj);
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_SHORT: {
      short *tmp = (short*)alloca(sizeof(short));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (short)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to short");
        *tmp = (short)bignum_to_i(state, obj);
      }
      *tmp = (short)N2I(obj);
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_USHORT: {
      unsigned short *tmp = (unsigned short*)alloca(sizeof(short));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (unsigned short)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to unsigned short");
        *tmp = (unsigned short)bignum_to_i(state, obj);
      }
      *tmp = (unsigned short)N2I(obj);
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_INT: {
      int *tmp = (int*)alloca(sizeof(int));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (int)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to int");
        *tmp = bignum_to_i(state, obj);
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_UINT: {
      unsigned int *tmp = (unsigned int*)alloca(sizeof(unsigned int));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (int)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to int");
        *tmp = bignum_to_i(state, obj);
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_LONG: {
      long *tmp = (long*)alloca(sizeof(long));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (long)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to long");
        if(sizeof(long) == sizeof(long long)) {
          *tmp = (long)bignum_to_ll(state, obj);
        } else {
          *tmp = bignum_to_i(state, obj);
        }
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_ULONG: {
      unsigned long *tmp = (unsigned long*)alloca(sizeof(long));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (unsigned long)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to unsigned long");
        if(sizeof(long) == sizeof(long long)) {
          *tmp = (unsigned long)bignum_to_ll(state, obj);
        } else {
          *tmp = bignum_to_i(state, obj);
        }
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_FLOAT: {
      float *tmp = (float*)alloca(sizeof(float));
      obj = stack_pop();
      type_assert(obj, FloatType, "converting to float");
      *tmp = (float)FLOAT_TO_DOUBLE(obj);
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_DOUBLE: {
      double *tmp = (double*)alloca(sizeof(double));
      obj = stack_pop();
      type_assert(obj, FloatType, "converting to double");
      *tmp = FLOAT_TO_DOUBLE(obj);
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_LL: {
      long long *tmp = (long long*)alloca(sizeof(long long));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (long long)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to unsigned long");
        *tmp = (long long)bignum_to_ll(state, obj);
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_ULL: {
      unsigned long long *tmp = (unsigned long long*)alloca(sizeof(long long));
      obj = stack_pop();
      if(FIXNUM_P(obj)) {
        *tmp = (unsigned long long)N2I(obj);
      } else {
        type_assert(obj, BignumType, "converting to unsigned long");
        *tmp = (unsigned long long)bignum_to_ll(state, obj);
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_STATE:
      values[i] = &state;
      break;
    case RBX_FFI_TYPE_OBJECT: {
      OBJECT *tmp = (OBJECT*)alloca(sizeof(OBJECT));
      obj = stack_pop();
      *tmp = obj;
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_PTR: {
      void **tmp = (void**)alloca(sizeof(void*));
      obj = stack_pop();
      if(NIL_P(obj)) {
        *tmp = NULL;
      } else {
        type_assert(obj, MemPtrType, "converting to pointer");
        *tmp = *DATA_STRUCT(obj, void**);
      }
      values[i] = tmp;
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char **tmp = (char**)alloca(sizeof(char*));
      obj = stack_pop();
      if(NIL_P(obj)) {
        *tmp = NULL;
      } else {
        int len = N2I(string_get_bytes(obj));
        *tmp = string_byte_address(state, obj);
        (*tmp)[len] = 0;
      }
      values[i] = tmp;
      break;
    }
    }
  }

  switch(stub->ret_type) {
  case RBX_FFI_TYPE_CHAR: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = I2N((char)result);
    break;
  }
  case RBX_FFI_TYPE_UCHAR: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = UI2N((unsigned char)result);
    break;
  }
  case RBX_FFI_TYPE_SHORT: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = I2N((short)result);
    break;
  }
  case RBX_FFI_TYPE_USHORT: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = UI2N((unsigned short)result);
    break;
  }
  case RBX_FFI_TYPE_INT: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = I2N((int)result);
    break;
  }
  case RBX_FFI_TYPE_UINT: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = UI2N((unsigned int)result);
    break;
  }
  case RBX_FFI_TYPE_LONG: {
    long result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = I2N(result);
    break;
  }
  case RBX_FFI_TYPE_ULONG: {
    unsigned long result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = UI2N(result);
    break;
  }
  case RBX_FFI_TYPE_FLOAT: {
    float result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = float_new(state, (double)result); 
    break;
  }
  case RBX_FFI_TYPE_DOUBLE: {
    double result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = float_new(state, result);
    break;
  }
  case RBX_FFI_TYPE_LL: {
    long long result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = LL2N(result);
    break;
  }
  case RBX_FFI_TYPE_ULL: {
    unsigned long long result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = ULL2N(result);
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
      ret = ffi_new_pointer(state, result);
    }
    break;
  }
  case RBX_FFI_TYPE_STRING: {
    char* result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
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
    
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);

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
  case RBX_FFI_TYPE_VOID: {
    ffi_arg result;
    ffi_call(&stub->cif, FFI_FN(stub->ep), &result, values);
    ret = Qnil;
    break;
  }
  }

  stack_push(ret);
}
