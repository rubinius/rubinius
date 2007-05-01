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
#include "fixnum.h"

#include <string.h>
#include <errno.h>

#define EXCESSIVE_TRACING state->excessive_tracing
#define USE_GLOBAL_CACHING 1
#define USE_INLINE_CACHING 1

#define set_int(i,s) ((i)=(((s)[0] << 24) | ((s)[1] << 16) | ((s)[2] << 8) | (s)[3]))
#define next_int set_int(_int,(c->ip_ptr)); c->ip_ptr += 4
// #define next_int set_int(_int,(c->data + c->ip)); c->ip += 4; c->ip_ptr += 4

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
    
    if(NIL_P(sup)) {
      sup = state->global->object;
    } else if(sup == Qfalse) {
      /* Support class detached from the normal class heirarchy. */
      sup = Qnil;
    } else if(!ISA(sup, state->global->class)) {
      /* Validate sup is a valid superclass-like object. */
      
      sup_itr = sup;
      while(!NIL_P(sup_itr)) {
        if(NUM_FIELDS(sup_itr) <= CLASS_f_SUPERCLASS ||
           !ISA(class_get_methods(sup_itr), state->global->hash)) {
          /* Ok, this wont work as a superclass. */
        
          cpu_raise_exception(state, c, 
            cpu_new_exception(state, c, state->global->exc_arg, "Invalid superclass"));
          return Qnil;
        } else {
          sup_itr = class_get_superclass(sup_itr);
        }
      }
      
      /* Ok, we validated the hierarchy as being superclass-like, so it's
         ok to use. */
    }
    
    val = class_create(state);
    
    /* Push superclass instance information down. */
    if(NIL_P(sup) || NUM_FIELDS(sup) <= CLASS_f_INSTANCE_FIELDS) {
      /* When this object is detatched from the normal class hierarchy, we give
         it the normal fields and flags info by default. */
      class_set_instance_fields(val, class_get_instance_fields(state->global->object));
      class_set_instance_flags(val, class_get_instance_flags(state->global->object));
    } else {
      class_set_instance_fields(val, class_get_instance_fields(sup));
      class_set_instance_flags(val, class_get_instance_flags(sup));
    }
    
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
    while(!NIL_P(sup_itr)) {
      cpu_perform_hook(state, c, sup_itr, state->global->sym_inherited, val);
      sup_itr = class_get_superclass(sup_itr);
      if(sup_itr == state->global->object) { break; }
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
  OBJECT ok, hsh, cache, orig_klass, meth, ser;
  struct method_cache *ent;
  
  cache = Qnil;

#if USE_INLINE_CACHING
  /* Skip if we aren't running a normal method. */
  if(c->type == FASTCTX_NORMAL) {
    
  cache = cmethod_get_cache(c->method);
  
  /* There is a cache index for this send, use it! */
  if(c->cache_index > 0) {
    if(tuple_at(state, cache, c->cache_index) == klass) {
      meth = tuple_at(state, cache, c->cache_index + 2);
      ser =  tuple_at(state, cache, c->cache_index + 3);
      if(cmethod_get_serial(meth) == ser) {
#if TRACK_STATS
        state->cache_inline_hit++;
#endif
        *mod = tuple_at(state, cache, c->cache_index + 1);
        return meth;
      } else {
#if TRACK_STATS
        state->cache_inline_stale++;
#endif
      }
    }
  }

  }
#endif

#if USE_GLOBAL_CACHING
  ent = state->method_cache + CPU_CACHE_HASH(klass, name);
  if(ent->name == name && ent->klass == klass) {
    *mod = ent->module;

#if USE_INLINE_CACHING
    /* Update the inline cache. */
    if(c->type == FASTCTX_NORMAL && c->cache_index > 0) {
      tuple_put(state, cache, c->cache_index, klass);
      tuple_put(state, cache, c->cache_index + 1, ent->module);
      tuple_put(state, cache, c->cache_index + 2, ent->method);
      tuple_put(state, cache, c->cache_index + 3, cmethod_get_serial(ent->method));
    }
#endif
    
#if TRACK_STATS
    state->cache_hits++;
#endif
    return ent->method;
  }
#if TRACK_STATS
  if(ent->name) {
    state->cache_collisions++;
  }
  state->cache_misses++;
#endif
#endif

  /* Validate klass is valid even. */
  if(NUM_FIELDS(klass) <= CLASS_f_SUPERCLASS) {
    printf("Warning: encountered invalid class (not big enough).\n");
    *mod = Qnil;
    return Qnil;
  }

  hsh = module_get_methods(klass);
  
  /* Ok, rather than assert, i'm going to just bail. Makes the error
     a little strange, but handle-able in ruby land. */
  
  if(!ISA(hsh, state->global->hash)) {
    printf("Warning: encountered invalid module (methods not a hash).\n");
    *mod = Qnil;
    return Qnil; 
  }
  
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
    
    /* Validate klass is still valid. */
    if(NUM_FIELDS(klass) <= CLASS_f_SUPERCLASS) {
      printf("Warning: encountered invalid class (not big enough).\n");
      *mod = Qnil;
      return Qnil;
    }
    
    klass = class_get_superclass(klass);
    if(NIL_P(klass)) { break; }
    /*
    printf("Looking for method (sup): %s in %ul (%s)\n", 
      string_as_string(state, symtbl_find_string(state, state->global->symbols, name)), klass,
      string_as_string(state, symtbl_find_string(state, state->global->symbols, 
        class_get_name(klass)))
    );
    */
    hsh = module_get_methods(klass);
    if(!ISA(hsh, state->global->hash)) {
      printf("Warning: encountered invalid module (methods not a hash).\n");
      *mod = Qnil;
      return Qnil; 
    }
        
    meth = hash_find(state, hsh, name);
  }
  
  *mod = klass;

