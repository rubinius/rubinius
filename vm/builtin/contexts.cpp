#include "builtin/contexts.hpp"
#include "objectmemory.hpp"
#include "builtin/block_environment.hpp"

#define CountContextCaches 2

/* These numbers were taken from Squeak */
#define SmallContextSize   16
#define SmallContextCache  0
#define LargeContextSize   56
#define LargeContextCache  1

namespace rubinius {

  /* Implements a simple context cache based on having context of a couple
   * sizes only. This implementation also support 'huge' contexts by
   * always just heap allocating them. */
  class ContextCache {
  public:
    /* A stack depth, indicates how many more contexts we can recycle.
     * This is reset to 0 if it's determined that context recycle needs
     * to be disabled for all current contexts. */
    int reclaim;

    /* A simple array of MethodContext's. The contexts are treated like
     * linked lists, chaining to the next context off their home field.
     * This lets us store any number of contexts quickly in a couple
     * of different caches. Each cache contains context of all the same
     * size. */
    MethodContext* caches[CountContextCaches];

    /* Reset all state to empty. */
    void reset() {
      reclaim = 0;
      for(size_t i = 0; i < CountContextCaches; i++) {
        caches[i] = (MethodContext*)Qnil;
      }
    }

    /* Retrieve a context from +which+ cache. Return NULL if the cache
     * was empty. */
    MethodContext* get(size_t which) {
      MethodContext* ctx;
      if((ctx = caches[which]) != Qnil) {
        /* We chain off ->home */
        caches[which] = ctx->home;
        return ctx;
      }

      return NULL;
    }

    /* Add context +ctx+ to cache +which+. */
    void add(size_t which, MethodContext* ctx) {
      ctx->home = caches[which];
      caches[which] = ctx;

      reclaim--;
    }
  };

  /* Calculate how much big of an object (in bytes) to allocate
   * for one with a body of +original+ and a stack of +stack+ */
  static size_t add_stack(size_t original, size_t stack) {
    return original + (sizeof(OBJECT) * stack);
  }

  /* Initialize +ctx+'s fields */
  static void init_context(STATE, MethodContext* ctx, size_t stack) {
    ctx->ip = 0;
    SET(ctx, block, Qnil);
    SET(ctx, name, Qnil);
    SET(ctx, home, Qnil);

    ctx->stack_size = stack;
    for(size_t i = 0; i < stack; i++) {
      ctx->stk[i] = Qnil;
    }

    ctx->js.stack = ctx->stk - 1;
  }

  /* Find a context to use. Either use a cache or create one in the heap. */
  static MethodContext* allocate(STATE, Class* cls, size_t stack_size) {
    MethodContext* ctx;
    size_t which_cache = SmallContextCache;

    /* If it's small enough, use the set small size. */
    if(stack_size < SmallContextSize) {
      stack_size = SmallContextSize;

    /* If it's bigger than our large size, always use the heap. */
    } else if(stack_size > LargeContextSize) {
      goto allocate_heap;

    /* Otherwise use a large size. */
    } else {
      stack_size = LargeContextSize;
      which_cache = LargeContextCache;
    }

    if((ctx = state->context_cache->get(which_cache)) != NULL) {
      goto initialize;
    }

allocate_heap:
    size_t bytes = add_stack(sizeof(MethodContext), stack_size);
    ctx = (MethodContext*)state->new_struct(cls, bytes);

initialize:
    state->context_cache->reclaim++;
    init_context(state, ctx, stack_size);
    return ctx;
  }

  /* Attempt to recycle +this+ context into the context cache, based
   * on it's size. */
  void MethodContext::recycle(STATE) {
    if(state->context_cache->reclaim > 0) {
      size_t which;
      if(stack_size == SmallContextSize) {
        which = SmallContextCache;
      } else if(stack_size != LargeContextSize) {
        return;
      } else {
        which = LargeContextSize;
      }

      state->context_cache->add(which, this);

    }
  }

  /* Create a ContextCache object and install it in +state+ */
  void MethodContext::initialize_cache(STATE) {
    if(state->context_cache) {
      delete state->context_cache;
    }

    state->context_cache = new ContextCache;
    state->context_cache->reset();
  }

  /* Zero out the caches. */
  void MethodContext::reset_cache(STATE) {
    state->context_cache->reset();
  }

  /* Return a new +MethodContext+ object, which needs +stack_size fields
   * worth of stack. */
  MethodContext* MethodContext::create(STATE, size_t stack_size) {
    return allocate(state, G(methctx), stack_size);
  }

  /* Retrieve the BlockEnvironment from +this+ BlockContext. We reuse the
   * name field from MethodContext and use a type-safe cast. */
  BlockEnvironment* BlockContext::env() {
    return as<BlockEnvironment>(name);
  }

  /* Called when a context is referenced. Typically, this is via the push_context
   * opcode or MethodContext#sender. */
  void MethodContext::reference(STATE) {
    /* TODO when this is called via MethodContext#sender, we don't need to wipe
     * out the reclaim count, since that context is alread protected. */
    state->context_cache->reclaim = 0;
  }

  /* Return a new +BlockContext+ object, which needs +stack_size+ fields
   * worth of stack. */
  BlockContext* BlockContext::create(STATE, size_t stack_size) {
    BlockContext* ctx = (BlockContext*)allocate(state, G(blokctx), stack_size);
    return ctx;
  }
}
