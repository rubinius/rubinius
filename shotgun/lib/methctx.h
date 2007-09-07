#ifndef RBS_METHCTX
#define RBS_METHCTX 1

OBJECT blokctx_s_under_context(STATE, OBJECT ctx);
OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, int count);
OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp);

#include "flags.h"

#define methctx_is_context_p(st, ctx) (OBJ_TYPE(ctx) == TYPE_MCONTEXT || OBJ_TYPE(ctx) == TYPE_BCONTEXT)
#define blokctx_s_block_context_p(state, ctx) (FASTCTX(ctx)->type == FASTCTX_BLOCK)

#define methctx_is_fast_p(st, ctx) (OBJ_TYPE(ctx) == TYPE_MCONTEXT)

#define blokctx_home(state, obj) (blokenv_get_home(FASTCTX(obj)->name))
#define blokctx_env(state, self) (FASTCTX(self)->name)
#define blokctx_locals(state, self) (FASTCTX(self)->locals)

static inline void methctx_reference(STATE, OBJECT ctx) {  
  /* Don't do it again. */
  if(!stack_context_p(ctx)) return;
  
  HEADER(ctx)->flags = 0;
  HEADER(ctx)->flags2 = 0;
  HEADER(ctx)->gc = 0;
  GC_ZONE_SET(ctx, GC_YOUNG_OBJECTS);

  switch(FASTCTX(ctx)->type) {
  case FASTCTX_NORMAL:
    HEADER(ctx)->klass = BASIC_CLASS(fastctx);
    OBJ_TYPE_SET(ctx, TYPE_MCONTEXT);
    FLAG_SET(ctx, CTXFastFlag);
    break;
  case FASTCTX_BLOCK:  
    HEADER(ctx)->klass = BASIC_CLASS(blokctx);
    OBJ_TYPE_SET(ctx, TYPE_BCONTEXT);
    break;
  case FASTCTX_NMC:
    HEADER(ctx)->klass = BASIC_CLASS(nmc);
    OBJ_TYPE_SET(ctx, TYPE_MCONTEXT);
    break;
  }
  SET_NUM_FIELDS(ctx, FASTCTX_FIELDS);
  
  FLAG_SET(ctx, StoresBytesFlag);
    
  GC_MAKE_FOREVER_YOUNG(ctx);
  
  object_memory_context_referenced(state->om, ctx);
}

#endif
