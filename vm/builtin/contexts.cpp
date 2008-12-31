#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/tuple.hpp"

#include "gc_object_mark.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"

#include "builtin/contexts.hpp"
#include "builtin/nativemethodcontext.hpp"

#include <iostream>

namespace rubinius {

  void MethodContext::init(STATE) {
    GO(methctx).set(state->new_class("MethodContext", G(object)));
    G(methctx)->set_object_type(state, MethodContextType);

    GO(blokctx).set(state->new_class("BlockContext", G(methctx)));
    G(blokctx)->set_object_type(state, BlockContextType);
  }

  /* Initialize +ctx+'s fields */
  static inline void init_context(STATE, MethodContext* ctx, size_t stack) {
    ctx->StoresBytes = 1;
    ctx->ip = 0;
    ctx->native_ip = 0;
    ctx->current_unwind = 0;
    ctx->ivars(state, Qnil);

    // Do this here because it's possible for us to pass a context
    // around and it could leak out before the real sender is set.
    ctx->sender(state, (MethodContext*)Qnil);

    // Don't initialize any fields you KNOW are always set later
    // on before the ctx is used. We just waste precious time if we do.

    ctx->stack_size = stack;
    for(size_t i = 0; i < stack; i++) {
      ctx->stk[i] = Qnil;
    }

    ctx->js.stack = ctx->stk - 1;
    ctx->js.stack_top = (ctx->stk + stack - 1);
  }

