#include "shotgun.h"
#include <stdarg.h>
#include "nmc.h"
#include "ruby.h"
#include "symbol.h"
#include "string.h"
#include "hash.h"

OBJECT nmethod_new(STATE, OBJECT mod, char *file, char *name, void *func, int args);
#define AS_HNDL(obj) ((rni_handle*)obj)

#define HNDL(obj) handle_to_object(ctx->state, ctx->state->handle_tbl, (rni_handle*)obj)
#define CTX rni_context* ctx = subtend_retrieve_context()
#define NEW_HANDLE(ctx, val) nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, val)

ID rb_intern(char *name) {
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
    val = Qnil;
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
  swapcontext(&n->cont, &n->system);
  /* When we return here, the call has been done. */
  return n->value;
}

VALUE rb_funcall(VALUE recv, ID meth, int args, ...) {
  va_list ar;
  VALUE *arg_array;
  int i;
  VALUE ret;

  arg_array = calloc(args, sizeof(VALUE));
  va_start(ar, args);
  for(i = 0; i < args; i++) {
    arg_array[i] = va_arg(ar, VALUE);
  }
  
  va_end(ar);
  
  ret = _push_and_call(recv, meth, args, arg_array);
  free(arg_array);
  return ret;
  
}

VALUE rb_funcall2(VALUE recv, ID meth, int args, VALUE* array) {
  return _push_and_call(recv, meth, args, array);
}

/* Functions for adding behaviors (ie, classes/modules and methods) */

VALUE rb_define_class_under(VALUE parent, char *name, VALUE super) {
  CTX;
  OBJECT cls;
  
  cls = rbs_class_new_with_namespace(ctx->state, name, 1, HNDL(super), HNDL(parent));
  
  return NEW_HANDLE(ctx, cls);
}

VALUE rb_define_class(char *name, VALUE super) {
  CTX;
  OBJECT cls;
  cls = rbs_class_new_with_namespace(ctx->state, name, 1, HNDL(super), ctx->state->global->object);
  
  return NEW_HANDLE(ctx, cls);
}

VALUE rb_define_module_under(VALUE parent, char *name) {
  CTX;
  OBJECT mod;
  mod = rbs_module_new(ctx->state, name, HNDL(parent));
  
  return NEW_HANDLE(ctx, mod);
}

VALUE rb_define_module(char *name) {
  CTX;
  OBJECT mod;
  mod = rbs_module_new(ctx->state, name, ctx->state->global->object);
  
  return NEW_HANDLE(ctx, mod);
}

VALUE rb_const_get(VALUE klass, ID id) {
  CTX;
  return NEW_HANDLE(ctx, cpu_const_get(ctx->state, ctx->cpu, (OBJECT)id, HNDL(klass)));
}

VALUE rb_ivar_get(VALUE obj, ID sym) {
  CTX;
  OBJECT val;
  
  val = object_get_ivar(ctx->state, HNDL(obj), HNDL(sym));
  
  return NEW_HANDLE(ctx, val);
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

void rb_define_method_(char *file, VALUE vmod, char *name, void *func, int args, int kind) {
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

char *rb_id2name(ID sym) {
  OBJECT obj;
  CTX;
  
  obj = (OBJECT)sym;
  if(!SYMBOL_P(obj)) return NULL;
  return rbs_symbol_to_cstring(ctx->state, obj);
}

VALUE rb_ary_get(VALUE self, int index) {
  OBJECT ary, out;
  CTX;
  
  ary = HNDL(self);
  out = array_get(ctx->state, ary, index);
  
  return NEW_HANDLE(ctx, out);
}

VALUE rb_ary_set(VALUE self, int index, VALUE val) {
  CTX;
  
  array_set(ctx->state, HNDL(self), index, HNDL(val));
  
  return val;  
}

int rb_ary_size(VALUE self) {
  OBJECT ary;
  CTX;
  
  ary = HNDL(self);
  
  return FIXNUM_TO_INT(array_get_total(ary));
}

/* The same value as 1.8.x */
#define ARRAY_DEFAULT_SIZE 16

/* Array */

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
  return NEW_HANDLE(ctx, ary);
}

VALUE rb_str_new(const char *ptr, long len) {
  CTX;
  return NEW_HANDLE(ctx, string_new2(ctx->state, (char*)ptr, len));
}

VALUE rb_str_dup(VALUE str) {
  CTX;
  return NEW_HANDLE(ctx, string_dup(ctx->state, HNDL(str)));
}

VALUE rb_str_buf_cat(VALUE str, const char *ptr, long len) {
  CTX;
  return NEW_HANDLE(ctx, string_append(ctx->state, HNDL(str), HNDL(rb_str_new(ptr, len))));
}

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
  return NEW_HANDLE(ctx, hash_set(ctx->state, hsh, k, v));
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
  cpu_raise_exception(ctx->state, ctx->cpu,
    cpu_new_exception(ctx->state, ctx->cpu, HNDL(exc), buf));
}

/*

Still needed for Mongrel - Kev

rb_define_alloc_func
rb_str_substr

*/

/*

rb_define_singleton_method
rb_gc_mark
rb_gv_get

rb_class2name
rb_obj_classname

rb_call_super
rb_respond_to

rb_str_new2
rb_str_catf
rb_str_resize
rb_str_cat2
rb_ary_entry

rb_block_given_p
rb_block_proc
rb_throw
rb_obj_alloc
rb_obj_call_init
rb_ary_dup
rb_ary_unshift
rb_ary_clear
rb_float_new
Data_Wrap_Struct

rb_num2ull
rb_ull2inum

rb_gc_register_address
rb_gc_unregister_address

*/
