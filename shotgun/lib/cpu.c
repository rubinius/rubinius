#include "shotgun.h"
#include "cpu.h"
#include "machine.h"
#include <stdlib.h>
#include "tuple.h"
#include "methctx.h"
#include "object.h"
#include "bytearray.h"
#include "string.h"
#include "class.h"
#include "hash.h"
#include "symbol.h"

cpu cpu_new(STATE) {
  cpu c = (cpu)calloc(1, sizeof(struct rubinius_cpu));
  c->paths = ptr_array_new(8);
  return c;
}

#define FC_STACK_SIZE    3000

void cpu_initialize(STATE, cpu c) {
  state->global->tuple = Qnil;
  state->global->hash = Qnil;
  state->global->methtbl = Qnil;
  c->stack_top = (OBJECT*)calloc(InitialStackSize, sizeof(OBJECT));
  c->sp_ptr = c->stack_top;
  c->stack_size = InitialStackSize;
  c->sp = 0;
  c->ip = 0;
  c->self = Qnil;
  c->exception = Qnil;
  c->enclosing_class = Qnil;
  c->new_class_of = Qnil;
  c->outstanding = Qnil;
  c->args = 0;
  c->depth = 0;
  c->call_flags = 0;
  c->cache_index = -1;
  c->current_task = Qnil;
  c->debug_channel = Qnil;
  c->control_channel = Qnil;

  cpu_sampler_init(state, c);
}

void cpu_setup_top_scope(STATE, cpu c) {
  c->enclosing_class = state->global->object;
  c->new_class_of = state->global->class;
}

OBJECT cpu_scope_push(STATE, cpu c, OBJECT mod) {
  OBJECT scope = staticscope_allocate(state);
  staticscope_set_module(scope, mod);
  staticscope_set_parent(scope, c->current_scope);
  
  c->current_scope = scope;
  return scope;
}

OBJECT cpu_scope_pop(STATE, cpu c) {
  c->current_scope = staticscope_get_parent(c->current_scope);
  return c->current_scope;
}

void cpu_initialize_context(STATE, cpu c) {
  c->active_context = Qnil;
  c->home_context = c->active_context;
  c->enclosing_class = state->global->object;
  c->new_class_of = state->global->class;
  c->exception = Qnil;
  c->main = object_new(state);
  rbs_const_set(state, state->global->object, "MAIN", c->main);
  
  state->global->method_missing = string_to_sym(state, 
        string_new(state, "method_missing"));
  
  state->global->sym_inherited = string_to_sym(state, 
        string_new(state, "inherited"));

  state->global->sym_method_added = string_to_sym(state, 
        string_new(state, "method_added"));
        
  state->global->sym_s_method_added = string_to_sym(state,
        string_new(state, "singleton_method_added"));
        
  state->global->sym_plus =  symbol_from_cstr(state, "+");
  state->global->sym_minus = symbol_from_cstr(state, "-");
  state->global->sym_equal = symbol_from_cstr(state, "==");
  state->global->sym_nequal = symbol_from_cstr(state, "!=");
  state->global->sym_tequal = symbol_from_cstr(state, "===");
  state->global->sym_lt =    symbol_from_cstr(state, "<");
  state->global->sym_gt =    symbol_from_cstr(state, ">");
  state->global->sym_send =    symbol_from_cstr(state, "__send__");
  state->global->sym_public = symbol_from_cstr(state, "public");
  state->global->sym_private = symbol_from_cstr(state, "private");
  state->global->sym_protected = symbol_from_cstr(state, "protected");
  state->global->sym_const_missing = SYM("const_missing");
  state->global->sym_object_id = SYM("object_id");
  state->global->sym_from_literal = SYM("from_literal");
  state->global->sym_opened_class = SYM("opened_class");
  
  c->current_thread = cpu_thread_new(state, c);
  c->main_thread = c->current_thread;
  c->current_task = cpu_thread_get_task(state, c->current_thread);
  c->main_task = c->current_task;
  
  c->current_scope = Qnil;
  cpu_scope_push(state, c, BASIC_CLASS(object));
  state->global->top_scope = c->current_scope;

  cpu_event_setup_children(state, c);
}

