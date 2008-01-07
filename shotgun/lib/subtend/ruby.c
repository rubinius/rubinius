#include "shotgun.h"
#include <stdarg.h>
#include "nmc.h"
#include "symbol.h"
#include "string.h"
#include "hash.h"
#include "class.h"
#include "module.h"

#undef SYMBOL_P
#include "ruby.h"

#define RBX_SYMBOL_P(obj) (DATA_TAG(obj) == DATA_TAG_SYMBOL)


OBJECT nmethod_new(STATE, OBJECT mod, const char *file, const char *name, void *func, int args);
#define AS_HNDL(obj) ((rni_handle*)obj)

#define HNDL(obj) handle_to_object(ctx->state, ctx->state->handle_tbl, (rni_handle*)obj)
#define CTX rni_context* ctx = subtend_retrieve_context()
#define NEW_HANDLE(ctx, val) nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, val)

#define CHAR2STR(name) NEW_HANDLE(ctx, string_new(ctx->state, name))

ID rb_intern(const char *name) {
  CTX;
  return (ID)symtbl_lookup_cstr(ctx->state, ctx->state->global->symbols, name);
}

VALUE subtend_get_exception(int which) {
  VALUE val;
  CTX;

  val = Qnil;
  
  switch(which) {
    case 0:
    val = rb_const_get(rb_cObject, rb_intern("Exception"));
    break;

    case 1:
    val = rb_const_get(rb_cObject, rb_intern("SystemExit"));
    break;

    case 2:
    val = rb_const_get(rb_cObject, rb_intern("Interrupt"));
    break;

    case 3:
    val = rb_const_get(rb_cObject, rb_intern("SignalException"));
    break;

    case 4: /* wtf is fatal? */
    break;

    case 5:
    val = rb_const_get(rb_cObject, rb_intern("StandardError"));
    break;

    case 6:
    val = rb_const_get(rb_cObject, rb_intern("RuntimeError"));
    break;

    case 7:
    val = rb_const_get(rb_cObject, rb_intern("TypeError"));
    break;

    case 8:
    val = rb_const_get(rb_cObject, rb_intern("ArgumentError"));
    break;

    case 9:
    val = rb_const_get(rb_cObject, rb_intern("IndexError"));
    break;

    case 10:
    val = rb_const_get(rb_cObject, rb_intern("RangeError"));
    break;
    
    case 11:
    val = rb_const_get(rb_cObject, rb_intern("NameError"));
    break;
    
    case 12:
    val = rb_const_get(rb_cObject, rb_intern("NoMethodError"));
    break;
    
    case 13:
    val = rb_const_get(rb_cObject, rb_intern("SecurityError"));
    break;
    
    case 14:
    val = rb_const_get(rb_cObject, rb_intern("NotImplementedError"));
    break;
    
    case 15:
    val = rb_const_get(rb_cObject, rb_intern("NoMemoryError"));
    break;

    case 16:
    val = rb_const_get(rb_cObject, rb_intern("ScriptError"));
    break;

    case 17:
    val = rb_const_get(rb_cObject, rb_intern("SyntaxError"));
    break;
    
    case 18:
    val = rb_const_get(rb_cObject, rb_intern("LoadError"));
    break;
    
    case 19:
    val = rb_const_get(rb_cObject, rb_intern("SystemCallError"));
    break;

    case 20:
    val = rb_const_get(rb_cObject, rb_intern("Errno"));
    break;
    
    case 21:
    val = rb_const_get(rb_cObject, rb_intern("IOError"));
    break;
  }
  
  return NEW_HANDLE(ctx, HNDL(val));
}

VALUE subtend_get_global(int which) {
  OBJECT val;
  CTX;
  
  switch(which) {
    case 0:
    val = ctx->state->global->object;
    break;
        
    default:
    val = (OBJECT)Qnil;
  }
  
  return NEW_HANDLE(ctx, val);
}

void rb_global_object(VALUE val) {
  handle_make_global(AS_HNDL(val));
}

void rb_free_global(VALUE val) {
  handle_clear_global(AS_HNDL(val));
}

/* Functions for calling methods or object. */