#if USE_GLOBAL_CACHING
  /* Update the cache. */
  if(RTEST(meth)) {
    ent->klass = orig_klass;
    ent->name = name;
    ent->module = klass;
    ent->method = meth;

#if USE_INLINE_CACHING
    /* Update the inline cache. */
    if(c->type == FASTCTX_NORMAL && c->cache_index > 0) {
      tuple_put(state, cache, c->cache_index, orig_klass);
      tuple_put(state, cache, c->cache_index + 1, ent->module);
      tuple_put(state, cache, c->cache_index + 2, ent->method);
      tuple_put(state, cache, c->cache_index + 3, cmethod_get_serial(ent->method));
    }
#endif    
  }
#endif
  
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
  
  cpu_flush_sp(c);
  
#if CTX_USE_FAST

  /*
  ctx = c->context_cache;
  if(!ctx) {
    ctx = object_memory_new_object(state->om, state->global->fastctx, FASTCTX_FIELDS);
  } else {
    c->context_cache = HEADER(ctx)->klass;
    HEADER(ctx)->klass = state->global->fastctx;
  }
  */
  
  ctx = _om_new_ultra(state->om, state->global->fastctx, (HEADER_SIZE + FASTCTX_FIELDS) * REFSIZE);
  SET_NUM_FIELDS(ctx, FASTCTX_FIELDS);
  
  FLAG_SET(ctx, CTXFastFlag);
  FLAG_SET(ctx, StoresBytesFlag);
    
  CHECK_PTR(mo);
  
  fc = FASTCTX(ctx);
  // memset(fc, 0, sizeof(struct fast_context));
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
  fc->type = FASTCTX_NORMAL;
    
#else

  ctx = methctx_s_from_method(state, mo, sender, mod);
  methctx_set_receiver(ctx, recv);
  methctx_set_name(ctx, name);
  methctx_set_sp(ctx, I2N(c->sp));
  methctx_set_method(ctx, mo);
  methctx_set_block(ctx, block);
  methctx_set_argcount(ctx, I2N(args));
