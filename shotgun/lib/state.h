#ifndef RBS_STATE_H
#define RBS_STATE_H

#include <ucontext.h>
#include <hashtable.h>
#include <ptr_array.h>

#include <termios.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/memutil.h"
#include "shotgun/lib/subtend/PortableUContext.h"

/* These are for custom literals. We'll need an API to set it
   eventually. */
#define CUSTOM_CLASS Qnil

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

struct rubinius_globals {
  
  /* classes for the core 'types' */
  OBJECT blokctx, cmethod, tuple, module, object, array;
  OBJECT class, hash, methtbl, bytearray, methctx, blank;
  OBJECT blokenv, bignum, regexp, regexpdata, matchdata;
  OBJECT string, symbol, io, metaclass, symtbl;
  OBJECT nil_class, true_class, false_class, fixnum_class, undef_class;
  OBJECT floatpoint, fastctx, data, nmethod, nmc, task, list, list_node;
  OBJECT channel, thread, staticscope;
  
  /* the primary symbol table */
  OBJECT symbols;
  OBJECT method_missing;
  OBJECT sym_inherited, sym_opened_class;
  OBJECT sym_from_literal, sym_method_added, sym_s_method_added, sym_init_copy;
  OBJECT sym_plus, sym_minus, sym_equal, sym_nequal, sym_tequal, sym_lt, sym_gt;
  OBJECT exc_arg, exc_segfault;
  OBJECT exc_loe, exc_type;
  OBJECT exc_stack_explosion;
  OBJECT exc_primitive_failure, sym_initialize;
  
  OBJECT external_ivars, scheduled_threads, errno_mapping;
  OBJECT config, ffi_ptr, ffi_func, sym_send;
  OBJECT sym_public, sym_private, sym_protected, sym_const_missing;
  OBJECT sym_object_id;
  OBJECT exception, iseq, icache;
  OBJECT top_scope, on_gc_channel;
  
  OBJECT special_classes[SPECIAL_CLASS_SIZE];
};


#define NUM_OF_GLOBALS (sizeof(struct rubinius_globals) / sizeof(OBJECT))

#define CPU_CACHE_MASK 0xfff
#define CPU_CACHE_HASH(c,m) ((((uintptr_t)(c)>>3)^((uintptr_t)m)) & CPU_CACHE_MASK)
#define CPU_CACHE_TOLERANCE 3

/* size is mask + 1 + tolerance */
#define CPU_CACHE_SIZE 0x1003

struct method_cache {
  OBJECT klass;
  OBJECT name;
  OBJECT module;
  OBJECT method;
};

struct rubinius_state;

typedef struct rubinius_state* rstate;

rstate rubinius_state_new();
void state_destroy(rstate);

#ifdef STATE
#undef STATE
#endif

#define STATE rstate state

#define FASTCTX_FIELDS 18
#define FASTCTX_NORMAL 1
#define FASTCTX_BLOCK  3
#define FASTCTX_NMC    4

#include "shotgun/lib/cpu.h"
#include "shotgun/lib/object_memory.h"
#include "shotgun/lib/subtend/handle.h"

struct rubinius_state {
  object_memory om;
  
  struct method_cache method_cache[CPU_CACHE_SIZE + CPU_CACHE_TOLERANCE];
  
#ifdef TRACK_STATS
  int cache_hits;
  int cache_misses;
  int cache_used;
  int cache_collisions;
  int cache_inline_hit;
  int cache_inline_stale;
  int cache_inline_const_hit;
#endif

  struct rubinius_globals *global;
  
  /* Used to pass information down to the garbage collectors */
  OBJECT *current_stack;
  OBJECT *current_sp;
  int ac_on_stack, home_on_stack, sender_on_stack;

#ifdef USE_CINVOKE
  CInvContext *c_context;
#endif

  rni_handle_table *handle_tbl;
  
  unsigned long *stack_bottom;
  
  struct hashtable *cleanup;
  struct hashtable *config;
  
  void *event_base;
  void *thread_infos;
  
  OBJECT *samples;
  int max_samples, cur_sample;
  
  int excessive_tracing, gc_stats;
  int check_events, pending_threads, pending_events;

