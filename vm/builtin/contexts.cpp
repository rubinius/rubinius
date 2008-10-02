#include "objectmemory.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

#include "context_cache.hpp"
#include "builtin/contexts.hpp"
#include "builtin/nativemethodcontext.hpp"

#include <iostream>

#define DISABLE_CACHE 0

namespace rubinius {

  void MethodContext::init(STATE) {
    GO(methctx).set(state->new_class("MethodContext", G(object)));
    G(methctx)->set_object_type(state, MethodContextType);

    GO(blokctx).set(state->new_class("BlockContext", G(methctx)));
    G(blokctx)->set_object_type(state, BlockContextType);
  }

  /* Calculate how much big of an object (in bytes) to allocate
   * for one with a body of +original+ and a stack of +stack+ */
  static size_t add_stack(size_t original, size_t stack) {
    return original + (sizeof(OBJECT) * stack);
  }

  /* Initialize +ctx+'s fields */
  static void init_context(STATE, MethodContext* ctx, size_t stack) {
    ctx->ip = 0;
    ctx->block(state, Qnil);
    ctx->name(state, Qnil);
    ctx->home(state, (MethodContext*)Qnil);
    ctx->ivars(state, Qnil);
    ctx->sender(state, (MethodContext*)Qnil);
    ctx->self(state, Qnil);
    ctx->cm(state, (CompiledMethod*)Qnil);
    ctx->module(state, (Module*)Qnil);

    ctx->stack_size = stack;
    for(size_t i = 0; i < stack; i++) {
      ctx->stk[i] = Qnil;
    }

    ctx->js.stack = ctx->stk - 1;
    ctx->js.stack_top = ctx->stk + stack;
  }

  /* Find a context to use. Either use a cache or create one in the heap. */
  static MethodContext* allocate(STATE, Class* cls, size_t stack_size) {
    MethodContext* ctx;
    size_t which_cache = SmallContextCache;
    size_t bytes;

#if DISABLE_CACHE
    goto allocate_heap;
#endif

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
      ctx->obj_type = (object_type)cls->instance_type()->to_native();
      ctx->klass(state, cls);
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

  int MethodContext::line(STATE) {
    if(cm_->nil_p()) return -2;        // trampoline context
    if(cm_->lines()->nil_p()) return -3;

    for(size_t i = 0; i < cm_->lines()->num_fields(); i++) {
      Tuple* entry = as<Tuple>(cm_->lines()->at(state, i));

      FIXNUM start_ip = as<Fixnum>(entry->at(state, 0));
      FIXNUM end_ip   = as<Fixnum>(entry->at(state, 1));
      FIXNUM line     = as<Fixnum>(entry->at(state, 2));

      if(start_ip->to_native() <= ip && end_ip->to_native() >= ip)
        return line->to_native();
    }

    return -1;
  }

  void MethodContext::post_copy(MethodContext* old) {
    this->position_stack(old->calculate_sp());
    this->js.stack_top = this->stk + this->stack_size;
    if(this->obj_type == MethodContextType) {
      assert(this->home() == old);
    }
  }

  /* Attempt to recycle +this+ context into the context cache, based
   * on it's size. Returns true if the context was recycled, otherwise
   * false. */
  bool MethodContext::recycle(STATE) {
    if(state->context_cache->reclaim > 0) {
      state->context_cache->reclaim--;

      /* Only recycle young contexts */
      if(zone != YoungObjectZone) return false;

      size_t which;
      if(stack_size == SmallContextSize) {
        which = SmallContextCache;
      } else if(stack_size != LargeContextSize) {
        return false;
      } else {
        which = LargeContextCache;
      }

      // HACK this is broken. It's adding context which
      // are still live into the cache (busting those contexts)
      state->context_cache->add(state, which, this);
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
    MethodContext* ctx = MethodContext::create(state, meth->stack_size()->to_native());

    ctx->sender(state, (MethodContext*)Qnil);
    ctx->self(state, recv);
    ctx->cm(state, meth);
    ctx->module(state, G(object));
    ctx->home(state, ctx);

    ctx->vmm = meth->backend_method_;
    ctx->position_stack(meth->number_of_locals() - 1);

    return ctx;
  }

  /* Called as the context_dup primitive
   */
  MethodContext* MethodContext::dup(STATE) {
    MethodContext* ctx = create(state, this->stack_size);

    ctx->sender(state, this->sender());
    ctx->self(state, this->self());
    ctx->cm(state, this->cm());
    ctx->module(state, this->module());
    ctx->block(state, this->block());
    ctx->name(state, this->name());

    if(this->obj_type == MethodContextType) {
      ctx->home(state, ctx);
    } else {
      ctx->home(state, this->home());
    }

    /* Set the obj_type because we get called
     * for both BlockContext and MethodContext
     */
    ctx->obj_type = this->obj_type;

    ctx->vmm = this->vmm;
    ctx->js = this->js;
    ctx->ip = this->ip;
    ctx->args = this->args;
    ctx->stack_size = this->stack_size;

    for(size_t i = 0; i < this->stack_size; i++) {
      ctx->stk[i] = this->stk[i];
    }

    /* Stack Management procedures. Make sure that we don't
     * miss object stored into the stack of a context
     */
    if(ctx->zone == MatureObjectZone) {
      state->om->remember_object(ctx);
    }

    /* This ctx is escaping into Ruby-land */
    ctx->reference(state);

    return ctx;
  }

  /* Retrieve the BlockEnvironment from +this+ BlockContext. We reuse the
   * block field from MethodContext and use a type-safe cast. */
  BlockEnvironment* BlockContext::env() {
    return as<BlockEnvironment>(block());
  }

  /* Called as the block_context_env primitive
   */
  BlockEnvironment* BlockContext::env(STATE) {
    return this->env();
  }

  /* Called when a context is referenced. Typically, this is via the push_context
   * opcode or MethodContext#sender. */
  void MethodContext::reference(STATE) {
    /* TODO when this is called via MethodContext#sender, we don't need to wipe
     * out the reclaim count, since that context is alread protected. */
    state->context_cache->reclaim = 0;
  }

  /* Retrieve a field within the context, referenced by name. This
   * is used as a primitive. */
  OBJECT MethodContext::get_internal_data(STATE, FIXNUM type) {
    switch(type->to_native()) {
    case 1:
      return Fixnum::from(ip);
    }

    sassert(0 && "invalid index");
    return Qnil;
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

    if(ctx->obj_type == MethodContextType) {
      assert(ctx->home() == obj);
    }

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

  void MethodContext::Info::show(STATE, OBJECT self, int level) {
    MethodContext* ctx = as<MethodContext>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); ctx->name()->show(state, level);
    indent_attribute(level, "sender");
    if(ctx->sender()->nil_p()) {
      ctx->sender()->show(state, level);
    } else {
      class_info(state, ctx->sender(), true);
    }
    indent_attribute(level, "home");
    if(ctx->home()->nil_p()) {
      ctx->home()->show(state, level);
    } else {
      class_info(state, ctx->home(), true);
    }
    indent_attribute(level, "self"); ctx->self()->show(state, level);
    indent_attribute(level, "cm"); ctx->cm()->show(state, level);
    indent_attribute(level, "module"); ctx->module()->show(state, level);
    indent_attribute(level, "block"); ctx->block()->show(state, level);
    close_body(level);
  }
}
