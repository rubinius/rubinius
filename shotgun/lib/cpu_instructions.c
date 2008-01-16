#include <string.h>
#include <errno.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/module.h"
#include "shotgun/lib/class.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/fixnum.h"
#include "shotgun/lib/primitive_util.h"

#define RISA(obj,cls) (REFERENCE_P(obj) && ISA(obj,BASIC_CLASS(cls)))

#define next_int _int = *ip_ptr++;

#if DIRECT_THREADED
#include "shotgun/lib/instruction_funcs.gen"
DT_ADDRESSES;

#ifdef SHOW_OPS
#define NEXT_OP printf(" => %p\n", *ip_ptr); sassert(*ip_ptr); goto **ip_ptr++
#else
#define NEXT_OP goto **ip_ptr++
#endif

#endif

#define next_literal next_int; _lit = fast_fetch(cpu_current_literals(state, c), _int)

OBJECT cpu_open_class(STATE, cpu c, OBJECT under, OBJECT sup, OBJECT sym, int *created) {
  OBJECT val, s1, s2, s3, s4, sup_itr;
    
  *created = FALSE;
    
  val = module_const_get(state, under, sym);
  if(RTEST(val)) {
    if(ISA(val, BASIC_CLASS(class))) {
      if(!NIL_P(sup) && class_superclass(state, val) != sup) {
        cpu_raise_exception(state, c, 
          cpu_new_exception(state, c, state->global->exc_type, "superclass mismatch"));
        return Qundef;
      }
    } else {
      cpu_raise_exception(state, c, 
        cpu_new_exception(state, c, state->global->exc_type, "constant is not a class"));
      return Qundef;
    }
    
    return val;
  } else {
    val = class_constitute(state, sup, under);
    if(NIL_P(val)) {
      cpu_raise_exception(state, c, 
        cpu_new_exception(state, c, state->global->exc_arg, "Invalid superclass"));
      return Qundef;
    }
    
    *created = TRUE;
    
    /*
    printf("Defining %s under %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(under));
    */
    if(under != state->global->object) {
      s1 = symbol_to_string(state, module_get_name(under));
      s2 = symbol_to_string(state, sym);
      s3 = string_dup(state, s1);
      string_append(state, s3, string_new(state, "::"));
      string_append(state, s3, s2);
      s4 = string_to_sym(state, s3);
      module_set_name(val, s4);
    } else {
      module_set_name(val, sym);
    }
    module_const_set(state, under, sym, val);
    sup_itr = sup;    
  }
  return val;
}

OBJECT cpu_open_module(STATE, cpu c, OBJECT under, OBJECT sym) {
  OBJECT val, s1;
  
  val = module_const_get(state, under, sym);
  if(!RTEST(val)) {
    val = module_allocate_mature(state, 0);
    module_setup_fields(state, val);

    /*
    printf("Defining %s under %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(under));
    */
    if(under != state->global->object) {
      s1 = symbol_to_string(state, module_get_name(under));
      s1 = string_dup(state, s1);
      string_append(state, s1, string_new(state, "::"));
      string_append(state, s1, symbol_to_string(state, sym));
      module_set_name(val, string_to_sym(state, s1));
    } else {
      module_set_name(val, sym);
    }
    
    module_const_set(state, under, sym, val);
    module_setup_fields(state, object_metaclass(state, val));
    module_set_parent(val, under);    
    module_set_parent(object_metaclass(state, val), under);
  }
  
  return val;
}

static inline OBJECT _real_class(STATE, OBJECT obj) {
  if(REFERENCE_P(obj)) {
    return obj->klass;
  } else {
    return object_class(state, obj);
  }
}

/* Locate the method object for calling method +name+ on an instance of +klass+.
   +mod+ is updated to point to the Module that holds the method.
   
   
   * The method is then looked for in the hash tables up the superclass
     chain.
     
 */
 
#define TUPLE_P(obj) (CLASS_OBJECT(obj) == BASIC_CLASS(tuple))
 
static inline OBJECT cpu_check_for_method(STATE, cpu c, OBJECT hsh, OBJECT name, OBJECT recv, OBJECT mod) {
  OBJECT meth, vis;

  meth = hash_find(state, hsh, name);

  if(!RTEST(meth)) { return meth; }

  if(c->call_flags == 1) { return meth; }

  /* Check that unless we can look for private methods that method isn't private. */
  if(TUPLE_P(meth)) {
    vis = tuple_at(state, meth, 0);
    if(vis == state->global->sym_private) {
      /* We stop on private methods. */
      return Qfalse;
    } else if(vis == state->global->sym_protected) {
      /* If it's protected, bail if the receiver isn't the same
         class as self. */
      if(!object_kind_of_p(state, c->self, mod)) return Qfalse;
    }
  }
  
  return meth;
}

