#include "object_memory.h"

struct rubinius_globals {
  
  /* classes for the core 'types' */
  OBJECT blokctx, cmethod, tuple, module, object, array;
  OBJECT class, hash, methtbl, bytearray, methctx, blank;
  OBJECT blokenv, bignum, regexp, regexpdata, matchdata;
  OBJECT string, symbol, io, metaclass, symtbl;
  OBJECT nil_class, true_class, false_class, fixnum_class, undef_class;
  OBJECT floatpoint;
  
  /* the primary symbol table */
  OBJECT symbols;
  OBJECT method_missing;
  OBJECT sym_inherited;
  OBJECT sym_from_literal, sym_method_added, sym_s_method_added;
  
  OBJECT exc_arg;
  OBJECT exc_loe;
  OBJECT exc_stack_explosion;
  OBJECT exc_primitive_failure;
  
  /* The external ivars table. */
  OBJECT external_ivars;
};

#define GLOBAL_cmethod

#define NUM_OF_GLOBALS (sizeof(struct rubinius_globals) / sizeof(OBJECT))

struct rubinius_state {
  object_memory om;
  GPtrArray *free_contexts;
  
  struct rubinius_globals *global;
};

typedef struct rubinius_state* rstate;

rstate rubinius_state_new();

#ifdef STATE
#undef STATE
#endif

#define STATE rstate state
#define BASIC_CLASS(kind) state->global->kind
#define NEW_OBJECT(kls, size) object_memory_new_object(state->om, kls, size)
#define NEW_STRUCT(obj, str, kls, kind) \
  obj = object_memory_new_opaque(state, kls, sizeof(kind)); \
  str = (kind * )BYTES_OF(obj)
  
#define DATA_STRUCT(obj, kind) ((kind)BYTES_OF(obj))

#include "bignum.h"
#include "float.h"
#include "fixnum.h"
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
inline void object_memory_write_barrier(object_memory om, OBJECT target, OBJECT val);
//#define CHECK_PTR(obj) object_memory_check_ptr(main_om, obj)
#define CHECK_PTR(obj)

#define SET_FIELD(obj, fel, val) rbs_set_field(state->om, obj, fel, val)

static inline OBJECT rbs_get_field(OBJECT in, int fel) {
  OBJECT obj;
  assert(fel < HEADER(in)->fields);
  obj = NTH_FIELD_DIRECT(in, fel);
#ifdef INTERNAL_MACROS
  CHECK_PTR(obj);
#endif
  return obj;
}

#define NTH_FIELD(obj, fel) rbs_get_field(obj, fel)

static inline OBJECT rbs_set_field(object_memory om, OBJECT obj, int fel, OBJECT val) {
  if(fel >= HEADER(obj)->fields) {
    printf("Attempted to access field %d in an object with %lu fields (%s).\n", 
      fel, NUM_FIELDS(obj), _inspect(obj));
    assert(0);
  }
  OBJECT *slot = (OBJECT*)ADDRESS_OF_FIELD(obj, fel);
  assert(val != 12);
#ifdef INTERNAL_MACROS
  object_memory_write_barrier(om, obj, val);
  CHECK_PTR(val);
#endif
  *slot = val;
  return val;
}
