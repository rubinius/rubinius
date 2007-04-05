OBJECT methctx_new(STATE, int cnt);
void methctx_init_registers(STATE, OBJECT self);
void methctx_init_locals(STATE, OBJECT self, int cnt);
OBJECT methctx_s_from_method(STATE, OBJECT meth, OBJECT from, OBJECT mod);
OBJECT blokctx_s_under_context(STATE, OBJECT ctx);
OBJECT blokenv_s_under_context(STATE, OBJECT ctx, int start, OBJECT lst, OBJECT vlst);
OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp);
OBJECT blokctx_home(STATE, OBJECT self);
void methctx_s_reuse(STATE, OBJECT self, OBJECT meth, OBJECT from, OBJECT mod);

#include "flags.h"

#define methctx_is_fast_p(st, ctx) FLAG_SET_P(ctx, CTXFastFlag)
#define FASTCTX(ctx) ((struct fast_context*)BYTES_OF(ctx))

static inline int methctx_s_was_referenced_p(STATE, OBJECT obj) {
  return FLAG_SET_P(obj, WasReferencedFlag);
}

static inline void methctx_reference(STATE, OBJECT self) {
  OBJECT ctx;
  
  ctx = self;
  while(RTEST(ctx)) {
    FLAG_SET(ctx, WasReferencedFlag);
#if CTX_USE_FAST
    ctx = FASTCTX(ctx)->sender;
#else
    ctx = methctx_get_sender(ctx);
#endif
  }
}

static inline int blokctx_s_block_context_p(STATE, OBJECT ctx) {
  return FLAG_SET_P(ctx, IsBlockContextFlag);
}
