#include "vm/vm.hpp"
#include "vm/fiber_data.hpp"

namespace rubinius {
  void FiberData::take_stack(STATE) {
    assert(stack_);

    assert(status_ != eDead);

    if(status_ == eOnStack || status_ == eRunning) return;

    if(stack_->shared_p()) stack_->flush(state);
    stack_->set_user(this);

    if(status_ == eOnHeap) {
      memcpy(stack_bottom(), heap_, heap_size_);
    }

    status_ = eOnStack;
  }

  void FiberData::copy_to_heap(STATE) {
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
    stack_->orphan(state, this);
    stack_ = 0;
  }
}
