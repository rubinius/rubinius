#include "vm.hpp"
#include "fiber_stack.hpp"
#include "fiber_data.hpp"

#include "configuration.hpp"

#include "bug.hpp"

#include "gc/gc.hpp"

#include <stdlib.h>
#ifdef HAVE_VALGRIND_H
#include <valgrind/valgrind.h>
#endif

namespace rubinius {

  FiberStack::FiberStack(size_t size)
    : address_(0)
    , size_(size)
    , refs_(0)
    , user_(0)
#ifdef HAVE_VALGRIND_H
    , valgrind_id_(0)
#endif
  {}

  void FiberStack::allocate() {
    assert(!address_);
    address_ = malloc(size_);
#ifdef HAVE_VALGRIND_H
    valgrind_id_ = VALGRIND_STACK_REGISTER(address_, (char *)address_ + size_);
#endif
  }

  void FiberStack::free() {
    if(!address_) return;
#ifdef HAVE_VALGRIND_H
    VALGRIND_STACK_DEREGISTER(valgrind_id_);
#endif
    ::free(address_);
    address_ = 0;
  }

  void FiberStack::flush(STATE) {
    if(!user_) return;

    // TODO assumes higher to lower stack growth.
    user_->copy_to_heap(state);
  }

  void FiberStack::orphan(STATE, FiberData* user) {
    if(user == user_) {
      user_ = 0;
    }

    dec_ref();
  }

  FiberStacks::FiberStacks(SharedState& shared)
    : max_stacks_(shared.config.fiber_stacks)
    , stack_size_(shared.config.fiber_stack_size)
    , trampoline_(0)
  {}

  FiberStacks::~FiberStacks() {
    for(Stacks::iterator i = stacks_.begin();
        i != stacks_.end();
        ++i)
    {
      i->free();
    }
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

    if(stacks_.size() < max_stacks_) {
      stacks_.push_back(FiberStack(stack_size_));
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

  void* FiberStacks::trampoline() {
    if(trampoline_ == 0) {
      trampoline_ = malloc(cTrampolineSize);
    }

    return trampoline_;
  }
}