  /* Find a context to use. Either use a cache or create one in the heap. */
  template <class T>
    static inline T* allocate(STATE, Class* cls, size_t stack_size) {
      T* ctx;

      ctx = reinterpret_cast<T*>(state->om->allocate_context(stack_size));
      if(ctx) {
        assert((uintptr_t)ctx + ctx->full_size < (uintptr_t)state->om->contexts.last);
        ctx->klass(state, (Class*)Qnil);
        ctx->obj_type = T::type;
      } else {
        // We're going to end up referencing a stack context from a heap
        // context, so we need to be sure all stack contexts stick around!
        state->om->clamp_contexts();
        ctx = state->new_struct<T>(cls, stack_size * sizeof(Object*));
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
    this->js.stack_top = this->stk + (this->stack_size - 1);
    this->position_stack(old->calculate_sp());
    if(this->obj_type == MethodContextType) {
      assert(this->home() == old->home());
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
    return allocate<MethodContext>(state, G(methctx), stack_size);
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
      ctx->locals_home_ = ctx;

    } else {
      // We're going to end up referencing a stack context from a heap
      // context, so we need to be sure all stack contexts stick around!
      state->om->clamp_contexts();
      ctx = state->new_struct<MethodContext>(G(methctx), stack_size * sizeof(Object*));

      ctx->self(state, recv);
      ctx->cm(state, meth);
      ctx->home(state, ctx);
      ctx->locals_home(state, ctx);
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

    ctx->home(state, this->home());
    this->home()->reference(state); // so that the closure won't be deallocated.

    ctx->locals_home(state, this->locals_home());
    this->locals_home()->reference(state);

    /* Set the obj_type because we get called
     * for both BlockContext and MethodContext
     */
    ctx->obj_type = this->obj_type;

    ctx->vmm = this->vmm;
    ctx->js = this->js;
    ctx->ip = this->ip;
    ctx->args = this->args;
    ctx->stack_size = this->stack_size;

    ctx->full_size = this->full_size;
    ctx->current_unwind = this->current_unwind;

    for(int i = 0; i < this->current_unwind; i++) {
      ctx->unwinds[i] = this->unwinds[i];
    }

    for(size_t i = 0; i < this->stack_size; i++) {
      ctx->stk[i] = this->stk[i];
    }

    ctx->js.stack_top = ctx->stk + (ctx->stack_size - 1);
    ctx->position_stack(this->calculate_sp());

    /* Stack Management procedures. Make sure that we don't
     * miss object stored into the stack of a context
     */
    if(ctx->zone == MatureObjectZone) {
      state->om->remember_object(ctx);
    }

    return ctx;
  }

  MethodContext* MethodContext::dup_chain(STATE) {

    LookupTable* map = LookupTable::create(state);

    MethodContext* ret = this->dup(state);

    for(MethodContext* ctx = ret; !ctx->sender()->nil_p(); ctx = ctx->sender()) {
      MethodContext* old_sender = ctx->sender();
      ctx->sender(state, old_sender->dup(state));
      if(old_sender->obj_type == MethodContextType) {
        map->store(state, old_sender, ctx->sender());
      }
    }

    for(MethodContext* ctx = ret; !ctx->sender()->nil_p(); ctx = ctx->sender()) {
      if(ctx->obj_type == BlockContextType) {
        BlockEnvironment* old_env = as<BlockContext>(ctx)->env();
        Object* new_env_home = map->aref(state, (old_env->home()));
        if(new_env_home->nil_p()) continue;
        BlockEnvironment* new_env = old_env->dup(state);
        new_env->home(state, as<MethodContext>(new_env_home));
        ctx->block(state, new_env);
      }
    }

    for(MethodContext* ctx = this; !ctx->sender()->nil_p(); ctx = ctx->sender()) {
      if(ctx->obj_type == MethodContextType) {
        map->remove(state, ctx);
      }
    }

    return ret;
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
    case 2:
      return Fixnum::from(calculate_sp());
    case 3:
      if(native_ip == 0) {
        return Qnil;
      } else {
        return Integer::from(state, reinterpret_cast<uintptr_t>(native_ip));
      }
    }

    sassert(0 && "invalid index");
    return Qnil;
  }

  Array* MethodContext::locals(STATE) {
    int n = this->vmm->number_of_locals;
    Array* ary = Array::create(state, n);
    for(int i = 0; i < n; i++) {
      ary->set(state, i, this->get_local(i));
    }
    return ary;
  }

  /* Return a new +BlockContext+ object, which needs +stack_size+ fields
   * worth of stack. */
  BlockContext* BlockContext::create(STATE, size_t stack_size) {
    BlockContext* ctx = allocate<BlockContext>(state, G(blokctx), stack_size);

    ctx->block(state, Qnil);
    ctx->cm(state, reinterpret_cast<CompiledMethod*>(Qnil));
    ctx->home(state, reinterpret_cast<MethodContext*>(Qnil));
    ctx->locals_home(state, reinterpret_cast<MethodContext*>(Qnil));
    ctx->module(state, reinterpret_cast<Module*>(Qnil));
    ctx->name(state, Qnil);
    ctx->self(state, Qnil);
    ctx->sender(state, reinterpret_cast<MethodContext*>(Qnil));

    return ctx;
  }

  void MethodContext::Info::mark(Object* obj, ObjectMark& mark) {
    MethodContext* ctx = as<MethodContext>(obj);

    // Detect a context on the special context stack and fix it up.
    if(ctx->klass_->nil_p()) {
      ctx->initialize_as_reference(state());
    }

    // FIXME this is to help detect an ellusive bug
    if(!ctx->sender()->nil_p() &&
        (ctx->sender()->zone == UnspecifiedZone ||
         !ctx->sender()->StoresBytes)) {
      MethodContext* s = ctx->sender();
      std::cout << "Corrupt context detected!\n";
      std::cout << "origin = " << obj << "\n";
      std::cout << "object = " << s << "\n";
      std::cout << "full_size = " << s->full_size << "\n";
      std::cout << "klass = " << s->klass_ << "\n";

      ObjectPosition pos = mark.gc->object_memory->validate_object(ctx->sender());
      if(pos == cContextStack) {
        Assertion::raise("A sender on the context stack has an UnspecifiedZone");
      } else if(pos == cUnknown){
        Assertion::raise("A sender in unknown memory has an UnspecifiedZone");
      } else {
        Assertion::raise("A sender in normal heap has an UnspecifiedZone");
      }
    }

    // MethodContext's need to be inspected on every GC collection, young
    // and old. This is because we perform stores into the MethodContext's
    // stack without running the write barrier. So if the context is
    // in mature, we remember it.
    if(!ctx->young_object_p()) {
      mark.gc->object_memory->remember_object(ctx);
    }

    auto_mark(obj, mark);

    /* Now also mark the stack. Set elements after sp to nil to prevent
     * memory leak */
    int sp = ctx->calculate_sp();
    if(sp < 0) {
      for(size_t i = 0; i < ctx->stack_size; i++) {
        ctx->stack_put(i, Qnil);
      }
    } else {
      size_t usp = sp;
      for(size_t i = 0; i <= usp; i++) {
        Object* stack_obj = ctx->stack_at(i);
        if(!stack_obj) continue;
        Object* marked = mark.call(stack_obj);
        if(marked) {
          ctx->stack_put(i, marked);
          mark.just_set(ctx, marked);
        }
      }
      for(size_t i = usp+1; i < ctx->stack_size; i++) {
        ctx->stack_put(i, Qnil);
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
      if(ctx->sender()->klass()->nil_p()) {
        std::cout << "<stack context:" << (void*)ctx->sender() << ">\n";
      } else {
        class_info(state, ctx->sender(), true);
      }
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
