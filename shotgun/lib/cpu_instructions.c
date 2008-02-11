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
#include "shotgun/lib/sendsite.h"
#include "shotgun/lib/subtend/ffi.h"
#include "shotgun/lib/subtend/nmc.h"

#include <sys/time.h>

#if TIME_LOOKUP

#include <stdint.h>
#include <time.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

/*
static inline uint64_t measure_cpu_time() {
  uint64_t x;
  __asm__ __volatile__ ("rdtsc" : "=A" (x));
  return x;
}

uint64_t get_cpu_frequency()
{
    uint64_t x, y;
    time_t start;
    static uint64_t result = 0;
    if(result) return result;

    start = time(NULL);
    x = measure_cpu_time();
    while(time(NULL) == start);
    y = measure_cpu_time();
    result = (y - x);
    return result;
}
*/

#define measure_cpu_time mach_absolute_time

void cpu_show_lookup_time(STATE) {
  struct mach_timebase_info timeinfo;
  uint64_t nano;
  double seconds, total;

  mach_timebase_info(&timeinfo);

  nano = (state->lookup_time * timeinfo.numer / timeinfo.denom); 

  seconds = (double)(nano / (double)1000000000);

  nano = ((mach_absolute_time() - state->system_start) * timeinfo.numer / timeinfo.denom);

  total = (double)(nano / (double)1000000000);

  printf("Total  time: % 3.3f\n", total);
  printf("Lookup time: % 3.3f\n", seconds);
  printf("Percent:     % 3.3f\n", (seconds / total) * 100);
}

#endif

#if CONFIG_ENABLE_DTRACE
#include "shotgun/dtrace.h"
#define ENABLE_DTRACE 1
#else
#define ENABLE_DTRACE 0
#endif

#define RISA(obj,cls) (REFERENCE_P(obj) && ISA(obj,BASIC_CLASS(cls)))

#define next_int_into(val) val = *ip_ptr++;
#define next_int next_int_into(_int);

#if DIRECT_THREADED
#include "shotgun/lib/instruction_funcs.gen"
DT_ADDRESSES;

#ifdef SHOW_OPS
#define NEXT_OP printf(" => %p\n", *ip_ptr); sassert(*ip_ptr); goto **ip_ptr++
#else
#define NEXT_OP goto **ip_ptr++
#endif

#endif

#define next_literal_into(val) next_int; val = fast_fetch(cpu_current_literals(state, c), _int)
#define next_literal next_literal_into(_lit)

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

/* Return the module object corresponding to the name or, if
 * the module does not exist yet, create a new one first.
 */
