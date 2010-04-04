/* This MUST all be at the top */
#include "builtin/fiber.hpp"
/* end must be at the top */


#include "builtin/object.hpp"
#include "vm/vm.hpp"
#include "builtin/class.hpp"
#include "builtin/integer.hpp"

#include "objectmemory.hpp"
#include "gc/gc.hpp"

#include "call_frame.hpp"
#include "arguments.hpp"

namespace rubinius {

  void Fiber::init(STATE) {
    GO(fiber).set(state->new_class("Fiber", G(object), G(rubinius)));
    G(fiber)->set_object_type(state, FiberType);
  }

  Fiber* Fiber::current(STATE) {
#ifdef FIBER_ENABLED
    Fiber* fib = state->current_fiber.get();

    // Lazily allocate a root fiber.
    if(fib->nil_p()) {
      fib = state->new_object<Fiber>(G(fiber));
      fib->prev_ = reinterpret_cast<Fiber*>(Qnil);
      fib->top_ = 0;
      fib->root_ = true;
      fib->status_ = Fiber::eRunning;
      fib->state_ = state;
      fib->stack_size_ = state->stack_size();
      fib->stack_ = state->stack_start();
      fib->context_ = new ucontext_t;

      state->om->needs_finalization(fib, (FinalizerFunction)&Fiber::finalize);

      state->current_fiber.set(fib);
    }

    return fib;
#else
    return reinterpret_cast<Fiber*>(Qnil);
#endif
  }

  void Fiber::start_on_stack() {
#ifdef FIBER_ENABLED
    VM* state = VM::current();

    Fiber* fib = Fiber::current(state);

    // Affix this fiber to this thread now.
    fib->state_ = state;

    fib->starter()->send(state, NULL, G(sym_call));
    // GC has run! Don't use stack vars!

    fib = Fiber::current(state);
    fib->status_ = Fiber::eDead;

    Fiber* dest = fib->prev();
    assert(!dest->nil_p());

    dest->run();
    dest->value(state, Qnil);
    state->set_current_fiber(dest);

    if(setcontext(dest->ucontext()) != 0)
      assert(0 && "fatal swapcontext() error");

    assert(0 && "fatal start_on_stack error");
#else
    abort();
#endif
  }

  Fiber* Fiber::create(STATE, Integer* i_stack_size, Object* callable) {
#ifdef FIBER_ENABLED
    int stack_size = i_stack_size->to_native();

    if(stack_size < 64 * 1024) {
      stack_size = 64 * 1024;
    }

    Fiber* fib = state->new_object<Fiber>(G(fiber));
    fib->starter(state, callable);
    fib->prev_ = (Fiber*)Qnil;
    fib->top_ = 0;
    fib->root_ = false;
    fib->state_ = 0;
    fib->status_ = Fiber::eSleeping;
    fib->stack_size_ = stack_size;
    fib->stack_ = malloc(stack_size);
    fib->context_ = new ucontext_t;

    state->om->needs_finalization(fib, (FinalizerFunction)&Fiber::finalize);

    ucontext_t* ctx = fib->ucontext();

    if(getcontext(ctx) != 0) assert(0 && "fatal getcontext() error");

    ctx->uc_link = 0;
    ctx->uc_stack.ss_sp = (char *) fib->stack_;
    ctx->uc_stack.ss_size = stack_size;
    ctx->uc_stack.ss_flags = 0;

    makecontext(ctx, start_on_stack, 0);

    return fib;
#else
    return reinterpret_cast<Fiber*>(Primitives::failure());
#endif
  }

  Object* Fiber::resume(STATE, Arguments& args, CallFrame* calling_environment) {
#ifdef FIBER_ENABLED
    if(!prev_->nil_p() || root_) return Primitives::failure();

    Object* val = Qnil;
    if(args.total() == 1) {
      val = args.get_argument(0);
    } else if(args.total() > 1) {
      val = args.as_array(state);
    }

    value(state, val);

    Fiber* cur = Fiber::current(state);
    prev(state, cur);

    cur->sleep(calling_environment);

    run();
    state->set_current_fiber(this);

    if(swapcontext(cur->ucontext(), context_) != 0)
      assert(0 && "fatal swapcontext() error");

    // Back here when someone yields back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);
    return cur->value();
#else
    return Primitives::failure();
#endif
  }

  Object* Fiber::s_yield(STATE, Arguments& args, CallFrame* calling_environment) {
#ifdef FIBER_ENABLED
    Fiber* cur = Fiber::current(state);
    Fiber* dest_fib = cur->prev();

    assert(cur != dest_fib);

    cur->prev(state, (Fiber*)Qnil);

    Object* val = Qnil;
    if(args.total() == 1) {
      val = args.get_argument(0);
    } else if(args.total() > 1) {
      val = args.as_array(state);
    }

    dest_fib->value(state, val);

    cur->sleep(calling_environment);

    dest_fib->run();
    state->set_current_fiber(dest_fib);

    if(swapcontext(cur->ucontext(), dest_fib->ucontext()) != 0)
      assert(0 && "fatal swapcontext() error");

    // Back here when someone yields back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);
    return cur->value();
#else
    return Primitives::failure();
#endif
  }

  void Fiber::finalize(STATE, Fiber* fib) {
#ifdef FIBER_ENABLED
    delete fib->context_;
    if(fib->stack_ && !fib->root_) free(fib->stack_);
#endif
  }

  void Fiber::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    Fiber* fib = (Fiber*)obj;
    if(CallFrame* cf = fib->call_frame()) {
      mark.gc->walk_call_frame(cf);
    }
  }
}