static VALUE _push_and_call(VALUE recv, ID meth, int args, VALUE *ary) {
  rni_context *ctx;
  rni_nmc *n;
  int i;
  /* This has to be volatile so that gcc doesn't optimize
     it away and it's test around getcontext/setcontext works
     properly. */
  volatile int swapped;
  OBJECT tmp;
  ctx = subtend_retrieve_context();
    
  if(!ctx->nmc) {
    printf("ERROR: tried to do rb_funcall, but there is no context!\n");
    return NULL;
  }
  
  /* Push all the arguments on the stack in reverse order... */
  for(i = args - 1; i >= 0; i--) {
    tmp = handle_to_object(ctx->state, ctx->state->handle_tbl, ary[i]);
    cpu_stack_push(ctx->state, ctx->cpu, tmp, 0);
  }
  
  n = ctx->nmc;
  
  n->value = recv;
  n->symbol = (OBJECT)meth;
  n->args = args;

  n->jump_val = CALL_METHOD;
  swapped = 0;
  getcontext(&n->cont);
  if(!swapped) {
    swapped++;
    setcontext(&n->system);
  }
  /* When we return here, the call has been done. */
  return n->value;
}

VALUE rb_funcall(VALUE recv, ID meth, int args, ...) {
  va_list ar;
  VALUE *arg_array;
  int i;
  VALUE ret;

  arg_array = ALLOC_N(VALUE, args);
  va_start(ar, args);
  for(i = 0; i < args; i++) {
    arg_array[i] = va_arg(ar, VALUE);
  }
  
  va_end(ar);
  
  ret = _push_and_call(recv, meth, args, arg_array);
  XFREE(arg_array);
  return ret;
  
}

VALUE rb_funcall2(VALUE recv, ID meth, int args, VALUE* array) {
  return _push_and_call(recv, meth, args, array);
}

/* Functions for adding behaviors (ie, classes/modules and methods) */

VALUE rb_define_class_under(VALUE parent, const char *name, VALUE super) {
  CTX;
  OBJECT cls;
  
  cls = rbs_class_new_with_namespace(ctx->state, name, 1, HNDL(super), HNDL(parent));
  
  return NEW_HANDLE(ctx, cls);
}

VALUE rb_define_class(const char *name, VALUE super) {
  CTX;
  OBJECT cls;
  cls = rbs_class_new_with_namespace(ctx->state, name, 1, HNDL(super), ctx->state->global->object);
  
  return NEW_HANDLE(ctx, cls);
}

VALUE rb_define_module_under(VALUE parent, const char *name) {
  CTX;
  OBJECT mod;
  mod = rbs_module_new(ctx->state, name, HNDL(parent));
  
  return NEW_HANDLE(ctx, mod);
}

VALUE rb_define_module(const char *name) {
  CTX;
  OBJECT mod;
  mod = rbs_module_new(ctx->state, name, ctx->state->global->object);
  
  return NEW_HANDLE(ctx, mod);
}

void rb_include_module(VALUE klass, VALUE module) {
  rb_funcall(klass, rb_intern("include"), 1, module);
}

VALUE rb_const_get(VALUE klass, ID id) {
  CTX;
  return NEW_HANDLE(ctx, cpu_const_get(ctx->state, ctx->cpu, (OBJECT)id, HNDL(klass)));
}

void rb_define_const(VALUE klass, const char* key, VALUE val) {
  CTX;
  if(NIL_P(klass)) rb_raise(rb_eTypeError, "no class/module to define constant %s", key);
  module_const_set(ctx->state, HNDL(klass), ID2SYM(rb_intern(key)), HNDL(val));
}

int rb_const_defined(VALUE klass, ID id) {
  return rb_funcall(klass, rb_intern("const_defined?"), 1, ID2SYM(id)) == Qtrue ? 1 : 0;
}

VALUE rb_ivar_get(VALUE obj, ID sym) {
  CTX;
  OBJECT val;
  
  val = object_get_ivar(ctx->state, HNDL(obj), HNDL(sym));
  
  return NEW_HANDLE(ctx, val);
}

VALUE rb_attr_get(VALUE obj, ID sym) {
  return rb_ivar_get(obj, sym);
}

void rb_define_attr(VALUE klass, ID id, int read, int write) {
  if(read == 1) rb_funcall(klass, rb_intern("attr_reader_cv"), 1, id);
  if(write == 1) rb_funcall(klass, rb_intern("attr_writer_cv"), 1, id);    
}

VALUE rb_iv_get(VALUE obj, char *name) {
  return rb_ivar_get(obj, rb_intern(name));
}

VALUE rb_ivar_set(VALUE obj, ID sym, VALUE val) {
  CTX;
  
  object_set_ivar(ctx->state, HNDL(obj), HNDL(sym), HNDL(val));
  
  return val;  
}

VALUE rb_iv_set(VALUE obj, char *name, VALUE val) {
  return rb_ivar_set(obj, rb_intern(name), val);
}