#define PUBLIC_P(meth) (fast_fetch(meth, 0) == state->global->sym_public)
#define VISIBLE_P(cp, mo, tu) ((c->call_flags == 1) || (!tup) || PUBLIC_P(meth))
#define UNVIS_METHOD(var) if(TUPLE_P(var)) { var = tuple_at(state, var, 1); }
#define UNVIS_METHOD2(var) if(tup) { var = tuple_at(state, var, 1); }

static inline OBJECT cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT recv, OBJECT name,  OBJECT *mod) {
  OBJECT ok, hsh, cache, orig_klass, meth;
  int tup, tries;
  struct method_cache *ent, *fent;
  
  cache = Qnil;

#if USE_GLOBAL_CACHING
  tries = CPU_CACHE_TOLERANCE;
  fent = state->method_cache + CPU_CACHE_HASH(klass, name);
  ent = fent;
  while(tries--) {
    /* We hit a hole. Stop. */
    if(!ent->name) {
      fent = ent;
      break;
    }
    if(ent->name == name && ent->klass == klass) {
      *mod = ent->module;
      meth = ent->method;
      tup = TUPLE_P(meth);
    
      if(VISIBLE_P(c, meth, tup)) {
        UNVIS_METHOD2(meth);
    
  #if TRACK_STATS
        state->cache_hits++;
  #endif
        return meth;
      }
    }
    ent++;
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
    sassert(0);
    *mod = Qnil;
    return Qnil;
  }

  hsh = module_get_methods(klass);
  
  /* Ok, rather than assert, i'm going to just bail. Makes the error
     a little strange, but handle-able in ruby land. */
  
  if(!ISA(hsh, state->global->hash)) {
    printf("Warning: encountered invalid module (methods not a hash).\n");
    sassert(0);
    *mod = Qnil;
    return Qnil; 
  }
  
  meth = cpu_check_for_method(state, c, hsh, name, recv, klass);
  
  /*
  printf("Looking for method: %s in %p (%s)\n", 
    string_byte_address(state, symtbl_find_string(state, state->global->symbols,
                                                  name)),
    klass,
    string_byte_address(state, symtbl_find_string(state, state->global->symbols,
                                                  class_get_name(klass)))
    );
  */
  
  orig_klass = klass;
  while(NIL_P(meth)) {
    ok = klass;
    
    /* Validate klass is still valid. */
    if(NUM_FIELDS(klass) <= CLASS_f_SUPERCLASS) {
      printf("Warning: encountered invalid class (not big enough).\n");
      sassert(0);
      *mod = Qnil;
      return Qnil;
    }
    
    klass = class_get_superclass(klass);
    if(NIL_P(klass)) { break; }
    /*
    printf("Looking for method (sup): %s in %p (%s)\n", 
      string_byte_address(state, symtbl_find_string(state, state->global->symbols, name)),
      klass,
      string_byte_address(state, symtbl_find_string(state, state->global->symbols, 
        class_get_name(klass)))
    );
    */
    hsh = module_get_methods(klass);
    if(!ISA(hsh, state->global->hash)) {
      printf("Warning: encountered invalid module (methods not a hash).\n");
      sassert(0);
      *mod = Qnil;
      return Qnil; 
    }
        
    meth = cpu_check_for_method(state, c, hsh, name, recv, klass);
  }
  
  *mod = klass;
  
  if(FALSE_P(meth)) return Qnil;

#if USE_GLOBAL_CACHING
  /* Update the cache. */
  if(RTEST(meth)) {
    fent->klass = orig_klass;
    fent->name = name;
    fent->module = klass;
    fent->method = meth;
    
    UNVIS_METHOD(meth);
  }
#else
  if(RTEST(meth)) {
    UNVIS_METHOD(meth);
  }
#endif
  
  return meth;
}

OBJECT exported_cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT name, OBJECT *mod) {
    return cpu_find_method(state, c, klass, Qnil, name, mod);
}

OBJECT cpu_locate_method_on(STATE, cpu c, OBJECT obj, OBJECT sym, OBJECT include_private) {
  OBJECT mod, meth;
  int call_flags;

  if(TRUE_P(include_private)) {
    // save and change call_flags to allow searching for private methods
    call_flags = c->call_flags;
    c->call_flags = 1;
    meth = cpu_find_method(state, c, _real_class(state, obj), obj, sym, &mod);
    c->call_flags = call_flags;
  } else {
    meth = cpu_find_method(state, c, _real_class(state, obj), obj, sym, &mod);       
  }
  

  if(RTEST(meth)) {
    return tuple_new2(state, 2, meth, mod);
  }
  return Qnil;
}