OBJECT cpu_open_module(STATE, cpu c, OBJECT under, OBJECT sym) {
  OBJECT module, s1;

  module = module_const_get(state, under, sym);
  if(!RTEST(module)) {
    module = module_allocate_mature(state, 0);
    module_setup_fields(state, module);

    /*
    printf("Defining %s under %s.\n", rbs_symbol_to_cstring(state, sym), _inspect(under));
    */
    if(under != BASIC_CLASS(object)) {
      s1 = symbol_to_string(state, module_get_name(under));
      s1 = string_dup(state, s1);
      string_append(state, s1, string_new(state, "::"));
      string_append(state, s1, symbol_to_string(state, sym));
      module_set_name(module, string_to_sym(state, s1));
    } else {
      module_set_name(module, sym);
    }

    module_const_set(state, under, sym, module);
    module_setup_fields(state, object_metaclass(state, module));
    module_set_encloser(module, under);
    module_set_encloser(object_metaclass(state, module), under);
  }

  return module;
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
 
/* returns TRUE if we found a method object that should be considered
 * returns FALSE if we need to keep looking 'up' for the method */
static inline int cpu_check_for_method(STATE, cpu c, OBJECT hsh, struct message *msg) {
  OBJECT vis, obj;

  msg->method = hash_find(state, hsh, msg->name);

  if(NIL_P(msg->method)) return FALSE;

  /* false means to terminate method lookup. */
  if(FALSE_P(msg->method)) return TRUE;

  if(msg->priv) return TRUE;

  /* Check that unless we can look for private methods that method isn't private. */
  if(TUPLE_P(msg->method)) {
    vis = tuple_at(state, msg->method, 0);
    if(vis == state->global->sym_private) {
      /* We stop on private methods. */
      msg->method = Qfalse;
      return TRUE;
    } else if(vis == state->global->sym_protected) {
      /* If it's protected, bail if the receiver isn't the same
         class as self. */
      if(!object_kind_of_p(state, c->self, msg->module)) {
        msg->method = Qfalse;
        return TRUE;
      }
    }

    obj = tuple_at(state, msg->method, 1);
    if(NIL_P(obj)) {
      /* The method was callable, but we need to keep looking 
       * for the implementation, so make the invocation bypass all further
       * visibility checks */
      msg->priv = TRUE;
      return FALSE;
    }

  }

  return TRUE;
}

#define UNVIS_METHOD(var) if(TUPLE_P(var)) { var = tuple_at(state, var, 1); }

static inline int cpu_find_method(STATE, cpu c, struct message *msg) {
  OBJECT hsh, klass;
  struct method_cache *ent;
  
#if USE_GLOBAL_CACHING
  ent = state->method_cache + CPU_CACHE_HASH(msg->klass, msg->name);
  /* We hit a hole. Stop. */
  if(ent->name == msg->name && ent->klass == msg->klass) {

    /* TODO does this need to check for protected? */
    if(msg->priv || ent->is_public) {
      msg->method = ent->method;
      msg->module = ent->module;

#if TRACK_STATS
      state->cache_hits++;
#endif
      return TRUE;
    }
  }

#if TRACK_STATS
  if(ent->name) {
    state->cache_collisions++;
  }
  state->cache_misses++;
#endif
#endif

  klass = msg->klass;

  do {

    /* Validate klass is valid even. */
    if(NUM_FIELDS(klass) <= CLASS_f_SUPERCLASS) {
      printf("Warning: encountered invalid class (not big enough).\n");
      sassert(0);
      return FALSE;
    }

    hsh = module_get_method_table(klass);

    /* Ok, rather than assert, i'm going to just bail. Makes the error
       a little strange, but handle-able in ruby land. */

    if(!ISA(hsh, state->global->hash)) {
      printf("Warning: encountered invalid module (methods not a hash).\n");
      sassert(0);
      return FALSE; 
    }

    msg->module = klass;
    if(cpu_check_for_method(state, c, hsh, msg)) {
      goto cache;
    }
    
    klass = class_get_superclass(klass);
    if(NIL_P(klass)) break;

  } while(1);

cache:
  
  if(!RTEST(msg->method)) return FALSE;

#if USE_GLOBAL_CACHING
  /* Update the cache. */
  if(RTEST(msg->method)) {
    ent->klass = msg->klass;
    ent->name = msg->name;
    ent->module = klass;

    if(TUPLE_P(msg->method)) {
      ent->method = NTH_FIELD(msg->method, 1);
      if(NTH_FIELD(msg->method, 0) == state->global->sym_public) {
        ent->is_public = TRUE;
      } else {
        ent->is_public = FALSE;
      }

      msg->method = ent->method;
    } else {
      ent->method = msg->method;
      ent->is_public = TRUE;
    }
  }
#else
  if(RTEST(msg->method)) {
    UNVIS_METHOD(msg->method);
  }
#endif
  
  return TRUE;
}

OBJECT exported_cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT name, OBJECT *mod) {
  struct message msg;
  
  msg.name = name;
  msg.klass = klass;
  msg.recv = Qnil;
  msg.priv = TRUE;
  msg.module = Qnil;
  msg.method = Qnil;
  
  if(!cpu_find_method(state, c, &msg)) {
    *mod = Qnil;
    return Qnil;
  }

  *mod = msg.module;
  return msg.method;
}

OBJECT cpu_locate_method_on(STATE, cpu c, OBJECT obj, OBJECT sym, OBJECT include_private) {
  struct message msg;

  msg.recv = obj;
  msg.name = sym;
  msg.klass = _real_class(state, obj);
  msg.priv = TRUE_P(include_private);
  msg.method = Qnil;
  msg.module = Qnil;

  if(cpu_find_method(state, c, &msg)) {
    if(RTEST(msg.method)) {
      return tuple_new2(state, 2, msg.method, msg.module);
    }
  }

  return Qnil;
}

static inline int cpu_locate_method(STATE, cpu c, struct message *msg) {
  int ret;
  struct message missing;

#if ENABLE_DTRACE
  if(RUBINIUS_VM_LOOKUP_BEGIN_ENABLED()) {
    RUBINIUS_VM_LOOKUP_BEGIN();
  }
#endif

  ret = TRUE;
  
  if(cpu_find_method(state, c, msg)) goto done;
 
  missing = *msg;
  missing.priv = TRUE;
  missing.name = state->global->method_missing;

  /* If we couldn't even find method_missing. bad. */
  if(!cpu_find_method(state, c, &missing)) { ret = FALSE; goto done; }

  msg->method = missing.method;
  msg->module = missing.module;
  msg->missing = TRUE;

done:
#if ENABLE_DTRACE
  if(RUBINIUS_VM_LOOKUP_END_ENABLED()) {
    RUBINIUS_VM_LOOKUP_END();
  }
#endif
  // printf("Found method: %p\n", mo);
  
  return ret;
}

