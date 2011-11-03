/* This MUST all be at the top */
#include "builtin/fiber.hpp"
/* end must be at the top */


#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "vm/vm.hpp"
#include "builtin/class.hpp"
#include "builtin/integer.hpp"
#include "builtin/exception.hpp"

#include "objectmemory.hpp"
#include "gc/gc.hpp"

#include "call_frame.hpp"
#include "arguments.hpp"

#include "ontology.hpp"

#include "on_stack.hpp"

#ifdef FIBER_NATIVE

#if defined(FIBER_ASM_X8664)

struct fiber_context_t {
  void* rip;
  void* rsp;
  void* rbp;
  void* rbx;
  void* r12;
  void* r13;
  void* r14;
  void* r15;
};

static void fiber_wrap_main(void) {
  __asm__ __volatile__ ("\tmovq %r13, %rdi\n\tjmpq *%r12\n");
}

static inline void fiber_switch(fiber_context_t* from, fiber_context_t* to) {
  __asm__ __volatile__ (
    "leaq 1f(%%rip), %%rax\n\t"
    "movq %%rax, (%0)\n\t" "movq %%rsp, 8(%0)\n\t" "movq %%rbp, 16(%0)\n\t"
    "movq %%rbx, 24(%0)\n\t" "movq %%r12, 32(%0)\n\t" "movq %%r13, 40(%0)\n\t"
    "movq %%r14, 48(%0)\n\t" "movq %%r15, 56(%0)\n\t"
    "movq 56(%1), %%r15\n\t" "movq 48(%1), %%r14\n\t" "movq 40(%1), %%r13\n\t"
    "movq 32(%1), %%r12\n\t" "movq 24(%1), %%rbx\n\t" "movq 16(%1), %%rbp\n\t"
    "movq 8(%1), %%rsp\n\t" "jmpq *(%1)\n" "1:\n"
    : "+S" (from), "+D" (to) :
    : "rax", "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", "cc");
}

static void fiber_makectx(fiber_context_t* ctx, void* func, void** stack_bottom,
                          int stack_size)
{
  // Get a pointer to the highest address as stack that is properly aligned
  // with room for the fake return value.
  uintptr_t s = ((uintptr_t)stack_bottom) + stack_size;
  uintptr_t diff = s % 16;

  void** stack = (void**)(s - diff) - 1;

  *--stack = (void*)0xdeadcafedeadcafe;  /* Dummy return address. */
  ctx->rip = (void*)fiber_wrap_main;
  ctx->rsp = stack;
  ctx->rbp = 0;
  ctx->rbx = 0;
  ctx->r12 = func;
  ctx->r13 = 0;
  ctx->r14 = 0;
  ctx->r15 = 0;

}

#elif defined(FIBER_ASM_X8632)

struct fiber_context_t {
  void* eip;
  void* esp;
  void* ebp;
  void* ebx;
};

static inline void fiber_switch(fiber_context_t* from, fiber_context_t* to) {
  __asm__ __volatile__ (
    "call 1f\n" "1:\tpopl %%eax\n\t" "addl $(2f-1b),%%eax\n\t"
    "movl %%eax, (%0)\n\t" "movl %%esp, 4(%0)\n\t"
    "movl %%ebp, 8(%0)\n\t" "movl %%ebx, 12(%0)\n\t"
    "movl 12(%1), %%ebx\n\t" "movl 8(%1), %%ebp\n\t"
    "movl 4(%1), %%esp\n\t" "jmp *(%1)\n" "2:\n"
    : "+S" (from), "+D" (to) : : "eax", "ecx", "edx", "memory", "cc");
}

static void fiber_makectx(fiber_context_t* ctx, void* func, void** stack_bottom,
                          int stack_size)
{
  // Get a pointer to the highest address as stack that is properly aligned
  // with room for the fake return value.
  uintptr_t s = ((uintptr_t)stack_bottom) + stack_size;
  uintptr_t diff = s % 16;

  void** stack = (void**)(s - diff) - 1;

  *--stack = (void*)0xdeadcafe;

  ctx->eip = func;
  ctx->esp = stack;
  ctx->ebp = 0;
}
#endif

#else // FIBER_NATIVE

#ifndef FIBER_ENABLED
struct fiber_context_t {
  int dummy;
}
#endif

#endif

namespace rubinius {

