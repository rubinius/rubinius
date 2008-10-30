#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

#include "gc_object_mark.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"

#include "builtin/contexts.hpp"
#include "builtin/nativemethodcontext.hpp"

#include <iostream>

#define SmallContextSize   16
#define LargeContextSize   56

namespace rubinius {

  void MethodContext::init(STATE) {
    GO(methctx).set(state->new_class("MethodContext", G(object)));
    G(methctx)->set_object_type(state, MethodContextType);

    GO(blokctx).set(state->new_class("BlockContext", G(methctx)));
    G(blokctx)->set_object_type(state, BlockContextType);
  }

  /* Calculate how much big of an object (in bytes) to allocate
   * for one with a body of +original+ and a stack of +stack+ */
  static inline size_t add_stack(size_t original, size_t stack) {
    return original + (sizeof(Object*) * stack);
  }

  static inline size_t round_stack(size_t original) {
    if(original <= SmallContextSize) {
      return SmallContextSize;
    } else if(original <= LargeContextSize) {
      return LargeContextSize;
    }

    return original;
  }

  /* Initialize +ctx+'s fields */
  static inline void init_context(STATE, MethodContext* ctx, size_t stack) {
    ctx->ip = 0;
    ctx->ivars(state, Qnil);

    // Don't initialize any fields you KNOW are always set later
    // on before the ctx is used. We just waste precious time if we do.

    ctx->stack_size = stack;
    for(size_t i = 0; i < stack; i++) {
      ctx->stk[i] = Qnil;
    }

    ctx->js.stack = ctx->stk - 1;
    ctx->js.stack_top = ctx->stk + stack;
  }

  /* Find a context to use. Either use a cache or create one in the heap. */
  static inline MethodContext* allocate(STATE, Class* cls, size_t stack_size) {
    MethodContext* ctx;
    size_t bytes;

    stack_size = round_stack(stack_size);

    ctx = state->om->allocate_context(stack_size);
    if(ctx) {
      assert((uintptr_t)ctx + ctx->full_size < (uintptr_t)state->om->contexts.last);
      ctx->klass(state, (Class*)Qnil);
      ctx->obj_type = (object_type)cls->instance_type()->to_native();
    } else {
      bytes = add_stack(sizeof(MethodContext), stack_size);
      ctx = (MethodContext*)state->new_struct(cls, bytes);
    }

    init_context(state, ctx, stack_size);
    return ctx;
  }

  int MethodContext::line(STATE) {
    if(cm_->nil_p()) return -2;        // trampoline context
    if(cm_->lines()->nil_p()) return -3;

    for(size_t i = 0; i < cm_->lines()->num_fields(); i++) {
      Tuple* entry = as<Tuple>(cm_->lines()->at(state, i));

      Fixnum* start_ip = as<Fixnum>(entry->at(state, 0));
      Fixnum* end_ip   = as<Fixnum>(entry->at(state, 1));
      Fixnum* line     = as<Fixnum>(entry->at(state, 2));

      if(start_ip->to_native() <= ip && end_ip->to_native() >= ip)
        return line->to_native();
    }

    return -1;
  }

  void MethodContext::post_copy(MethodContext* old) {
    this->js.stack_top = this->stk + this->stack_size;
    this->position_stack(old->calculate_sp());
    if(this->obj_type == MethodContextType) {
      assert(this->home() == old);
    }
  }

  /* Attempt to recycle +this+ context into the context cache, based
   * on it's size. Returns true if the context was recycled, otherwise
   * false. */
  bool MethodContext::recycle(STATE) {
    if(zone != YoungObjectZone) return false;
    return state->om->deallocate_context(this);
  }


  /* Create a ContextCache object and install it in +state+ */
  void MethodContext::initialize_cache(STATE) {
  }

  /* Zero out the caches. */
  void MethodContext::reset_cache(STATE) {
    state->om->clamp_contexts();
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
  MethodContext* MethodContext::create(STATE, Object* recv, CompiledMethod* meth) {
    size_t stack_size = meth->backend_method_->stack_size;

    MethodContext* ctx = state->om->allocate_context(stack_size);
    if(likely(ctx)) {
      ctx->klass_ = (Class*)Qnil;
      ctx->obj_type = MethodContextType;

      ctx->self_ = recv;
      ctx->cm_ = meth;
      ctx->home_ = ctx;

    } else {

      size_t bytes = add_stack(sizeof(MethodContext), stack_size);
      ctx = (MethodContext*)state->new_struct(G(methctx), bytes);

      ctx->self(state, recv);
      ctx->cm(state, meth);
      ctx->home(state, ctx);
    }

    init_context(state, ctx, stack_size);

    ctx->vmm = meth->backend_method_;

    // nil out just where the locals are
    native_int locals = ctx->vmm->number_of_locals;

    ctx->position_stack(locals - 1);

    return ctx;
  }

  /* Called as the context_dup primitive
   */
  MethodContext* MethodContext::dup(STATE) {
    MethodContext* ctx = create(state, this->stack_size);

    /* This ctx is escaping into Ruby-land */
    ctx->reference(state);

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

  /* Lazy initialize fields that might have been left uninitialized
   * when +this+ was only used in the context cache. */
  void MethodContext::initialize_as_reference(STATE) {
    switch(obj_type) {
    case MethodContext::type:
      this->klass(state, G(methctx));
      break;
    case BlockContext::type:
      this->klass(state, G(blokctx));
      break;
    case NativeMethodContext::type:
      break;
    default:
      abort();
    }
  }

  /* Called when a context is referenced. Typically, this is via the push_context
   * opcode or MethodContext#sender. */
  void MethodContext::reference(STATE) {
    state->om->reference_context(this);
    initialize_as_reference(state);
  }

  /* Retrieve a field within the context, referenced by name. This
   * is used as a primitive. */
  Object* MethodContext::get_internal_data(STATE, Fixnum* type) {
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
    ctx->self(state, Qnil);
    ctx->module(state, (Module*)Qnil);
    ctx->name(state, Qnil);
    return ctx;
  }

  void MethodContext::Info::mark(Object* obj, ObjectMark& mark) {
    MethodContext* ctx = as<MethodContext>(obj);

    // Detect a context on the special context stack and fix it up.
    if(ctx->klass_->nil_p()) {
      ctx->initialize_as_reference(state);
    }

    // MethodContext's need to be inspected on every GC collection, young
    // and old. This is because we perform stores into the MethodContext's
    // stack without running the write barrier. So if the context is
    // in mature, we remember it.
    if(!ctx->young_object_p()) {
      mark.gc->object_memory->remember_object(ctx);
    }

    auto_mark(obj, mark);

    if(ctx->obj_type == MethodContextType) {
      assert(ctx->home() == obj);
    }

    /* Now also mark the stack */
    for(size_t i = 0; i < ctx->stack_size; i++) {
      Object* stack_obj = ctx->stack_at(i);
      if(!stack_obj) continue;
      Object* marked = mark.call(stack_obj);
      if(marked) {
        ctx->stack_put(i, marked);
        mark.just_set(ctx, marked);
      }
    }
  }

  void MethodContext::Info::show(STATE, Object* self, int level) {
    MethodContext* ctx = as<MethodContext>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); ctx->name()->show(state, level);
    indent_attribute(level, "sp");
    std::cout << ctx->calculate_sp() << "\n";

    indent_attribute(level, "ip");
    std::cout << ctx->ip << "\n";

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