  struct termios * termios;
};

#define BASIC_CLASS(kind) state->global->kind
#define NEW_OBJECT(kls, size) object_memory_new_object(state->om, kls, size)
#define NEW_STRUCT(obj, str, kls, kind) \
  obj = object_memory_new_opaque(state, kls, sizeof(kind)); \
  str = (kind * )BYTES_OF(obj)

#define NEW_OBJECT_MATURE(kls, size) object_memory_new_object_mature(state->om, kls, size)  

#define DATA_STRUCT(obj, kind) ((kind)BYTES_OF(obj))
#define BYTES2FIELDS(bytes) (bytes % 4 == 0 ? bytes : ((bytes + 4) - ((bytes + 4) % 4)))

#include "shotgun/lib/bignum.h"
#include "shotgun/lib/float.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/machine.h"

#define FIRE_ACCESS 1
#define FIRE_NULL   2
#define FIRE_STACK  3
#define FIRE_ASSERT 4
#define FIRE_TYPE   5

OBJECT rbs_const_set(STATE, OBJECT module, const char *name, OBJECT obj);
OBJECT rbs_const_get(STATE, OBJECT module, const char *name);
OBJECT rbs_class_new(STATE, const char *name, int fields, OBJECT obj);
OBJECT rbs_class_new_with_namespace(STATE, const char *name, int fields, OBJECT obj, OBJECT ns);
const char *rbs_symbol_to_cstring(STATE, OBJECT sym);
OBJECT rbs_symbol_to_string(STATE, OBJECT sym);
const char *rbs_inspect(STATE, OBJECT obj);
const char *rbs_inspect_verbose(STATE, OBJECT obj);
const char *_inspect(OBJECT obj);
OBJECT rbs_module_new(STATE, const char *name, OBJECT ns);
OBJECT rbs_class_new_instance(STATE, OBJECT cls);

void *XMALLOC(size_t n);
void *XREALLOC(void *p, size_t n);
void *XCALLOC(size_t n, size_t s);
void XFREE(void *p);

#define ALLOC_N(t,n) (t*)XCALLOC(n, sizeof(t))
#define ALLOC(t) (t*)XMALLOC(sizeof(t))
#define REALLOC_N(v,t,n) (v)=(t*)XREALLOC((void*)(v), sizeof(t)*n)
#define FREE(v) XFREE(v)

static inline long rbs_to_int(OBJECT obj) {
  return STRIP_TAG(obj);
}

static inline OBJECT rbs_int_to_fixnum(STATE, int num) {
  OBJECT ret;
  ret = APPLY_TAG(num, TAG_FIXNUM);

  /* Number is too big for fixnum. Use bignum. */
  if((int)rbs_to_int(ret) != num) {
    return bignum_new(state, num);
  }
  return ret;
}

static inline OBJECT rbs_uint_to_fixnum(STATE, unsigned int num) {
  OBJECT ret;

  if (num > FIXNUM_MAX) {
    return bignum_new_unsigned(state, num);
  } else {
    ret = APPLY_TAG(num, TAG_FIXNUM);
    rbs_to_int(ret);
    return ret;
  }
}

static inline OBJECT rbs_ll_to_integer(STATE, long long num) {
  OBJECT ret;
  ret = APPLY_TAG(num, TAG_FIXNUM);

  /* Number is too big for fixnum. Use bignum. */
  if((int)rbs_to_int(ret) != num) {
    return bignum_from_ll(state, num);
  }
  return ret;
}

static inline OBJECT rbs_ull_to_integer(STATE, unsigned long long num) {
  OBJECT ret;

  if (num > FIXNUM_MAX) {
    return bignum_from_ull(state, num);
  } else {
    ret = APPLY_TAG(num, TAG_FIXNUM);
    rbs_to_int(ret);
    return ret;
  }
}

static inline double rbs_fixnum_to_double(OBJECT obj) {
  double val = rbs_to_int(obj);
  return val;
}


