#include "signal.hpp"
#include "vm.hpp"

#include "native_thread.hpp"

#include "builtin/module.hpp"
#include <iostream>
#include <sys/select.h>

namespace rubinius {
  struct CallFrame;

  static SignalHandler* handler_ = 0;

  SignalHandler::SignalHandler(VM* vm)
    : vm_(vm)
  {
    handler_ = this;
  }

  void SignalHandler::signal_tramp(int sig) {
    handler_->handle_signal(sig);
  }

  void SignalHandler::handle_signal(int sig) {
    thread::SpinLock::LockGuard guard(lock_);
    pending_signals_.push_back(sig);
    vm_->check_local_interrupts = true;
    vm_->wakeup();
  }

  void SignalHandler::add_signal(int sig, bool def) {
    thread::SpinLock::LockGuard guard(lock_);

    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, sig);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);

    struct sigaction action;

    if(def) {
      action.sa_handler = SIG_DFL;
    } else {
      action.sa_handler = signal_tramp;
    }

    action.sa_flags = 0;
    sigfillset(&action.sa_mask);

    sigaction(sig, &action, NULL);
  }

  void SignalHandler::deliver_signals(CallFrame* call_frame) {
    thread::SpinLock::LockGuard guard(lock_);
    Array* args = Array::create(vm_, 1);

    // We run all handlers, even if one handler raises an exception. The
    // rest of the signals handles will simply see the exception.
    for(std::list<int>::iterator i = pending_signals_.begin();
        i != pending_signals_.end();
        i++) {
      args->set(vm_, 0, Fixnum::from(*i));

      vm_->globals().rubinius->send(vm_, call_frame,
          vm_->symbol("received_signal"), args, Qnil);
    }

    pending_signals_.clear();
  }
}
