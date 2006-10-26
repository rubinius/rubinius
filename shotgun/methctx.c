#include "rubinius.h"
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

void methctx_setup_from_method(STATE, OBJECT mc, OBJECT meth, OBJECT from) {
  methctx_set_raiseable(mc, Qtrue);
  methctx_set_bytecodes(mc, cmethod_get_bytecodes(meth));
  methctx_set_sender(mc, from);
  methctx_set_literals(mc, cmethod_get_literals(meth));
  methctx_set_method(mc, meth);
}

OBJECT methctx_s_from_method(STATE, OBJECT meth, OBJECT from) {
  OBJECT mc;
  mc = methctx_new(state, FIXNUM_TO_INT(cmethod_get_locals(meth)));
  methctx_setup_from_method(state, mc, meth, from);
  return mc;
}

void methctx_s_reuse(STATE, OBJECT self, OBJECT meth, OBJECT from) {
  int lcls, fels;
  
  lcls = FIXNUM_TO_INT(cmethod_get_locals(meth));
  fels = NUM_FIELDS(methctx_get_locals(self));
  if(fels < lcls) {
    methctx_init_locals(state, self, lcls);
  }
  methctx_init_registers(state, self);
  methctx_setup_from_method(state, self, meth, from);
}

#define WasReferenced 0x20

int methctx_s_was_referenced_p(STATE, OBJECT obj) {
  return FLAG_SET_P(obj, WasReferenced);
}

void methctx_reference(STATE, OBJECT self) {
  FLAG_SET(self, WasReferenced);
}

char *methctx_describe(STATE, OBJECT ctx) {
  printf("MethodContext: %p, called %s on %p. Method used: %p\n",
    ctx,
    rbs_symbol_to_string(state, methctx_get_name(ctx)),
    methctx_get_receiver(ctx),
    methctx_get_method(ctx)
  );
}

#define IsBlockContextFlag 0x40

OBJECT blokctx_s_under_context(STATE, OBJECT ctx) {
  OBJECT obj;
  
  obj = blokctx_allocate(state);
  FLAG_SET(obj, IsBlockContextFlag);
  blokctx_set_raiseable(obj, Qtrue);
  blokctx_set_ip(obj, I2N(0));
  blokctx_set_sp(obj, I2N(0));
  blokctx_set_home(obj, ctx);
  blokctx_set_last_op(obj, I2N(0));
  blokctx_set_start_op(obj, I2N(FIXNUM_TO_INT(methctx_get_ip(ctx)) + 5 ));
  return obj;
}

int blokctx_s_block_context_p(STATE, OBJECT ctx) {
  return FLAG_SET_P(ctx, IsBlockContextFlag);
}