#ifndef RBS_METHCTX
#define RBS_METHCTX 1

OBJECT methctx_new(STATE, int cnt);
void methctx_init_registers(STATE, OBJECT self);
void methctx_init_locals(STATE, OBJECT self, int cnt);
OBJECT methctx_s_from_method(STATE, OBJECT meth, OBJECT from, OBJECT mod);
OBJECT blokctx_s_under_context(STATE, OBJECT ctx);
OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, int count);
OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp);
OBJECT blokctx_home(STATE, OBJECT self);
void methctx_s_reuse(STATE, OBJECT self, OBJECT meth, OBJECT from, OBJECT mod);

#include "flags.h"

#define methctx_is_fast_p(st, ctx) FLAG_SET_P(ctx, CTXFastFlag)
#define FASTCTX(ctx) ((struct fast_context*)BYTES_OF(ctx))

static inline int methctx_s_was_referenced_p(STATE, OBJECT obj) {
  return FLAG_SET_P(obj, WasReferencedFlag);
}

static inline void methctx_reference(STATE, OBJECT ctx) {  
  /* If it has a class object setup, we've already adjusted it
     and everything. Don't do it again. */
  if(HEADER(ctx)->klass != Qnil) return;
  
  HEADER(ctx)->flags = 0;
  HEADER(ctx)->flags2 = 0;
  HEADER(ctx)->gc = 0;
  GC_ZONE_SET(ctx, GC_YOUNG_OBJECTS);
  
  HEADER(ctx)->klass = state->global->fastctx;
  SET_NUM_FIELDS(ctx, FASTCTX_FIELDS);
  
  FLAG_SET(ctx, CTXFastFlag);
  FLAG_SET(ctx, StoresBytesFlag);
    
  GC_MAKE_FOREVER_YOUNG(ctx);
  
  object_memory_context_referenced(state->om, ctx);
}

static inline int blokctx_s_block_context_p(STATE, OBJECT ctx) {
  return HEADER(ctx)->klass != Qnil && FLAG_SET_P(ctx, IsBlockContextFlag);
}

#endif
