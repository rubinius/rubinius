#include "vm.hpp"
#include "fiber_stack.hpp"
#include "fiber_data.hpp"

#include "bug.hpp"

#include <stdlib.h>

namespace rubinius {

  FiberStack::FiberStack(size_t size)
    : address_(0)
    , size_(size)
    , refs_(0)
    , user_(0)
  {}

  void FiberStack::allocate() {
    assert(!address_);
    address_ = malloc(size_);
  }

  void FiberStack::free() {
    if(!address_) return;
    ::free(address_);
    address_ = 0;
  }

  void FiberStack::flush(STATE) {
    if(!user_) return;

    // TODO assumes higher to lower stack growth.
    user_->copy_to_heap(state);
  }

  void FiberStack::orphan(STATE, FiberData* user) {
    assert(user_ == user);
    dec_ref();
  }

  FiberStack* FiberStacks::allocate() {
    for(Stacks::iterator i = stacks_.begin();
        i != stacks_.end();
        ++i)
    {
      if(i->unused_p()) {
        i->inc_ref();
        return &*i;
      }
    }

    FiberStack* stack = 0;

    if(stacks_.size() < cMaxStacks) {
      stacks_.push_back(FiberStack(cStackSize));
      stack = &stacks_.back();

      stack->allocate();
    } else {
      for(Stacks::iterator i = stacks_.begin();
          i != stacks_.end();
          ++i)
      {
        if(!stack || i->refs() < stack->refs()) {
          stack = &*i;
        }
      }

      assert(stack);
    }

    stack->inc_ref();

    return stack;
  }
}
