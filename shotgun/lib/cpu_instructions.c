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
#include "machine.h"
#include "flags.h"
#include "bytearray.h"

#include <string.h>

#define EXCESSIVE_TRACING 0

#define set_int(i,s) ((i)=(((s)[0] << 24) | ((s)[1] << 16) | ((s)[2] << 8) | (s)[3]))
#define next_int set_int(_int,(c->data + c->ip)); c->ip += 4

/* #ifndef __BIG_ENDIAN__ */
/* #define next_int _int = swap32(*(int*)(c->data + c->ip)); c->ip += 4 */
/* #else */
/* #define next_int _int = *(int*)(c->data + c->ip); c->ip += 4 */
/* #endif */

#define next_literal next_int; _lit = tuple_at(state, c->literals, _int)

OBJECT cpu_open_class(STATE, cpu c, OBJECT under, OBJECT sup) {
  OBJECT sym, _lit, val, s1, s2, s3, s4, sup_itr;
  int _int;
    
  next_literal;
  sym = _lit;
    
  val = module_const_get(state, under, sym);
  if(!RTEST(val)) {
    val = class_create(state);
    
    if(NIL_P(sup)) {
      sup = state->global->object;
    }
    
    /* Push superclass instance information down. */
    class_set_instance_fields(val, class_get_instance_fields(sup));
    class_set_instance_flags(val, class_get_instance_flags(sup));
    
    // printf("Setting superclass of %p to: %p\n", val, sup);
    class_set_superclass(val, sup);
    module_setup_fields(state, val);
    module_set_parent(val, under);
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
    sup_itr = sup;
    while(1) {
      cpu_perform_hook(state, c, sup_itr, state->global->sym_inherited, val);
      sup_itr = class_get_superclass(sup_itr);
      if(NIL_P(sup_itr) || sup_itr == state->global->object) { break; }
    }
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
    val = module_allocate_mature(state, 0);
    module_setup_fields(state, val);
    module_set_name(val, sym);
    module_const_set(state, under, sym, val);
    module_setup_fields(state, object_metaclass(state, val));
    module_set_parent(val, under);
  }
  
  return val;
}

static inline OBJECT _real_class(STATE, OBJECT obj) {
  if(REFERENCE_P(obj)) {
    return HEADER(obj)->klass;
  } else {
    return object_logical_class(state, obj);
  }
}

/* Locate the method object for calling method +name+ on an instance of +klass+.
   +mod+ is updated to point to the Module that holds the method.
   
   
   * The method is then looked for in the hash tables up the superclass
     chain.
     
 */
static inline OBJECT cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT name,  OBJECT *mod) {
  OBJECT ok, meth, hsh, orig_klass;
  struct method_cache *ent;
  
  *mod = Qnil;
  if(NIL_P(klass)) { return Qnil; }
  
  ent = state->method_cache + CPU_CACHE_HASH(klass, name);
  if(ent->name == name && ent->klass == klass) {
    *mod = ent->module;
    state->cache_hits++;
    return ent->method;
  }
  
  state->cache_misses++;

  hsh = module_get_methods(klass);
  
  meth = hash_find(state, hsh, name);
  
  /*
  printf("Looking for method: %s in %p (%s)\n", 
    string_as_string(state, symtbl_find_string(state, state->global->symbols, name)), obj,
    string_as_string(state, symtbl_find_string(state, state->global->symbols, 
        class_get_name(object_logical_class(state, obj))))
    );
  */
  
  orig_klass = klass;
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
  
  *mod = klass;
  
  /* Update the cache. */
  if(RTEST(meth)) {
    ent = state->method_cache + CPU_CACHE_HASH(orig_klass, name);
    ent->klass = orig_klass;
    ent->name = name;
    ent->module = klass;
    ent->method = meth;
  }
  
  return meth;
}

OBJECT exported_cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT name, OBJECT *mod) {
    return cpu_find_method(state, c, klass, name, mod);
}

static inline OBJECT cpu_locate_method(STATE, cpu c, OBJECT obj, OBJECT sym, 
          OBJECT *mod, int *missing) {
  OBJECT mo;
  
  *missing = FALSE;
  
  mo = cpu_find_method(state, c, obj, sym, mod);
  if(NIL_P(mo)) {
    // printf("method missing: %p\n", state->global->method_missing);
    mo = cpu_find_method(state, c, obj, state->global->method_missing, mod);
    *missing = TRUE;
  }
  
  // printf("Found method: %p\n", mo);
  
  return mo;
}

