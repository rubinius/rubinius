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

int ffi_get_alloc_size(int type) {
  int i;
  i = ffi_type_size(type);
  i = (i + 3) & ~3;
  return i;
}

OBJECT ffi_to_object() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  return obj;
}

void ffi_from_object(OBJECT obj) {
  rni_context *ctx = subtend_retrieve_context();
  cpu_stack_push(ctx->state, ctx->cpu, obj, FALSE);
}

/* char */

char ffi_to_char() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  type_assert(obj, FixnumType, "converting to char");

  return (char)N2I(obj);
}

void ffi_from_char(char c) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;
  ret = I2N((int)c);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

unsigned char ffi_to_uchar() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  type_assert(obj, FixnumType, "converting to unsigned char");

  return (unsigned char)N2I(obj);
}

void ffi_from_uchar(unsigned char c) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;

  ret = I2N((int)c);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* short */

short ffi_to_short() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  type_assert(obj, FixnumType, "converting to short");

  return (short)N2I(obj);
}

void ffi_from_short(short c) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;
  ret = I2N((int)c);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

unsigned short ffi_to_ushort() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  type_assert(obj, FixnumType, "converting to unsigned short");

  return (unsigned short)N2I(obj);
}

void ffi_from_ushort(unsigned short c) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;

  ret = I2N((int)c);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}


/* int */

int ffi_to_int() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  if(FIXNUM_P(obj)) {
    return N2I(obj);
  } else {
    type_assert(obj, BignumType, "converting to int");
    return bignum_to_i(ctx->state, obj);
  }
}

void ffi_from_int(int obj) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;

  ret = I2N(obj);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

unsigned int ffi_to_uint() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  if(FIXNUM_P(obj)) {
    return (unsigned int)N2I(obj);
  } else {
    type_assert(obj, BignumType, "converting to unsigned int");
    return bignum_to_ui(ctx->state, obj);
  }
}

void ffi_from_uint(unsigned int obj) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();

  state = ctx->state;

  ret = UI2N(obj);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* ll */
long long ffi_to_ll() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  if(FIXNUM_P(obj)) {
    return (long long)N2I(obj);
  } else {
    type_assert(obj, BignumType, "converting to long long");
    return bignum_to_ll(ctx->state, obj);
  }
}

void ffi_from_ll(long long val) {
  rni_context *ctx = subtend_retrieve_context();
  STATE;
  state = ctx->state;

  cpu_stack_push(ctx->state, ctx->cpu, LL2N(val), FALSE);
}

unsigned long long ffi_to_ull() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  if(FIXNUM_P(obj)) {
    return (unsigned long long)N2I(obj);
  } else {
    type_assert(obj, BignumType, "converting to unsigned long long");
    return (unsigned long long)bignum_to_ull(ctx->state, obj);
  }
}

void ffi_from_ull(unsigned long long val) {
  rni_context *ctx = subtend_retrieve_context();
  STATE;
  state = ctx->state;

  cpu_stack_push(ctx->state, ctx->cpu, ULL2N(val), FALSE);
}

/* long */
long ffi_to_long() {
  if(sizeof(long) == sizeof(long long)) {
    return (long)ffi_to_ll();
  } else {
    return (long)ffi_to_int();
  }
}

void ffi_from_long(long obj) {
  if(sizeof(long) == sizeof(long long)) {
    return ffi_from_ll(obj);
  } else {
    return ffi_from_int(obj);
  }
}

unsigned long ffi_to_ulong() {
  if(sizeof(long) == sizeof(long long)) {
    return (unsigned long)ffi_to_ull();
  } else {
    return (unsigned long)ffi_to_uint();
  }
}

void ffi_from_ulong(unsigned long obj) {
  if(sizeof(long) == sizeof(long long)) {
    return ffi_from_ull(obj);
  } else {
    return ffi_from_uint(obj);
  }
}

/* float */
float ffi_to_float() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  type_assert(obj, FloatType, "converting to float");

  return (float)FLOAT_TO_DOUBLE(obj);
}

void ffi_from_float(float val) {
  OBJECT ret;
  rni_context *ctx = subtend_retrieve_context();
  ret = float_new(ctx->state, (double)val);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* double */
double ffi_to_double() {
  OBJECT obj;
  double ret;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  type_assert(obj, FloatType, "converting to double");

  ret = FLOAT_TO_DOUBLE(obj);
  return ret;
}

void ffi_from_double(double val) {
  OBJECT ret;
  rni_context *ctx = subtend_retrieve_context();
  ret = float_new(ctx->state, val);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* ptr */
void* ffi_to_ptr() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);

  if(NIL_P(obj)) return NULL;

  type_assert(obj, MemPtrType, "converting to pointer");

  return (*DATA_STRUCT(obj, void**));
}

void ffi_from_ptr(void *ptr) {
  OBJECT ret;
  STATE;

  void **code_start;

  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;

  if(ptr == NULL) {
    ret = Qnil;
  } else {
    NEW_STRUCT(ret, code_start, BASIC_CLASS(ffi_ptr), void*);
    *code_start = ptr;
  }
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

void ffi_from_void(int dummy) {
  rni_context *ctx = subtend_retrieve_context();
  cpu_stack_push(ctx->state, ctx->cpu, Qnil, FALSE);
}

/* string */

char *ffi_to_string() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
  /* We explicitly allow a NULL for a string, because
     stuff like Socket.getaddrinfo needs it. */
  if(obj == Qnil) {
    return NULL;
  } else {
    type_assert(obj, StringType, "converting to C string");
    return string_byte_address(ctx->state, obj);
  }
}

