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

#include "rubinius.h"

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
  c->locals = Qnil;
  c->literals = Qnil;
  c->block = Qnil;
  c->method = Qnil;
  c->method_module = Qnil;
  c->argcount = 0;
  c->args = 0;
  c->depth = 0;
  c->cache_index = -1;
  c->current_task = Qnil;

  /*
#if CTX_USE_FAST
  printf("[ FastMethodContext objects enabled.]\n");
  #if CTX_CACHE_ENABLED
    printf("[ Context caching enabled.]\n");
  #endif
#endif
  */
}

void cpu_setup_top_scope(STATE, cpu c) {
  c->enclosing_class = state->global->object;
  c->method_module = state->global->object;
  c->new_class_of = state->global->class;
  rbs_const_set(state, state->global->object, "MAIN", c->main);
}

void cpu_initialize_context(STATE, cpu c) {
  c->active_context = Qnil;
  c->top_context = c->active_context;
  c->home_context = c->active_context;
  c->enclosing_class = state->global->object;
  c->new_class_of = state->global->class;
  c->exception = Qnil;
  c->main = object_new(state);
  
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
  state->global->sym_tequal = symbol_from_cstr(state, "===");
  state->global->sym_lt =    symbol_from_cstr(state, "<");
  state->global->sym_gt =    symbol_from_cstr(state, ">");
  
  c->current_thread = cpu_thread_new(state, c);
  c->main_thread = c->current_thread;
  c->current_task = cpu_thread_get_task(state, c->current_thread);
  c->main_task = c->current_task;
}

void cpu_add_roots(STATE, cpu c, GPtrArray *roots) {
  int i, len;
  gpointer t;
  #define ar(obj) if(REFERENCE_P(obj)) { \
    g_ptr_array_add(roots, (gpointer)obj); \
  }
  ar(c->sender);
  ar(c->self);
  ar(c->exception);
  ar(c->enclosing_class);
  ar(c->new_class_of);
  ar(c->locals);
  ar(c->block);
  ar(c->method);
  ar(c->active_context);
  ar(c->home_context);
  ar(c->main);
  ar(c->literals);
  ar(c->exceptions);
  ar(c->top_context);
  ar(c->method_module);
  ar(c->current_thread);
  ar(c->main_thread);
  ar(c->current_task);
  ar(c->main_task);
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
  ar(c->sender);
  ar(c->self);
  ar(c->exception);
  ar(c->enclosing_class);
  ar(c->new_class_of);
  ar(c->locals);
  ar(c->block);
  ar(c->method);
  ar(c->active_context);
  ar(c->home_context);
  ar(c->main);
  ar(c->literals);
  ar(c->exceptions);
  ar(c->top_context);
  ar(c->method_module);
  ar(c->current_thread);
  ar(c->main_thread);
  ar(c->current_task);
  ar(c->main_task);
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
  OBJECT ctx, table, ent;
  int cur, total, target, idx, l, r;
  c->exception = exc;
  ctx = c->active_context;
  
  if(INTERNAL_DEBUG && getenv("EXCHALT")) {
    printf("An exception has occured: %s\n", _inspect(exc));
    assert(0);
  }
  
  cpu_flush_ip(c);
  
  while(!NIL_P(ctx)) {
    // printf("Searching for exception handler in %p / %p..\n", ctx, c->exceptions);
    if(!c->raiseable) return;
    
    table = cmethod_get_exceptions(c->method);
    
    if(!table || NIL_P(table)) {
      cpu_return_to_sender(state, c, TRUE);
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
        target = FIXNUM_TO_INT(tuple_at(state, ent, 2));
        c->ip = target;
        cpu_cache_ip(c);
        return;
      }
    }
    
    cpu_return_to_sender(state, c, FALSE);
    ctx = c->active_context;
  }
  
  /* Reset it because it can get overriden in the return_to_senders. */
  c->exception = exc;
  
  // printf("Unable to find exception handler, i'm confused.\n");
  return;
}

OBJECT cpu_new_exception(STATE, cpu c, OBJECT klass, char *msg) {
  OBJECT obj, str;
  
  obj = class_new_instance(state, klass);
  str = string_new(state, msg);
  SET_FIELD(obj, 0, str);
  methctx_reference(state, c->active_context);
  SET_FIELD(obj, 1, c->active_context);
  return obj;
}

