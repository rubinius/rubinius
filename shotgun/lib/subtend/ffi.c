#include "shotgun.h"
#include "nmethod.h"
#include "symbol.h"
#include "object.h"
#include "nmc.h"
#include "ffi.h"
#include "library.h"
#include "string.h"
#include "hash.h"
#include "primitive_indexes.h"

#include <lightning.h>

#define RBS_DISASS 1

void Init_ffi(STATE) {
  OBJECT mod;
  BASIC_CLASS(ffi_ptr) = rbs_class_new(state, "MemoryPointer", 0, BASIC_CLASS(object));
  BASIC_CLASS(ffi_func) = rbs_class_new(state, "NativeFunction", 0, BASIC_CLASS(object));
  mod = rbs_module_new(state, "FFI", BASIC_CLASS(object));
  rbs_const_set(state, mod, "TYPE_OBJECT",  I2N(FFI_TYPE_OBJECT));
  rbs_const_set(state, mod, "TYPE_CHAR",    I2N(FFI_TYPE_CHAR));
  rbs_const_set(state, mod, "TYPE_UCHAR",   I2N(FFI_TYPE_UCHAR));
  rbs_const_set(state, mod, "TYPE_SHORT",   I2N(FFI_TYPE_SHORT));
  rbs_const_set(state, mod, "TYPE_USHORT",  I2N(FFI_TYPE_USHORT));
  rbs_const_set(state, mod, "TYPE_INT",     I2N(FFI_TYPE_INT));
  rbs_const_set(state, mod, "TYPE_UINT",    I2N(FFI_TYPE_UINT));
  rbs_const_set(state, mod, "TYPE_LONG",    I2N(FFI_TYPE_LONG));
  rbs_const_set(state, mod, "TYPE_ULONG",   I2N(FFI_TYPE_ULONG));
  rbs_const_set(state, mod, "TYPE_FLOAT",   I2N(FFI_TYPE_FLOAT));
  rbs_const_set(state, mod, "TYPE_DOUBLE",  I2N(FFI_TYPE_DOUBLE));
  rbs_const_set(state, mod, "TYPE_PTR",     I2N(FFI_TYPE_PTR));
  rbs_const_set(state, mod, "TYPE_VOID",    I2N(FFI_TYPE_VOID));
  rbs_const_set(state, mod, "TYPE_STRING",  I2N(FFI_TYPE_STRING));
  
}

OBJECT ffi_new_pointer(STATE, void *ptr) {
  OBJECT o;
  
  o = object_memory_new_opaque(state, BASIC_CLASS(ffi_ptr), sizeof(void*));
  memcpy(BYTES_OF(o), &ptr, sizeof(ptr));
  return o;
}

#ifdef RBS_DISASS
void disassemble(FILE*, char*, char*);
#endif

static rni_handle* _ffi_pop() {
  rni_handle *h;
  rni_context *ctx = subtend_retrieve_context();
  h = nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, cpu_stack_pop(ctx->state, ctx->cpu));
  return h;
}

#define ffi_pop(reg) jit_prepare(0); jit_calli(_ffi_pop); jit_retval_p(reg);

