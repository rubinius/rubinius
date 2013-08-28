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

#include "dtrace/dtrace.h"

#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif

#include "windows_compat.h"

namespace rubinius {
  static SignalHandler* signal_handler_ = 0;

  Object* signal_handler_tramp(STATE) {
    state->shared().signal_handler()->perform(state);
    return cNil;
  }

  SignalHandler::SignalHandler(STATE)
    : AuxiliaryThread()
    , shared_(state->shared())
    , target_(state->vm())
    , self_(NULL)
    , queued_signals_(0)
    , paused_(false)
    , exit_(false)
    , thread_(state)
  {
    signal_handler_ = this;

    shared_.auxiliary_threads()->register_thread(this);
    shared_.set_signal_handler(this);

    for(int i = 0; i < NSIG; i++) {
      pending_signals_[i] = 0;
    }

    worker_lock_.init();
    worker_cond_.init();
    pause_cond_.init();

    start_thread(state);
  }

  SignalHandler::~SignalHandler() {
    shared_.auxiliary_threads()->unregister_thread(this);
  }

  void SignalHandler::start_thread(STATE) {
    SYNC(state);
    if(self_) return;
    utilities::thread::Mutex::LockGuard lg(worker_lock_);
    self_ = state->shared().new_vm();
    paused_ = false;
    exit_ = false;
    thread_.set(Thread::create(state, self_, G(thread), signal_handler_tramp, false, true));
    run(state);
  }

  void SignalHandler::stop_thread(STATE) {
    SYNC(state);
    if(!self_) return;

    pthread_t os = self_->os_thread();
    {
      utilities::thread::Mutex::LockGuard lg(worker_lock_);
      // Thread might have already been stopped
      exit_ = true;
      worker_cond_.signal();
    }

    void* return_value;
    pthread_join(os, &return_value);
    self_ = NULL;
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
    start_thread(state);
  }

  void SignalHandler::before_fork(STATE) {
    utilities::thread::Mutex::LockGuard lg(worker_lock_);
    while(!paused_ && self_->run_state() == ManagedThread::eRunning) {
      pause_cond_.wait(worker_lock_);
    }
  }

  void SignalHandler::after_fork_parent(STATE) {
    utilities::thread::Mutex::LockGuard lg(worker_lock_);
    pause_cond_.signal();
  }

  void SignalHandler::after_fork_child(STATE) {
    worker_lock_.init();
    worker_cond_.init();
    pause_cond_.init();

    if(self_) {
      VM::discard(state, self_);
      self_ = NULL;
    }

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
    const char* thread_name = "rbx.signal";
    self_->set_name(thread_name);

    RUBINIUS_THREAD_START(thread_name, state->vm()->thread_id(), 1);

    state->vm()->thread->hard_unlock(state, gct, 0);

    while(!exit_) {
      {
        utilities::thread::Mutex::LockGuard lg(worker_lock_);
        if(exit_) break;
        state->gc_independent(gct, 0);
        paused_ = true;
        pause_cond_.signal();
        worker_cond_.wait(worker_lock_);
        // If we should exit now, don't try to become
        // dependent first but break and exit the thread
        if(exit_) break;
      }
      state->gc_dependent(gct, 0);
      {
        utilities::thread::Mutex::LockGuard lg(worker_lock_);
        if(exit_) break;
        paused_ = false;
      }

      target_->wakeup(state, gct, 0);
    }
    RUBINIUS_THREAD_STOP(thread_name, state->vm()->thread_id(), 1);
  }

  void SignalHandler::signal_tramp(int sig) {
    signal_handler_->handle_signal(sig);
  }

  void SignalHandler::handle_signal(int sig) {
    if(exit_) return;

    queued_signals_ = 1;
    pending_signals_[sig] = 1;

    target_->set_check_local_interrupts();

    if(target_->should_interrupt_with_signal()) {
      if(!pthread_equal(pthread_self(), target_->os_thread())) {
#ifdef RBX_WINDOWS
        // TODO: Windows
#else
        pthread_kill(target_->os_thread(), SIGVTALRM);
#endif
      }
    }

    worker_cond_.signal();
  }

  void SignalHandler::add_signal(STATE, int sig, HandlerType type) {
    SYNC(state);
    utilities::thread::Mutex::LockGuard lg(worker_lock_);

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