void ffi_from_string(char *str) {
  OBJECT ret;
  rni_context *ctx = subtend_retrieve_context();

  if (NULL == str) {
    ret = Qnil;
  } else {
    ret = string_new(ctx->state, str);
  }

  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* state */
void* ffi_to_state() {
  rni_context *ctx = subtend_retrieve_context();
  return (void*)ctx->state;
}

/* strptr */
void ffi_from_strptr(char *str) {
  OBJECT obj, ptr, ret;
  STATE;

  void **code_start;

  rni_context *ctx = subtend_retrieve_context();
  obj = string_new(ctx->state, str);

  state = ctx->state;
  NEW_STRUCT(ptr, code_start, BASIC_CLASS(ffi_ptr), void*);
  *code_start = (void*)str;

  ret = array_new(ctx->state, 2);
  array_set(ctx->state, ret, 0, obj);
  array_set(ctx->state, ret, 1, ptr);

  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

void* ffi_get_to_converter(int type) {
  switch(type) {
    case RBX_FFI_TYPE_OBJECT:
    return ffi_to_object;

    case RBX_FFI_TYPE_CHAR:
    return ffi_to_char;

    case RBX_FFI_TYPE_UCHAR:
    return ffi_to_uchar;

    case RBX_FFI_TYPE_SHORT:
    return ffi_to_short;

    case RBX_FFI_TYPE_USHORT:
    return ffi_to_ushort;

    case RBX_FFI_TYPE_INT:
    return ffi_to_int;

    case RBX_FFI_TYPE_UINT:
    return ffi_to_uint;

    case RBX_FFI_TYPE_LONG:
    return ffi_to_long;

    case RBX_FFI_TYPE_ULONG:
    return ffi_to_ulong;

    case RBX_FFI_TYPE_LL:
    return ffi_to_ll;

    /* FIXME: have a real converter */
    case RBX_FFI_TYPE_ULL:
    return ffi_to_ull;

    case RBX_FFI_TYPE_FLOAT:
    return ffi_to_float;

    case RBX_FFI_TYPE_DOUBLE:
    return ffi_to_double;

    case RBX_FFI_TYPE_PTR:
    return ffi_to_ptr;

    case RBX_FFI_TYPE_STRING:
    return ffi_to_string;

    case RBX_FFI_TYPE_STATE:
    return ffi_to_state;

    default:

    return NULL;
  }
}

void* ffi_get_from_converter(int type) {
  switch(type) {
    case RBX_FFI_TYPE_OBJECT:
    return ffi_from_object;

    case RBX_FFI_TYPE_CHAR:
    return ffi_from_char;

    case RBX_FFI_TYPE_UCHAR:
    return ffi_from_uchar;

    case RBX_FFI_TYPE_SHORT:
    return ffi_from_short;

    case RBX_FFI_TYPE_USHORT:
    return ffi_from_ushort;

    case RBX_FFI_TYPE_INT:
    return ffi_from_int;

    case RBX_FFI_TYPE_UINT:
    return ffi_from_uint;

    case RBX_FFI_TYPE_LONG:
    return ffi_from_long;

    case RBX_FFI_TYPE_ULONG:
    return ffi_from_ulong;

    case RBX_FFI_TYPE_LL:
    return ffi_from_ll;

    /* FIXME: have a real converter */
    case RBX_FFI_TYPE_ULL:
    return ffi_from_ull;

    case RBX_FFI_TYPE_FLOAT:
    return ffi_from_float;

    case RBX_FFI_TYPE_DOUBLE:
    return ffi_from_double;

    case RBX_FFI_TYPE_PTR:
    return ffi_from_ptr;

    case RBX_FFI_TYPE_VOID:
    return ffi_from_void;

    case RBX_FFI_TYPE_STRING:
    return ffi_from_string;

    case RBX_FFI_TYPE_STRPTR:
    return ffi_from_strptr;

    default:

    return NULL;
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
  void *ep;
  int *arg_types;
  int ret_type;
  int i, tot, arg_count;
  OBJECT ptr, func, sym, type; /* meths; */

  ep = subtend_find_symbol(state, library, name);
  if(!ep) return Qnil;

  tot = N2I(array_get_total(args));
  arg_count = tot;
  /* We don't support more than 6 args currently. */
  if(tot > 6) {
    return Qnil;
  } else if(tot > 0) {
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

  ptr = ffi_libffi_generate(state, tot, arg_types, ret_type, ep);

  sym = string_to_sym(state, name);
  func = ffi_function_new(state, ptr, sym, arg_count);

  return func;
}

OBJECT ffi_get_field(char *ptr, int offset, int type) {
  int sz;
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
  int sz;
  STATE = current_machine->s;
  uint8_t u8;
  uint16_t u16;
  uint32_t u32;

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
      result = string_byte_address(state, val);
    }
    WRITE(char*, result);
    break;
  }
  default:
    sassert(0);
  }
}