char *ffi_generate_c_stub(STATE, int args, void *func) {
  char *start, *end;
  char *codebuf;
  int in, vars, i, aligned;
  int *ids;
    
  /* The stub is called with the receiver, so if there are no args, we
     can just use func as the stub. */
  if(args == 0) return func;
  
  /* Until lightning supports more than 6 args, we can only generate a stub
     for 5 args (0 is the receiver). */
  if(args > 5) return NULL;
  
  codebuf = (char*)malloc(512);
  start = codebuf;
  
  (void)jit_set_ip((jit_insn*)codebuf);
      
  jit_prolog(1);
  
  in = jit_arg_p();
  jit_getarg_p(JIT_V0, in);
  
  if(args == 0) {
    jit_prepare_i(1);
    jit_pusharg_p(JIT_V0);
    
  }
 /* else if(args == 1) {
    ffi_pop(JIT_V1);
    jit_prepare_i(2);
    jit_pusharg_p(JIT_V1);
    jit_pusharg_p(JIT_V0);
    
  } else if(args == 2) {
    ffi_pop(JIT_V1);
    ffi_pop(JIT_V2);
    jit_prepare_i(3);
    jit_pusharg_p(JIT_V2);
    jit_pusharg_p(JIT_V1);
    jit_pusharg_p(JIT_V0);
    
  } */ 
  else {
    
    ids = calloc(args, sizeof(int));
    
    for(i = 0; i < args; i++) {    
      ffi_pop(JIT_V1);
      ids[i] = jit_allocai(sizeof(void*));
      jit_stxi_p(ids[i], JIT_FP, JIT_V1);
    }
        
    jit_prepare_i(args + 1);
    
    for(i = args - 1; i >= 0; i--) {
      jit_ldxi_p(JIT_R2, JIT_FP, ids[i]);
      jit_pusharg_p(JIT_R2);
    }
            
    jit_pusharg_p(JIT_V0);
    free(ids);
  }    
  
  jit_finish(func);
  jit_retval_p(JIT_RET);
  jit_ret();
  
  if(getenv("RBX_SHOW_STUBS")) {
    printf("Assembly stub for: %p (%d)\n", func, args);

    #ifdef RBS_DISASS
    disassemble(stderr, start, jit_get_ip().ptr);
    #endif
  }
  
  jit_flush_code(start, jit_get_ip().ptr);
  
  return start;
}

int ffi_type_size(int type) {
  switch(type) {
    case FFI_TYPE_CHAR:
    case FFI_TYPE_UCHAR:
    return sizeof(char);
    
    case FFI_TYPE_SHORT:
    case FFI_TYPE_USHORT:
    return sizeof(short);
    
    case FFI_TYPE_INT:
    case FFI_TYPE_UINT:
    return sizeof(int);
    
    case FFI_TYPE_LONG:
    case FFI_TYPE_ULONG:
    return sizeof(long);
    
    case FFI_TYPE_LL:
    case FFI_TYPE_ULL:
    return sizeof(long long);
    
    case FFI_TYPE_FLOAT:
    return sizeof(float);
    
    case FFI_TYPE_DOUBLE:
    return sizeof(double);
    
    case FFI_TYPE_PTR:
    case FFI_TYPE_STRING:
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
  
  return (char)FIXNUM_TO_INT(obj);
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
  
  return (unsigned char)FIXNUM_TO_INT(obj);
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

char ffi_to_short() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
  return (short)FIXNUM_TO_INT(obj);
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
  
  return (unsigned short)FIXNUM_TO_INT(obj);
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
  
  return FIXNUM_TO_INT(obj);
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
  
  return (unsigned int)FIXNUM_TO_INT(obj);
}

void ffi_from_uint(unsigned int obj) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  
  state = ctx->state;
  
  ret = UI2N(obj);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* long */
long ffi_to_long() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
  return FIXNUM_TO_INT(obj);
}

void ffi_from_long(long obj) {
  OBJECT ret;
  STATE;
  rni_context *ctx = subtend_retrieve_context();
  
  state = ctx->state;
  
  ret = I2N(obj);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

unsigned long ffi_to_ulong() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
  return (unsigned long)FIXNUM_TO_INT(obj);
}

void ffi_from_ulong(unsigned long obj) {
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
    return (long long)FIXNUM_TO_INT(obj);
  } else {
    return bignum_to_ll(ctx->state, obj);
  }
}

void ffi_from_ll(long long val) {
  int out;
  OBJECT ret;
  rni_context *ctx = subtend_retrieve_context();
  
  ret = (val << FIXNUM_SHIFT) | FIXNUM_MARKER;
  if((int)val != rbs_to_int(ret)) {
    ret = bignum_from_ll(ctx->state, val);    
  }
  
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

/* float */
float ffi_to_float() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
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
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
  return FLOAT_TO_DOUBLE(obj);
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
  
  return (*DATA_STRUCT(obj, void**));  
}