static inline OBJECT cpu_create_context(STATE, cpu c, OBJECT recv, OBJECT mo, 
      OBJECT name, OBJECT mod, unsigned long int args, OBJECT block) {
  OBJECT sender, ctx, ba;
  int num_lcls;
  struct fast_context *fc;
  
  sender = c->active_context;
  
  num_lcls = FIXNUM_TO_INT(cmethod_get_locals(mo));

#if CTX_USE_FAST

  ctx = c->context_cache;
  if(!ctx) {
    ctx = object_memory_new_object(state->om, state->global->fastctx, FASTCTX_FIELDS);
  } else {
    c->context_cache = HEADER(ctx)->klass;
    HEADER(ctx)->klass = state->global->fastctx;
  }
  
  FLAG_SET(ctx, CTXFastFlag);
  FLAG_SET(ctx, StoresBytesFlag);
  
  fc = FASTCTX(ctx);
  memset(fc, 0, sizeof(struct fast_context));
  fc->sender = sender;
  fc->ip = 0;
  fc->sp = c->sp;
  fc->block = block;
  fc->raiseable = Qtrue;
  fc->method = mo;
  ba = cmethod_get_bytecodes(mo);
  fc->data = bytearray_byte_address(state, ba);
  fc->data_size = bytearray_bytes(state, ba);
  fc->literals = cmethod_get_literals(mo);
  fc->self = recv;
  fc->locals = tuple_new(state, num_lcls + 2);
  fc->argcount = args;
  fc->name = name;
  fc->method_module = mod;
  fc->num_locals = num_lcls;
  fc->is_fast = 1;
  
#else

  ctx = methctx_s_from_method(state, mo, sender, mod);
  methctx_set_receiver(ctx, recv);
  methctx_set_name(ctx, name);
  methctx_set_sp(ctx, I2N(c->sp));
  methctx_set_method(ctx, mo);
  methctx_set_block(ctx, block);
  methctx_set_argcount(ctx, I2N(args));
#endif

  return ctx;
}

void cpu_raise_arg_error(STATE, cpu c, int args, int req) {
  char msg[1024];
  snprintf(msg, 1024, "wrong number of arguments (got %d, required %d)", args, req);
  cpu_raise_exception(state, c, cpu_new_exception(state, c, state->global->exc_arg, msg));
}

void cpu_raise_primitive_failure(STATE, cpu c, int primitive_idx) {
  char msg[1024];
  OBJECT primitive_failure;
  snprintf(msg, 1024, "Primitive with index (%d) failed", primitive_idx);

  primitive_failure = cpu_new_exception(state, c, state->global->exc_primitive_failure, msg);
  cpu_raise_exception(state, c, primitive_failure);
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
      if(cpu_perform_primitive(state, c, prim, mo, args)) {
        /* Worked! */
        return TRUE;
      }
      /* Didn't work, need to remove the recv we put on before. */
      stack_pop();
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
  ctx = cpu_create_context(state, c, recv, meth, name, 
        _real_class(state, recv), (unsigned long int)count, Qnil);
  cpu_activate_context(state, c, ctx, ctx);
}

inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg) {
  OBJECT ctx, mo, mod;
  mo = cpu_find_method(state, c, _real_class(state, recv), meth, &mod);
  if(NIL_P(mo)) return;
  stack_push(arg);
  
  ctx = cpu_create_context(state, c, recv, mo, meth, 
        _real_class(state, recv), 1, Qnil);
  cpu_activate_context(state, c, ctx, ctx);
}