static inline OBJECT cpu_check_serial(STATE, cpu c, OBJECT obj, OBJECT sym, int serial) {
  struct message msg;

  msg.name = sym;
  msg.recv = obj;
  msg.klass = _real_class(state, obj);
  msg.priv = TRUE;

  if(!cpu_find_method(state, c, &msg)) {
    return Qfalse;
  }

  if(N2I(fast_fetch(msg.method, CMETHOD_f_SERIAL)) == serial) {
    return Qtrue;
  }

  return Qfalse;
}

OBJECT cpu_compile_method(STATE, OBJECT cm) {
  OBJECT ba, bc;
  int target_size;

  ba = cmethod_get_compiled(cm);
  bc = cmethod_get_bytecodes(cm);

  /* If we're direct threaded, the compiled version is an array of the pointer
   * size. */
#if DIRECT_THREADED
  target_size = BYTEARRAY_SIZE(bc) * sizeof(uintptr_t);
#else
  target_size = BYTEARRAY_SIZE(bc);
#endif

  if(NIL_P(ba) || BYTEARRAY_SIZE(ba) < target_size) {
    /* First time this method has been compiled, or size of current
       bytearray is insufficient to hold revised bytecode */
    ba = bytearray_new(state, target_size);
  }

  cpu_compile_instructions(state, bc, ba);
  cmethod_set_compiled(cm, ba);

  /* Allocate a tuple to hold the cache entries for method calls */
  OBJECT cs;
  cs = cmethod_get_cache(cm);
  if(FIXNUM_P(cs)) {
    native_int sz = N2I(cs);
    if(sz > 0) {
      cs = tuple_new(state, sz);
      // Reserve field 0 for call sites that are not cached
      fast_unsafe_set(cs, 0, Qfalse);
      cmethod_set_cache(cm, cs);
    }
  }
  
  return ba;
}

void cpu_compile_instructions(STATE, OBJECT bc, OBJECT comp) {
  /* If this is not a big endian platform, we need to adjust
     the iseq to have the right order */
#if !CONFIG_BIG_ENDIAN && !DIRECT_THREADED
  iseq_flip(state, bc, comp);
#elif DIRECT_THREADED
  /* If we're compiled with direct threading, then translate
     the compiled version into addresses. */
  calculate_into_gotos(state, bc, comp, _dt_addresses, _dt_size);
#endif
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

static inline OBJECT cpu_create_context(STATE, cpu c, const struct message *msg) {
  OBJECT ctx;
  struct fast_context *fc;
  
  ctx = _allocate_context(state, c, msg->method, N2I(cmethod_get_locals(msg->method)));
  fc = FASTCTX(ctx);
  
  fc->ip = 0;
  cpu_flush_sp(c);
  fc->sp = c->sp;
  /* fp points to the location on the stack as the context
     was being created. */
  fc->fp = c->sp;
  
  fc->block = msg->block;
  fc->self = msg->recv;
  fc->argcount = msg->args;
  fc->name = msg->name;
  fc->method_module = msg->module;
  fc->type = FASTCTX_NORMAL;
  
  return ctx;
}

OBJECT cpu_create_block_context(STATE, cpu c, OBJECT env, int sp) {
  OBJECT ctx;
  struct fast_context *fc;
  
  ctx = _allocate_context(state, c, blokenv_get_method(env),
                      N2I(blokenv_get_local_count(env)));
  fc = FASTCTX(ctx);
  
  fc->ip = N2I(blokenv_get_initial_ip(env));
  fc->sp = sp;
  
  fc->block = Qnil;
  fc->self = Qnil;
  fc->argcount = 0;
  
  /* env lives here */
  fc->name = env;
  
  fc->method_module = Qnil;
  fc->type = FASTCTX_BLOCK;
  
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
  cpu_flush_ip(c);
  cpu_raise_exception(state, c, cpu_new_exception(state, c, state->global->exc_arg, msg));
}

void cpu_raise_primitive_failure(STATE, cpu c, int primitive_idx) {
  char msg[1024];
  OBJECT primitive_failure;
  snprintf(msg, 1024, "Primitive with index (%d) failed", primitive_idx);

  primitive_failure = cpu_new_exception(state, c, state->global->exc_primitive_failure, msg);
  cpu_raise_exception(state, c, primitive_failure);
}

static inline int cpu_try_primitive(STATE, cpu c, const struct message *msg) {
  int prim, req;
  OBJECT prim_obj;
  
  prim_obj = fast_fetch(msg->method, CMETHOD_f_PRIMITIVE);
  
  if(!FIXNUM_P(prim_obj)) {
    if(SYMBOL_P(prim_obj)) {
      prim = calc_primitive_index(state, symbol_to_string(state, prim_obj));
    } else {
      prim = -1;
    }
    cmethod_set_primitive(msg->method, I2N(prim));
  } else {
    prim = N2I(prim_obj); 
  }  
  
  if(prim < 0) return FALSE;
      
  req = N2I(cmethod_get_required(msg->method));
  
  if(msg->args == req || req < 0) {
#if ENABLE_DTRACE
  if (RUBINIUS_FUNCTION_ENTRY_ENABLED()) {
    char *module_name = msg->module == Qnil ? "<unknown>" : (char*)rbs_symbol_to_cstring(state, module_get_name(msg->module));
    char *method_name = (char*)rbs_symbol_to_cstring(state, msg->name);

    cpu_flush_ip(c);

    struct fast_context *fc = FASTCTX(c->active_context);
    int line_number = cpu_ip2line(state, fc->method, fc->ip);
    char *filename = (char*)rbs_symbol_to_cstring(state, cmethod_get_file(fc->method));

    RUBINIUS_FUNCTION_ENTRY(module_name, method_name, filename, line_number);
  }
#endif
    
    if(cpu_perform_system_primitive(state, c, prim, msg)) {
      /* Worked! */
      
      if(EXCESSIVE_TRACING) {
        printf("%05d: Called prim %s => %s on %s.\n", c->depth,
          rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c))),  
          rbs_symbol_to_cstring(state, msg->name), _inspect(msg->recv));
      }
      
#if ENABLE_DTRACE
  if (RUBINIUS_FUNCTION_RETURN_ENABLED()) {
    char *module_name = msg->module == Qnil ? "<unknown>" : (char*)rbs_symbol_to_cstring(state, module_get_name(msg->module));
    char *method_name = (char*)rbs_symbol_to_cstring(state, msg->name);

    cpu_flush_ip(c);

    struct fast_context *fc = FASTCTX(c->active_context);
    int line_number = cpu_ip2line(state, fc->method, fc->ip);
    char *filename = rbs_symbol_to_cstring(state, cmethod_get_file(fc->method));

    RUBINIUS_FUNCTION_RETURN(module_name, method_name, filename, line_number);
  }
#endif
      return TRUE;
    }
    if(EXCESSIVE_TRACING) {
      printf("[[ Primitive failed! -- %d ]]\n", prim);
    }
    
    return FALSE;
  }
    
  /* raise an exception about them not doing it right. */
  cpu_raise_arg_error(state, c, msg->args, req);
  
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


