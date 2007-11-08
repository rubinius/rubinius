#ifndef RBS_METHCTX
#define RBS_METHCTX 1

OBJECT blokctx_s_under_context(STATE, OBJECT ctx);
OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, OBJECT locals);
OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp);

#define methctx_is_context_p(st, ctx) (ctx->obj_type == MContextType || ctx->obj_type == BContextType)
#define blokctx_s_block_context_p(state, ctx) (FASTCTX(ctx)->type == FASTCTX_BLOCK)

#define methctx_is_fast_p(st, ctx) (ctx->obj_type == MContextType)

#define blokctx_home(state, obj) (blokenv_get_home(FASTCTX(obj)->name))
#define blokctx_env(state, self) (FASTCTX(self)->name)
#define blokctx_locals(state, self) (FASTCTX(self)->locals)

static inline void methctx_reference(STATE, OBJECT ctx) {  
  /* Don't do it again. */
  if(!stack_context_p(ctx)) return;
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
  object_memory_context_referenced(state->om, ctx);
}

#endif