void cpu_add_roots(STATE, cpu c, ptr_array roots) {
  int i, len;
  xpointer t;
  #define ar(obj) if(REFERENCE_P(obj)) { \
    ptr_array_append(roots, (xpointer)obj); \
  }
  
  if(!stack_context_p(c->active_context)) {
    ar(c->active_context);
    state->ac_on_stack = 0;
  } else {
    state->ac_on_stack = 1;
  }
  
  if(!stack_context_p(c->home_context)) {
    ar(c->home_context);
    state->home_on_stack = 0;
  } else {
    state->home_on_stack = 1;
  }
  
  if(REFERENCE_P(c->sender)) {
    if(!stack_context_p(c->sender)) {
      ar(c->sender);
      state->sender_on_stack = 0;
    } else {
      state->sender_on_stack = 1;
    }
  }
  
  ar(c->self);
  ar(c->cache);
  ar(c->exception);
  ar(c->enclosing_class);
  ar(c->new_class_of);
  ar(c->main);
  ar(c->exceptions);
  ar(c->current_thread);
  ar(c->main_thread);
  ar(c->current_task);
  ar(c->main_task);
  ar(c->outstanding);
  ar(c->debug_channel);
  ar(c->control_channel);
  ar(c->current_scope);
  len = ptr_array_length(c->paths);
  ptr_array_append(roots, (xpointer)I2N(len));
  // printf("Paths: %d\n", len);
  for(i = 0; i < len; i++) {
    t = ptr_array_remove_index_ordered(c->paths, 0);
    //printf("Pulled %s out of paths.\n", _inspect(t));
    ar(t);
  }
  //printf("Paths should be empty: %d\n", c->paths->len);
  
  #undef ar
}

void cpu_update_roots(STATE, cpu c, ptr_array roots, int start) {
  xpointer tmp;
  int i, len;
  #define ar(obj) if(REFERENCE_P(obj)) { \
    tmp = ptr_array_get_index(roots, start++); \
    obj = (OBJECT)tmp; \
  }
  
  if(state->ac_on_stack) {
    /* if active_context is on the stack, it's the last object. */
    /* if context_offset is 0, then we didn't need to compact, they're
       still pointed at the right point. */
    
    if(state->om->context_offset != 0) {
      c->active_context = (OBJECT)((uintptr_t)(c->active_context) - state->om->context_offset);
      state->om->context_offset = 0;
    }
  } else {
    ar(c->active_context);
  }
  
  if(state->home_on_stack) {
    /* If it's on the stack, it's the same as active */
    c->home_context = c->active_context;
  } else {
    ar(c->home_context);
  }
  
  if(REFERENCE_P(c->sender)) {
    if(state->sender_on_stack) {
      c->sender = FASTCTX(c->active_context)->sender;
    } else {
      ar(c->sender);
    }
  }
  
  ar(c->self);
  ar(c->cache);
  ar(c->exception);
  ar(c->enclosing_class);
  ar(c->new_class_of);
  ar(c->main);
  ar(c->exceptions);
  ar(c->current_thread);
  ar(c->main_thread);
  ar(c->current_task);
  ar(c->main_task);
  ar(c->outstanding);
  ar(c->debug_channel);
  ar(c->control_channel);
  ar(c->current_scope);
  tmp = ptr_array_get_index(roots, start++);
  len = FIXNUM_TO_INT((OBJECT)tmp);
  for(i = 0; i < len; start++, i++) {
    tmp = ptr_array_get_index(roots, start);
    //printf("Adding path %s back in...\n", _inspect(tmp));
    ptr_array_append(c->paths, tmp);
  }
  //printf("Paths is %d\n", c->paths->len);
  #undef ar
  
  cpu_flush_ip(c);
}