void rb_define_method_(const char *file, VALUE vmod, const char *name, void *func, int args, int kind) {
  OBJECT meth, sym, mod;
  CTX;
  
  mod = HNDL(vmod);
  
  meth = nmethod_new(ctx->state, mod, file, name, func, args);
  sym = symtbl_lookup_cstr(ctx->state, ctx->state->global->symbols, name);
  if(kind == 0) {
    cpu_add_method(ctx->state, ctx->cpu, mod, sym, meth);
  } else {
    cpu_attach_method(ctx->state, ctx->cpu, mod, sym, meth);    
  }
}

void rb_define_alloc_func(VALUE class, void *func) {
  rb_define_singleton_method(class, "allocate", func, 0);
}

const char *rb_id2name(ID sym) {
  OBJECT obj;
  CTX;
  
  obj = (OBJECT)sym;
  if(!RBX_SYMBOL_P(obj)) return NULL;
  return rbs_symbol_to_cstring(ctx->state, obj);
}

VALUE rb_obj_alloc(VALUE klass) {
  CTX;
  OBJECT o;

  o = class_new_instance(ctx->state, HNDL(klass));
  return NEW_HANDLE(ctx, o);
}

void rb_obj_call_init(VALUE obj, int nargs, VALUE *args) {
  rb_funcall2(obj, rb_intern("initialize"), nargs, args);
}

VALUE rb_obj_is_instance_of(VALUE obj, VALUE klass) {
  return rb_funcall(obj, rb_intern("instance_of?"), 1, klass);
}

VALUE rb_obj_is_kind_of(VALUE obj, VALUE klass) {
  return rb_funcall(obj, rb_intern("kind_of?"), 1, klass);
}

int rb_respond_to(VALUE obj, ID sym) {
  return rb_funcall(obj, rb_intern("respond_to?"), 1, sym) == Qtrue ? 1 : 0;
}

ID rb_to_id(VALUE obj) {
  return SYM2ID(rb_funcall(obj, rb_intern("to_sym"), 0));
}

VALUE rb_check_array_type(VALUE ary) {
  CTX;
  VALUE coercable = rb_funcall(ary, rb_intern("respond_to?"), 1, CHAR2STR("to_ary"));
  if(coercable == Qtrue) return rb_funcall(ary, rb_intern("to_ary"), 0);
  else return Qnil;
}

VALUE rb_check_string_type(VALUE str) {
  CTX;
  VALUE coercable = rb_funcall(str, rb_intern("respond_to?"), 1, CHAR2STR("to_str"));
  if(coercable == Qtrue) return rb_funcall(str, rb_intern("to_str"), 0);
  else return NEW_HANDLE(ctx, string_new(ctx->state, ""));  
}

VALUE rb_check_convert_type(VALUE val, int type, const char* tname, const char* method) {
  CTX;
  VALUE coercable = rb_funcall(val, rb_intern("respond_to?"), 1, CHAR2STR(method));
  if(coercable == Qtrue) return rb_funcall(val, rb_intern(method), 0);
  else return Qnil;  
}

VALUE rb_class_new_instance(int nargs, VALUE *args, VALUE klass) {
  VALUE obj;

  obj = rb_obj_alloc(klass);
  rb_obj_call_init(obj, nargs, args);

  return obj;
}

void rb_thread_schedule() {
  rb_funcall2(rb_const_get(rb_cObject, rb_intern("Thread")), rb_intern("pass"), 0, NULL);
}

void rb_secure(int level) {
  return;
}

int rb_ary_size(VALUE self) {
  OBJECT ary;
  CTX;
  
  ary = HNDL(self);
  
  return FIXNUM_TO_INT(array_get_total(ary));
}

/* The same value as 1.8.x */
#define ARRAY_DEFAULT_SIZE 16

/* Float */

VALUE rb_float_new(double d) {
  OBJECT flt;
  CTX;
  
  flt = float_new(ctx->state, d);
  
  return NEW_HANDLE(ctx, flt);
}

/* Array */

VALUE rb_Array(VALUE val) {
  CTX;
  VALUE ary, tmp;
  
  ary = HNDL(val);
  if(object_kind_of_p(ctx->state, ary, ctx->state->global->array)) {
    return val;
  }
  
  tmp = array_new(ctx->state, 1);
  array_append(ctx->state, tmp, ary);
  
  return NEW_HANDLE(ctx, tmp);
}

VALUE rb_ary_new(void) {
  return rb_ary_new2(ARRAY_DEFAULT_SIZE);
}

VALUE rb_ary_new2(long length) {
  OBJECT ary;
  CTX;

  ary = array_new(ctx->state, length);
  
  return NEW_HANDLE(ctx, ary);
}

