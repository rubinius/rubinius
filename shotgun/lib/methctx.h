#ifndef RBS_METHCTX
#define RBS_METHCTX 1

OBJECT blokctx_s_under_context(STATE, OBJECT ctx);
OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, OBJECT locals);
OBJECT blokenv_s_under_context2(STATE, OBJECT cmethod, OBJECT ctx, OBJECT ctx_block);

OBJECT methctx_dup(STATE, OBJECT self);
OBJECT methctx_dup_chain(STATE, OBJECT ctx, OBJECT *also);

#define methctx_is_context_p(st, ctx) (ctx->obj_type == MContextType || ctx->obj_type == BContextType)
#define blokctx_s_block_context_p(state, ctx) (FASTCTX(ctx)->type == FASTCTX_BLOCK)

#define methctx_is_fast_p(st, ctx) (ctx->obj_type == MContextType)

#define blokctx_home(state, obj) (blokenv_get_home(FASTCTX(obj)->name))
#define blokctx_env(state, self) (FASTCTX(self)->name)
#define blokctx_locals(state, self) (FASTCTX(self)->locals)

void methctx_reference(STATE, OBJECT ctx);


#if 0
static inline void methctx_reference(STATE, OBJECT ctx) {
  struct fast_context *fc;
  /* Don't do it again. */
  if(!stack_context_p(ctx)) return;
  
  /* Has to be done first because this uses informated we're about
     to overwrite. */
  object_memory_context_referenced(state->om, ctx);
  
  CLEAR_FLAGS(ctx);
  ctx->gc_zone = YoungObjectZone;
  switch(FASTCTX(ctx)->type) {
  case FASTCTX_NORMAL:
    ctx->klass = BASIC_CLASS(fastctx);
    ctx->obj_type = MContextType;
    ctx->CTXFast = TRUE;
    break;
  case FASTCTX_BLOCK:  
    ctx->klass = BASIC_CLASS(blokctx);
    ctx->obj_type = BContextType;
    break;
  case FASTCTX_NMC:
    ctx->klass = BASIC_CLASS(nmc);
    ctx->obj_type = MContextType;
    break;
  }
  SET_NUM_FIELDS(ctx, FASTCTX_FIELDS);
  ctx->StoresBytes = TRUE;
  ctx->ForeverYoung = TRUE;
  
  fc = FASTCTX(ctx);
  
  /* Fixup the locals tuple. */
  if(!NIL_P(fc->locals)) {
    CLEAR_FLAGS(fc->locals);
    fc->locals->gc_zone = YoungObjectZone;
    fc->locals->klass = BASIC_CLASS(tuple);
  }
}

#endif

#endif
