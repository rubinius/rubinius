#include "object_memory.h"

struct rubinius_globals {
  
  /* classes for the core 'types' */
  OBJECT symtbl, blokctx, cmethod, tuple, module, object, array;
  OBJECT class, hash, methtbl, bytearray, methctx, blank;
  OBJECT blokenv;
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