#endif

  GC_MAKE_FOREVER_YOUNG(ctx);

  return ctx;
}

void cpu_raise_from_errno(STATE, cpu c, char *msg) {
  OBJECT cls, exc;
  char buf[1024];
  
  cls = hash_get(state, state->global->errno_mapping, I2N(errno));
  if(NIL_P(cls)) {
    cls = state->global->exc_arg;
    sprintf(buf, "Unknown errno %d", errno);
    msg = buf;
  }
    
  cpu_raise_exception(state, c, cpu_new_exception(state, c, cls, msg));
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

static inline int cpu_try_primitive(STATE, cpu c, OBJECT mo, OBJECT recv, int args, OBJECT sym, OBJECT mod) {
  int prim, req, ret;
  
  prim = FIXNUM_TO_INT(cmethod_get_primitive(mo));
  req = FIXNUM_TO_INT(cmethod_get_required(mo));
  
  ret = FALSE;
  if(prim > -1) {
    if(req < 0 || args == req) {
      stack_push(recv);
      // printf("Running primitive: %d\n", prim);
      if(cpu_perform_primitive(state, c, prim, mo, args, sym, mod)) {
        /* Worked! */
        return TRUE;
      }
      /* Didn't work, need to remove the recv we put on before. */
      stack_pop();
      if(EXCESSIVE_TRACING) {
        printf("[[ Primitive failed! -- %d ]]\n", prim);
      }
    } else if(req >= 0 && object_kind_of_p(state, mo, state->global->cmethod)) {
      /* raise an exception about them not doing it right. */
      cpu_raise_arg_error(state, c, args, req);
      ret = TRUE;
    }
  }
  
  return ret;
}

/* Raw most functions for moving in a method. Adjusts register. */

inline void cpu_save_registers(STATE, cpu c) {
  if(!RTEST(c->active_context)) return;
  cpu_flush_ip(c);
  cpu_flush_sp(c);
  if(methctx_is_fast_p(state, c->active_context)) {
    struct fast_context *fc;
    fc = (struct fast_context*)BYTES_OF(c->active_context);
    fc->sp = c->sp;
    fc->ip = c->ip;
  } else {
    methctx_set_sp(c->active_context, I2N(c->sp));
    methctx_set_ip(c->active_context, I2N(c->ip));
  }
}

#include <string.h>
void nmc_activate(STATE, cpu c, OBJECT nmc, int reraise);

inline void cpu_restore_context_with_home(STATE, cpu c, OBJECT ctx, OBJECT home, int ret, int is_block) {
  int ac;
  
  ac = c->argcount;
  c->depth--;
  
  /* Home is actually the main context here because it's the method
     context that holds all the data. So if it's a fast, we restore
     it's data, then if ctx != home, we restore a little more */
  
  if(methctx_is_fast_p(state, home)) {
    struct fast_context *fc;
    fc = (struct fast_context*)BYTES_OF(home);
    CHECK_PTR(fc->self);
    CHECK_PTR(fc->method);
    memcpy((void*)c, (void*)fc, sizeof(struct fast_context));
    // printf("Restoring fast context %p\n", home);
    /* Only happens if we're restoring a block. */
    if(ctx != home) {
      // assert(!ISA(ctx, state->global->fastctx));
      c->sp = FIXNUM_TO_INT(methctx_get_sp(ctx));
      c->ip = FIXNUM_TO_INT(methctx_get_ip(ctx));
            
      c->sender = methctx_get_sender(ctx);
      /* FIXME: seems like we should set c->block too.. but that
         seems to break things.. */
    }
    
    if(ret && ac > 0) {
      if (is_block) {
        --c->sp;
      } else {
        c->sp -= ac;
      }
    }
    
    /* Ok, reason we'd be restoring a native context:
       1) the native context used rb_funcall and we need to return
          it the result of the call.
    */
    if(fc->type == FASTCTX_NMC) {
      nmc_activate(state, c, home, FALSE);
      /* We return because nmc_activate will setup the cpu to do whatever
         it needs to next. */
      return;
    }
    
  } else {
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
  
    c->raiseable = RTEST(methctx_get_raiseable(home));
    c->sender = methctx_get_sender(ctx);
    c->self = methctx_get_receiver(home);
    c->locals = methctx_get_locals(home);
    c->block = methctx_get_block(home);
    c->method = methctx_get_method(home);
    c->literals = methctx_get_literals(home);
    c->argcount = FIXNUM_TO_INT(methctx_get_argcount(home));
    c->method_module = methctx_get_module(home);
  
    if(RTEST(c->method)) {
      c->exceptions = cmethod_get_exceptions(c->method);
    } else {
      c->exceptions = Qnil;
    }
  }
  
  cpu_cache_ip(c);
  cpu_cache_sp(c);
  
  c->home_context = home;
  c->active_context = ctx;
}

/* Layer 2 method movement: use lower level only. */

inline void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home) {
  c->depth += 2;
  cpu_save_registers(state, c);
  cpu_restore_context_with_home(state, c, ctx, home, FALSE, FALSE);
}