void ffi_from_ptr(void *ptr) {
  OBJECT ret;
  STATE;
  
  void **code_start;
    
  rni_context *ctx = subtend_retrieve_context();
  state = ctx->state;
  
  NEW_STRUCT(ret, code_start, BASIC_CLASS(ffi_ptr), void*);
  *code_start = ptr;
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

void ffi_from_void(int dummy) {
  OBJECT ret;
  rni_context *ctx = subtend_retrieve_context();
  cpu_stack_push(ctx->state, ctx->cpu, Qnil, FALSE);
}

/* string */

char *ffi_to_string() {
  OBJECT obj;
  rni_context *ctx = subtend_retrieve_context();
  obj = cpu_stack_pop(ctx->state, ctx->cpu);
  
  return string_byte_address(ctx->state, obj);
}

void ffi_from_string(char *str) {
  OBJECT ret;
  rni_context *ctx = subtend_retrieve_context();
  ret = string_new(ctx->state, str);
  cpu_stack_push(ctx->state, ctx->cpu, ret, FALSE);
}

void* ffi_get_to_converter(int type) {
  switch(type) {
    case FFI_TYPE_OBJECT:
    return ffi_to_object;
    
    case FFI_TYPE_CHAR:
    return ffi_to_char;
    
    case FFI_TYPE_UCHAR:
    return ffi_to_uchar;
    
    case FFI_TYPE_SHORT:
    return ffi_to_short;
    
    case FFI_TYPE_USHORT:
    return ffi_to_ushort;
        
    case FFI_TYPE_INT:
    return ffi_to_int;
    
    case FFI_TYPE_UINT:
    return ffi_to_uint;
    
    case FFI_TYPE_LONG:
    return ffi_to_long;
    
    case FFI_TYPE_ULONG:
    return ffi_to_ulong;
    
    case FFI_TYPE_LL:
    return ffi_to_ll;
    
    /* FIXME: have a real converter */
    case FFI_TYPE_ULL:
    return ffi_to_ll;
    
    case FFI_TYPE_FLOAT:
    return ffi_to_float;
    
    case FFI_TYPE_DOUBLE:
    return ffi_to_double;
    
    case FFI_TYPE_PTR:
    return ffi_to_ptr;
    
    case FFI_TYPE_STRING:
    return ffi_to_string;
    
    default:
    
    return NULL;
  }
}

void* ffi_get_from_converter(int type) {
  switch(type) {
    case FFI_TYPE_OBJECT:
    return ffi_from_object;
    
    case FFI_TYPE_CHAR:
    return ffi_from_char;
    
    case FFI_TYPE_UCHAR:
    return ffi_from_uchar;
    
    case FFI_TYPE_SHORT:
    return ffi_from_short;
    
    case FFI_TYPE_USHORT:
    return ffi_from_ushort;
        
    case FFI_TYPE_INT:
    return ffi_from_int;
    
    case FFI_TYPE_UINT:
    return ffi_from_uint;
    
    case FFI_TYPE_LONG:
    return ffi_from_long;
    
    case FFI_TYPE_ULONG:
    return ffi_from_ulong;
    
    case FFI_TYPE_LL:
    return ffi_from_ll;
    
    /* FIXME: have a real converter */
    case FFI_TYPE_ULL:
    return ffi_from_ll;
    
    case FFI_TYPE_FLOAT:
    return ffi_from_float;
    
    case FFI_TYPE_DOUBLE:
    return ffi_from_double;
    
    case FFI_TYPE_PTR:
    return ffi_from_ptr;
    
    case FFI_TYPE_VOID:
    return ffi_from_void;
    
    case FFI_TYPE_STRING:
    return ffi_from_string;
    
    default:
    
    return NULL;
  }
}

OBJECT ffi_generate_typed_c_stub(STATE, int args, int *arg_types, int ret_type, void *func) {
  char *start, *end;
  void **code_start;
  char *codebuf;
  int in, vars, i, aligned, reg;
  int *ids;
  void *conv;
  OBJECT obj;
      
  /* Until lightning supports more than 6 args, we can only generate a stub
     for 5 args (0 is the receiver). */
  if(args > 5) return Qnil;
  
  codebuf = (char*)malloc(512);
  start = codebuf;
  
  (void)jit_set_ip((jit_insn*)codebuf);
  
  jit_prolog(0);
  
  if(args > 0) {
    ids = calloc(args, sizeof(int));
  
    for(i = 0; i < args; i++) {
      conv = ffi_get_to_converter(arg_types[i]);
      switch(arg_types[i]) {
      case FFI_TYPE_FLOAT:
      case FFI_TYPE_DOUBLE:
        reg = JIT_FPR0;
      default:
        reg = JIT_V1;
      }
    
      jit_prepare(0); jit_calli(conv); jit_retval_p(reg);
      ids[i] = jit_allocai(ffi_get_alloc_size(arg_types[i]));
    
      switch(arg_types[i]) {
      case FFI_TYPE_CHAR:
      case FFI_TYPE_UCHAR:
        jit_stxi_c(ids[i], JIT_FP, reg);
        break;
      case FFI_TYPE_SHORT:
      case FFI_TYPE_USHORT:
        jit_stxi_s(ids[i], JIT_FP, reg);
        break;
      case FFI_TYPE_INT:
      case FFI_TYPE_UINT:
        jit_stxi_i(ids[i], JIT_FP, reg);
        break;
      case FFI_TYPE_LONG:
      case FFI_TYPE_ULONG:
        jit_stxi_l(ids[i], JIT_FP, reg);
        break;
    
      case FFI_TYPE_FLOAT:
        jit_stxi_f(ids[i], JIT_FP, reg);
        break;
      case FFI_TYPE_DOUBLE:
        jit_stxi_d(ids[i], JIT_FP, reg);
        break;
      case FFI_TYPE_OBJECT:
      case FFI_TYPE_PTR:
      case FFI_TYPE_STRING:
        jit_stxi_p(ids[i], JIT_FP, reg);
      }
    }
      
    jit_prepare_i(args);
  
    for(i = args - 1; i >= 0; i--) {
      switch(arg_types[i]) {
      case FFI_TYPE_CHAR:
      case FFI_TYPE_UCHAR:
        jit_ldxi_c(JIT_R2, JIT_FP, ids[i]);      
        jit_pusharg_c(JIT_R2);
        break;
      
      case FFI_TYPE_SHORT:
      case FFI_TYPE_USHORT:
        jit_ldxi_s(JIT_R2, JIT_FP, ids[i]);            
        jit_pusharg_s(JIT_R2);
        break;
      
      case FFI_TYPE_INT:
      case FFI_TYPE_UINT:
        jit_ldxi_i(JIT_R2, JIT_FP, ids[i]);
        jit_pusharg_i(JIT_R2);
        break;
      
      case FFI_TYPE_LONG:
      case FFI_TYPE_ULONG:
        jit_ldxi_l(JIT_R2, JIT_FP, ids[i]);
        jit_pusharg_l(JIT_R2);
        break;
    
      case FFI_TYPE_FLOAT:
        jit_ldxi_f(JIT_FPR5, JIT_FP, ids[i]);
        jit_pusharg_f(JIT_FPR5);    
        break;
      
      case FFI_TYPE_DOUBLE:
        jit_ldxi_d(JIT_FPR5, JIT_FP, ids[i]);
        jit_pusharg_d(JIT_FPR5);
        break;
      
      case FFI_TYPE_OBJECT:
      case FFI_TYPE_PTR:
      case FFI_TYPE_STRING:
      default:
        jit_ldxi_p(JIT_R2, JIT_FP, ids[i]);
        jit_pusharg_p(JIT_R2);
      }
    
    }
    free(ids);
  }
  
  switch(ret_type) {
  case FFI_TYPE_FLOAT:
  case FFI_TYPE_DOUBLE:
    reg = JIT_FPR0;
  default:
    reg = JIT_V1;
  }
  
  jit_finish(func);
  jit_retval_p(reg);
  
  jit_prepare_i(1);
  switch(ret_type) {
  case FFI_TYPE_CHAR:
  case FFI_TYPE_UCHAR:
    jit_pusharg_c(reg);
    break;
    
  case FFI_TYPE_SHORT:
  case FFI_TYPE_USHORT:
    jit_pusharg_s(reg);
    break;
    
  case FFI_TYPE_INT:
  case FFI_TYPE_UINT:
    jit_pusharg_i(reg);
    break;
    
  case FFI_TYPE_LONG:
  case FFI_TYPE_ULONG:
    jit_pusharg_l(reg);
    break;
  
  case FFI_TYPE_FLOAT:
    jit_pusharg_f(reg);    
    break;
    
  case FFI_TYPE_DOUBLE:
    jit_pusharg_d(reg);
    break;
    
  case FFI_TYPE_VOID:
    jit_movi_i(reg, 1);
    jit_pusharg_i(reg);
    break;
    
  case FFI_TYPE_OBJECT:
  case FFI_TYPE_PTR:
  case FFI_TYPE_STRING:
  default:
    jit_pusharg_p(reg);
  }

  conv = ffi_get_from_converter(ret_type);
  jit_finish(conv);
  /* probably don't need this, since the return value is void, but it
     doesn't hurt to clear it. */
  jit_movi_i(JIT_RET, 1);
  jit_ret();
  
  if(getenv("RBX_SHOW_STUBS")) {
    printf("Assembly stub for: %p (%d)\n", func, args);

    #ifdef RBS_DISASS
    disassemble(stderr, start, jit_get_ip().ptr);
    #endif
  }
  
  jit_flush_code(start, jit_get_ip().ptr);
  
  // obj = object_memory_new_opaque(state, BASIC_CLASS(ffi_ptr), sizeof(void*));
  NEW_STRUCT(obj, code_start, BASIC_CLASS(ffi_ptr), void*);
  *code_start = (void*)start;
  
  //memcpy(BYTES_OF(obj), &start, sizeof(void*));
  return obj;
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

/* The main interface function, handles looking up the pointer in the library, generating
   the stub, wrapping it up and attaching it to the module. */
OBJECT ffi_function_create(STATE, OBJECT library, OBJECT name, OBJECT args, OBJECT ret) {
  void *ep;
  int *arg_types;
  int ret_type;
  int i, tot;
  OBJECT ptr, func, meths, sym;
  
  ep = subtend_find_symbol(state, library, name);
  if(!ep) return Qnil;
  
  tot = FIXNUM_TO_INT(array_get_total(args));
  if(tot > 0) {
    arg_types = calloc(tot, sizeof(int));
  
    for(i = 0; i < tot; i++) {
      if(!FIXNUM_P(array_get(state, args, i))) return Qnil;
      arg_types[i] = FIXNUM_TO_INT(array_get(state, args, i));  
    }
    
  /* We don't support more than 5 args currently. */
  } else if(tot > 5) {
    return Qnil;
  } else {
    arg_types = NULL;
  }
  
  ret_type = FIXNUM_TO_INT(ret);
  
  ptr = ffi_generate_typed_c_stub(state, tot, arg_types, ret_type, ep);
  sym = string_to_sym(state, name);
  func = ffi_function_new(state, ptr, sym, tot);

  return func;
}

void ffi_call(STATE, cpu c, OBJECT ptr) {
  nf_stub_ffi func;
  rni_context *ctx;

  ctx = subtend_retrieve_context();
  ctx->state = state;
  ctx->cpu = c;
    
  func = (nf_stub_ffi)(*DATA_STRUCT(ptr, void**));
  func();
}

OBJECT ffi_get_field(char *ptr, int offset, int type) {
  nf_converter conv = (nf_converter)ffi_get_from_converter(type);
  STATE;
  int *loc;
  rni_context *ctx = subtend_retrieve_context();
  
  loc = (int*)(ptr + offset);
  state = ctx->state;
  conv(*loc);
  
  return cpu_stack_pop(ctx->state, ctx->cpu);
}

void ffi_set_field(char *ptr, int offset, int type, OBJECT val) {
  int sz;
  STATE;
  int data;
  int *loc;
  rni_context *ctx = subtend_retrieve_context();
  nf_converter conv = (nf_converter)ffi_get_to_converter(type);
  sz = ffi_get_alloc_size(type);

  cpu_stack_push(ctx->state, ctx->cpu, val, FALSE);

  /* Fix to work with datatypes that aren't 4 bytes. */
  data = conv();
  loc = (int*)(ptr + offset);
  *loc = data;
}
