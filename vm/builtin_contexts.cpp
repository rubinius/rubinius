#include "objects.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  MethodContext* MethodContext::create(STATE) {
    MethodContext* ctx;
    ctx = (MethodContext*)state->new_struct(G(methctx), sizeof(MethodContext));
    ctx->no_value = false;
    return ctx;
  }

  void MethodContext::reference(STATE) {

  }

  void MethodContext::Info::mark(MethodContext* obj) {
  }

  BlockContext* BlockContext::create(STATE) {
    return (BlockContext*)state->new_struct(G(blokctx), sizeof(BlockContext));
  }

  void BlockEnvironment::call(STATE, size_t args) {

  }

  void BlockEnvironment::call(STATE, Message& msg) {

  }

  BlockContext* BlockEnvironment::create_context(STATE) {
    BlockContext* ctx = BlockContext::create(state);
    SET(ctx, sender, (MethodContext*)Qnil);
    SET(ctx, name, (SYMBOL)this);
    SET(ctx, cm, method);
    SET(ctx, stack, Tuple::create(state, method->stack_size->to_nint()));

    ctx->vmm = method->vmmethod(state);
    ctx->ip = 0;
    ctx->sp = method->number_of_locals() - 1;

    return ctx;
  }

  BlockEnvironment* BlockEnvironment::under_context(STATE, CompiledMethod* cm,
      MethodContext* parent, MethodContext* active) {

    BlockEnvironment* be = (BlockEnvironment*)state->new_object(G(blokenv));

    parent->reference(state);
    active->reference(state);

    SET(be, home, parent);
    SET(be, home_block, active);
    SET(be, method, cm);
    SET(be, local_count, cm->local_count);

    return be;
  }
}