static inline void cpu_restore_context(STATE, cpu c, OBJECT x) {
    cpu_restore_context_with_home(state, c, x, x, FALSE, FALSE);
}

/* Layer 2.5: Uses lower layers to return to the calling context.
   Returning ends here. */

inline void cpu_return_to_sender(STATE, cpu c, int consider_block) {
  OBJECT sender, top, home, home_sender;
  int is_block;
  
  is_block = blokctx_s_block_context_p(state, c->active_context);
  sender = c->sender;
  
  if(sender == Qnil) {
    c->active_context = Qnil;
    
    /* Thread exitting, reschedule.. */
    if(c->current_thread != c->main_thread) {
      cpu_thread_run_best(state, c);
      
    /* Switch back to the main task... */
    } else if(c->current_task != c->main_task) {
      cpu_task_select(state, c, c->main_task);
    }
    
  } else {
    if(cpu_stack_empty_p(state, c)) {
      top = Qnil;
    } else {
      top = cpu_stack_top(state, c);
    }
    
    if(consider_block && is_block) {
      home = blokctx_home(state, c->active_context);
      if(methctx_is_fast_p(state, home)) {
        home_sender = FASTCTX(home)->sender;
      } else {
        home_sender = methctx_get_sender(home);
      }
      
      sender = home_sender;
    }
        
    if(blokctx_s_block_context_p(state, sender)) {
      home = blokctx_home(state, sender);
    } else {
      home = sender;
    }
    
    /* Cache the context if we can. */
#if CTX_USE_FAST && CTX_CACHE_ENABLED
    if(!is_block && !methctx_s_was_referenced_p(state, c->active_context)) {
      HEADER(c->active_context)->klass = 0;
      if(!c->context_cache) {
        c->context_cache = c->active_context;
      } else {
        HEADER(c->context_cache)->klass = c->active_context;
      }
    }
#endif

    if(EXCESSIVE_TRACING) {
      printf("Returning to %s.\n", _inspect(sender));
    }
    cpu_restore_context_with_home(state, c, sender, home, TRUE, is_block);
    cpu_stack_push(state, c, top, FALSE);
  }
}

/* Layer 2.6: Uses lower layers to return to the created context of the
   current block (ie, break in a block) */
static inline void cpu_return_to_block_creator(STATE, cpu c) {
  OBJECT caller, home, top;
  
  top = cpu_stack_top(state, c);
  caller = blokctx_home(state, c->active_context);
  
  if(blokctx_s_block_context_p(state, caller)) {
    home = blokctx_home(state, caller);
  } else {
    home = caller;
  }
  
  cpu_restore_context_with_home(state, c, caller, home, TRUE, TRUE);
  cpu_stack_push(state, c, top, FALSE);
}

