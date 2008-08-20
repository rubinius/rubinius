#include "builtin/contexts.hpp"
#include "objectmemory.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

#include "context_cache.hpp"

namespace rubinius {

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
    size_t bytes;

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
      ctx->obj_type = (object_type)cls->instance_type->to_native();
      goto initialize;
    }

allocate_heap:
    bytes = add_stack(sizeof(MethodContext), stack_size);
    ctx = (MethodContext*)state->new_struct(cls, bytes);

initialize:
    state->context_cache->reclaim++;
    init_context(state, ctx, stack_size);
    return ctx;
  }

  int MethodContext::line() {
    if(this->cm->nil_p()) return -2;        // trampoline context
    if(this->cm->lines->nil_p()) return -3;

    for(size_t i = 0; i < this->cm->lines->field_count; i++) {
      Tuple* entry = as<Tuple>(this->cm->lines->at(i));

      FIXNUM start_ip = as<Fixnum>(entry->at(0));
      FIXNUM end_ip   = as<Fixnum>(entry->at(1));
      FIXNUM line     = as<Fixnum>(entry->at(2));

      if(start_ip->to_native() <= this->ip &&
         end_ip->to_native()   >= this->ip)
        return line->to_native();
    }

    return -1;
  }

  /* Attempt to recycle +this+ context into the context cache, based
   * on it's size. Returns true if the context was recycled, otherwise
   * false. */
  bool MethodContext::recycle(STATE) {
    /* Only recycle young contexts */
    if(zone != YoungObjectZone) return false;

    if(state->context_cache->reclaim > 0) {
      size_t which;
      if(stack_size == SmallContextSize) {
        which = SmallContextCache;
      } else if(stack_size != LargeContextSize) {
        return false;
      } else {
        which = LargeContextSize;
      }

      state->context_cache->add(which, this);
      return true;
    }

    return false;
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
   * worth of stack.
   */
  MethodContext* MethodContext::create(STATE, size_t stack_size) {
    return allocate(state, G(methctx), stack_size);
  }

  /* Generate a MethodContext for the provided receiver and CompiledMethod
   * The returned MethodContext is in an 'initial' state. The caller is
   * expected to SET any fields it needs to, e.g. +module+
   */
  MethodContext* MethodContext::create(STATE, OBJECT recv, CompiledMethod* meth) {
    MethodContext* ctx = MethodContext::create(state, meth->stack_size->to_native());

    SET(ctx, sender, (MethodContext*)Qnil);
    SET(ctx, self, recv);
    SET(ctx, cm, meth);
    SET(ctx, module, G(object));
    SET(ctx, home, ctx);

    ctx->vmm = (VMMethod*)meth->executable;
    ctx->position_stack(meth->number_of_locals() - 1);

    return ctx;
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

  void MethodContext::Info::mark(OBJECT obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    MethodContext* ctx = as<MethodContext>(obj);

    /* Now also mark the stack */
    OBJECT stack_obj, marked;
    for(size_t i = 0; i < ctx->stack_size; i++) {
      stack_obj = ctx->stack_at(i);
      marked = mark.call(stack_obj);
      if(marked) {
        ctx->stack_put(i, marked);
        mark.just_set(ctx, marked);
      }
    }
  }
}