#define FIXNUM_TO_INT(obj) rbs_to_int(obj)
#define FIXNUM_TO_DOUBLE(obj) rbs_fixnum_to_double(obj)
#define BIGNUM_TO_INT(o) bignum_to_int(state, o)
#define INT_TO_FIXNUM(int) rbs_int_to_fixnum(state, int)
#define I2N(i) INT_TO_FIXNUM(i)
#define UI2N(i) rbs_uint_to_fixnum(state, i)
#define ULL2I(i) rbs_ull_to_integer(state, i)
#define LL2I(i) rbs_ll_to_integer(state, i)
#define FLOAT_TO_DOUBLE(k) (*DATA_STRUCT(k, double*))

void object_memory_check_ptr(void *ptr, OBJECT obj);
static inline void object_memory_write_barrier(object_memory om, OBJECT target, OBJECT val);

// #define XDEBUG 1

#ifdef XDEBUG
/* Copied from assert.h */
#define xassert(cond) ((void)((cond) ? 0 : xassert_message(#cond, __FILE__, __LINE__)))
#define xassert_message(str, file, line) \
  (printf("%s:%u: failed assertion '%s'\n", file, line, str), abort(), 0)
#else
#define xassert(cond) 
#endif

#define sassert(cond) ((void)((cond) ? 0 : machine_handle_assert(#cond, __FILE__, __LINE__)))

// #define CHECK_PTR(obj) object_memory_check_ptr(current_machine->om, obj)
#define CHECK_PTR(obj) 

#include "shotgun/lib/object_memory-barrier.h"

#define SET_FIELD(obj, fel, val) rbs_set_field(state->om, obj, fel, val)
#define NTH_FIELD(obj, fel) rbs_get_field(obj, fel)

#define IS_REF_P(val) REFERENCE_P(val)
#define RUN_WB(obj, val) if(IS_REF_P(val)) object_memory_write_barrier(state->om, obj, val)

#define rbs_set_class(om, obj, cls) ({ if(IS_REF_P(cls)) object_memory_write_barrier(om, obj, cls); obj->klass = cls; })
#define SET_CLASS(obj, cls) ({ RUN_WB(obj, cls); obj->klass = cls; })

void machine_handle_fire(int);
void machine_handle_assert(const char *reason, const char *file, int line);
void machine_handle_type_error(OBJECT);

#include "shotgun/lib/environment.h"

#define current_machine (environment_current_machine())

/* No bounds checking! Be careful! */
#define fast_fetch(obj, idx) NTH_FIELD_DIRECT(obj, idx)

/* Only ever call this with constant arguments! */
#define fast_set(obj, idx, val) ({ \
  if(REFERENCE_P(val)) object_memory_write_barrier(state->om, obj, val); \
  SET_FIELD_DIRECT(obj, idx, val); \
  val; \
})

#define fast_unsafe_set(obj, idx, val) SET_FIELD_DIRECT(obj, idx, val)

#define fast_set_int(obj, idx, int) fast_unsafe_set(obj, idx, I2N(int))
#define fast_inc(obj, idx) fast_unsafe_set(obj, idx, (void*)((uintptr_t)fast_fetch(obj, idx) + (1 << TAG_SHIFT)))

#define ACCESS_MACROS 1
#define EXTRA_PROTECTION 0

#if ACCESS_MACROS

#if DISABLE_CHECKS

#define rbs_set_field(om, obj, fel, val) ({ \
  OBJECT _v = (val), _o = (obj); \
  if(REFERENCE_P(_v)) { \
    object_memory_write_barrier(om, _o, _v); \
  } \
  SET_FIELD_DIRECT(_o, fel, _v); })

#define rbs_get_field(obj, fel) NTH_FIELD_DIRECT(obj, fel)

#else

static void _bad_reference(OBJECT in) {
  printf("Attempted to access field of non-reference.\n");
  if(current_machine->g_use_firesuit) {
    machine_handle_fire(FIRE_NULL);
  } 
}

static void _bad_reference2(OBJECT in, int fel) {
  printf("Attempted to access field %d in an object with %lu fields.\n", 
    fel, (unsigned long)NUM_FIELDS(in));
    
  if(current_machine->g_use_firesuit) {
    machine_handle_fire(FIRE_ACCESS);
  }
}

#ifdef EXTRA_PROTECTION

