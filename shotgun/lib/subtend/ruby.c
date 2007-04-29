#include "shotgun.h"
#include <stdarg.h>
#include "nmc.h"
#include "ruby.h"
#include "symbol.h"

OBJECT nmethod_new(STATE, OBJECT mod, char *file, char *name, void *func, int args);
#define AS_HNDL(obj) ((rni_handle*)obj)

#define HNDL(obj) handle_to_object(ctx->state, ctx->state->handle_tbl, (rni_handle*)obj)
#define CTX rni_context* ctx = subtend_retrieve_context()

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
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, val);
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
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, cls);
}

VALUE rb_define_class(char *name, VALUE super) {
  CTX;
  OBJECT cls;
  cls = rbs_class_new_with_namespace(ctx->state, name, 1, HNDL(super), ctx->state->global->object);
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, cls);
}

VALUE rb_define_module_under(VALUE parent, char *name) {
  CTX;
  OBJECT mod;
  mod = rbs_module_new(ctx->state, name, HNDL(parent));
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, mod);
}

VALUE rb_define_module(char *name) {
  CTX;
  OBJECT mod;
  mod = rbs_module_new(ctx->state, name, ctx->state->global->object);
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, mod);
}

ID rb_intern(char *name) {
  CTX;
  return (ID)symtbl_lookup_cstr(ctx->state, ctx->state->global->symbols, name);
}

VALUE rb_ivar_get(VALUE obj, ID sym) {
  CTX;
  OBJECT val;
  
  val = object_get_ivar(ctx->state, HNDL(obj), HNDL(sym));
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, val);
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

VALUE rb_ary_get(VALUE self, int index) {
  OBJECT ary, out;
  CTX;
  
  ary = HNDL(self);
  out = array_get(ctx->state, ary, index);
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, out);
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

char *rb_id2name(ID sym) {
  OBJECT obj;
  CTX;
  
  obj = (OBJECT)sym;
  if(!SYMBOL_P(obj)) return NULL;
  return rbs_symbol_to_cstring(ctx->state, obj);
}

/* The same value as 1.8.x */
#define ARRAY_DEFAULT_SIZE 16

VALUE rb_ary_new(void) {
  return rb_ary_new2(ARRAY_DEFAULT_SIZE);
}

VALUE rb_ary_new2(long length) {
  OBJECT ary;
  CTX;

  ary = array_new(ctx->state, length);
  
  return nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, ary);
}

/*

rb_define_singleton_method
rb_gc_mark
rb_gv_get

rb_class2name
rb_obj_classname

rb_call_super
rb_respond_to
rb_raise

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
rb_ary_push
rb_ary_clear
rb_float_new
Data_Wrap_Struct

rb_hash_new
rb_hash_aset

rb_num2ull
rb_ull2inum

rb_gc_register_address
rb_gc_unregister_address

*/