/* Layer 3: goto. Basically jumps directly into the specificed method. 
   no lookup required. */

inline void cpu_goto_method(STATE, cpu c, OBJECT recv, OBJECT meth,
                                     int count, OBJECT name, OBJECT block) {
  OBJECT ctx;
  
  if(cpu_try_primitive(state, c, meth, recv, count, name, Qnil)) { return; }
  ctx = cpu_create_context(state, c, recv, meth, name, 
        _real_class(state, recv), (unsigned long int)count, block);
  cpu_activate_context(state, c, ctx, ctx);
}

/* Layer 3: hook. Shortcut for running hook methods. */

inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg) {
  OBJECT ctx, mo, mod;
  mo = cpu_find_method(state, c, _real_class(state, recv), meth, &mod);
  if(NIL_P(mo)) return;
  stack_push(arg);
  
  ctx = cpu_create_context(state, c, recv, mo, meth, 
        _real_class(state, recv), 1, Qnil);
  cpu_activate_context(state, c, ctx, ctx);
}

/* Layer 4: High level method calling. */

/* Callings +mo+ either as a primitive or by allocating a context and
   activating it. */

static inline void _cpu_build_and_activate(STATE, cpu c, OBJECT mo, 
        OBJECT recv, OBJECT sym, int args, OBJECT block, int missing, OBJECT mod) {
  OBJECT ctx;
  int prim = 0;
  if(missing) {
    cpu_flush_ip(c);
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
    if(cpu_try_primitive(state, c, mo, recv, args, sym, mod)) {
      if(EXCESSIVE_TRACING) {
        printf("%05d: Called prim %s => %s on %s.\n", c->depth,
          rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),  
          rbs_symbol_to_cstring(state, sym), _inspect(recv));
      }
      return;
    }
    prim = 1;
  }

  if(EXCESSIVE_TRACING) {
    cpu_flush_ip(c);
    printf("%05d: Calling %s => %s#%s on %s (%p/%d) (%s).\n", c->depth,
      rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),  
      rbs_symbol_to_cstring(state, module_get_name(mod)),
      rbs_symbol_to_cstring(state, sym), 
      _inspect(recv), c->method, c->ip,
      prim ? "" : "PRIM FAILED"
      );
  }
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

/* Layer 4: send. Primary method calling function. */

static inline void cpu_unified_send(STATE, cpu c, OBJECT recv, int idx, int args, OBJECT block) {
  OBJECT sym, mo, mod;
  int missing;

  sym = tuple_at(state, c->literals, idx);
  
  if(0 && c->depth == 1000) {
    printf("Runaway depth...\n");
    abort();
  }
  
  missing = 0;
  
  mo = cpu_locate_method(state, c, _real_class(state, recv), sym, &mod, &missing);
  if(NIL_P(mo)) {
    cpu_flush_ip(c);
    printf("%05d: Calling %s on %s (%p/%lu) (%d).\n", c->depth, rbs_symbol_to_cstring(state, sym), _inspect(recv), (void *)c->method, c->ip, missing);
    printf("Fuck. no method found at all, was trying %s on %s.\n", rbs_symbol_to_cstring(state, sym), rbs_inspect(state, recv));
    assert(RTEST(mo));
  }
  
  /* Make sure no one else sees the a recently set cache_index, it was
     only for us! */
  c->cache_index = -1;
  
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
  
  /* Make sure no one else sees the a recently set cache_index, it was
     only for us! */
  c->cache_index = -1;
  
  _cpu_build_and_activate(state, c, mo, recv, sym, args, block, missing, mod);
}

