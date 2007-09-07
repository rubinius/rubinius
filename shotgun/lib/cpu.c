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
#include <glib.h>

#define EXCESSIVE_TRACING state->excessive_tracing

cpu cpu_new(STATE) {
  cpu c = (cpu)calloc(1, sizeof(struct rubinius_cpu));
  c->paths = g_ptr_array_new();
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

  state->global->sym_from_literal = string_to_sym(state, 
        string_new(state, "from_literal"));
        
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
  
  c->current_thread = cpu_thread_new(state, c);
  c->main_thread = c->current_thread;
  c->current_task = cpu_thread_get_task(state, c->current_thread);
  c->main_task = c->current_task;
  
  cpu_event_setup_children(state, c);
}

void cpu_add_roots(STATE, cpu c, GPtrArray *roots) {
  int i, len;
  gpointer t;
  #define ar(obj) if(REFERENCE_P(obj)) { \
    g_ptr_array_add(roots, (gpointer)obj); \
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
  
  /*
  if(REFERENCE_P(c->sender)) {
    if(!stack_context_p(c->sender)) {
      ar(c->sender);
      state->sender_on_stack = 0;
    } else {
      state->sender_on_stack = 1;
    }
  }
  */
  
  ar(c->self);
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
  len = c->paths->len;
  g_ptr_array_add(roots, (gpointer)I2N(len));
  // printf("Paths: %d\n", len);
  for(i = 0; i < len; i++) {
    t = g_ptr_array_remove_index(c->paths, 0);
    //printf("Pulled %s out of paths.\n", _inspect(t));
    ar(t);
  }
  //printf("Paths should be empty: %d\n", c->paths->len);
  
  #undef ar
}

