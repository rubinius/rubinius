#include "shotgun.h"
#include "cpu.h"
#include "methctx.h"
#include "tuple.h"
#include "bytearray.h"

OBJECT methctx_dup(STATE, OBJECT self) {
  OBJECT ctx;
  struct fast_context *cur, *old;
  
  ctx = NEW_OBJECT(object_class(state, self), FASTCTX_FIELDS);

  ctx->StoresBytes = TRUE;
  if(self->obj_type == MContextType) {
    ctx->CTXFast = TRUE;
  }
  ctx->obj_type = self->obj_type;
 
  old = FASTCTX(self);
  cur = FASTCTX(ctx);

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
  int num_lcls = FIXNUM_TO_INT(cmethod_get_locals(cmethod));
    
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


OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp) {
  OBJECT ctx, ins;
  int cnt, i;
  struct fast_context *fc;
  
  cnt = FIXNUM_TO_INT(blokenv_get_local_count(self));
  
  ctx = object_memory_new_context(state->om, cnt);
  if(ctx >= state->om->context_last) {
    state->om->collect_now |= OMCollectYoung;
  }
  
  if(state->excessive_tracing) {
    printf("CTX:           block running %d\n", (int)ctx);
  }
  
  CLEAR_FLAGS(ctx);
  ctx->gc_zone = 0;
  ctx->field_count = FASTCTX_FIELDS;
  ctx->klass = Qnil;
  
  fc = FASTCTX(ctx);
  fc->sender = sender;
  fc->ip = FIXNUM_TO_INT(blokenv_get_initial_ip(self));
  fc->sp = sp;
  /* env lives here */
  fc->name = self;
  fc->self = Qnil;
  fc->method = blokenv_get_method(self);

  ins = cmethod_get_compiled(fc->method);
  
  if(NIL_P(ins)) {
    ins = cpu_compile_method(state, fc->method);
  }
  
  fc->data = bytearray_byte_address(state, ins);
  
  fc->literals = cmethod_get_literals(fc->method);
  fc->block = Qnil;
  fc->method_module = Qnil;
  
  if(cnt > 0) {
    // fc->locals = tuple_new(state, cnt);
    
    fc->locals = object_memory_context_locals(ctx);
    CLEAR_FLAGS(fc->locals);
    fc->locals->gc_zone = 0;
    fc->locals->klass = BASIC_CLASS(tuple);
    SET_NUM_FIELDS(fc->locals, cnt);
    
    for(i = 0; i < cnt; i++) {
      SET_FIELD_DIRECT(fc->locals, i, Qnil);
    }
    
  } else {
    fc->locals = Qnil;
  }
  fc->flags = 0;
  
  /* If post send is nil, that means we're not allowed to return directly to
     the home context. */
  if(NIL_P(blokenv_get_post_send(self))) {
    fc->flags |= CTX_FLAG_NO_LONG_RETURN;
  }
  
  fc->type = FASTCTX_BLOCK;
  return ctx;
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
  if(!NIL_P(fc->locals) && fc->locals->gc_zone == 0) {
    fc->locals->gc_zone = YoungObjectZone;
  }

}

