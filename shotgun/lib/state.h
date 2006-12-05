#include "object_memory.h"

struct rubinius_globals {
  
  /* classes for the core 'types' */
  OBJECT symtbl, blokctx, cmethod, tuple, module, object, array;
  OBJECT class, hash, methtbl, bytearray, methctx, blank;
  OBJECT blokenv, bignum, regexp, regexpdata, matchdata;
  OBJECT string, symbol, io, metaclass;
  OBJECT nil_class, true_class, false_class, fixnum_class, undef_class;
  
  /* the primary symbol table */
  OBJECT symbols;
  OBJECT method_missing;
  OBJECT sym_inherited;
  OBJECT sym_from_literal, sym_method_added, sym_s_method_added;
  
  OBJECT exc_arg;
  OBJECT exc_loe;
  OBJECT exc_stack_explosion;
  
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

static inline long rbs_to_int(OBJECT obj) {
  long val = ((unsigned long)obj) >> 3;
  if(FIXNUM_NEG(obj)) {
    val = -val;
  }
  return val;
}

static inline OBJECT rbs_int_to_fixnum(STATE, int num) {
  OBJECT ret;
  int ab;
  ab = abs(num);
  ret = (ab << 3) | 1;
  
  /* Number is too big for fixnum. Use bignum. */
  if(ab != (ret >> 3)) {
    return bignum_new(state, num);
  }
  
  if(num < 0) {
    ret = ret | 4;
  }
  /*
  ret |= 1;
  */
  return ret;
}

#define FIXNUM_TO_INT(obj) rbs_to_int(obj)
#define INT_TO_FIXNUM(int) rbs_int_to_fixnum(state, int)
#define I2N(i) INT_TO_FIXNUM(i)