/* FIXME: the inline caches of constants aren't flushed! */

#define update_cache(val) if(c->cache_index >= 0) tuple_put(state, cmethod_get_cache(c->method), c->cache_index, val)

OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under) {
  OBJECT hsh, val, kls;
  int i;
  
  if(c->cache_index >= 0) {
#if TRACK_STATS
    state->cache_inline_const_hit++;
#endif
    val = tuple_at(state, cmethod_get_cache(c->method), c->cache_index);
    if(val != Qnil) return val;
  }
  
  val = Qnil;
  
  hsh = module_get_constants(under);
  val = hash_find(state, hsh, sym);
  
  /*
  printf("Looking for %s starting from %s (%d) (self=%s)\n", rbs_symbol_to_cstring(state, sym),
    _inspect(under), c->paths->len, _inspect(c->self));
  */
  if(RTEST(val)) {
    update_cache(val);
    return val;
  }
  
  /* Case 1: self itself contains constants, so check there. */
  
  /* Ick. I'd love to be able to not have to only check in
     classes and modules. */
  kls = object_logical_class(state, c->self);
  if(REFERENCE_P(c->self) && (
     kls == state->global->module ||
     kls == state->global->class)) {
       hsh = module_get_constants(c->self);
       val = hash_find(state, hsh, sym);
       if(RTEST(val)) {
         update_cache(val);
         // printf("Found under %s.\n", _inspect(c->self));
         return val;
       }
  }
  
  /* Case 2: Check in the module that defined the method. */
  hsh = module_get_constants(c->method_module);
  val = hash_find(state, hsh, sym);
  if(RTEST(val)) {
    update_cache(val);
    return val; 
  }
  
  /* Case 3: Look up the compile time lexical stack. */
  
  //printf("Path from %s\n", _inspect(c->method_module));
  under = module_get_parent(c->method_module);
  while(RTEST(under)) {
    hsh = module_get_constants(under);
    val = hash_find(state, hsh, sym);
    if(RTEST(val)) {
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
      val = hash_find(state, hsh, sym);
      
      /*
      printf("Looking for %s starting from %s in path\n", rbs_symbol_to_cstring(state, sym),
        _inspect(under));
      */
      // printf(" => %d\n", val);
      if(RTEST(val) && val != Qundef) { break; }
    }
    
    if(NIL_P(val)) {  
      // FIXME: raise some type of constant not found error or something here?
      // had to remove printf side effect because its annoying
      //printf("Couldn't find constant %s.\n", 
      //  string_as_string(state, symtbl_find_string(state, state->global->symbols, sym)));
      val = Qnil;
    } /*
    else {
      printf("Found constant %s under %p => %p (%d).\n", 
        string_as_string(state, symtbl_find_string(state, state->global->symbols, sym)),
        under, val, val);
    } */
    
  }
  
  if(val != Qnil) {
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
  OBJECT meths, cur;
  // Handle a special case where we try and add a method to main
  if(target == c->main) {
    target = c->enclosing_class;
  }
  // printf("Attaching %s to %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(target));
  
  cpu_clear_cache_for_method(state, c, sym);
  
  meths = module_get_methods(target);
  cur = hash_find(state, meths, sym);
  
  /* If there is already a method there, increment it's serial number
     to invalidate it in any caches. */
     
  if(RTEST(cur)) {
    cmethod_set_serial(cur, FIXNUM_TO_INT(cmethod_get_serial(cur)) + 1);
  }
  hash_set(state, meths, sym, method);
}

void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method) {
  OBJECT meta;
  meta = object_metaclass(state, target);
  cpu_add_method(state, c, meta, sym, method);
}

char *cpu_show_context(STATE, cpu c, OBJECT ctx) {
  char *buf;
  OBJECT self;
  
  self = methctx_get_receiver(ctx);
  
  buf = malloc(1024);
  
  snprintf(buf, 1024, "%s#%s (%d)", 
    rbs_symbol_to_cstring(state, module_get_name(object_logical_class(state, self))),
    rbs_symbol_to_cstring(state, methctx_get_name(ctx)),
    (int)FIXNUM_TO_INT(methctx_get_ip(ctx))
  );
  
  return buf;
  
}
