#include "shotgun.h"

#ifdef USE_CINVOKE
#include <cinvoke.h>
#endif

struct rubinius_globals {
  
  /* classes for the core 'types' */
  OBJECT blokctx, cmethod, tuple, module, object, array;
  OBJECT class, hash, methtbl, bytearray, methctx, blank;
  OBJECT blokenv, bignum, regexp, regexpdata, matchdata;
  OBJECT string, symbol, io, metaclass, symtbl;
  OBJECT nil_class, true_class, false_class, fixnum_class, undef_class;
  OBJECT floatpoint, fastctx, data, nmethod, nmc, task, list, list_node;
  OBJECT channel, thread;
  
  /* the primary symbol table */
  OBJECT symbols;
  OBJECT method_missing;
  OBJECT sym_inherited;
  OBJECT sym_from_literal, sym_method_added, sym_s_method_added;
  OBJECT sym_plus, sym_minus, sym_equal, sym_tequal, sym_lt, sym_gt;
  OBJECT exc_arg, exc_segfault;
  OBJECT exc_loe;
  OBJECT exc_stack_explosion;
  OBJECT exc_primitive_failure;
  
  OBJECT external_ivars, scheduled_threads, errno_mapping;
  OBJECT recent_children, config;
};

#define GLOBAL_cmethod

#define NUM_OF_GLOBALS (sizeof(struct rubinius_globals) / sizeof(OBJECT))

#define CPU_CACHE_SIZE 0x1000
#define CPU_CACHE_MASK 0xfff
#define CPU_CACHE_HASH(c,m) ((((c)>>3)^(m)) & CPU_CACHE_MASK)

struct method_cache {
  OBJECT klass;
  OBJECT name;
  OBJECT module;
  OBJECT method;
};

struct rubinius_state;

typedef struct rubinius_state* rstate;

rstate rubinius_state_new();

#ifdef STATE
#undef STATE
#endif

#define STATE rstate state

#include "object_memory.h"
#include "subtend/handle.h"

struct rubinius_state {
  object_memory om;
  GPtrArray *free_contexts;
  
  struct method_cache method_cache[CPU_CACHE_SIZE];
  
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

#ifdef USE_CINVOKE
  CInvContext *c_context;
#endif

  rni_handle_table *handle_tbl;
  
  unsigned long *stack_bottom;
  
  GHashTable *cleanup;
  GHashTable *config;
  
  void *event_base;
  void *thread_infos;
  
  OBJECT *samples;
  int max_samples, cur_sample;
  
  int excessive_tracing;
};

#define BASIC_CLASS(kind) state->global->kind
#define NEW_OBJECT(kls, size) object_memory_new_object(state->om, kls, size)
#define NEW_STRUCT(obj, str, kls, kind) \
  obj = object_memory_new_opaque(state, kls, sizeof(kind)); \
  str = (kind * )BYTES_OF(obj)

#define NEW_OBJECT_MATURE(kls, size) object_memory_new_object_mature(state->om, kls, size)  

#define DATA_STRUCT(obj, kind) ((kind)BYTES_OF(obj))
#define BYTES2FIELDS(bytes) (bytes % 4 == 0 ? bytes : ((bytes + 4) - ((bytes + 4) % 4)))

#include "bignum.h"
#include "float.h"
#include "array.h"

// rubinius.h defines STATE as void*, which means these prototypes fail to match.
// This is pretty confusing, since they look identical before the pre-processor runs.
// HACK
#ifdef __SHOTGUN__
OBJECT rbs_const_set(STATE, OBJECT module, char *name, OBJECT obj);
OBJECT rbs_const_get(STATE, OBJECT module, char *name);
OBJECT rbs_class_new(STATE, char *name, int fields, OBJECT obj);
OBJECT rbs_class_new_with_namespace(STATE, char *name, int fields, OBJECT obj, OBJECT ns);
char *rbs_symbol_to_cstring(STATE, OBJECT sym);
OBJECT rbs_symbol_to_string(STATE, OBJECT sym);
char *rbs_inspect(STATE, OBJECT obj);
char *rbs_inspect_verbose(STATE, OBJECT obj);
char *_inspect(OBJECT obj);
OBJECT rbs_module_new(STATE, char *name, OBJECT ns);
OBJECT rbs_class_new_instance(STATE, OBJECT cls);

#endif

static inline long rbs_to_int(OBJECT obj) {
  int val = ((int)obj) >> FIXNUM_SHIFT;
  return val;
}

