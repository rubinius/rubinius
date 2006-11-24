#include "shotgun.h"
#include "cpu.h"
#include "tuple.h"
#include "module.h"
#include "class.h"
#include "hash.h"
#include "methctx.h"
#include "array.h"
#include "string.h"
#include "symbol.h"

#define next_int _int = *(int*)(c->data + c->ip); c->ip += 4
#define next_literal next_int; _lit = tuple_at(state, c->literals, _int)

OBJECT cpu_open_class(STATE, cpu c, OBJECT under, OBJECT sup) {
  OBJECT sym, _lit, val, s1, s2, s3, s4;
  int _int;
    
  next_literal;
  sym = _lit;
  
  val = module_const_get(state, under, sym);
  if(!RTEST(val)) {
    val = class_allocate(state);
    
    if(NIL_P(sup)) {
      sup = state->global->object;
    }
    
    /* Push superclass instance information down. */
    class_set_instance_fields(val, class_get_instance_fields(sup));
    class_set_instance_flags(val, class_get_instance_flags(sup));
    
    // printf("Setting superclass of %p to: %p\n", val, sup);
    class_set_superclass(val, sup);
    module_setup_fields(state, val);
    /*
    printf("Defining %s under %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(c->enclosing_class));
    */
    if(c->enclosing_class != state->global->object) {
      s1 = symbol_to_string(state, module_get_name(c->enclosing_class));
      s2 = symbol_to_string(state, sym);
      s3 = string_dup(state, s1);
      string_append(state, s3, string_new(state, "::"));
      string_append(state, s3, s2);
      s4 = string_to_sym(state, s3);
      module_set_name(val, s4);
      // printf("Module %s name set to %s (%d)\n", _inspect(val), rbs_symbol_to_cstring(state, s4), FIXNUM_TO_INT(class_get_instance_fields(val)));
    } else {
      module_set_name(val, sym);
      // printf("Module %s name set to %s (%d)\n", _inspect(val), rbs_symbol_to_cstring(state, sym), FIXNUM_TO_INT(class_get_instance_fields(val)));
    }
    module_const_set(state, under, sym, val);
    module_setup_fields(state, object_metaclass(state, val));
    cpu_perform_hook(state, c, sup, state->global->sym_inherited, val);
    /* perform_hook(sup, @inherited, val) */
  }
  return val;
}

OBJECT cpu_open_module(STATE, cpu c, OBJECT under) {
  OBJECT sym, _lit, val;
  int _int;
  next_literal;
  
  sym = _lit;
  
  val = module_const_get(state, under, sym);
  if(!RTEST(val)) {
    val = module_allocate(state);
    module_setup_fields(state, val);
    module_set_name(val, sym);
    module_const_set(state, under, sym, val);
    module_setup_fields(state, object_metaclass(state, val));
  }
  
  return val;
}

static inline OBJECT cpu_find_method(STATE, cpu c, OBJECT obj, OBJECT name) {
  OBJECT ok, klass, meth, hsh;
  if(REFERENCE_P(obj)) {
    klass = HEADER(obj)->klass;
  } else {
    klass = object_logical_class(state, obj);
  }
  hsh = module_get_methods(klass);
  
  meth = hash_find(state, hsh, name);
  
  /*
  printf("Looking for method: %s in %p (%s)\n", 
    string_as_string(state, symtbl_find_string(state, state->global->symbols, name)), obj,
    string_as_string(state, symtbl_find_string(state, state->global->symbols, 
        class_get_name(object_logical_class(state, obj))))
    );
  */
  while(NIL_P(meth)) {
    ok = klass;
    klass = class_get_superclass(klass);
    if(NIL_P(klass)) { break; }
    /*
    printf("Looking for method (sup): %s in %ul (%s)\n", 
      string_as_string(state, symtbl_find_string(state, state->global->symbols, name)), klass,
      string_as_string(state, symtbl_find_string(state, state->global->symbols, 
        class_get_name(klass)))
    );
    */
    meth = hash_find(state, module_get_methods(klass), name);
  }
  
  if(REFERENCE_P(klass) && SYMBOL_P(module_get_name(klass))) {
    // printf("Found method.\n");
  }
  
  return meth;
}

