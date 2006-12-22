#include "shotgun.h"
#include "methctx.h"
#include "tuple.h"

OBJECT methctx_new(STATE, int cnt) {
  OBJECT obj;
  obj = methctx_allocate(state);
  methctx_init_registers(state, obj);
  methctx_init_locals(state, obj, cnt);
  return obj;
}

void methctx_init_registers(STATE, OBJECT self) {
  methctx_set_argcount(self, I2N(0));
  methctx_set_ip(self, I2N(0));
  methctx_set_sp(self, I2N(0));
}

void methctx_init_locals(STATE, OBJECT self, int cnt) {
  methctx_set_locals(self, tuple_new(state, cnt+2));
}

void methctx_setup_from_method(STATE, OBJECT mc, OBJECT meth, OBJECT from, OBJECT mod) {
  methctx_set_raiseable(mc, Qtrue);
  methctx_set_bytecodes(mc, cmethod_get_bytecodes(meth));
  methctx_set_sender(mc, from);
  methctx_set_literals(mc, cmethod_get_literals(meth));
  methctx_set_method(mc, meth);
  methctx_set_module(mc, mod);
}

OBJECT methctx_s_from_method(STATE, OBJECT meth, OBJECT from, OBJECT mod) {
  OBJECT mc;
  mc = methctx_new(state, FIXNUM_TO_INT(cmethod_get_locals(meth)));
  methctx_setup_from_method(state, mc, meth, from, mod);
  return mc;
}

void methctx_s_reuse(STATE, OBJECT self, OBJECT meth, OBJECT from, OBJECT mod) {
  int lcls, fels;
  
  lcls = FIXNUM_TO_INT(cmethod_get_locals(meth));
  fels = NUM_FIELDS(methctx_get_locals(self));
  if(fels < lcls) {
    methctx_init_locals(state, self, lcls);
  }
  methctx_init_registers(state, self);
  methctx_setup_from_method(state, self, meth, from, mod);
}

#define WasReferenced 0x20

int methctx_s_was_referenced_p(STATE, OBJECT obj) {
  return FLAG_SET_P(obj, WasReferenced);
}

void methctx_reference(STATE, OBJECT self) {
  FLAG_SET(self, WasReferenced);
}

void methctx_describe(STATE, OBJECT ctx, int count) {
  int i;
  for(i = 0; RTEST(ctx) && i < count; i++) {
    printf("%2d: %15s on %s. (ip: %d, ctx: %p)\n",
      i,
      (char*)rbs_symbol_to_cstring(state, methctx_get_name(ctx)),
      (char*)_inspect(methctx_get_receiver(ctx)),
      (int)FIXNUM_TO_INT(methctx_get_ip(ctx)),
      (void*)ctx
    );
    ctx = methctx_get_sender(ctx);
  }
}

#define IsBlockContextFlag 0x40

OBJECT blokenv_s_under_context(STATE, OBJECT ctx, int start, OBJECT lst, OBJECT vlst) {
  OBJECT obj;
  
  obj = blokenv_allocate(state);
  blokenv_set_home(obj, ctx);
  blokenv_set_initial_ip(obj, I2N(start));
  // blokenv_set_initial_ip(obj, I2N(FIXNUM_TO_INT(methctx_get_ip(ctx)) + 5 ));
  blokenv_set_last_ip(obj, lst);
  blokenv_set_post_send(obj, vlst);
  
  return obj;
}

OBJECT blokenv_create_context(STATE, OBJECT self) {
  OBJECT obj;
  
  obj = blokctx_allocate(state);
  FLAG_SET(obj, IsBlockContextFlag);
  blokctx_set_raiseable(obj, Qtrue);
  blokctx_set_ip(obj, blokenv_get_initial_ip(self));
  blokctx_set_sp(obj, I2N(0));
  blokctx_set_env(obj, self);
  return obj;
}

int blokctx_s_block_context_p(STATE, OBJECT ctx) {
  return FLAG_SET_P(ctx, IsBlockContextFlag);
}

OBJECT blokctx_home(STATE, OBJECT self) {
  OBJECT env, home;
  env = blokctx_get_env(self);
  home = blokenv_get_home(env);
  return home;
}
