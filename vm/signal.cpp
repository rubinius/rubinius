#include "config.h"
#include "vm.hpp"
#include "signal.hpp"

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
  static SignalHandler* handler_ = 0;
  pthread_t main_thread;

  Object* handle_tramp(STATE) {
    handler_->perform(state);
    return cNil;
  }

  SignalHandler::SignalHandler(STATE)
    : AuxiliaryThread()
    , shared_(state->shared())
    , target_(state->vm())
    , self_(NULL)
    , queued_signals_(0)
    , exit_(false)
    , thread_(state)
  {
    handler_ = this;
    main_thread = pthread_self();

    shared_.auxiliary_threads()->register_thread(this);
    shared_.set_signal_handler(this);

    for(int i = 0; i < NSIG; i++) {
      pending_signals_[i] = 0;
    }

    open_pipes();
    start_thread(state);
  }

  SignalHandler::~SignalHandler() {
    shared_.auxiliary_threads()->unregister_thread(this);
    close(write_fd_);
    close(read_fd_);
  }

  void SignalHandler::start_thread(STATE) {
    SYNC(state);
    if(self_) return;
    self_ = state->shared().new_vm();
    thread_.set(Thread::create(state, self_, G(thread), handle_tramp, false));
    run(state);

  }

  void SignalHandler::stop_thread(STATE) {
    SYNC(state);
    if(!self_) return;

    // Thread might have already been stopped
    pthread_t os = self_->os_thread();

    exit_ = true;
    if(write(write_fd_, "!", 1) < 0) {
      perror("SignalHandler::stop_thread failed to write");
    }

    void* return_value;
    pthread_join(os, &return_value);
    self_ = NULL;
  }

  void SignalHandler::open_pipes() {
    int f[2];
    if(pipe(f) < 0) {
      perror("SignalHandler::open_pipes failed");
    }
    read_fd_ = f[0];
    write_fd_ = f[1];
  }

  void SignalHandler::shutdown(STATE) {
    for(std::list<int>::iterator i = watched_signals_.begin();
        i != watched_signals_.end();
        ++i)
    {
      signal(*i, SIG_DFL);
    }

    stop_thread(state);
  }

  void SignalHandler::before_exec(STATE) {
    stop_thread(state);
  }

  void SignalHandler::after_exec(STATE) {
    exit_ = false;
    start_thread(state);
  }

  void SignalHandler::before_fork(STATE) {
    stop_thread(state);
  }

  void SignalHandler::after_fork_parent(STATE) {
    exit_ = false;
    start_thread(state);
  }

  void SignalHandler::after_fork_child(STATE) {
    exit_ = false;
    open_pipes();
    start_thread(state);
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

    GCTokenImpl gct;
    utilities::thread::Thread::set_os_name("rbx.signal-dispatch");

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
          self_ = 0;
          return;
        }

        {
          target_->check_local_interrupts = true;
          target_->get_attention();
          target_->wakeup(state, gct);

        }
      }
    }
  }

  void SignalHandler::signal_tramp(int sig) {
    handler_->handle_signal(sig);
  }

  void SignalHandler::handle_signal(int sig) {
    if(exit_) return;

    queued_signals_ = 1;
    pending_signals_[sig] = 1;

    target_->check_local_interrupts = true;

    if(target_->should_interrupt_with_signal()) {
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
    struct sigaction action;

    if(type == eDefault) {
      action.sa_handler = SIG_DFL;
      watched_signals_.remove(sig);
    } else if(type == eIgnore) {
      action.sa_handler = SIG_IGN;
      watched_signals_.push_back(sig);
    } else {
      action.sa_handler = signal_tramp;
      watched_signals_.push_back(sig);
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
               state->symbol("received_signal"), args, cNil)) {
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