static inline OBJECT cpu_locate_method(STATE, cpu c, OBJECT klass, OBJECT obj, OBJECT sym, 
          OBJECT *mod, int *missing) {
  OBJECT mo;
  
  *missing = FALSE;
  
  mo = cpu_find_method(state, c, klass, obj, sym, mod);
  if(!NIL_P(mo)) { return mo; }
    
  // printf("method missing: %p\n", state->global->method_missing);
  mo = cpu_find_method(state, c, klass, obj, state->global->method_missing, mod);
  *missing = TRUE;
  
  // printf("Found method: %p\n", mo);
  
  return mo;
}

OBJECT cpu_compile_method(STATE, OBJECT cm) {
  OBJECT ba;
  ba = bytearray_dup(state, cmethod_get_bytecodes(cm));
  
  /* If this is not a big endian platform, we need to adjust
     the iseq to have the right order */
#if !CONFIG_BIG_ENDIAN
  iseq_flip(state, ba);
#endif

  /* If we're compiled with direct threading, then translate
     the compiled version into addresses. */
#if DIRECT_THREADED
  calculate_into_gotos(state, ba, _dt_addresses, _dt_size);
#endif

  cmethod_set_compiled(cm, ba);

  /* Allocate a tuple to hold the cache entries for method calls */
  OBJECT cs;
  cs = cmethod_get_cache(cm);
  if(FIXNUM_P(cs)) {
    int sz = FIXNUM_TO_INT(cs);
    if(sz > 0) {
      cs = tuple_new(state, sz);
      // Reserve field 0 for call sites that are not cached
      fast_unsafe_set(cs, 0, Qfalse);
      cmethod_set_cache(cm, cs);
    }
  }
  
  return ba;
}

static inline OBJECT _allocate_context(STATE, cpu c, OBJECT meth, int locals) {
  OBJECT ctx, ins;
  struct fast_context *fc;
  int i;
  
  ctx = object_memory_new_context(state->om, locals);
  if(ctx >= state->om->context_last) {
    state->om->collect_now |= OMCollectYoung;
  }

  ins = fast_fetch(meth, CMETHOD_f_COMPILED);

  if(NIL_P(ins)) {
    ins = cpu_compile_method(state, meth);
  }

  CLEAR_FLAGS(ctx);
  ctx->klass = Qnil;
  ctx->field_count = FASTCTX_FIELDS;

  fc = FASTCTX(ctx);
  fc->flags = 0;
  fc->sender = c->active_context;

  fc->method = meth;
  fc->data = bytearray_byte_address(state, ins);
  fc->literals = fast_fetch(meth, CMETHOD_f_LITERALS);
  
  if(locals > 0) {
    fc->locals = object_memory_context_locals(ctx);
    CLEAR_FLAGS(fc->locals);
    fc->locals->gc_zone = 0;
    fc->locals->klass = BASIC_CLASS(tuple);
    SET_NUM_FIELDS(fc->locals, locals);

    for(i = 0; i < locals; i++) {
      SET_FIELD_DIRECT(fc->locals, i, Qnil);
    }
    
  } else {
    fc->locals = Qnil;
  }
  // printf("Locals for %p at %p (%d, %d)\n", ctx, fc->locals, num_lcls, FASTCTX(ctx)->size);

  return ctx;
}

static inline OBJECT cpu_create_context(STATE, cpu c, OBJECT recv, 
      OBJECT mo, OBJECT name, OBJECT mod, 
      unsigned long int args, OBJECT block) {
  OBJECT ctx;
  struct fast_context *fc;
  
  ctx = _allocate_context(state, c, mo, FIXNUM_TO_INT(cmethod_get_locals(mo)));
  fc = FASTCTX(ctx);
  
  fc->ip = 0;
  cpu_flush_sp(c);
  fc->sp = c->sp;
  /* fp points to the location on the stack as the context
     was being created. */
  fc->fp = c->sp;
  
  fc->block = block;
  fc->self = recv;
  fc->argcount = args;
  fc->name = name;
  fc->method_module = mod;
  fc->type = FASTCTX_NORMAL;
  
  return ctx;
}

OBJECT cpu_create_block_context(STATE, cpu c, OBJECT env, int sp) {
  OBJECT ctx;
  struct fast_context *fc;
  
  ctx = _allocate_context(state, c, blokenv_get_method(env),
                      FIXNUM_TO_INT(blokenv_get_local_count(env)));
  fc = FASTCTX(ctx);
  
  fc->ip = FIXNUM_TO_INT(blokenv_get_initial_ip(env));
  fc->sp = sp;
  
  fc->block = Qnil;
  fc->self = Qnil;
  fc->argcount = 0;
  
  /* env lives here */
  fc->name = env;
  
  fc->method_module = Qnil;
  fc->type = FASTCTX_BLOCK;
  
  /* If post send is nil, that means we're not allowed to return directly to
     the home context. */
  if(NIL_P(blokenv_get_post_send(env))) {
    fc->flags |= CTX_FLAG_NO_LONG_RETURN;
  }
  
  return ctx;
}

