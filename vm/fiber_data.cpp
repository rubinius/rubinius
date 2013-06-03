#include <string.h>

#include "vm.hpp"
#include "fiber_data.hpp"
#include "builtin/fiber.hpp"

namespace rubinius {

#ifdef RBX_FIBER_ENABLED

#if defined(FIBER_ASM_X8664)

static void fiber_wrap_main(void) {
  __asm__ __volatile__ ("\tmovq %r13, %rdi\n\tjmpq *%r12\n");
}

static inline void fiber_switch(fiber_context_t* from, fiber_context_t* to) {
  __asm__ __volatile__ (
    "leaq 1f(%%rip), %%rax\n\t"
    "movq %%rax, (%0)\n\t"
    "movq %%rsp, 8(%0)\n\t"
    "movq %%rbp, 16(%0)\n\t"
    "movq %%rbx, 24(%0)\n\t"
    "movq %%r12, 32(%0)\n\t"
    "movq %%r13, 40(%0)\n\t"
    "movq %%r14, 48(%0)\n\t"
    "movq %%r15, 56(%0)\n\t"
    "movq 56(%1), %%r15\n\t"
    "movq 48(%1), %%r14\n\t"
    "movq 40(%1), %%r13\n\t"
    "movq 32(%1), %%r12\n\t"
    "movq 24(%1), %%rbx\n\t"
    "movq 16(%1), %%rbp\n\t"
    "movq 8(%1), %%rsp\n\t"
    "jmpq *(%1)\n" "1:\n"

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

#else

static inline void fiber_switch(fiber_context_t* from, fiber_context_t* to) {
  rubinius::bug("Fibers not supported on this platform");
}

static void fiber_makectx(fiber_context_t* ctx, void* func, void** stack_bottom,
                          int stack_size)
{
  rubinius::bug("Fibers not supported on this platform");
}

#endif

}

namespace rubinius {

  FiberData::~FiberData() {
    if(heap_) free(heap_);
    if(!thread_) return;
    thread_->remove_fiber_data(this);
  }

  void FiberData::take_stack(STATE) {
    assert(status_ != eDead);

    if(status_ == eOnStack || status_ == eRunning) return;

    assert(stack_);

    if(stack_->shared_p()) stack_->flush(state);
    stack_->set_user(this);

    if(status_ == eOnHeap) {
      memcpy(stack_bottom(), heap_, heap_size_);
    }

    status_ = eOnStack;
  }

  void FiberData::copy_to_heap(STATE) {
    assert(status_ != eDead);
    assert(stack_);

    heap_size_ = (uintptr_t)stack_->top_address() - (uintptr_t)stack_bottom();
    if(heap_capacity_ < heap_size_) {
      // Round to nearest 1k
      heap_capacity_ = (heap_size_ + 1023) & ~1023;

      if(heap_) free(heap_);
      heap_ = malloc(heap_capacity_);
    }

    memcpy(heap_, stack_bottom(), heap_size_);

    status_ = eOnHeap;
  }

  FiberStack* FiberData::allocate_stack(STATE) {
    assert(!stack_);
    stack_ = state->vm()->allocate_fiber_stack();
    return stack_;
  }

  void FiberData::orphan(STATE) {
    status_ = eDead;
    call_frame_ = 0;

    if(!stack_) return;

    stack_->orphan(state, this);
    stack_ = 0;
  }

  static void save_and_switch() {
    VM* vm = VM::current();
    State state(vm);

    Fiber* fib = Fiber::current(&state);

    fib->data()->take_stack(&state);

    fiber_context_t tmp;
    fiber_switch(&tmp, fib->ucontext());
  }

  void FiberData::switch_to(STATE, FiberData* from) {
    if(uninitialized_p()) {
      FiberStack* stack = allocate_stack(state);

      fiber_makectx(machine(), (void*)Fiber::start_on_stack,
                    (void**)stack->address(),
                    stack->size());
    }

    if(currently_on_stack_p(from)) {
      fiber_context_t dest;

      fiber_makectx(&dest, (void*)save_and_switch,
                    (void**)state->vm()->fiber_trampoline(),
                    FiberStacks::cTrampolineSize);

      fiber_switch(from->machine(), &dest);
    } else {
      take_stack(state);
      fiber_switch(from->machine(), machine());
    }
  }

  void FiberData::switch_and_orphan(STATE, FiberData* from) {
    fiber_context_t dummy;

    if(currently_on_stack_p(from)) {
      from->orphan(state);

      fiber_context_t dest;

      fiber_makectx(&dest, (void*)save_and_switch,
                    (void**)state->vm()->fiber_trampoline(),
                    FiberStacks::cTrampolineSize);

      fiber_switch(&dummy, &dest);
    } else {
      from->orphan(state);
      take_stack(state);
      fiber_switch(&dummy, machine());
    }
  }

  void FiberData::die() {
    status_ = eDead;
    call_frame_ = 0;
    thread_ = 0;
    stack_ = 0;
  }
}