static inline void _cpu_build_and_activate(STATE, cpu c, OBJECT mo, 
        OBJECT recv, OBJECT sym, int args, OBJECT block, int missing, OBJECT mod) {
  OBJECT ctx;
  int prim = 0;
  if(missing) {
    DEBUG("%05d: Missed Calling %s => %s on %s (%p/%d) (%d).\n", c->depth,
     rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),
     rbs_symbol_to_cstring(state, sym), _inspect(recv), c->method, c->ip, missing);
  }

  if(missing) {
    args += 1;
    stack_push(sym);
    // printf("EEK! method_missing!\n");
    // abort();
  } else {
    if(cpu_try_primitive(state, c, mo, recv, args)) {
      #if EXCESSIVE_TRACING
      printf("%05d: Called prim %s => %s on %s.\n", c->depth,
       rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),  
       rbs_symbol_to_cstring(state, sym), _inspect(recv));
      #endif
      return;
    }
    prim = 1;
  }

  #if EXCESSIVE_TRACING
  printf("%05d: Calling %s => %s#%s on %s (%p/%d) (%s).\n", c->depth,
    rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),  
    rbs_symbol_to_cstring(state, module_get_name(mod)),
    rbs_symbol_to_cstring(state, sym), 
    _inspect(recv), c->method, c->ip,
    prim ? "" : "PRIM FAILED"
   );
  #endif
  ctx = cpu_create_context(state, c, recv, mo, sym, mod, (unsigned long int)args, block);
  /*
  if(RTEST(block)) {
    printf("in send to '%s', block %p\n", rbs_symbol_to_cstring(state, sym), block);
  }
  */
  cpu_activate_context(state, c, ctx, ctx);
  //printf("Setting method module to: %s\n", _inspect(mod));
  c->method_module = mod;
}

static inline void cpu_unified_send(STATE, cpu c, OBJECT recv, int idx, int args, OBJECT block) {
  OBJECT sym, mo, mod;
  int missing;
  assert(RTEST(c->literals));
  sym = tuple_at(state, c->literals, idx);
  
  if(0 && c->depth == 1000) {
    printf("Runaway depth...\n");
    abort();
  }
  
  missing = 0;
  
  mo = cpu_locate_method(state, c, _real_class(state, recv), sym, &mod, &missing);
  if(NIL_P(mo)) {
    printf("%05d: Calling %s on %s (%p/%lu) (%d).\n", c->depth, rbs_symbol_to_cstring(state, sym), _inspect(recv), (void *)c->method, c->ip, missing);
    printf("Fuck. no method found at all, was trying %s on %s.\n", rbs_symbol_to_cstring(state, sym), rbs_inspect(state, recv));
    assert(RTEST(mo));
  }
  _cpu_build_and_activate(state, c, mo, recv, sym, args, block, missing, mod);
}

/* This is duplicated from above rather than adding another parameter
   because unified_send is used SO often that I didn't want to slow it down
   any with checking a flag. */
static inline void cpu_unified_send_super(STATE, cpu c, OBJECT recv, int idx, int args, OBJECT block) {
  OBJECT sym, mo, klass, mod;
  int missing;
  assert(RTEST(c->literals));
  sym = tuple_at(state, c->literals, idx);
  
  missing = 0;
  
  // printf("Looking up from: %s\n", _inspect(c->method_module));
  
  klass = class_get_superclass(c->method_module);
    
  mo = cpu_locate_method(state, c, klass, sym, &mod, &missing);
  if(NIL_P(mo)) {
    printf("Fuck. no method found at all, was trying %s on %s.\n", rbs_symbol_to_cstring(state, sym), rbs_inspect(state, recv));
    assert(RTEST(mo));
  }
  _cpu_build_and_activate(state, c, mo, recv, sym, args, block, missing, mod);
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

void state_collect(STATE, cpu c);
void state_major_collect(STATE, cpu c);

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
    
    #if EXCESSIVE_TRACING
    printf("%-15s: OP: %s (%d/%d)\n", 
      rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),
      cpu_op_to_name(state, op), op, c->ip);
    #endif
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
    if (0 && !state->om->collect_now) object_memory_check_memory(state->om); /* XXX */
    if(state->om->collect_now) {
      int cm = state->om->collect_now;
      /* We're supposed to tenure all the objects now. */
      if(cm & 0x4) {
        printf("Tenuring all objects...\n");
        state->om->tenure_now = 1;
        state_collect(state, c);
        state->om->collect_now = 0;
        printf("Tenure finished.\n");
      }
      
      /* Collect the first generation. */
      if(cm & 0x1) {
        // printf("Collecting children..\n");
        state_collect(state, c);
      }
      
      /* Collect the old generation. */
      if(cm & 0x2) {
        // printf("Starting major collection.\n");
        state_major_collect(state, c);        
        // printf("Done with major collection.\n");
      }
      
      state->om->collect_now = 0;
    }
  }   
}

void cpu_run_script(STATE, cpu c, OBJECT meth) {
  OBJECT name;
  name = string_to_sym(state, string_new(state, "__script__"));
  cpu_goto_method(state, c, c->main, meth, 0, name);
}