void cpu_raise_from_errno(STATE, cpu c, const char *msg) {
  OBJECT cls;
  char buf[32];
  
  cls = hash_find(state, state->global->errno_mapping, I2N(errno));
  if(NIL_P(cls)) {
    cls = state->global->exc_arg;
    snprintf(buf, sizeof(buf), "Unknown errno %d", errno);
    msg = buf;
  }
    
  cpu_raise_exception(state, c, cpu_new_exception(state, c, cls, msg));
}

void cpu_raise_arg_error_generic(STATE, cpu c, const char *msg) {
  cpu_raise_exception(state, c, cpu_new_exception(state, c, state->global->exc_arg, msg));
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

static inline int cpu_try_primitive(STATE, cpu c, OBJECT mo, OBJECT recv, int args, OBJECT sym, OBJECT mod, OBJECT block) {
  int prim, req;
  OBJECT prim_obj;
  
  prim_obj = fast_fetch(mo, CMETHOD_f_PRIMITIVE);
  
  if(!FIXNUM_P(prim_obj)) {
    if(SYMBOL_P(prim_obj)) {
      prim = calc_primitive_index(state, symbol_to_string(state, prim_obj));
    } else {
      prim = -1;
    }
    cmethod_set_primitive(mo, I2N(prim));    
  } else {
    prim = FIXNUM_TO_INT(prim_obj); 
  }  
  
  if(prim < 0) return FALSE;
      
  req = FIXNUM_TO_INT(cmethod_get_required(mo));
  
  if(args == req || req < 0) {
    stack_push(recv);
    if(cpu_perform_primitive(state, c, prim, mo, args, sym, mod, block)) {
      /* Worked! */
      return TRUE;
    }
    /* Didn't work, need to remove the recv we put on before. */
    (void)stack_pop();
    if(EXCESSIVE_TRACING) {
      printf("[[ Primitive failed! -- %d ]]\n", prim);
    }
    
    return FALSE;
  }
    
  /* raise an exception about them not doing it right. */
  cpu_raise_arg_error(state, c, args, req);
  
  /* Return TRUE to indicate that the work for this primitive has
     been done. */
  return TRUE;  
}

/* Raw most functions for moving in a method. Adjusts register. */
/* Stack offset is used to adjust sp when it's saved so when
   this context is swapped back in,  any arguments are automatically
   removed from the stack */
inline void cpu_save_registers(STATE, cpu c, int offset) {
  struct fast_context *fc;
  
  cpu_flush_ip(c);
  cpu_flush_sp(c);
  fc = (struct fast_context*)BYTES_OF(c->active_context);
  fc->sp = c->sp - offset;
  fc->ip = c->ip;
}


inline void cpu_restore_context_with_home(STATE, cpu c, OBJECT ctx, OBJECT home, int ret, int is_block) {
  struct fast_context *fc;
    
  /* Home is actually the main context here because it's the method
     context that holds all the data. So if it's a fast, we restore
     it's data, then if ctx != home, we restore a little more */

  fc = FASTCTX(home);
  CHECK_PTR(fc->self);
  CHECK_PTR(fc->method);
  
  c->argcount = fc->argcount;
  c->self = fc->self;
    
  /* Only happens if we're restoring a block. */
  if(ctx != home) {
    fc = FASTCTX(ctx);
  }

  c->data = fc->data;
  c->type = fc->type;
  
  if(fc->type != FASTCTX_NMC) {
    c->cache = fast_fetch(fc->method, CMETHOD_f_CACHE);
  } else {
    c->cache = Qnil;
  }

  c->sender = fc->sender;
  c->sp = fc->sp;
  c->ip = fc->ip;
  c->fp = fc->fp;

  cpu_cache_ip(c);
  cpu_cache_sp(c);

  c->home_context = home;
  c->active_context = ctx;
}

/* Layer 2 method movement: use lower level only. */

inline void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home, int so) {
  if(c->active_context != Qnil) {
    cpu_save_registers(state, c, so);
  }
  cpu_restore_context_with_home(state, c, ctx, home, FALSE, FALSE);
}

/* Layer 2.5: Uses lower layers to return to the calling context.
   Returning ends here. */

void nmc_activate(STATE, cpu c, OBJECT nmc, OBJECT val, int reraise);