void cpu_send_method(STATE, cpu c, OBJECT recv, OBJECT sym, int args) {
  OBJECT mo, mod;
  int missing;
  
  missing = 0;
  
  /* No cache location, sorry. */
  c->cache_index = -1;
  
  mo = cpu_locate_method(state, c, _real_class(state, recv), sym, &mod, &missing);
  if(NIL_P(mo)) {
    cpu_flush_ip(c);
    printf("%05d: Calling %s on %s (%p/%lu) (%d).\n", c->depth, rbs_symbol_to_cstring(state, sym), _inspect(recv), (void *)c->method, c->ip, missing);
    printf("Fuck. no method found at all, was trying %s on %s.\n", rbs_symbol_to_cstring(state, sym), rbs_inspect(state, recv));
    assert(RTEST(mo));
  }
    
  _cpu_build_and_activate(state, c, mo, recv, sym, args, Qnil, missing, mod);
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

  op = *c->ip_ptr++;
  // printf("IP: %d, SP: %d, OP: %s (%d)\n", c->ip, c->sp, cpu_op_to_name(state, op), op);
  #include "instructions.gen"
  return TRUE;
}

void state_collect(STATE, cpu c);
void state_major_collect(STATE, cpu c);

void cpu_run(STATE, cpu c) {
  unsigned char op;
  
  g_use_firesuit = 1;
  g_access_violation = 0;
  getcontext(&g_firesuit);
  
  /* Ok, we jumped back here because something went south. */
  if(g_access_violation) {
    cpu_raise_exception(state, c, 
      cpu_new_exception(state, c, state->global->exc_arg, "Accessed outside bounds of object"));
  }
  
  while(c->active_context != Qnil) {
    
    /* This check I've commented out is a safety blanket. I've tested
       running my benchmark (compiling sirb.rb) and it's not hit. It's
       therefore just excess overhead per VM instruction, so it's not
       going to be run normally. */
       
    #if 0
    if(!c->data || c->ip >= c->data_size) {
      cpu_return_to_sender(state, c, FALSE);
      continue;
    }
    #endif
    
    op = *c->ip_ptr++;
    
    // #undef stack_push
    // #define stack_push(obj) if(!cpu_stack_push(state, c, obj, TRUE)) { goto stack_error; }
    
    if(EXCESSIVE_TRACING) {
    cpu_flush_ip(c);
    printf("%-15s: OP: %s (%d/%d)\n", 
      rbs_symbol_to_cstring(state, cmethod_get_name(c->method)),
      cpu_op_to_name(state, op), op, c->ip);
    }
    #include "instructions.gen"

    goto check_interupts;
//stack_error:
    /* enlarge the stack to handle the exception */
    c->stack_top = realloc(c->stack_top, c->stack_size += 128);
    
    // printf("HEAP INFO: %d used, %d total.\n", state->om->gc->current->current - state->om->gc->current->address, state->om->gc->current->size);
    cpu_raise_exception(state, c, 
          cpu_new_exception(state, c, state->global->exc_stack_explosion,
          "Stack has exploded"));
check_interupts:
    // if (0 && !state->om->collect_now) object_memory_check_memory(state->om);
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
        if(EXCESSIVE_TRACING) {
        printf("[[ Collecting young objects. ]]\n");
        printf("[[ method=%p, data=%p, ip_ptr=%p, ip=%d, op=%d ]]\n", c->method, c->data, c->ip_ptr, c->ip, *c->ip_ptr);
        }
        state_collect(state, c);
        if(EXCESSIVE_TRACING) {
        printf("[[ method=%p, data=%p, ip_ptr=%p, ip=%d, op=%d ]]\n", c->method, c->data, c->ip_ptr, c->ip, *c->ip_ptr);
        printf("[[ Finished collect. ]]\n");
        }
      }
      
      /* Collect the old generation. */
      if(cm & 0x2) {
        if(EXCESSIVE_TRACING) {
        printf("[[ Collecting old objects. ]\n");
        }
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
  cpu_goto_method(state, c, c->main, meth, 0, name, Qnil);
}