static inline OBJECT cpu_locate_method(STATE, cpu c, OBJECT obj, OBJECT sym, int *missing) {
  OBJECT mo;
  
  *missing = FALSE;
  
  mo = cpu_find_method(state, c, obj, sym);
  if(NIL_P(mo)) {
    // printf("method missing: %p\n", state->global->method_missing);
    mo = cpu_find_method(state, c, obj, state->global->method_missing);
    *missing = TRUE;
    if(NIL_P(mo)) {
      printf("Fuck. no method_missing, was trying %s.\n", rbs_symbol_to_cstring(state, sym));
      assert(0);
    }
  }
  
  // printf("Found method: %p\n", mo);
  
  return mo;
}

static inline OBJECT cpu_create_context(STATE, cpu c, OBJECT recv, OBJECT mo, OBJECT name) {
  OBJECT sender, ctx;
  int len;
  
  sender = c->active_context;
  if(NIL_P(sender)) {
    sender = Qnil;
  }

  /* Check in context cache here. */
  len = state->free_contexts->len;
  if(len == 0) {
    ctx = methctx_s_from_method(state, mo, sender);
    // printf("Created context %p...\n", ctx);
  } else {
    /* Use the last one so that it's a lifo */
    ctx = (OBJECT)g_ptr_array_remove_index(state->free_contexts, len - 1);
    methctx_s_reuse(state, ctx, mo, sender);
    // printf("Reusing context %d %p '%s'...\n", len, ctx, _inspect(ctx));
  }
  
  methctx_set_receiver(ctx, recv);
  methctx_set_name(ctx, name);
  methctx_set_sp(ctx, I2N(c->sp));
  methctx_set_method(ctx, mo);
  return ctx;
}

void cpu_raise_arg_error(STATE, cpu c, int args, int req) {
  char msg[1024];
  
  snprintf(msg, 1024, "wrong number of arguments (%d for %d)", args, req);
  
  cpu_raise_exception(state, c, cpu_new_exception(state, c, state->global->exc_arg, msg));
}

static inline int cpu_try_primitive(STATE, cpu c, OBJECT mo, OBJECT recv, int args) {
  int prim, req, ret;
  
  prim = FIXNUM_TO_INT(cmethod_get_primitive(mo));
  req = FIXNUM_TO_INT(cmethod_get_required(mo));
  
  ret = FALSE;
  if(prim > -1) {
    if(req < 0 || args == req) {
      stack_push(recv);
      // printf("Running primitive: %d\n", prim);
      if(cpu_perform_primitive(state, c, prim, mo)) {
        /* Worked! */
        return TRUE;
      }
      // printf("Primitive failed! -- %d\n", prim);
    } else if(req >= 0 && object_kind_of_p(state, mo, state->global->cmethod)) {
      /* raise an exception about them not doing it right. */
      cpu_raise_arg_error(state, c, args, req);
      ret = TRUE;
    }
  }
  
  return ret;
}

inline void cpu_goto_method(STATE, cpu c, OBJECT recv, OBJECT meth,
                                     int count, OBJECT name) {
  OBJECT ctx;
  
  if(cpu_try_primitive(state, c, meth, recv, count)) { return; }
  ctx = cpu_create_context(state, c, recv, meth, name);
  methctx_set_argcount(ctx, I2N(count));
  cpu_activate_context(state, c, ctx, ctx);
}

inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg) {
  OBJECT ctx, mo;
  mo = cpu_find_method(state, c, recv, meth);
  if(NIL_P(mo)) return;
  stack_push(arg);
  
  ctx = cpu_create_context(state, c, recv, mo, meth);
  methctx_set_argcount(ctx, I2N(1));
  methctx_set_block(ctx, Qnil);
  cpu_activate_context(state, c, ctx, ctx);
}

