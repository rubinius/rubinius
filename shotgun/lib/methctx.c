#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/bytearray.h"

OBJECT methctx_dup(STATE, OBJECT self) {
  OBJECT ctx;
  struct fast_context *cur, *old;
  
  methctx_reference(state, self);
  
  ctx = NEW_OBJECT(object_class(state, self), FASTCTX_FIELDS);

  ctx->StoresBytes = TRUE;
  if(self->obj_type == MContextType) {
    ctx->CTXFast = TRUE;
  }
  ctx->obj_type = self->obj_type;
  
  assert(ctx->obj_type == MContextType || ctx->obj_type == BContextType);
 
  old = FASTCTX(self);
  cur = FASTCTX(ctx);

  if(!NIL_P(old->sender)) methctx_reference(state, old->sender);
  SET_STRUCT_FIELD(ctx, cur->sender, old->sender);
  SET_STRUCT_FIELD(ctx, cur->block, old->block);
  SET_STRUCT_FIELD(ctx, cur->method, old->method);
  SET_STRUCT_FIELD(ctx, cur->literals, old->literals);
  SET_STRUCT_FIELD(ctx, cur->locals, old->locals);
  cur->argcount = old->argcount;
  SET_STRUCT_FIELD(ctx, cur->name, old->name);
  SET_STRUCT_FIELD(ctx, cur->method_module, old->method_module);
  cur->opaque_data = old->opaque_data;
  SET_STRUCT_FIELD(ctx, cur->self, old->self);
  cur->data = old->data;
  cur->type = old->type;
  cur->ip = old->ip;
  cur->sp = old->sp;
  cur->fp = old->fp;
 
  ctx->ForeverYoung = TRUE;
  
  return ctx;  
}

OBJECT methctx_dup_chain(STATE, OBJECT ctx, OBJECT *also) {
  OBJECT sender, top;
  top = methctx_dup(state, ctx);
  ctx = top;
    
  for(;;) {
    sender = FASTCTX(ctx)->sender;
    if(NIL_P(sender)) break;
    
    methctx_reference(state, sender);
    
    FASTCTX(ctx)->sender = methctx_dup(state, sender);
    /* Update another ref if one is passed in
       (used to also update home_context) */
    if(also && *also == sender) {
      *also = FASTCTX(ctx)->sender;
    }
    ctx = sender;
  }
    
  return top;
}

OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, OBJECT locals) {
  OBJECT obj;
    
  obj = blokenv_allocate(state);
  blokenv_set_home(obj, ctx);
  blokenv_set_initial_ip(obj, I2N(start));
  blokenv_set_last_ip(obj, lst);
  blokenv_set_post_send(obj, vlst);
  blokenv_set_home_block(obj, ctx_block);
  blokenv_set_method(obj, FASTCTX(ctx)->method); // Calling method's CompiledMethod
  if(NIL_P(locals)) {
    blokenv_set_local_count(obj, I2N(0));
  } else {
    blokenv_set_local_count(obj, I2N(NUM_FIELDS(locals)));
  }
  blokenv_set_bonus(obj, locals);
  return obj;
}

OBJECT blokenv_s_under_context2(STATE, OBJECT cmethod, OBJECT ctx, OBJECT ctx_block) {
  OBJECT obj;
  int num_lcls = N2I(cmethod_get_locals(cmethod));
    
  obj = blokenv_allocate(state);
  blokenv_set_home(obj, ctx);
  blokenv_set_initial_ip(obj, I2N(0));
  // We have no real last since the block gets it's own method.
  // Just set this very large so that comparison for bounds
  // checks are always true.
  blokenv_set_last_ip(obj, I2N(1 << 25));
  blokenv_set_post_send(obj, I2N(0));
  blokenv_set_home_block(obj, ctx_block);
  blokenv_set_method(obj, cmethod);
  blokenv_set_local_count(obj, I2N(num_lcls));
  return obj;
}

void methctx_reference(STATE, OBJECT ctx) {
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
    SET_CLASS(ctx, BASIC_CLASS(fastctx));
    ctx->obj_type = MContextType;
    ctx->CTXFast = TRUE;
    break;
  case FASTCTX_BLOCK:  
    SET_CLASS(ctx, BASIC_CLASS(blokctx));
    ctx->obj_type = BContextType;
    break;
  case FASTCTX_NMC:
    SET_CLASS(ctx, BASIC_CLASS(nmc));
    ctx->obj_type = MContextType;
    break;
  }
  SET_NUM_FIELDS(ctx, FASTCTX_FIELDS);
  ctx->StoresBytes = TRUE;
  ctx->ForeverYoung = TRUE;
  
  fc = FASTCTX(ctx);
  
  /* Fixup the locals tuple. */
  if(!NIL_P(fc->locals) && fc->locals->gc_zone == 0) {
    fc->locals->gc_zone = YoungObjectZone;
  }

}

