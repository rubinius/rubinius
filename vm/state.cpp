#include "vm.hpp"
#include "builtin/exception.hpp"
#include "builtin/location.hpp"
#include "builtin/array.hpp"

#include "signal.hpp"
#include "call_frame.hpp"
#include "helpers.hpp"

#include "park.hpp"

namespace rubinius {
  void State::raise_stack_error(CallFrame* call_frame) {
    Class* stack_error = globals().stack_error.get();
    Exception* exc = new_object<Exception>(stack_error);
    exc->locations(this, Location::from_call_stack(this, call_frame));
    vm_->thread_state()->raise_exception(exc);
  }

  bool State::process_async(CallFrame* call_frame) {
    vm_->check_local_interrupts = false;

    if(vm_->run_signals_) {
      if(!vm_->shared.signal_handler()->deliver_signals(this, call_frame)) {
        return false;
      }
    }

    Exception* exc = vm_->interrupted_exception_.get();
    if(!exc->nil_p()) {
      vm_->interrupted_exception_.set(nil<Exception>());

      // Only write the locations if there are none.
      if(exc->locations()->nil_p() || exc->locations()->size() == 0) {
        exc->locations(this, Location::from_call_stack(this, call_frame));
      }

      vm_->thread_state_.raise_exception(exc);
      return false;
    }

    return true;
  }

  void State::check_exception(CallFrame* call_frame) {
    if(vm_->thread_state()->raise_reason() == cNone) {
      std::cout << "Exception propagating, but none registered!\n";
      call_frame->print_backtrace(this);
      rubinius::abort();
    }
  }

  bool State::check_interrupts(GCToken gct, CallFrame* call_frame, void* end) {
    // First, we might be here because someone reset the stack_limit_ so that
    // we'd fall into here to check interrupts even if the stack is fine,
    //
    // So fix up the stack_limit_ if thats the case first.

    // If this is true, stack_limit_ was just changed to get our attention, reset
    // it now.
    if(vm_->stack_limit_ == vm_->stack_start_) {
      vm_->reset_stack_limit();
    } else {
      if(!check_stack(call_frame, end)) return false;
    }

    if(unlikely(vm_->check_local_interrupts)) {
      if(!process_async(call_frame)) return false;
    }

    // If the current thread is trying to step, debugger wise, then assist!
    if(vm_->thread_step()) {
      vm_->clear_thread_step();
      if(!Helpers::yield_debugger(this, gct, call_frame, cNil)) return false;
    }

    return true;
  }


  Object* State::park(GCToken gct, CallFrame* call_frame) {
    return vm_->park_->park(this, call_frame);
  }

  Object* State::park_timed(GCToken gct, CallFrame* call_frame, struct timespec* ts) {
    return vm_->park_->park_timed(this, call_frame, ts);
  }
}