static inline void cpu_unified_send(STATE, cpu c, OBJECT recv, int idx, int args, OBJECT block) {
  OBJECT sym, mo, ctx;
  int missing;
  assert(RTEST(c->literals));
  sym = tuple_at(state, c->literals, idx);
  
  if(0 && c->depth == 1000) {
    printf("Runaway depth...\n");
    abort();
  }
  
  missing = 0;
  
  mo = cpu_locate_method(state, c, recv, sym, &missing);
  
  if(missing) {
    DEBUG("%05d: Calling %s on %s (%p/%d) (%d).\n", c->depth, rbs_symbol_to_cstring(state, sym), _inspect(recv), c->method, c->ip, missing);
  }
  
  if(missing) {
    args += 1;
    stack_push(sym);
    // printf("EEK! method_missing!\n");
    // abort();
  } else {
    if(cpu_try_primitive(state, c, mo, recv, args)) { return; }
  }
  
  ctx = cpu_create_context(state, c, recv, mo, sym);
  methctx_set_argcount(ctx, I2N(args));
  /*
  if(RTEST(block)) {
    printf("in send to '%s', block %p\n", rbs_symbol_to_cstring(state, sym), block);
  }
  */
  methctx_set_block(ctx, block);
  cpu_activate_context(state, c, ctx, ctx);
}

char *cpu_op_to_name(STATE, char op) {
  #include "instruction_names.gen"
  return instruction_names[(int)op];
}

void cpu_run_old(STATE, cpu c) {
  // printf("Running: %p\n", c->active_context);
  while(RTEST(c->active_context)) {
    if(!cpu_dispatch(state, c)) {
      cpu_return_to_sender(state, c, FALSE);
    }
  }
}

int cpu_dispatch(STATE, cpu c) {
  unsigned char op;
  
  if(!c->data) { return FALSE; }
  if(c->ip >= c->data_size) { return FALSE; }
  
  op = (unsigned char)(c->data[c->ip]);
  c->ip += 1;
  // printf("IP: %d, SP: %d, OP: %s (%d)\n", c->ip, c->sp, cpu_op_to_name(state, op), op);
  #include "instructions.gen"
  return TRUE;
}

void cpu_run(STATE, cpu c) {
  unsigned char op;
  
  while(RTEST(c->active_context)) {
    if(!c->data || c->ip >= c->data_size) {
      cpu_return_to_sender(state, c, FALSE);
      continue;
    }
    
    op = (unsigned char)(c->data[c->ip]);
    c->ip += 1;
    
    #undef stack_push
    // #define stack_push(obj) SET_FIELD(c->stack, ++(c->sp), obj)
    #define stack_push(obj) if(!cpu_stack_push(state, c, obj, TRUE)) { goto stack_error; }
    
    // printf("IP: %d, SP: %d, OP: %s (%d)\n", c->ip, c->sp, cpu_op_to_name(state, op), op);
    #include "instructions.gen"
    goto check_interupts;
stack_error:
    /* enlarge the stack to handle the exception */
    c->stack = tuple_enlarge(state, c->stack, NUM_FIELDS(c->stack) + 128);
    // printf("HEAP INFO: %d used, %d total.\n", state->om->gc->current->current - state->om->gc->current->address, state->om->gc->current->size);
    cpu_raise_exception(state, c, 
          cpu_new_exception(state, c, state->global->exc_stack_explosion,
          "Stack has exploded"));
check_interupts:
    if(state->om->collect_now) {
      int b;
      DEBUG("Memory condition detected!\n");
      b = object_memory_used(state->om);
      state_collect(state, c);
      DEBUG("Recovered %d bytes.\n", b - object_memory_used(state->om));
      state->om->collect_now = 0;
    }
  }   
}

void cpu_run_script(STATE, cpu c, OBJECT meth) {
  OBJECT name;
  name = string_to_sym(state, string_new(state, "__script__"));
  cpu_goto_method(state, c, c->main, meth, 0, name);
}