static inline OBJECT rbs_int_to_fixnum(STATE, int num) {
  OBJECT ret;
  ret = (num << FIXNUM_SHIFT) | FIXNUM_MARKER;
  
  /* Number is too big for fixnum. Use bignum. */
  if(rbs_to_int(ret) != num) {
    return bignum_new(state, num);
  }
  return ret;
}

static inline double rbs_fixnum_to_double(OBJECT obj) {
  double val = rbs_to_int(obj);
  return val;
}

static inline OBJECT rbs_uint_to_fixnum(STATE, unsigned int num) {
  OBJECT ret;
  
  ret = (num << FIXNUM_SHIFT) | FIXNUM_MARKER;
  
  if(rbs_to_int(ret) != num) {
    return bignum_new_unsigned(state, num);
  }
  
  return ret;
}

#define FIXNUM_TO_INT(obj) rbs_to_int(obj)
#define FIXNUM_TO_DOUBLE(obj) rbs_fixnum_to_double(obj)
#define BIGNUM_TO_INT(o) bignum_to_int(state, o)
#define INT_TO_FIXNUM(int) rbs_int_to_fixnum(state, int)
#define I2N(i) INT_TO_FIXNUM(i)
#define UI2N(i) rbs_uint_to_fixnum(state, i)
#define FLOAT_TO_DOUBLE(k) (*DATA_STRUCT(k, double*))

extern void* main_om;
void object_memory_check_ptr(void *ptr, OBJECT obj);
static inline void object_memory_write_barrier(object_memory om, OBJECT target, OBJECT val);
// #define CHECK_PTR(obj) object_memory_check_ptr(main_om, obj)
#define CHECK_PTR(obj) 

#include "object_memory-barrier.h"

#define SET_FIELD(obj, fel, val) rbs_set_field(state->om, obj, fel, val)
#define NTH_FIELD(obj, fel) rbs_get_field(obj, fel)

#define IS_REF_P(val) (((val & 1) == 0) && (val > 12))
#define RUN_WB(obj, val) if(IS_REF_P(val)) object_memory_write_barrier(state->om, obj, val)

#define rbs_set_class(om, obj, cls) ({ HEADER(obj)->klass = cls; if(IS_REF_P(cls)) object_memory_write_barrier(om, obj, cls); })
#define SET_CLASS(obj, cls) ({ HEADER(obj)->klass = cls; RUN_WB(obj, cls); })

// #define ACCESS_MACROS 1

#ifdef ACCESS_MACROS

#define rbs_set_field(om, obj, fel, val) ({ \
  OBJECT _v, _o; \
  _v = (val); _o = (obj);\
  if(((_v & 1) == 0) && (_v > 12)) { \
    object_memory_write_barrier(om, _o, _v); \
  } \
  OBJECT *slot = (OBJECT*)ADDRESS_OF_FIELD(_o, fel);\
  *slot = _v;\
})

#define rbs_get_field(obj, fel) NTH_FIELD_DIRECT(obj, fel)

#else

static inline OBJECT rbs_get_field(OBJECT in, int fel) {
  OBJECT obj;
#if DISABLE_CHECKS
  if(fel >= HEADER(in)->fields) {
    printf("Attempted to access field %d in an object with %lu fields.\n", 
      fel, (unsigned long)NUM_FIELDS(in));
    assert(0);
  }
#endif
  obj = NTH_FIELD_DIRECT(in, fel);
#ifdef INTERNAL_MACROS
  CHECK_PTR(obj);
#endif
  return obj;
}

static inline OBJECT rbs_set_field(object_memory om, OBJECT obj, int fel, OBJECT val) {
#if DISABLE_CHECKS
  if(fel >= HEADER(obj)->fields) {
    printf("Attempted to access field %d in an object with %lu fields (%s).\n", 
      fel, (unsigned long)NUM_FIELDS(obj), _inspect(obj));
    assert(0);
  }
#endif

  OBJECT *slot = (OBJECT*)ADDRESS_OF_FIELD(obj, fel);
#ifdef INTERNAL_MACROS
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

#include "object_memory-inline.h"

typedef void (*state_cleanup_func)(STATE, OBJECT);
#define SHOULD_CLEANUP_P(obj) (FLAG_SET_P(obj, RequiresCleanupFlag))
void state_add_cleanup(STATE, OBJECT cls, state_cleanup_func func);
void state_run_cleanup(STATE, OBJECT obj);
