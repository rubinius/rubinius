#include "builtin/contexts.hpp"
#include "objectmemory.hpp"
#include "builtin/block_environment.hpp"

namespace rubinius {
  MethodContext* MethodContext::create(STATE) {
    MethodContext* ctx;
    ctx = (MethodContext*)state->new_struct(G(methctx), sizeof(MethodContext));
    SET(ctx, block, Qnil);
    SET(ctx, name, Qnil);
    SET(ctx, home, Qnil);

    return ctx;
  }

  BlockEnvironment* BlockContext::env() {
    return as<BlockEnvironment>(name);
  }

  void MethodContext::reference(STATE) {

  }

  BlockContext* BlockContext::create(STATE) {
    return (BlockContext*)state->new_struct(G(blokctx), sizeof(BlockContext));
  }
}
