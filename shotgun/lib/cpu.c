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

#define InitialStackSize 4096

void cpu_initialize(STATE, cpu c) {
  state->global->tuple = Qnil;
  state->global->hash = Qnil;
  state->global->methtbl = Qnil;
  c->stack = tuple_new(state, InitialStackSize);
  // HEADER(c->stack)->gc |= 0x8000; /* Forever young. */
  c->sp = -1;
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
}

void cpu_setup_top_scope(STATE, cpu c) {
  c->enclosing_class = state->global->object;
  c->method_module = state->global->object;
  c->new_class_of = state->global->class;
  rbs_const_set(state, state->global->object, "MAIN", c->main);
}

void cpu_initialize_context(STATE, cpu c) {
  object_set_has_ivars(state, c->stack);
  c->active_context = methctx_new(state, 0);
  c->top_context = c->active_context;
  methctx_set_raiseable(c->active_context, Qfalse);
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
          
  HEADER(c->stack)->klass = state->global->tuple;
}

void cpu_add_roots(STATE, cpu c, GPtrArray *roots) {
  int i, len;
  gpointer t;
  #define ar(obj) if(REFERENCE_P(obj)) { \
    g_ptr_array_add(roots, (gpointer)obj); \
  }
  ar(c->stack);
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
  len = c->paths->len;
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
  #define ar(obj) if(REFERENCE_P(obj)) { \
    tmp = g_ptr_array_index(roots, start++); \
    obj = (OBJECT)tmp; \
  }
  ar(c->stack);
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
  //printf("Update roots: %d, %d\n", start, roots->len);
  for(; start < roots->len; start++) {
    tmp = g_ptr_array_index(roots, start);
    //printf("Adding path %s back in...\n", _inspect(tmp));
    g_ptr_array_add(c->paths, tmp);
  }
  //printf("Paths is %d\n", c->paths->len);
  #undef ar
}

void cpu_save_registers(STATE, cpu c) {
  if(!RTEST(c->active_context)) return;
  methctx_set_sp(c->active_context, I2N(c->sp));
  methctx_set_ip(c->active_context, I2N(c->ip));
}

void cpu_restore_context(STATE, cpu c, OBJECT x) {
    cpu_restore_context_with_home(state, c, x, x, FALSE, FALSE);
}

void cpu_restore_context_with_home(STATE, cpu c, OBJECT ctx, OBJECT home, int ret, int is_block) {
  c->depth--;
  c->sp = FIXNUM_TO_INT(methctx_get_sp(ctx));
  c->ip = FIXNUM_TO_INT(methctx_get_ip(ctx));
  if(ret && c->argcount > 0) {
    if (is_block) {
      --c->sp;
    } else {
      c->sp -= c->argcount;
    }
  }
  
  OBJECT ba;
  
  ba = methctx_get_bytecodes(home);
  if(!RTEST(ba)) {
    c->data = NULL;
    c->data_size = 0;
  } else {
    c->data = bytearray_byte_address(state, ba);    
    c->data_size = bytearray_bytes(state, ba);
  }
  
  c->active_context = ctx;
  c->self = methctx_get_receiver(home);
  c->home_context = home;
  c->locals = methctx_get_locals(home);
  c->block = methctx_get_block(home);
  if(RTEST(c->block)) {
    // printf("Restored block as: %s\n", _inspect(c->block));
  }
  c->method = methctx_get_method(home);
  c->literals = methctx_get_literals(home);
  c->argcount = FIXNUM_TO_INT(methctx_get_argcount(home));
  c->method_module = methctx_get_module(home);
  //printf("Restored method_module to %s\n", _inspect(c->method_module));
  
  if(RTEST(c->method)) {
    c->exceptions = cmethod_get_exceptions(c->method);
  } else {
    c->exceptions = Qnil;
  }
}

void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home) {
  c->depth += 2;
  cpu_save_registers(state, c);
  cpu_restore_context_with_home(state, c, ctx, home, FALSE, FALSE);
}