void cpu_raise_exception(STATE, cpu c, OBJECT exc) {
  OBJECT ctx, table, ent, env;
  int cur, total, target, idx, l, r, is_block;
  c->exception = exc;
  ctx = c->active_context;
  
  if(INTERNAL_DEBUG && getenv("EXCHALT")) {
    printf("An exception has occured: %s\n", _inspect(exc));
    assert(0);
  }
  
  cpu_flush_ip(c);
  
  /* NOTE: using return_to_sender worries me a little because it can
     switch to a different task if you try to return off the top
     of a task.. */
  
  while(!NIL_P(ctx)) {
    is_block = blokctx_s_block_context_p(state, ctx);
    
    if(c->type == FASTCTX_NMC) {
      cpu_return_to_sender(state, c, Qnil, FALSE, TRUE);
      ctx = c->active_context;
      continue;
    }    
    
    table = cmethod_get_exceptions(cpu_current_method(state, c));
    
    if(!table || NIL_P(table)) {
      cpu_return_to_sender(state, c, Qnil, FALSE, TRUE);
      ctx = c->active_context;
      continue;
    }
    
    cur = c->ip;
    total = NUM_FIELDS(table);
    target = 0;
    for(idx=0; idx < total; idx++) {
      ent = tuple_at(state, table, idx);
      l = FIXNUM_TO_INT(tuple_at(state, ent, 0));
      r = FIXNUM_TO_INT(tuple_at(state, ent, 1));
      if(cur >= l && cur <= r) {
        /* Make sure the bounds are within the block, therwise, don't use
           it. */
        if(is_block) {
          env = blokctx_env(state, ctx);
          if(l < FIXNUM_TO_INT(blokenv_get_initial_ip(env))
                  || r > FIXNUM_TO_INT(blokenv_get_last_ip(env))) {
            continue;
          }
        }
        target = FIXNUM_TO_INT(tuple_at(state, ent, 2));
        c->ip = target;
        cpu_cache_ip(c);
        return;
      }
    }
    
    cpu_return_to_sender(state, c, Qnil, FALSE, TRUE);
    ctx = c->active_context;
  }
  
  /* Reset it because it can get overriden in the return_to_senders. */
  c->exception = exc;
  
  // printf("Unable to find exception handler, i'm confused.\n");
  return;
}

OBJECT cpu_new_exception(STATE, cpu c, OBJECT klass, const char *msg) {
  OBJECT obj, str;
  
  obj = class_new_instance(state, klass);
  str = string_new(state, msg);
  exception_set_message(obj, str);
  methctx_reference(state, c->active_context);
  exception_set_context(obj, c->active_context);
  return obj;
}

OBJECT cpu_const_get_in_context(STATE, cpu c, OBJECT sym) {
  OBJECT cur, klass, start, hsh, val;
  OBJECT cref, cbase;

  c->cache_index = -1;
  
  /* Look up the lexical scope first */
  
  cref = cmethod_get_staticscope(cpu_current_method(state, c));
  if(NIL_P(cref)) {
    start = state->global->object;
  } else {
    cbase = cref;

    while(!NIL_P(cbase)) {
      klass = staticscope_get_module(cbase);
      hsh = module_get_constants(klass);
      val = hash_find_undef(state, hsh, sym);
      if(val != Qundef) return val;
    
      cbase = staticscope_get_parent(cbase);
    }
    
  
    start = cur = staticscope_get_module(cref);
  
    while(!NIL_P(cur) && cur != state->global->object) {
    
      hsh = module_get_constants(cur);
      val = hash_find_undef(state, hsh, sym);
      if(val != Qundef) return val;
      cur = module_get_superclass(cur);
    }
  }
  
  // As a last rescue, we search in Object's constants
  hsh = module_get_constants(state->global->object);
  val = hash_find_undef(state, hsh, sym);
  if(val != Qundef) return val;

  c->cache_index = -1;
  stack_push(sym);
  cpu_unified_send(state, c, start, state->global->sym_const_missing, 1, Qnil);
  return Qundef;
}

