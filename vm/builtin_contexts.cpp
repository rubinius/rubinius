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

  BlockContext* BlockContext::create(STATE) {
    return (BlockContext*)state->new_struct(G(blokctx), sizeof(BlockContext));
  }

  void BlockEnvironment::call(STATE, size_t args) {

  }

  BlockEnvironment* BlockEnvironment::under_context(STATE, CompiledMethod* cm,
      MethodContext* parent, MethodContext* active) {

  }
}