VALUE rb_ary_push(VALUE array, VALUE val) {
  CTX;
  OBJECT ary = HNDL(array);
  array_append(ctx->state, ary, HNDL(val));
  return array;
}

VALUE rb_ary_pop(VALUE array) {
  CTX;
  OBJECT ary = HNDL(array);
  return NEW_HANDLE(ctx, array_pop(ctx->state, ary));
}

VALUE rb_ary_entry(VALUE array, int offset) {
  CTX;
  OBJECT val, ary = HNDL(array);

  int total = FIXNUM_TO_INT(array_get_total(ary));

  /* support wrap-around */
  if(offset < 0) {
    offset += total;
  }

  /* we silently ignore out-of-range errors */
  if(offset < 0 || offset >= total) {
    return Qnil;
  }

  val = array_get(ctx->state, ary, offset);

  return NEW_HANDLE(ctx, val);
}

VALUE rb_ary_clear(VALUE array) {
  return rb_funcall2(array, rb_intern("clear"), 0, NULL);
}

VALUE rb_ary_dup(VALUE array) {
  return rb_funcall2(array, rb_intern("dup"), 0, NULL);
}

VALUE rb_ary_join(VALUE array1, VALUE array2) {
  return rb_funcall(array1, rb_intern("join"), 1, array2);
}

VALUE rb_ary_reverse(VALUE array) {
  return rb_funcall(array, rb_intern("reverse"), 0);
}

VALUE rb_ary_unshift(VALUE array, VALUE val) {
  return rb_funcall2(array, rb_intern("unshift"), 1, &val);
}

VALUE rb_ary_shift(VALUE array) {
  return rb_funcall2(array, rb_intern("shift"), 0, NULL);
}

void rb_ary_store(VALUE array, int offset, VALUE val) {
  CTX;
  OBJECT ary = HNDL(array);

  int total = FIXNUM_TO_INT(array_get_total(ary));

  /* support wrap-around */
  if(offset < 0) {
    offset += total;
  }

  /* if the offset is still negative, we need to raise IndexError */
  if(offset < 0) {
    rb_raise(rb_eIndexError, "index %i out of range", offset);
  }

  array_set(ctx->state, ary, offset, HNDL(val));
}

/* Strings */

VALUE rb_str_new(const char *ptr, long len) {
  CTX;
  return NEW_HANDLE(ctx, string_new2(ctx->state, (char*)ptr, len));
}

VALUE rb_str_new2(const char *ptr) {
  if (!ptr)
    rb_raise(rb_eArgError, "NULL pointer given");

  return rb_str_new(ptr, strlen(ptr));
}

VALUE rb_tainted_str_new2(const char *ptr) {
  return rb_str_new2(ptr);
}

VALUE rb_str_dup(VALUE str) {
  CTX;
  return NEW_HANDLE(ctx, string_dup(ctx->state, HNDL(str)));
}

VALUE rb_str_buf_cat(VALUE str, const char *ptr, long len) {
  CTX;
  return NEW_HANDLE(ctx, string_append(ctx->state, HNDL(str), string_new2(ctx->state, ptr, len)));
}

VALUE rb_str_append(VALUE str, VALUE str2) {
  CTX;
  return NEW_HANDLE(ctx, string_append(ctx->state, HNDL(str), HNDL(str2)));
}

VALUE rb_str_cat(VALUE str, const char *ptr, long len) {
  if(len < 0) rb_raise(rb_eArgError, "negative string size (or size too big)");
  return rb_funcall(str, rb_intern("+"), 1, rb_str_new(ptr, len));
}

VALUE rb_str_plus(VALUE str1, VALUE str2) {
  CTX;
  OBJECT new_string = string_dup(ctx->state, HNDL(str1));
  return NEW_HANDLE(ctx, string_append(ctx->state, new_string, HNDL(str2)));
}

VALUE rb_str_cmp(VALUE str1, VALUE str2) {
  return rb_funcall(str1, rb_intern("<=>"), 1, str2);
}

VALUE rb_str_split(VALUE str, const char *sep) {
  CTX;
  return rb_funcall(str, rb_intern("split"), 1, CHAR2STR(sep));
}

VALUE rb_str2inum(VALUE str, int base) {
  return rb_funcall(str, rb_intern("to_i"), 1, INT2NUM(base));
}

VALUE rb_cstr2inum(VALUE str, int base) {
  CTX;
  return rb_funcall(CHAR2STR(str), rb_intern("to_i"), 1, INT2NUM(base));
}

VALUE rb_str_substr(VALUE str, long beg, long len) {
  return rb_funcall(str,  rb_intern("substring"), 2, INT2NUM(beg), INT2NUM(len));
}