inline void cpu_yield_debugger_check(STATE, cpu c) {
  /* Yield to the debugger if flag is set */
  if(TASK_FLAG_P(c, TASK_DEBUG_ON_CTXT_CHANGE)) {
    if(EXCESSIVE_TRACING) {
      printf("Yielding to debugger due to context change\n");
    }
    cpu_yield_debugger(state, c);
  }
}


inline void cpu_restore_context_with_home(STATE, cpu c, OBJECT ctx, OBJECT home) {
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
 
  c->locals = FASTCTX(home)->locals;

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

/* Used in debugging. Verifies that the expected depth is the actual depth. */
/*
static void _verify_depth(cpu c) {
  int count = 0;
  OBJECT ctx = c->active_context;

  while(!NIL_P(ctx)) {
    count++;
    ctx = FASTCTX(ctx)->sender;
  }

  assert(count == c->depth);
}
*/

inline void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home, int so) {
  c->depth++;

  if(c->active_context != Qnil) {
    cpu_save_registers(state, c, so);
  }
  cpu_restore_context_with_home(state, c, ctx, home);
  cpu_yield_debugger_check(state, c);
}

/* Layer 2.5: Uses lower layers to return to the calling context.
   Returning ends here. */

void nmc_activate(STATE, cpu c, OBJECT nmc, OBJECT val, int reraise);

inline int cpu_simple_return(STATE, cpu c, OBJECT val) {
  OBJECT destination, home;

#if ENABLE_DTRACE
  if (RUBINIUS_FUNCTION_RETURN_ENABLED()) {
    OBJECT module = cpu_current_module(state, c);
    
    char *module_name = (module == Qnil) ? "<unknown>" : rbs_symbol_to_cstring(state, module_get_name(module));
    char *method_name = rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c)));
    
    cpu_flush_ip(c);
    
    struct fast_context *fc = FASTCTX(c->active_context);
    int line_number = cpu_ip2line(state, fc->method, fc->ip);
    char *filename = rbs_symbol_to_cstring(state, cmethod_get_file(fc->method));

    RUBINIUS_FUNCTION_RETURN(module_name, method_name, filename, line_number);
  }
