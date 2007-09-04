#include "shotgun.h"
#include "cpu.h"
#include "methctx.h"
#include "tuple.h"
#include "flags.h"

OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, int locals) {
  OBJECT obj;
    
  obj = blokenv_allocate(state);
  blokenv_set_home(obj, ctx);
  blokenv_set_initial_ip(obj, I2N(start));
  blokenv_set_last_ip(obj, lst);
  blokenv_set_post_send(obj, vlst);
  blokenv_set_home_block(obj, ctx_block);
  blokenv_set_local_count(obj, I2N(locals));
  return obj;
}

OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp) {
  OBJECT ctx;
  int cnt;
  struct fast_context *fc;
  
  ctx = object_memory_new_context(state->om);
  if(ctx >= state->om->context_last) {
    state->om->collect_now |= OMCollectYoung;
  }
  
  if(state->excessive_tracing) {
    printf("CTX:           block running %d\n", (int)ctx);
  }
  
  HEADER(ctx)->flags = 0;
  HEADER(ctx)->flags2 = 0;
  HEADER(ctx)->fields = FASTCTX_FIELDS;
  
  fc = FASTCTX(ctx);
  fc->sender = sender;
  fc->ip = FIXNUM_TO_INT(blokenv_get_initial_ip(self));
  fc->sp = sp;
  /* env lives here */
  fc->name = self;
  fc->self = Qnil;
  fc->method = Qnil;
  fc->block = Qnil;
  fc->literals = Qnil;
  fc->method_module = Qnil;
  
  cnt = FIXNUM_TO_INT(blokenv_get_local_count(self));
  if(cnt > 0) {
    fc->locals = tuple_new(state, cnt);
  } else {
    fc->locals = Qnil;
  }
  
  fc->type = FASTCTX_BLOCK;
  return ctx;
}