inline int cpu_simple_return(STATE, cpu c, OBJECT val) {
  OBJECT destination, home;

  destination = cpu_current_sender(c);
  
  // printf("Rtrnng frm %p (%d)\n", c->active_context, FASTCTX(c->active_context)->size);
  
  if(destination == Qnil) {
    object_memory_retire_context(state->om, c->active_context);
    
    c->active_context = Qnil;
    
    /* Thread exitting, reschedule.. */
    if(c->current_thread != c->main_thread) {
      cpu_thread_exited(state, c);
      return FALSE;
    /* Switch back to the main task... */
    } else if(c->current_task != c->main_task) {
      cpu_task_select(state, c, c->main_task);
      return FALSE;
    }
    /* The return value of the script is passed on the stack. */
    stack_push(val);
  } else {
    /* retire this one context. */
    object_memory_retire_context(state->om, c->active_context);      
    
    /* Now, figure out if the destination is a block, so we pass the correct
       home to restore_context */
    if(blokctx_s_block_context_p(state, destination)) {
      home = blokctx_home(state, destination);
    } else {
      home = destination;
    }
    
    /*
    if(EXCESSIVE_TRACING) {
      if(stack_context_p(destination)) {
        printf("Returning to a stack context %d / %d (%s).\n", (int)c->active_context, (int)destination, c->active_context - destination == CTX_SIZE ? "stack" : "REMOTE");
      } else {
        printf("Returning to %s.\n", _inspect(destination));
      }
    }
    */
    if(FASTCTX(home)->type == FASTCTX_NMC) {
      nmc_activate(state, c, home, val, FALSE);
      /* We return because nmc_activate will setup the cpu to do whatever
         it needs to next. */
      return TRUE;
    } else {
      cpu_restore_context_with_home(state, c, destination, home, TRUE, FALSE);
      stack_push(val);
    }
  }
  
  return TRUE;
   
}

inline int cpu_return_to_sender(STATE, cpu c, OBJECT val, int consider_block, int exception) {
  OBJECT destination, home;
  int is_block;
  
  is_block = blokctx_s_block_context_p(state, c->active_context);
  destination = cpu_current_sender(c);
    
  if(destination == Qnil) {
    object_memory_retire_context(state->om, c->active_context);
    
    c->active_context = Qnil;
    
    /* Thread exitting, reschedule.. */
    if(c->current_thread != c->main_thread) {
      cpu_thread_exited(state, c);
      return FALSE;
    /* Switch back to the main task... */
    } else if(c->current_task != c->main_task) {
      cpu_task_select(state, c, c->main_task);
      return FALSE;
    }
    /* The return value of the script is passed on the stack. */
    stack_push(val);
  } else {
    
    /* Implements a block causing the the context it was created in
       to return (IE, a non local return) */
    if(consider_block && is_block) {
      home = blokctx_home(state, c->active_context);
      destination = FASTCTX(home)->sender;
      if(EXCESSIVE_TRACING) {
        printf("CTX: remote return from %d to %d\n", (int)c->active_context, (int)destination);
      }
      
      /* If the current context is marked as not being allowed to
         return long, raise an exception instead. */
      if(FASTCTX(c->active_context)->flags & CTX_FLAG_NO_LONG_RETURN) {
        OBJECT exc;
        home = rbs_const_get(state, BASIC_CLASS(object), "IllegalLongReturn");
        
        exc = cpu_new_exception(state, c, home, "Unable to perform a long return");
        object_set_ivar(state, exc, SYM("@return_value"), val);
        
        cpu_raise_exception(state, c, exc);
          
        return TRUE;
      }
      
      /* If we're making a non-local return to a stack context... */
      if(om_on_stack(state->om, destination)) {
        /* If we're returning to a reference'd context, reset the 
           context stack to the virtual bottom */
        if(om_context_referenced_p(state->om, destination)) {
          state->om->contexts->current = state->om->context_bottom;          
        /* Otherwise set it to just beyond where we're returning to */
        } else {
          state->om->contexts->current = (void*)AFTER_CTX(destination);
        }
      
      /* It's a heap context, so reset the context stack to the virtual
         bottom. */
      } else {
        state->om->contexts->current = state->om->context_bottom;
      }
    /* It's a normal return. */
    } else {
      /* retire this one context. */
      object_memory_retire_context(state->om, c->active_context);      
    }
    
    /* Now, figure out if the destination is a block, so we pass the correct
       home to restore_context */
    if(blokctx_s_block_context_p(state, destination)) {
      home = blokctx_home(state, destination);
    } else {
      home = destination;
    }
    
    if(!is_block) {
      /* Break the chain. Lets us detect invalid non-local returns, as well
         is much nicer on the GC. exception is set if we're returning
         due to raising an exception. We keep the sender in this case so that
         the context chain can be walked to generate a backtrace. 
         
         NOTE: This might break Kernel#caller when called inside a block, which
         was created inside a method that has already returned. Thats an edge
         case I'm wiling to live with (for now). */
      if(!exception) { 
        // FASTCTX(c->active_context)->sender = Qnil;
      }
    }
    
    if(EXCESSIVE_TRACING) {
      if(stack_context_p(destination)) {
        printf("Returning to a stack context %d / %d (%s).\n", (int)c->active_context, (int)destination, (uintptr_t)c->active_context - (uintptr_t)destination == CTX_SIZE ? "stack" : "REMOTE");
      } else {
        printf("Returning to %s.\n", _inspect(destination));
      }
    }
    
    xassert(om_valid_context_p(state, destination));
    xassert(om_valid_context_p(state, home));
    
    /* Skip over NMCs for now. */
    if(exception && FASTCTX(destination)->type == FASTCTX_NMC) {
      c->active_context = destination;
      return cpu_return_to_sender(state, c, val, FALSE, TRUE);      
    }
    
    /* Ok, reason we'd be restoring a native context:
       1) the native context used rb_funcall and we need to return
          it the result of the call.
    */
    if(FASTCTX(home)->type == FASTCTX_NMC) {
      nmc_activate(state, c, home, val, FALSE);
      /* We return because nmc_activate will setup the cpu to do whatever
         it needs to next. */
      return TRUE;
    } else {
      cpu_restore_context_with_home(state, c, destination, home, TRUE, is_block);
      if(!exception) stack_push(val);
    }
  }
  
  return TRUE;
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
  
  if(cpu_try_primitive(state, c, meth, recv, count, name, Qnil, block)) { return; }
  ctx = cpu_create_context(state, c, recv, meth, name, 
        _real_class(state, recv), (unsigned long int)count, block);
  cpu_activate_context(state, c, ctx, ctx, 0);
}