OBJECT cpu_const_get_from(STATE, cpu c, OBJECT sym, OBJECT under) {
  OBJECT cur, hsh, val;
  
  // printf("Looking for %s under %s.\n", rbs_symbol_to_cstring(state, sym), rbs_symbol_to_cstring(state, module_get_name(under)));
  
  cur = under;
  
  while(!NIL_P(cur)) {
    // printf("   looking in %s\n", rbs_symbol_to_cstring(state, module_get_name(cur)));
    
    hsh = module_get_constants(cur);
    val = hash_find_undef(state, hsh, sym);
    if(val != Qundef) { 
      // printf("   found!\n");
      return val;
    }
    /* Object's superclass MUST be nil, but we check directly just
       to be safe. */
    if(cur == state->global->object) break;
    cur = class_get_superclass(cur);
  }
  
  if(object_kind_of_p(state, under, state->global->module)) {
    // printf("Looking directly in Object.\n");
    hsh = module_get_constants(state->global->object);
    val = hash_find_undef(state, hsh, sym);
    if(val != Qundef) { 
      // printf("   found!\n");
      return val;
    }
  }
  
  // printf("Still unable to find, firing const_missing.\n");
  
  c->cache_index = -1;
  stack_push(sym);
  cpu_unified_send(state, c, under, state->global->sym_const_missing, 1, Qnil);
  return Qundef;
}

OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under) {
  return cpu_const_get_from(state, c, sym, under);
}

OBJECT cpu_const_set(STATE, cpu c, OBJECT sym, OBJECT val, OBJECT under) {
  OBJECT hsh;
  
  hsh = module_get_constants(under);
  hash_set(state, hsh, sym, val);
  return val;
}

void cpu_set_encloser_path(STATE, cpu c, OBJECT cls) {
  int len;
  len = ptr_array_length(c->paths);
  /* add stuff for @paths here */
  ptr_array_append(c->paths, (xpointer)c->enclosing_class);

  cmethod_set_staticscope(cpu_current_method(state, c), 
                          cpu_scope_push(state, c, cls));
  
  c->enclosing_class = cls;
}

void cpu_push_encloser(STATE, cpu c) {
  int len;
  len = ptr_array_length(c->paths);
  if(len > 0) {
    c->enclosing_class = (OBJECT)ptr_array_remove_index_ordered(c->paths, len - 1);
    cpu_scope_pop(state, c);
  }
}

/* Increments serial numbers up the superclass chain. */
static void cpu_increment_serials(STATE, OBJECT module, OBJECT sym) {
  OBJECT hsh, meth;
  
  while(!NIL_P(module)) {
    hsh = module_get_methods(module);
    meth = hash_find(state, hsh, sym);
    
    if(REFERENCE_P(meth)) {
      if(CLASS_OBJECT(meth) == BASIC_CLASS(tuple)) { 
        meth = tuple_at(state, meth, 1);
      }
      fast_inc(meth, CMETHOD_f_SERIAL);
    }
    
    module = class_get_superclass(module);
  }
}

void cpu_add_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method) {
  OBJECT meths, vis;
  // Handle a special case where we try and add a method to main
  if(target == c->main) {
    target = c->enclosing_class;
  }
  
  cpu_clear_cache_for_method(state, c, sym, FALSE);
  
  cpu_increment_serials(state, target, sym);
  meths = module_get_methods(target);
  
  switch(c->call_flags) {
  default:
  case 0:
    vis = state->global->sym_public;
    break;
  case 1:
    vis = state->global->sym_private;
    break;
  }
  
  if(EXCESSIVE_TRACING) {
    printf("=> Adding method %s to %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(target));
  }
  
  // HACK. the 10 sucks, it protects things that go in a method table, but
  // aren't exactly CompiledMethods.
  if(NUM_FIELDS(method) > 10 && NIL_P(cmethod_get_staticscope(method))) {
    cmethod_set_staticscope(method, c->current_scope);
  }
  
  hash_set(state, meths, sym, tuple_new2(state, 2, vis, method));
  c->call_flags = 0;
}

void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method) {
  OBJECT meta;
  meta = object_metaclass(state, target);
  /* static visibility scope doesn't impact singleton classes.
     we force it to public everytime it's used. */
  c->call_flags = 0;
  cpu_add_method(state, c, meta, sym, method);
}

/* Updates the cpu registers by reading out of the active context.
   These get out of sync when the GC runs. */
void cpu_hard_cache(STATE, cpu c) {
  struct fast_context *fc;
  
  cpu_flush_ip(c);
  
  fc = (struct fast_context*)BYTES_OF(c->home_context);
  c->data = fc->data;
  
  cpu_cache_ip(c);  
}

