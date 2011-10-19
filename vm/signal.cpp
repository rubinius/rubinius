#include "config.h"
#include "signal.hpp"
#include "vm.hpp"

#include "builtin/module.hpp"
#include "builtin/array.hpp"

#include "builtin/array.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"

#include <iostream>
#include <fcntl.h>

#include "builtin/thread.hpp"

#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif

#include "windows_compat.h"

namespace rubinius {
  struct CallFrame;

  static SignalHandler* handler_ = 0;
  pthread_t main_thread;

  Object* handle_tramp(STATE) {
    handler_->perform(state);
    return Qnil;
  }

  SignalHandler::SignalHandler(VM* vm)
    : target_(vm)
    , queued_signals_(0)
    , exit_(false)
    , thread_(vm)
  {
    handler_ = this;
    main_thread = pthread_self();

    for(int i = 0; i < NSIG; i++) {
      pending_signals_[i] = 0;
    }

    reopen_pipes();

    self_ = vm->shared.new_vm();

    thread_.set(Thread::create(vm, self_, vm->shared.globals.thread.get(),
                               handle_tramp, false));
  }

  void SignalHandler::reopen_pipes() {
    int f[2];
    if(pipe(f) < 0) {
      perror("SignalHandler::reopen_pipes failed");
    }
    read_fd_ = f[0];
    write_fd_ = f[1];
  }

  void SignalHandler::on_fork(STATE, bool full) {
    if(handler_) handler_->on_fork_i(state, full);
  }

  void SignalHandler::on_fork_i(STATE, bool full) {
    exit_ = false;
    reopen_pipes();

    if(full && self_) rubinius::bug("signal thread restart issue");

    self_ = state->shared.new_vm();
    thread_.set(Thread::create(state, self_, state->shared.globals.thread.get(),
                               handle_tramp, false));

    run(state);
  }

  void SignalHandler::shutdown() {
    if(handler_) handler_->shutdown_i();
  }

  void SignalHandler::shutdown_i() {
    pthread_t os = self_->os_thread();

    exit_ = true;
    if(write(write_fd_, "!", 1) < 0) {
      perror("SignalHandler::shutdown_i failed to write");
    }

    void* blah;
    pthread_join(os, &blah);
  }

  void SignalHandler::run(STATE) {
    int error = thread_.get()->fork_attached(state);
    if(error) rubinius::bug("Unable to start signal handler thread");
  }

  void SignalHandler::perform(STATE) {
#ifndef RBX_WINDOWS
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif

    thread::Thread::set_os_name("rbx.signal-dispatch");

    for(;;) {
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET((int_fd_t)read_fd_, &fds);

      int n;

      {
        GCIndependent indy(state, 0);
        n = select(read_fd_ + 1, &fds, NULL, NULL, NULL);
      }

      if(n == 1) {
        // drain a bunch
        char buf[512];
        if(read(read_fd_, buf, sizeof(buf)) < 0) {
          perror("SignalHandler::perform failed to read");
        }

        if(exit_) {
          close(write_fd_);
          close(read_fd_);
          self_ = 0;
          return;
        }

        {
          target_->check_local_interrupts = true;
          target_->get_attention();
          target_->wakeup(state);

        }
      }
    }
  }

  void SignalHandler::signal_tramp(int sig) {
    handler_->handle_signal(sig);
  }

  void SignalHandler::handle_signal(int sig) {
    queued_signals_ = 1;
    pending_signals_[sig] = 1;

    // If the main thread is running, just tell it that
    // there are local interrupts waiting.
    if(!target_->waiting_p()) {
      target_->check_local_interrupts = true;
      return;
    }

    if(target_->should_interrupt_with_signal()) {
      target_->check_local_interrupts = true;

      if(!pthread_equal(pthread_self(), main_thread)) {
#ifdef RBX_WINDOWS
        // TODO: Windows
#else
        pthread_kill(main_thread, SIGVTALRM);
#endif
      }
      return;
    }

    if(write(write_fd_, "!", 1) < 0) {
      perror("SignalHandler::handle_signal failed to write");
    }
  }

  void SignalHandler::add_signal(STATE, int sig, HandlerType type) {
    SYNC(state);

#ifndef RBX_WINDOWS
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, sig);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);

    struct sigaction action;

    if(type == eDefault) {
      action.sa_handler = SIG_DFL;
    } else if(type == eIgnore) {
      action.sa_handler = SIG_IGN;
    } else {
      action.sa_handler = signal_tramp;
    }

    action.sa_flags = 0;
    sigfillset(&action.sa_mask);

    sigaction(sig, &action, NULL);
#endif
  }

  bool SignalHandler::deliver_signals(STATE, CallFrame* call_frame) {
    queued_signals_ = 0;

    for(int i = 0; i < NSIG; i++) {
      if(pending_signals_[i] > 0) {
        pending_signals_[i] = 0;

        Array* args = Array::create(state, 1);
        args->set(state, 0, Fixnum::from(i));

        // Check whether the send raised an exception and
        // stop running the handlers if that happens
        if(!G(rubinius)->send(state, call_frame,
               state->symbol("received_signal"), args, Qnil)) {
          if(state->thread_state()->raise_reason() == cException ||
             state->thread_state()->raise_reason() == cExit) {
            return false;
          }
        }
      }
    }

    return true;
  }
}