  void Fiber::init(STATE) {
    GO(fiber).set(ontology::new_class(state, "Fiber", G(object), G(rubinius)));
    G(fiber)->set_object_type(state, FiberType);

#ifdef FIBER_ENABLED
    G(fiber)->set_const(state, "ENABLED", Qtrue);
#else
    G(fiber)->set_const(state, "ENABLED", Qfalse);
#endif
  }

  Fiber* Fiber::current(STATE) {
#ifdef FIBER_ENABLED
    Fiber* fib = state->vm()->current_fiber.get();

    // Lazily allocate a root fiber.
    if(fib->nil_p()) {
      fib = state->new_object<Fiber>(G(fiber));
      if(fib->zone() != YoungObjectZone) {
        state->memory()->remember_object(fib);
      }
      fib->prev_ = nil<Fiber>();
      fib->top_ = 0;
      fib->root_ = true;
      fib->status_ = Fiber::eRunning;
      fib->vm_ = state->vm();
      fib->stack_size_ = state->vm()->stack_size();
      fib->stack_ = state->vm()->stack_start();

#ifdef FIBER_NATIVE
      fib->context_ = new fiber_context_t;
#else
      fib->context_ = new ucontext_t;
#endif

      state->memory()->needs_finalization(fib, (FinalizerFunction)&Fiber::finalize);

      state->vm()->current_fiber.set(fib);
      state->vm()->root_fiber.set(fib);
    }

    return fib;
#else
    return nil<Fiber>();
#endif
  }

  void Fiber::start_on_stack() {
#ifdef FIBER_ENABLED
    VM* vm = VM::current();
    State state(vm);

    Fiber* fib = Fiber::current(&state);

    OnStack<1> os(&state, fib);

    // Affix this fiber to this thread now.
    fib->vm_ = vm;

    Array* result = nil<Array>();
    Object* obj = fib->starter()->send(&state, NULL, state.globals().sym_call.get(), fib->value(), Qnil, false);
    // GC has run! Don't use stack vars!

    fib = Fiber::current(&state);
    fib->top_ = 0;
    fib->status_ = Fiber::eDead;
    fib->set_ivar(&state, state.symbol("@dead"), Qtrue);

    Fiber* dest = fib->prev();
    assert(!dest->nil_p());

    // Box this up so it's in a standard format at the point
    // of returning, so we can deal with it in the same way
    // as *args from #yield, #resume, and #transfer
    if(obj) {
      result = Array::create(&state, 1);
      result->set(&state, 0, obj);
    } else {
      if(state.vm()->thread_state()->raise_reason() == cException) {
        dest->exception(&state, state.vm()->thread_state()->current_exception());
      }
    }

    dest->run();
    dest->value(&state, result);
    state.vm()->set_current_fiber(dest);

#ifdef FIBER_NATIVE
    fiber_context_t dummy;
    fiber_switch(&dummy, dest->ucontext());
#else
    if(setcontext(dest->ucontext()) != 0)
      assert(0 && "fatal swapcontext() error");
#endif

    assert(0 && "fatal start_on_stack error");
#else
    rubinius::bug("Fibers not supported on this platform");
#endif
  }

  Fiber* Fiber::create(STATE, Integer* i_stack_size, Object* callable) {
#ifdef FIBER_ENABLED
    int stack_size = i_stack_size->to_native();

    if(stack_size < 1024 * 1024) {
      stack_size = 1024 * 1024;
    }

    Fiber* fib = state->new_object<Fiber>(G(fiber));
    if(fib->zone() != YoungObjectZone) {
      state->memory()->remember_object(fib);
    }
    fib->starter(state, callable);
    fib->prev(state, nil<Fiber>());
    fib->top_ = 0;
    fib->root_ = false;
    fib->vm_ = 0;
    fib->status_ = Fiber::eSleeping;
    fib->stack_size_ = stack_size;
    fib->stack_ = malloc(stack_size);

    state->memory()->needs_finalization(fib, (FinalizerFunction)&Fiber::finalize);

#ifdef FIBER_NATIVE
    fib->context_ = new fiber_context_t;
    fiber_makectx(fib->ucontext(), (void*)start_on_stack, (void**)fib->stack_,
                  stack_size);
#else
    fib->context_ = new ucontext_t;
    ucontext_t* ctx = fib->ucontext();

    if(getcontext(ctx) != 0) assert(0 && "fatal getcontext() error");

    ctx->uc_link = 0;
    ctx->uc_stack.ss_sp = (char *) fib->stack_;
    ctx->uc_stack.ss_size = stack_size;
    ctx->uc_stack.ss_flags = 0;

    makecontext(ctx, start_on_stack, 0);
#endif

    return fib;
#else
    return reinterpret_cast<Fiber*>(Primitives::failure());
#endif
  }