#endif

  c->depth--;

  destination = cpu_current_sender(c);
  
  // printf("Rtrnng frm %p (%d)\n", c->active_context, FASTCTX(c->active_context)->size);
  
  if(destination == Qnil) {
    object_memory_retire_context(state->om, c->active_context);
    
    c->active_context = Qnil;
    
    /* Thread exitting, reschedule.. */
    if(c->current_thread != c->main_thread) {
      THDEBUG("%d: thread reached end, dead.\n", getpid());
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
      cpu_restore_context_with_home(state, c, destination, home);
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
  
#if ENABLE_DTRACE
  if (RUBINIUS_FUNCTION_RETURN_ENABLED() && !is_block) {
    OBJECT module = cpu_current_module(state, c);

    char *module_name = (module == Qnil) ? "<unknown>" : rbs_symbol_to_cstring(state, module_get_name(module));
    char *method_name = rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c)));

    cpu_flush_ip(c);
    
    struct fast_context *fc = FASTCTX(c->active_context);
    int line_number = cpu_ip2line(state, fc->method, fc->ip);
    char *filename = rbs_symbol_to_cstring(state, cmethod_get_file(fc->method));

    RUBINIUS_FUNCTION_RETURN(module_name, method_name, filename, line_number);
  }
#endif

  c->depth--;
  
  if(destination == Qnil) {
    object_memory_retire_context(state->om, c->active_context);
    
    c->active_context = Qnil;
    
    /* Thread exitting, reschedule.. */
    if(c->current_thread != c->main_thread) {
      THDEBUG("%d: thread reached end, dead.\n", getpid());
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
        printf("CTX: remote return from %p to %p\n", c->active_context, destination);
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
    
    if(EXCESSIVE_TRACING) {
      if(stack_context_p(destination)) {
        printf("Returning to a stack context %p / %p (%s).\n", c->active_context, destination, (uintptr_t)c->active_context - (uintptr_t)destination == CTX_SIZE ? "stack" : "REMOTE");
      } else {
        printf("Returning to %s.\n", _inspect(destination));
      }
    }
    
    xassert(om_valid_context_p(state, destination));
    xassert(om_valid_context_p(state, home));

    /* Commenting out 02.01.08 - Caleb Tennis.
       I don't know the purpose of this code, but if an exception is throws from
       a rb_funcall in subtend, this causes an endless loop in cpu_return_to_sender.

       Commenting it out fixes that for now.
       Hopefully someone smarter than me knows a better fix for the future.
    
     Skip over NMCs for now. 

    if(exception && FASTCTX(destination)->type == FASTCTX_NMC) {
      c->active_context = destination;
      return cpu_return_to_sender(state, c, val, FALSE, TRUE);      
    }
    
    */

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
      cpu_restore_context_with_home(state, c, destination, home);
      if(!exception) stack_push(val);
    }
  }
  
  return TRUE;
}

/* Layer 3: goto. Basically jumps directly into the specificed method. 
   no lookup required. */

inline void cpu_goto_method(STATE, cpu c, OBJECT recv, OBJECT meth,
                                     int count, OBJECT name, OBJECT block) {
  OBJECT ctx;
  struct message msg;

  msg.recv = recv;
  msg.name = name;
  msg.method = meth;
  msg.module = Qnil;
  msg.block = block;
  msg.args = count;

  if(cpu_try_primitive(state, c, &msg)) return;

  ctx = cpu_create_context(state, c, &msg);
  cpu_activate_context(state, c, ctx, ctx, 0);
}

/* Layer 3: hook. Shortcut for running hook methods. */

inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg) {
  OBJECT rub, vm;
  struct message msg;

  msg.name = meth;
  msg.recv = recv;
  msg.klass = _real_class(state, recv);
  msg.priv = TRUE;

  if(!cpu_find_method(state, c, &msg)) return;

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

  cpu_send(state, c, vm, SYM("perform_hook"), 4, Qnil);
}

/* Layer 4: High level method calling. */

/* Layer 4: direct activation. Used for calling a method thats already
   been looked up. */
static inline void cpu_activate_method(STATE, cpu c, struct message *msg) {
  OBJECT ctx;

  c->depth++;
  if(c->depth == CPU_MAX_DEPTH) {
    machine_handle_fire(FIRE_STACK);
  }

  if(cpu_try_primitive(state, c, msg)) return;
  
  ctx = cpu_create_context(state, c, msg);

  cpu_save_registers(state, c, msg->args);
  cpu_restore_context_with_home(state, c, ctx, ctx);
}