#define rbs_set_field(om, obj, fel, val) ({ \
  OBJECT _v = (val), _o = (obj); \
  if(!REFERENCE_P(obj)) _bad_reference(obj); \
  if(fel >= obj->field_count) _bad_reference2(obj, fel); \
  if(REFERENCE_P(_v)) { \
    object_memory_write_barrier(om, _o, _v); \
  } \
  *(OBJECT*)ADDRESS_OF_FIELD(_o, fel) = _v; })

#define rbs_get_field(i_in, i_fel) ({ \
  OBJECT in = (i_in); int fel = (i_fel); \
  if(!REFERENCE_P(in)) _bad_reference(in); \
  if(fel >= in->field_count) _bad_reference2(in, fel); \
  NTH_FIELD_DIRECT(in, fel); })

#else

/* These are the typically used versions. The don't check for ref, they
   the segfault handler do that. */

#define rbs_set_field(om, obj, fel, val) ({ \
  OBJECT _v = (val), _o = (obj); \
  if(fel >= obj->field_count) _bad_reference2(obj, fel); \
  if(REFERENCE_P(_v)) { \
    object_memory_write_barrier(om, _o, _v); \
  } \
  *(OBJECT*)ADDRESS_OF_FIELD(_o, fel) = _v; })

#define rbs_get_field(i_in, i_fel) ({ \
  OBJECT in = (i_in); int fel = (i_fel); \
  if(fel >= in->field_count) _bad_reference2(in, fel); \
  NTH_FIELD_DIRECT(in, fel); })


#endif

#endif

#else


static inline OBJECT rbs_get_field(OBJECT in, int fel) {
  OBJECT obj;
#if DISABLE_CHECKS
  if(!REFERENCE_P(in)) {
    printf("Attempted to access field of non-reference.\n");
    if(current_machine->g_use_firesuit) {
      machine_handle_fire(FIRE_NULL);
    }
  }
  
  if(in->StoresBytes) {
    printf("Attempted to access field of byte addressed object.\n");
    if(current_machine->g_use_firesuit) {
      machine_handle_fire(FIRE_NULL);
    }
  }
  
  if(fel >= in->field_count) {
    printf("Attempted to access field %d in an object with %lu fields.\n", 
      fel, (unsigned long)NUM_FIELDS(in));
      
    if(current_machine->g_use_firesuit) {
      machine_handle_fire(FIRE_ACCESS);
    }
    
    assert(0);
  }
#endif
  obj = NTH_FIELD_DIRECT(in, fel);
#if INTERNAL_DEBUG
  CHECK_PTR(obj);
#endif
  return obj;
}

static inline OBJECT rbs_set_field(object_memory om, OBJECT obj, int fel, OBJECT val) {
#if DISABLE_CHECKS
  if(fel >= obj->field_count) {
    printf("Attempted to access field %d in an object with %lu fields (%s).\n", 
      fel, (unsigned long)NUM_FIELDS(obj), _inspect(obj));
    
    if(current_machine->g_use_firesuit) {
      machine_handle_fire(1);
    }
    
    assert(0);
  }
#endif

  if(obj->StoresBytes) {
    printf("Attempted to access field of byte addressed object.\n");
    if(current_machine->g_use_firesuit) {
      machine_handle_fire(FIRE_NULL);
    }
  }

  OBJECT *slot = (OBJECT*)ADDRESS_OF_FIELD(obj, fel);
#if INTERNAL_DEBUG
  /* Check that it's even, ie a ref, and above the special range. */
  if(IS_REF_P(val)) {
    object_memory_write_barrier(om, obj, val);
  }
  CHECK_PTR(val);
#endif
  *slot = val;
  return val;
} 

#endif

#define SET_STRUCT_FIELD(obj, fel, val) ({ OBJECT _tmp = (val); RUN_WB(obj, _tmp); fel = _tmp; _tmp; })  

#include "shotgun/lib/object_memory-inline.h"

typedef void (*state_cleanup_func)(STATE, OBJECT);
void state_add_cleanup(STATE, OBJECT cls, state_cleanup_func func);
void state_run_cleanup(STATE, OBJECT obj, OBJECT cls);

#endif /* __STATE__ */