  Object* Fiber::resume(STATE, Arguments& args, CallFrame* calling_environment) {
#ifdef FIBER_ENABLED
    if(status_ == Fiber::eDead) {
      Exception::fiber_error(state, "dead fiber called");
    }

    if(!prev_->nil_p()) {
      Exception::fiber_error(state, "double resume");
    }

    Array* val = args.as_array(state);
    value(state, val);

    Fiber* cur = Fiber::current(state);
    prev(state, cur);

    cur->sleep(calling_environment);

    run();
    state->vm()->set_current_fiber(this);

#ifdef FIBER_NATIVE
    fiber_switch(cur->ucontext(), context_);
#else
    if(swapcontext(cur->ucontext(), context_) != 0)
      assert(0 && "fatal swapcontext() error");
#endif

    // Back here when someone yields back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);

    if(!cur->exception()->nil_p()) {
      state->raise_exception(cur->exception());
      cur->exception(state, nil<Exception>());
      return 0;
    }

    Array* ret = cur->value();

    if(ret->nil_p()) return Qnil;

    switch(ret->size()) {
    case 0:  return Qnil;
    case 1:  return ret->get(state, 0);
    default: return ret;
    }
#else
    return Primitives::failure();
#endif
  }

  Object* Fiber::transfer(STATE, Arguments& args, CallFrame* calling_environment) {
#ifdef FIBER_ENABLED
    if(status_ == Fiber::eDead) {
      Exception::fiber_error(state, "dead fiber called");
    }

    Array* val = args.as_array(state);
    value(state, val);

    Fiber* cur = Fiber::current(state);
    Fiber* root = state->vm()->root_fiber.get();
    assert(root);

    prev(state, root);

    cur->sleep(calling_environment);

    run();
    state->vm()->set_current_fiber(this);

#ifdef FIBER_NATIVE
    fiber_switch(cur->ucontext(), context_);
#else
    if(swapcontext(cur->ucontext(), context_) != 0)
      assert(0 && "fatal swapcontext() error");
#endif

    // Back here when someone transfers back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);

    if(!cur->exception()->nil_p()) {
      state->raise_exception(cur->exception());
      cur->exception(state, nil<Exception>());
      return 0;
    }

    Array* ret = cur->value();

    if(ret->nil_p()) return Qnil;

    switch(ret->size()) {
    case 0:  return Qnil;
    case 1:  return ret->get(state, 0);
    default: return ret;
    }
#else
    return Primitives::failure();
#endif
  }

  Object* Fiber::s_yield(STATE, Arguments& args, CallFrame* calling_environment) {
#ifdef FIBER_ENABLED
    Fiber* cur = Fiber::current(state);
    Fiber* dest_fib = cur->prev();

    assert(cur != dest_fib);

    if(cur->root_) {
      Exception::fiber_error(state, "can't yield from root fiber");
    }

    cur->prev(state, nil<Fiber>());

    Array* val = args.as_array(state);
    dest_fib->value(state, val);

    cur->sleep(calling_environment);

    dest_fib->run();
    state->vm()->set_current_fiber(dest_fib);

#ifdef FIBER_NATIVE
    fiber_switch(cur->ucontext(), dest_fib->ucontext());
#else
    if(swapcontext(cur->ucontext(), dest_fib->ucontext()) != 0)
      assert(0 && "fatal swapcontext() error");
#endif

    // Back here when someone yields back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);

    Array* ret = cur->value();

    if(ret->nil_p()) return Qnil;

    switch(ret->size()) {
    case 0:  return Qnil;
    case 1:  return ret->get(state, 0);
    default: return ret;
    }
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

    mark.remember_object(obj);

    Fiber* fib = (Fiber*)obj;
    if(CallFrame* cf = fib->call_frame()) {
      mark.gc->walk_call_frame(cf);
    }
  }
}