static inline void cpu_perform(STATE, cpu c, const struct message *msg) {
  OBJECT ctx;

  c->depth++;
  if(c->depth == CPU_MAX_DEPTH) {
    machine_handle_fire(FIRE_STACK);
  }

  ctx = cpu_create_context(state, c, msg);

  /* If it was missing, setup some extra data in the MethodContext for
     the method_missing method to check out, to see why it was missing. */
  if(msg->missing && msg->priv) {
    methctx_reference(state, ctx);
    object_set_ivar(state, ctx, SYM("@send_private"), Qtrue);
  }
 
  cpu_save_registers(state, c, msg->args);
  cpu_restore_context_with_home(state, c, ctx, ctx);
}


static inline void
cpu_patch_mono(STATE, struct send_site *ss, struct message *msg);

static inline void
cpu_patch_missing(STATE, struct send_site *ss, struct message *msg);

static void 
_cpu_ss_basic(STATE, cpu c, struct send_site *ss, struct message *msg) {
  msg->missing = 0;
  
  sassert(cpu_locate_method(state, c, msg));
  
  /* If it's not method_missing, cache the details of msg in the send_site */
  if(!msg->missing) { 
    cpu_patch_mono(state, ss, msg);
  } else {
    cpu_patch_missing(state, ss, msg);
    msg->args += 1;
    stack_push(msg->name);
  }
    
  if(cpu_try_primitive(state, c, msg)) return;

  cpu_perform(state, c, msg);
}

void cpu_initialize_sendsite(STATE, struct send_site *ss) {
  ss->lookup = _cpu_ss_basic;
}

/* Send Site specialization 1: execute a primitive directly. */

static void _cpu_ss_mono_prim(STATE, cpu c, struct send_site *ss, 
    struct message *msg) {
  prim_func func;
  int _orig_sp;
  OBJECT *_orig_sp_ptr;

  if(_real_class(state, msg->recv) != ss->data1) {
    _cpu_ss_basic(state, c, ss, msg);
    return;
  }
  
  _orig_sp_ptr = c->sp_ptr;
  _orig_sp = c->sp;

  func = (prim_func)ss->c_data;
  
  msg->method = ss->data2;
  msg->module = ss->data3;

  if(!func(state, c, msg)) {
    c->sp_ptr = _orig_sp_ptr;
    c->sp = _orig_sp;

    cpu_perform(state, c, msg);
  }
}

/* Called before a primitive is run the slow way, allowing the send_site to be patch
 * to call the primitive directly. */
void cpu_patch_primitive(STATE, const struct message *msg, prim_func func) {
  struct send_site *ss;

  if(!REFERENCE_P(msg->send_site)) return;

  ss = SENDSITE(msg->send_site);
  
  SET_STRUCT_FIELD(msg->send_site, ss->data1, _real_class(state, msg->recv));
  SET_STRUCT_FIELD(msg->send_site, ss->data2, msg->method);
  SET_STRUCT_FIELD(msg->send_site, ss->data3, msg->module);
  
  ss->c_data = (void*)func;
  ss->lookup = _cpu_ss_mono_prim;
}

/* Send Site specialization 2: Run an ffi function directly. */
static void _cpu_ss_mono_ffi(STATE, cpu c, struct send_site *ss, 
    struct message *msg) {
  rni_context *ctx;
  nf_stub_ffi func;
  
  func = (nf_stub_ffi)ss->c_data;
  // printf("mono-ffi: %p (%s)\n", func, _inspect(ss->name));

  if(_real_class(state, msg->recv) != ss->data1) {
    _cpu_ss_basic(state, c, ss, msg);
    return;
  }
 
  ctx = subtend_retrieve_context();
  ctx->state = state;
  ctx->cpu = c;

  func();
}

/* Called before an FFI function is run the slow way, allowing the send_site to be patch
 * to call the function directly. */
void cpu_patch_ffi(STATE, const struct message *msg) {
  struct send_site *ss;

  if(!REFERENCE_P(msg->send_site)) return;

  ss = SENDSITE(msg->send_site);
  
  SET_STRUCT_FIELD(msg->send_site, ss->data1, _real_class(state, msg->recv));
  SET_STRUCT_FIELD(msg->send_site, ss->data2, msg->method);
  SET_STRUCT_FIELD(msg->send_site, ss->data3, msg->module);
  
  ss->c_data = *DATA_STRUCT(nfunc_get_data(msg->method), void**);
  ss->lookup = _cpu_ss_mono_ffi;

  return;
  /*
  
  if(!REFERENCE_P(msg->send_site)) return;
  
  ss = SENDSITE(msg->send_site);
  SET_STRUCT_FIELD(msg->send_site, ss->data1, msg->recv);
  SET_STRUCT_FIELD(msg->send_site, ss->data2, nfunc_get_data(msg->method));
  ss->lookup = _cpu_ss_mono_ffi;
  */
}