/* Layer 3: hook. Shortcut for running hook methods. */

inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg) {
  OBJECT mo, mod, rub, vm;
  int call_flags;

  /* Must be able to call private hooks too */
  call_flags = c->call_flags;
  c->call_flags = 1;

  mo = cpu_find_method(state, c, _real_class(state, recv), recv, meth, &mod);

  c->call_flags = call_flags;

  if(NIL_P(mo)) return;


  rub = rbs_const_get(state, BASIC_CLASS(object), "Rubinius");
  if(NIL_P(rub)) return;

  vm = rbs_const_get(state, rub, "VM");
  if(NIL_P(vm)) return;

  /* The top of the stack contains the value that should remain on the stack.
     we pass that to the perform_hook call so it is returned and stays on
     the top of the stack. Thats why we say there are 4 args.*/

  stack_push(arg);
  stack_push(meth);
  stack_push(recv);

  cpu_unified_send(state, c, vm, SYM("perform_hook"), 4, Qnil);
}

/* Layer 4: High level method calling. */

/* Callings +mo+ either as a primitive or by allocating a context and
   activating it. */

static inline void _cpu_build_and_activate(STATE, cpu c, OBJECT mo, 
        OBJECT recv, OBJECT sym, int args, OBJECT block, int missing, OBJECT mod) {
  OBJECT ctx;

  if(missing) {
    args += 1;
    stack_push(sym);
    // printf("EEK! method_missing!\n");
    // abort();
  } else {
    if(cpu_try_primitive(state, c, mo, recv, args, sym, mod, block)) {
      if(EXCESSIVE_TRACING) {
        printf("%05d: Called prim %s => %s on %s.\n", c->depth,
          rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c))),  
          rbs_symbol_to_cstring(state, sym), _inspect(recv));
      }
      c->call_flags = 0;
      return;
    }
  }

  if(EXCESSIVE_TRACING) {
    cpu_flush_ip(c);
    printf("%05d: Calling %s => %s#%s on %s (%p/%d) (%s).\n", c->depth,
      rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c))),  
      rbs_symbol_to_cstring(state, module_get_name(mod)),
      rbs_symbol_to_cstring(state, sym), 
      _inspect(recv), (void*)cpu_current_method(state, c), c->ip,
      missing ? "METHOD MISSING" : ""
      );
  }
  ctx = cpu_create_context(state, c, recv, mo, sym, mod, (unsigned long int)args, block);
  /* If it was missing, setup some extra data in the MethodContext for
     the method_missing method to check out, to see why it was missing. */
  if(missing) {
    if(c->call_flags == 1) {
      methctx_reference(state, ctx);
      object_set_ivar(state, ctx, SYM("@send_private"), Qtrue);
    }
  }
  /*
  if(RTEST(block)) {
    printf("in send to '%s', block %p\n", rbs_symbol_to_cstring(state, sym), block);
  }
  */
  if(EXCESSIVE_TRACING) {
    printf("CTX:                 running %d\n", (int)ctx);
  }
  c->call_flags = 0;
  cpu_activate_context(state, c, ctx, ctx, args);
}


/* Layer 4: direct activation. Used for calling a method thats already
   been looked up. */
static inline void cpu_activate_method(STATE, cpu c, OBJECT recv, OBJECT mo,
                                       OBJECT mod, int args, OBJECT name,
                                       OBJECT block) {
  
  _cpu_build_and_activate(state, c, mo, recv, name, args, block, 0, mod);
}