char *StringValuePtr(VALUE str) {
  CTX;
  return (char*)string_byte_address(ctx->state, HNDL(str));
}

VALUE rb_obj_as_string(VALUE val) {
  CTX;
  OBJECT str = HNDL(val);
    
  if(object_kind_of_p(ctx->state, str, ctx->state->global->string)) {
    return val;
  }
  
  return rb_funcall2(val, rb_intern("to_s"), 0, NULL);
}

void rb_string_value(VALUE *val) {
  *val = rb_obj_as_string(*val);
}

/* Hash */

VALUE rb_hash_new(void) {
  CTX;
  return NEW_HANDLE(ctx, hash_new(ctx->state));
}

VALUE rb_hash_aref(VALUE hash, VALUE key) {
  CTX;
  OBJECT val = hash_get(ctx->state, HNDL(hash), object_hash_int(ctx->state, HNDL(key)));
  return NEW_HANDLE(ctx, val);
}

VALUE rb_hash_aset(VALUE hash, VALUE key, VALUE val) {
  CTX;
  OBJECT hsh, k, v;
  hsh = HNDL(hash);
  k = HNDL(key);
  v = HNDL(val);
  hash_set(ctx->state, hsh, k, v);
  
  return val;
}

VALUE rb_hash_delete(VALUE hash, VALUE key) {
  CTX;
  OBJECT hsh = HNDL(hash);
  return NEW_HANDLE(ctx, hash_delete(ctx->state, hsh, object_hash_int(ctx->state, HNDL(key))));
}

/* Exceptions */

void rb_raise(VALUE exc, const char *fmt, ...) {
  CTX;
  va_list args;
  char buf[BUFSIZ];
  
  va_start(args, fmt);
  vsnprintf(buf, BUFSIZ, fmt, args);
  va_end(args);
  
  ctx->nmc->value = NEW_HANDLE(ctx, cpu_new_exception(ctx->state, ctx->cpu, HNDL(exc), buf));
  ctx->nmc->jump_val = RAISED_EXCEPTION;
  setcontext(&ctx->nmc->system);
}

VALUE rb_require(const char* name) {
  CTX;
  VALUE str = NEW_HANDLE(ctx, string_new(ctx->state, name));
  return rb_funcall((VALUE)Qnil, rb_intern("require"), 1, str);
}

int FIX2INT(VALUE val) {
  CTX;
  OBJECT arg = HNDL(val);
  return FIXNUM_TO_INT(arg);
}

VALUE INT2NUM(int num) {
  CTX;
  return NEW_HANDLE(ctx, rbs_int_to_fixnum(ctx->state, num));
}

int SYMBOL_P(VALUE obj) {
  return RBX_SYMBOL_P(obj);
}

/* RSTRING Macro Replacement */
char* rb_str_get_char_ptr(VALUE arg) {
  CTX;
  OBJECT str = HNDL(arg);
  
  return strdup(string_byte_address(ctx->state, str));
}

void rb_str_flush_char_ptr(VALUE arg, char* ptr) {
  CTX;
  STATE;
  state = ctx->state;
  OBJECT str = HNDL(arg);
  OBJECT new_string = string_new(ctx->state, ptr);
  string_set_data(str, string_get_data(new_string));
  // XXX This free may be resulting in double frees.
  XFREE(ptr);
}

int rb_str_get_char_len(VALUE arg) {
  CTX;
  OBJECT str = HNDL(arg);
  return strlen(string_byte_address(ctx->state, str));
}

char rb_str_get_char(VALUE arg, int index) {
  CTX;
  char *data;
  
  OBJECT str = HNDL(arg);
  
  data = string_byte_address(ctx->state, str);
  
  return data[index];
}

/* Data_Get_Struct and Data_Wrap_Struct support */

VALUE subtend_wrap_struct(VALUE klass, void *struct_value, void *mark_func, void *free_func) {
  CTX;
  STATE;
  state = ctx->state;
  
  OBJECT obj = NEW_OBJECT(HNDL(klass), sizeof(struct wraps_struct)/sizeof(uintptr_t));
  struct wraps_struct *s = (struct wraps_struct *)BYTES_OF(obj);
  s->ptr = struct_value;
  s->mark = mark_func;
  s->free = free_func;
  return NEW_HANDLE(ctx, obj);
}

void* subtend_get_struct(VALUE obj) {
  CTX;
  struct wraps_struct *s = (struct wraps_struct *)BYTES_OF(HNDL(obj));
  return s->ptr;
}

/*

Still needed for Mongrel - Kev

rb_global_variable

*/