void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start) {
  gpointer tmp;
  int i, len;
  #define ar(obj) if(REFERENCE_P(obj)) { \
    tmp = g_ptr_array_index(roots, start++); \
    obj = (OBJECT)tmp; \
  }
  
  if(state->ac_on_stack) {
    /* if active_context is on the stack, it's the last object. */
    /* if context_top is nil, then we didn't need to compact, they're
       still pointed at the right point. */
    
    if(state->om->context_top != Qnil) {
      c->active_context = state->om->context_top;
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
  
  /*
  if(REFERENCE_P(c->sender)) {
    if(state->sender_on_stack) {
      c->sender = FASTCTX(c->active_context)->sender;
    } else {
      ar(c->sender);
    }
  }
  */
  
  ar(c->self);
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
  tmp = g_ptr_array_index(roots, start++);
  len = FIXNUM_TO_INT((OBJECT)tmp);
  for(i = 0; i < len; start++, i++) {
    tmp = g_ptr_array_index(roots, start);
    //printf("Adding path %s back in...\n", _inspect(tmp));
    g_ptr_array_add(c->paths, tmp);
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

/* FIXME: the inline caches of constants aren't flushed! */

#define update_cache(val) if(c->cache_index >= 0) tuple_put(state, cmethod_get_cache(cpu_current_method(state, c)), c->cache_index, val)

OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under) {
  OBJECT hsh, val, kls, parent;
  int i;
  
  if(c->cache_index >= 0) {
#if TRACK_STATS
    state->cache_inline_const_hit++;
#endif
    val = tuple_at(state, cmethod_get_cache(cpu_current_method(state, c)), c->cache_index);
    if(val != Qnil) return val;
  }
  
  val = Qundef;
  
  /* A boundry case. */
  if(NIL_P(under)) {
    return Qnil;
  }
  
  /* We can only look for constants under modules */
  if(!ISA(under, state->global->module)) {
    cpu_raise_exception(state, c, 
      cpu_new_exception(state, c, 
        rbs_const_get(state, state->global->object, "TypeError"),
        "Can only access constants under a class/module"));
    return Qundef;
  }
  
  parent = under;
  
  hsh = module_get_constants(under);
  val = hash_find_undef(state, hsh, sym);
  
  /*
  printf("Looking for %s starting from %s (%d) (self=%s)\n", rbs_symbol_to_cstring(state, sym),
    _inspect(under), c->paths->len, _inspect(c->self));
  */
  if(val != Qundef) {
    update_cache(val);
    return val;
  }
  
  /* Case 1: self itself contains constants, so check there. */
  
  /* Ick. I'd love to be able to not have to only check in
     classes and modules. */
  kls = object_class(state, c->self);
  if(REFERENCE_P(c->self) && (
     kls == state->global->module ||
     kls == state->global->class)) {
       hsh = module_get_constants(c->self);
       val = hash_find_undef(state, hsh, sym);
       if(val != Qundef) {
         update_cache(val);
         // printf("Found under %s.\n", _inspect(c->self));
         return val;
       }
  }
  
  /* Case 2: Check in the module that defined the method. */
  hsh = module_get_constants(cpu_current_module(state, c));
  val = hash_find_undef(state, hsh, sym);
  if(val != Qundef) {
    update_cache(val);
    return val; 
  }
  
  /* Case 3: Look up the compile time lexical stack. */
  
  //printf("Path from %s\n", _inspect(cpu_current_module(state, c)));
  under = module_get_parent(cpu_current_module(state, c));
  while(RTEST(under)) {
    hsh = module_get_constants(under);
    val = hash_find_undef(state, hsh, sym);
    if(val != Qundef) {
      update_cache(val);
      return val;
    }
    under = module_get_parent(under);
  }
  
  // printf("Paths: %d\n", c->paths->len);
  
  /* Case 3: Look up the runtime lexical stack. */
  if(NIL_P(val) || val == Qundef) {
    for(i = 0; i < c->paths->len; i++) {
      under = (OBJECT)g_ptr_array_index(c->paths, i);
      hsh = module_get_constants(under);
      val = hash_find_undef(state, hsh, sym);
      
      /*
      printf("Looking for %s starting from %s in path\n", rbs_symbol_to_cstring(state, sym),
        _inspect(under));
      */
      // printf(" => %d\n", val);
      if(val != Qundef) { break; }
    }

    /* Totally unable to find the constant, call const_missing */
    if(val == Qundef) {  
      stack_push(sym);
      cpu_send_method(state, c, parent, state->global->sym_const_missing, 1);
      return Qundef;      
    } /*
    else {
      printf("Found constant %s under %p => %p (%d).\n", 
        string_byte_address(state, symtbl_find_string(state, state->global->symbols, sym)),
        under, val, val);
    } */
    
  }
  
  if(val != Qundef) {
    update_cache(val);
  }
  
  return val;
}

#undef update_cache

OBJECT cpu_const_set(STATE, cpu c, OBJECT sym, OBJECT val, OBJECT under) {
  OBJECT hsh;
  
  hsh = module_get_constants(under);
  hash_set(state, hsh, sym, val);
  return val;
}

void cpu_set_encloser_path(STATE, cpu c, OBJECT cls) {
  int len;
  len = c->paths->len;
  /*
  if(len > 0 && g_ptr_array_index(c->paths, len-1) == cls) {
    printf("Removing %p from paths.\n", cls);
    g_ptr_array_remove(c->paths, len-1); 
  } else {
    printf("Adding %p (%d) to the path...\n", cls, cls);
    g_ptr_array_add(c->paths, cls);
  }
  */
  /* add stuff for @paths here */
  g_ptr_array_add(c->paths, (gpointer)c->enclosing_class);
  /*
  printf("Push %s (%d) to paths (%d)\n", _inspect(c->enclosing_class), 
    c->enclosing_class, c->paths->len);
  */
  c->enclosing_class = cls;
}

void cpu_push_encloser(STATE, cpu c) {
  int len;
  len = c->paths->len;
  if(len > 0) {
    c->enclosing_class = (OBJECT)g_ptr_array_remove_index(c->paths, len - 1);
    //printf("Setting encloser to %s\n", _inspect(c->enclosing_class));
  }
}

void cpu_add_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method) {
  OBJECT meths, cur, vis;
  // Handle a special case where we try and add a method to main
  if(target == c->main) {
    target = c->enclosing_class;
  }
  
  cpu_clear_cache_for_method(state, c, sym);
  
  meths = module_get_methods(target);
  cur = hash_find(state, meths, sym);
  
  /* If there is already a method there, increment it's serial number
     to invalidate it in any caches. */
     
  if(RTEST(cur)) {
    if(CLASS_OBJECT(cur) == BASIC_CLASS(tuple)) {
      cur = tuple_at(state, cur, 1);
    }
    cmethod_set_serial(cur, FIXNUM_TO_INT(cmethod_get_serial(cur)) + 1);
  }
  
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
  
  hash_set(state, meths, sym, tuple_new2(state, 2, vis, method));
  // hash_set(state, meths, sym, method);
  c->call_flags = 0;
}

void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method) {
  OBJECT meta;
  meta = object_metaclass(state, target);
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