void cpu_return_to_sender(STATE, cpu c, int consider_block) {
  OBJECT sender, top, home, home_sender;
  int is_block;
  
  // printf("%d: Returning to sender..\n", c->depth);
  assert(RTEST(c->active_context));
  sender = methctx_get_sender(c->active_context);
  if(!RTEST(sender)) {
    c->active_context = Qnil;
  } else {
    if(cpu_stack_empty_p(state, c)) {
      top = Qnil;
    } else {
      top = cpu_stack_top(state, c);
    }
    
    is_block = blokctx_s_block_context_p(state, c->active_context);
    if(consider_block && is_block) {
      home = blokctx_home(state, c->active_context);
      home_sender = methctx_get_sender(home);
      sender = home_sender;
    }
    
    /* for context caching... */
    if(0 && !is_block && !methctx_s_was_referenced_p(state, c->active_context)) {
      if(state->free_contexts->len < 10) {
        // printf("Caching context %p...\n", c->active_context);
        g_ptr_array_add(state->free_contexts, (gpointer)c->active_context);
      }
    }
    
    if(blokctx_s_block_context_p(state, sender)) {
      home = blokctx_home(state, sender);
    } else {
      home = sender;
    }
    
    cpu_restore_context_with_home(state, c, sender, home, TRUE, is_block);
    cpu_stack_push(state, c, top, FALSE);
  }
}

void cpu_raise_exception(STATE, cpu c, OBJECT exc) {
  OBJECT ctx, table, ent;
  int cur, total, target, idx, l, r;
  // printf("Setting exception to %p\n", (void*)exc);
  c->exception = exc;
  ctx = c->active_context;
  
  if(INTERNAL_DEBUG && getenv("EXCHALT")) {
    printf("An exception has occured: %s\n", _inspect(exc));
    assert(0);
  }
  
  while(RTEST(ctx)) {
    // printf("Searching for exception handler in %p / %p / %p..\n", ctx, c->exceptions, c->top_context);
    if(!RTEST(methctx_get_raiseable(ctx))) { return; }
    if(ctx == c->top_context) {
      printf("Top of activation stack reached!\n");
      return;
    }
    
    table = c->exceptions;
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
  SET_FIELD(obj, 1, c->active_context);
  return obj;
}

OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under) {
  OBJECT hsh, val, kls;
  int i;
  
  val = Qnil;
  
  hsh = module_get_constants(under);
  val = hash_find(state, hsh, sym);
  
  /*
  printf("Looking for %s starting from %s (%d) (self=%s)\n", rbs_symbol_to_cstring(state, sym),
    _inspect(under), c->paths->len, _inspect(c->self));
  */
  if(RTEST(val)) {
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
         // printf("Found under %s.\n", _inspect(c->self));
         return val;
       }
  }
  
  /* Case 2: Check in the module that defined the method. */
  hsh = module_get_constants(c->method_module);
  val = hash_find(state, hsh, sym);
  if(RTEST(val)) { return val; }
  
  /* Case 3: Look up the compile time lexical stack. */
  
  //printf("Path from %s\n", _inspect(c->method_module));
  under = module_get_parent(c->method_module);
  while(RTEST(under)) {
    hsh = module_get_constants(under);
    val = hash_find(state, hsh, sym);
    if(RTEST(val)) { return val; }
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
      printf("Couldn't find constant %s.\n", 
        string_as_string(state, symtbl_find_string(state, state->global->symbols, sym)));
      val = Qnil;
    } /*
    else {
      printf("Found constant %s under %p => %p (%d).\n", 
        string_as_string(state, symtbl_find_string(state, state->global->symbols, sym)),
        under, val, val);
    } */
    
  }
  
  
  return val;
}

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
  OBJECT meths;
  // Handle a special case where we try and add a method to main
  if(target == c->main) {
    target = c->enclosing_class;
  }
  // printf("Attaching %s to %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(target));
  meths = module_get_methods(target);
  assert(RTEST(meths));
  hash_set(state, meths, sym, method);
  cpu_perform_hook(state, c, target, state->global->sym_method_added, sym);
}

void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method) {
  OBJECT meta;
  meta = object_metaclass(state, target);
  cpu_add_method(state, c, meta, sym, method);
  cpu_perform_hook(state, c, target, state->global->sym_s_method_added, sym);
}

int cpu_stack_push(STATE, cpu c, OBJECT oop, int check) {
  CHECK_PTR(oop);
  c->sp += 1;
  if(check) {
    if(NUM_FIELDS(c->stack) <= c->sp) {
      return FALSE;
    }
  }
  SET_FIELD(c->stack, c->sp, oop);
  return TRUE;
}

OBJECT cpu_stack_pop(STATE, cpu c) {
  OBJECT obj;
  obj = NTH_FIELD(c->stack, c->sp);
  c->sp -= 1;
  return obj;
}

OBJECT cpu_stack_top(STATE, cpu c) {
  return NTH_FIELD(c->stack, c->sp);
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
