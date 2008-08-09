#include "builtin/contexts.hpp"
#include "objectmemory.hpp"
#include "builtin/block_environment.hpp"

namespace rubinius {
  static size_t add_stack(size_t original, size_t stack) {
    return original + (sizeof(OBJECT) * stack);
  }

  static void init_context(STATE, MethodContext* ctx, size_t stack) {
    SET(ctx, block, Qnil);
    SET(ctx, name, Qnil);
    SET(ctx, home, Qnil);

    ctx->stack_size = stack;
    for(size_t i = 0; i < stack; i++) {
      ctx->stk[i] = Qnil;
    }

    ctx->js.stack = ctx->stk - 1;
  }

  MethodContext* MethodContext::create(STATE, size_t stack_size) {
    MethodContext* ctx;
    size_t bytes = add_stack(sizeof(MethodContext), stack_size);
    ctx = (MethodContext*)state->new_struct(G(methctx), bytes);
    init_context(state, ctx, stack_size);

    return ctx;
  }

  BlockEnvironment* BlockContext::env() {
    return as<BlockEnvironment>(name);
  }

  void MethodContext::reference(STATE) {

  }

  BlockContext* BlockContext::create(STATE, size_t stack_size) {
    size_t bytes = add_stack(sizeof(MethodContext), stack_size);
    BlockContext* ctx = (BlockContext*)state->new_struct(G(blokctx), bytes);
    init_context(state, ctx, stack_size);
    return ctx;
  }
}