/* Send Site specialzitation 3: simple monomorphic last implemenation cache. */
static void _cpu_ss_mono(STATE, cpu c, struct send_site *ss, 
    struct message *msg) {
  
  if(_real_class(state, msg->recv) != ss->data1) {
    _cpu_ss_basic(state, c, ss, msg);
    return;
  }
  
  msg->method = ss->data2;
  msg->module = ss->data3;
    
  if(cpu_try_primitive(state, c, msg)) return; 

  cpu_perform(state, c, msg);
}

/* Saves the details of +msg+ in +ss+ and install _cpu_ss_mono in +ss+, so 
 * that the next time +ss+ is used, it will try the cache details. */
static inline void
cpu_patch_mono(STATE, struct send_site *ss, struct message *msg) {
  ss->lookup = _cpu_ss_mono;
  SET_STRUCT_FIELD(msg->send_site, ss->data1, _real_class(state, msg->recv));
  SET_STRUCT_FIELD(msg->send_site, ss->data2, msg->method);
  SET_STRUCT_FIELD(msg->send_site, ss->data3, msg->module);
}

static void
_cpu_ss_missing(STATE, cpu c, struct send_site *ss, struct message *msg) {
  if(_real_class(state, msg->recv) != ss->data1) {
    _cpu_ss_basic(state, c, ss, msg);
    return;
  }
  
  msg->method = ss->data2;
  msg->module = ss->data3;

  msg->args += 1;
  stack_push(msg->name);
 
  if(cpu_try_primitive(state, c, msg)) return;

  cpu_perform(state, c, msg);
}

/* Saves the details of +msg+ in +ss+ and install _cpu_ss_mono in +ss+, so 
 * that the next time +ss+ is used, it will try the cache details. */
static inline void
cpu_patch_missing(STATE, struct send_site *ss, struct message *msg) {
  ss->lookup = _cpu_ss_missing;
  SET_STRUCT_FIELD(msg->send_site, ss->data1, _real_class(state, msg->recv));
  SET_STRUCT_FIELD(msg->send_site, ss->data2, msg->method);
  SET_STRUCT_FIELD(msg->send_site, ss->data3, msg->module);
}

static void _cpu_on_no_method(STATE, cpu c, const struct message *msg) {
  char *str;
  OBJECT exc;

  exc = rbs_const_get(state, BASIC_CLASS(object), "RuntimeError");

  str = malloc(1024);
  sprintf(str, "Unable to find any version of '%s' to run", _inspect(msg->name));

  cpu_raise_exception(state, c, cpu_new_exception(state, c, exc, str));

  free(str);
}

/* Layer 4: send. Primary method calling function. */
inline void cpu_send_message(STATE, cpu c, struct message *msg) {
  struct send_site *ss;

#ifdef TIME_LOOKUP
  uint64_t start = measure_cpu_time();
#endif

  //if(SENDSITE_P(msg->send_site)) {
    ss = SENDSITE(msg->send_site);
    msg->name = ss->name;
    ss->lookup(state, c, ss, msg);
  /*
  } else {
    msg->name = msg->send_site;
    msg->send_site = Qnil;

    msg->missing = 0;

    sassert(cpu_locate_method(state, c, msg));

    if(msg->missing) { 
      msg->args += 1;
      stack_push(msg->name);
    }

    if(cpu_try_primitive(state, c, msg)) return;

    cpu_perform(state, c, msg);
  }
  */

#ifdef TIME_LOOKUP
  state->lookup_time += (measure_cpu_time() - start);
#endif
}

void cpu_send_message_external(STATE, cpu c, struct message *msg) {
  OBJECT ctx;
  
  if(!cpu_locate_method(state, c, msg)) {
    _cpu_on_no_method(state, c, msg);
    return;
  }
  
  if(msg->missing) {
    msg->args += 1;
    stack_push(msg->name);
  } else {
    if(cpu_try_primitive(state, c, msg)) return;
  }
 
  c->depth++;
  if(c->depth == CPU_MAX_DEPTH) {
    machine_handle_fire(FIRE_STACK);
  }

  ctx = cpu_create_context(state, c, msg);

  /* If it was missing, setup some extra data in the MethodContext for
     the method_missing method to check out, to see why it was missing. */
  if(msg->missing && msg->priv) {
    methctx_reference(state, ctx);
    object_set_ivar(state, ctx, SYM("@send_private"), Qtrue);
  }
 
  cpu_save_registers(state, c, msg->args);
  cpu_restore_context_with_home(state, c, ctx, ctx);
}