/* Layer 4: send. Primary method calling function. */
static inline void _inline_cpu_unified_send(STATE, cpu c, OBJECT recv, OBJECT sym, int args, OBJECT block) {
  OBJECT mo, mod, cls, cache, ser, ic;
  int missing, count, ci;

  ci = c->cache_index;

  missing = 0;
  
  cls = _real_class(state, recv);
  
  ic = Qnil;
  cache = Qnil;
  count = 0;
  
#if USE_INLINE_CACHING
  /* There is a cache index for this send, use it! */
  if(ci != -1) {
    cache = c->cache;
    
    ic = fast_fetch(cache, ci);

    if(ic == Qfalse || ic == Qnil) {

      /* If it's false, the cache is disabled. */
      if(ic == Qfalse) {
        mo = cpu_locate_method(state, c, cls, recv, sym, &mod, &missing);
        if(NIL_P(mo)) goto really_no_method;
        goto dispatch;
      }

      /* Cache hasn't been populated ever. */
      goto lookup;
    }
        
    // fast_inc(ic, ICACHE_f_HOTNESS);

    if(fast_fetch(ic, ICACHE_f_CLASS) == cls) {
      mo =  fast_fetch(ic, ICACHE_f_METHOD);
      mod = fast_fetch(ic, ICACHE_f_MODULE);

      goto dispatch;

      ser = fast_fetch(ic, ICACHE_f_SERIAL);
      /* We don't check the visibility here because the inline cache has
         fixed visibility, meaning it will always be the same after it's
         populated. */
      if(fast_fetch(mo, CMETHOD_f_SERIAL) == ser) {
#if TRACK_STATS
        state->cache_inline_hit++;
#endif
        goto dispatch;
      } else {
#if TRACK_STATS
        state->cache_inline_stale++;
#endif
      }
    } else {      
      // count = FIXNUM_TO_INT(fast_fetch(ic, ICACHE_f_TRIP));
      // fast_set_int(ic, ICACHE_f_TRIP, count + 1);
    }
  }
#endif

  lookup:
  mo = cpu_locate_method(state, c, cls, recv, sym, &mod, &missing);
  if(NIL_P(mo)) {
    char *msg;
    really_no_method:
    
    ser = rbs_const_get(state, BASIC_CLASS(object), "RuntimeError");
    
    msg = malloc(1024);
    sprintf(msg, "Unable to find any version of '%s' to run", _inspect(sym));
    
    cpu_raise_exception(state, c, cpu_new_exception(state, c, ser, msg));

    free(msg);
    
    return;    
  }
  
#if USE_INLINE_CACHING
  /* Update the inline cache. */
  if(cache != Qnil && !missing) {
    if(ic == Qnil) {
      ic = icache_allocate(state);
      icache_set_class(ic, cls);
      icache_set_module(ic, mod);
      icache_set_method(ic, mo);
      icache_set_serial(ic, cmethod_get_serial(mo));
      icache_set_hotness(ic, I2N(0));
      icache_set_trip(ic, I2N(0));
      tuple_put(state, cache, ci, ic);
    } else {
      if(count > 100) {
        fast_unsafe_set(cache, ci, Qfalse);
      } else {
        icache_set_class(ic, cls);
        icache_set_module(ic, mod);
        icache_set_method(ic, mo);
        icache_set_serial(ic, cmethod_get_serial(mo));
      }
    }
  }
#endif
  
  dispatch:
  /* Make sure no one else sees the a recently set cache_index, it was
     only for us! */
  c->cache_index = -1;
  
  _cpu_build_and_activate(state, c, mo, recv, sym, args, block, missing, mod);
  return;  
}

void cpu_unified_send(STATE, cpu c, OBJECT recv, OBJECT sym, int args, OBJECT block) {
  _inline_cpu_unified_send(state, c, recv, sym, args, block);
}

/* This is duplicated from above rather than adding another parameter
   because unified_send is used SO often that I didn't want to slow it down
   any with checking a flag. */
static inline void cpu_unified_send_super(STATE, cpu c, OBJECT recv, OBJECT sym, int args, OBJECT block) {
  OBJECT mo, klass, mod;
  int missing;
  
  c->call_flags = 1;
    
  missing = 0;
  
  // printf("Looking up from: %s\n", _inspect(cpu_current_module(state, c)));
  
  klass = class_get_superclass(cpu_current_module(state, c));
    
  mo = cpu_locate_method(state, c, klass, recv, sym, &mod, &missing);
  if(NIL_P(mo)) {
    printf("Fuck. no method found at all, was trying %s on %s.\n", rbs_symbol_to_cstring(state, sym), rbs_inspect(state, recv));
    sassert(RTEST(mo));
  }
  
  /* Make sure no one else sees the a recently set cache_index, it was
     only for us! */
  c->cache_index = -1;
  c->call_flags = 0;
  
  _cpu_build_and_activate(state, c, mo, recv, sym, args, block, missing, mod);
}