/* A version used when there is no send_site. */
void cpu_send(STATE, cpu c, OBJECT recv, OBJECT sym, int args, OBJECT block) {
  struct message msg;

  msg.recv = recv;
  msg.name = sym;
  msg.args = args;
  msg.block = block;
  msg.klass = _real_class(state, recv);
  msg.priv = c->call_flags;
  msg.missing = 0;
  msg.send_site = Qnil;
  
  c->call_flags = 0;

  cpu_send_message_external(state, c, &msg);
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
  cpu_save_registers(state, c, 0);
  
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
      l = N2I(tuple_at(state, ent, 0));
      r = N2I(tuple_at(state, ent, 1));
      if(cur >= l && cur <= r) {
        /* Make sure the bounds are within the block, therwise, don't use
           it. */
        if(is_block) {
          env = blokctx_env(state, ctx);
          if(l < N2I(blokenv_get_initial_ip(env))
                  || r > N2I(blokenv_get_last_ip(env))) {
            continue;
          }
        }
        target = N2I(tuple_at(state, ent, 2));
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

void cpu_yield_debugger(STATE, cpu c) {
  /* Ensure the DEBUG_ON_CTXT_CHANGE flag is cleared so we don't try
    to yield more than once */
  if(TASK_FLAG_P(c, TASK_DEBUG_ON_CTXT_CHANGE)) {
    TASK_CLEAR_FLAG(c, TASK_DEBUG_ON_CTXT_CHANGE);
    struct cpu_task *task = (struct cpu_task*)BYTES_OF(c->current_task);
    TASK_CLEAR_FLAG(task, TASK_DEBUG_ON_CTXT_CHANGE);
  }

  cpu_flush_sp(c);
  cpu_flush_ip(c);
  methctx_reference(state, c->active_context);

  OBJECT dbg = c->debug_channel;
  if(dbg == Qnil) {
    /* No debug channel on the task, so use the VM default one (if any) */
    OBJECT mod, vm;
    mod = rbs_const_get(state, BASIC_CLASS(object), "Rubinius");
    if(!NIL_P(mod)) {
      vm = rbs_const_get(state, mod, "VM");
      if(!NIL_P(vm)) {
        dbg = object_get_ivar(state, vm, SYM("@debug_channel"));
      }
    }
  }

  if(dbg != Qnil) {
    if(c->control_channel == Qnil) {
      /* No control channel on the task, so create one */
      c->control_channel = cpu_channel_new(state);
    }

    sassert(cpu_channel_has_readers_p(state, dbg));
    cpu_channel_send(state, c, dbg, c->current_thread);
    /* This is so when this task is reactivated, the sent value wont be placed
       on the stack, keeping the stack clean. */
    TASK_SET_FLAG(c, TASK_NO_STACK);
    cpu_channel_receive(state, c, c->control_channel, c->current_thread);
  } else {
    cpu_raise_arg_error_generic(state, c, "Attempted to switch to debugger, no debugger installed");
  }
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
  const char *firesuit_arg;

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
      object_type_to_type(current_machine->g_firesuit_arg, firesuit_arg);
      cpu_raise_exception(state, c,
        cpu_new_exception2(state, c, global->exc_type,
            "Invalid type encountered %s: %s",
	    current_machine->g_firesuit_message, firesuit_arg));
      free(current_machine->g_firesuit_message);
      break;
    case FIRE_STACK:
      cpu_raise_exception(state, c, 
        cpu_new_exception(state, c, 
            rbs_const_get(state, BASIC_CLASS(object), "SystemStackError"), 
            "Maximum amount of stack space used"));
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

#if ENABLE_DTRACE
      if (RUBINIUS_GC_BEGIN_ENABLED()) {
        RUBINIUS_GC_BEGIN();
      }
#endif
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
      if(cpu_channel_has_readers_p(state, state->global->on_gc_channel)) {
        cpu_channel_send(state, c, state->global->on_gc_channel, Qtrue);
      }
      
      state->om->collect_now = 0;

#if ENABLE_DTRACE
      if (RUBINIUS_GC_END_ENABLED()) {
        RUBINIUS_GC_END();
      }
#endif      
    }
    
    if(state->check_events) {
      state->check_events = 0;
      if(state->pending_events) cpu_event_runonce(state);
      if(state->pending_threads) cpu_thread_preempt(state, c);
    }
  }
}

void cpu_run_script(STATE, cpu c, OBJECT meth) {
  OBJECT name;
  name = string_to_sym(state, string_new(state, "__script__"));
  cpu_goto_method(state, c, c->main, meth, 0, name, Qnil);
}