const char *cpu_op_to_name(STATE, char op) {
#include "shotgun/lib/instruction_names.h"
  return get_instruction_name(op);
}

void state_collect(STATE, cpu c);
void state_major_collect(STATE, cpu c);

void cpu_run(STATE, cpu c, int setup) {
  IP_TYPE op;
  IP_TYPE *ip_ptr = NULL;

  c->ip_ptr = &ip_ptr;

  struct rubinius_globals *global = state->global;

  if(setup) {
    (void)op;
#if DIRECT_THREADED
    SETUP_DT_ADDRESSES;
    return;
#else
    return;
#endif
  }
  
  /* recache ip_ptr to make it valid. */
  cpu_cache_ip(c);
  
  current_machine->g_use_firesuit = 1;
  current_machine->g_access_violation = 0;
  getcontext(&current_machine->g_firesuit);
  
  /* Ok, we jumped back here because something went south. */
  if(current_machine->g_access_violation) {
    switch(current_machine->g_access_violation) {
    case FIRE_ACCESS:
      cpu_raise_exception(state, c, 
        cpu_new_exception(state, c, state->global->exc_arg, 
            "Accessed outside bounds of object"));
      break;
    case FIRE_NULL:
      cpu_raise_exception(state, c, 
        cpu_new_exception(state, c, state->global->exc_arg, 
            "Attempted to access field of non-reference (null pointer)")); 
      break;
    case FIRE_ASSERT:
      cpu_raise_exception(state, c, 
        cpu_new_exception(state, c, 
            rbs_const_get(state, BASIC_CLASS(object), "VMAssertion"), 
            "An error has occured within the VM"));
      break;
    case FIRE_TYPE:
      cpu_raise_exception(state, c, 
        cpu_new_exception2(state, c, global->exc_type,
            "Invalid type encountered: %d", current_machine->g_firesuit_arg));
      break;
    default:
      cpu_raise_exception(state, c, 
        cpu_new_exception2(state, c, global->exc_type,
            "Unknown firesuit reason: %d", current_machine->g_access_violation));
      break;
    }
  }

insn_start:
  while(c->active_context != Qnil) {
    
#if DIRECT_THREADED
    if(EXCESSIVE_TRACING) {
      printf("%-15s: => %p\n",
        rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c))),
        (void*)*ip_ptr);
    }
    NEXT_OP;
    #include "shotgun/lib/instruction_dt.gen"
#else

next_op:
    op = *ip_ptr++;

    if(EXCESSIVE_TRACING) {
    cpu_flush_ip(c);
    cpu_flush_sp(c);
    printf("%-15s: OP: %s (%d/%d/%d)\n", 
      rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c))),
      cpu_op_to_name(state, op), op, c->ip, c->sp);
    }

    #include "shotgun/lib/instructions.gen"
    
#endif
check_interrupts:
    if(state->om->collect_now) {
      int cm = state->om->collect_now;
      
      /* Collect the first generation. */
      if(cm & OMCollectYoung) {
        if(EXCESSIVE_TRACING) {
          printf("[[ Collecting young objects. ]]\n");
          printf("[[ ctx=%p, data=%p, ip_ptr=%p, ip=%d, op=%d ]]\n", (void*)c->active_context, cpu_current_data(c), ip_ptr, c->ip, *ip_ptr);
        }
        state_collect(state, c);
        if(EXCESSIVE_TRACING) {
          printf("[[ ctx=%p, data=%p, ip_ptr=%p, ip=%d, op=%d ]]\n", (void*)c->active_context, cpu_current_data(c), ip_ptr, c->ip, *ip_ptr);
          printf("[[ Finished collect. ]]\n");
        }
      }
      
      /* Collect the old generation. */
      if(cm & OMCollectMature) {
        if(EXCESSIVE_TRACING) {
          printf("[[ Collecting old objects. ]\n");
        }
        state_major_collect(state, c);
        // printf("Done with major collection.\n");
      }
  
      /* If someone is reading the ON_GC channel, write to it to notify them. */
      if(cpu_channel_was_readers_p(state, state->global->on_gc_channel)) {
        cpu_channel_send(state, c, state->global->on_gc_channel, Qtrue);
      }
      
      state->om->collect_now = 0;
    }
    
    if(state->check_events) {
      state->check_events = 0;
      if(state->pending_events) cpu_event_runonce(state);
      if(state->pending_threads) cpu_thread_switch_best(state, c);
    }
  }
}

void cpu_run_script(STATE, cpu c, OBJECT meth) {
  OBJECT name;
  name = string_to_sym(state, string_new(state, "__script__"));
  cpu_goto_method(state, c, c->main, meth, 0, name, Qnil);
}
