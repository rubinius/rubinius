#include "config.h"
#include "vm.hpp"
#include "signal.hpp"
#include "configuration.hpp"

#include "builtin/module.hpp"
#include "builtin/array.hpp"

#include "builtin/array.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/thread.hpp"

#include <string>
#include <iostream>
#include <fcntl.h>

#include "util/logger.hpp"

#include "dtrace/dtrace.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

#ifdef RBX_WINDOWS
#include "windows_compat.h"
#else
#include <dlfcn.h>
#include <sys/utsname.h>
#include <sys/select.h>
#endif

namespace rubinius {
  using namespace utilities;

  static SignalHandler* signal_handler_ = 0;

  // Used by the segfault reporter. Calculated up front to avoid
  // crashing inside the crash handler.
  static struct utsname machine_info;

  Object* signal_handler_tramp(STATE) {
    state->shared().signal_handler()->perform(state);
    GCTokenImpl gct;
    state->gc_dependent(gct, 0);
    return cNil;
  }

  SignalHandler::SignalHandler(STATE, Configuration& config)
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

    initialize(state);
    setup_default_handlers();

    start_thread(state);
  }

  SignalHandler::~SignalHandler() {
    shared_.auxiliary_threads()->unregister_thread(this);
  }

  void SignalHandler::initialize(STATE) {
    worker_lock_.init();
    worker_cond_.init();
    pause_cond_.init();
  }

  void SignalHandler::start_thread(STATE) {
    SYNC(state);
    if(self_) return;
    utilities::thread::Mutex::LockGuard lg(worker_lock_);
    self_ = state->shared().new_vm();
    self_->metrics()->init(metrics::eRubyMetrics);
    paused_ = false;
    exit_ = false;
    thread_.set(Thread::create(state, self_, G(thread), signal_handler_tramp, true));
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
    initialize(state);

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
    RBX_DTRACE_CONST char* thread_name = const_cast<RBX_DTRACE_CONST char*>("rbx.signal");
    self_->set_name(thread_name);

    RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                          state->vm()->thread_id(), 1);

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
    RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                         state->vm()->thread_id(), 1);
  }

  void SignalHandler::signal_tramp(int sig) {
    signal_handler_->handle_signal(sig);
  }

  void SignalHandler::handle_signal(int sig) {
    if(exit_) return;

    target_->metrics()->system_metrics.os_signals_received++;

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

        target_->metrics()->system_metrics.os_signals_processed++;

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

  static void null_func(int sig) {}

#ifdef USE_EXECINFO
  static const char* rbx_signal_string(int sig) {
    switch(sig) {
    case SIGSEGV:
      return "SIGSEGV";
    case SIGBUS:
      return "SIGBUS";
    case SIGILL:
      return "SIGILL";
    case SIGABRT:
      return "SIGABRT";
    case SIGFPE:
      return "SIGFPE";
    default:
      return "UNKNOWN";
    }
  }

  static void segv_handler(int sig) {
    struct sigaction action;

    // Unset our handler for this signal.
    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    sigaction(sig, &action, NULL);

    char* pause_env = getenv("RBX_PAUSE_ON_CRASH");

    if(pause_env) {
      long timeout = strtol(pause_env, NULL, 10);
      if(timeout <= 0) {
        timeout = 60;
      } else {
        timeout *= 60;
      }
      std::cerr << "\n========== CRASH (" << getpid();
      std::cerr << "), pausing for " << timeout << " seconds to attach debugger\n";
      sleep(timeout);
    }

#define RBX_ABORT_CALLSTACK_SIZE    128
    void* callstack[RBX_ABORT_CALLSTACK_SIZE];
    int i, frames = backtrace(callstack, RBX_ABORT_CALLSTACK_SIZE);
    char** symbols = backtrace_symbols(callstack, frames);

    logger::fatal("The Rubinius process is aborting with signal: %s",
                  rbx_signal_string(sig));
    logger::fatal("--- begin system backtrace ---");
    for(i = 0; i < frames; i++) {
      logger::fatal("%s", symbols[i]);
    }
    logger::fatal("--- end system backtrace ---");

    logger::fatal("--- begin system info ---");
    logger::fatal("sysname: %s", machine_info.sysname);
    logger::fatal("nodename: %s", machine_info.nodename);
    logger::fatal("release: %s", machine_info.release);
    logger::fatal("version: %s", machine_info.version);
    logger::fatal("machine: %s", machine_info.machine);
    logger::fatal("--- end system info ---");

    raise(sig);
  }
#endif

  void SignalHandler::setup_default_handlers() {
#ifndef RBX_WINDOWS
    // Get the machine info.
    uname(&machine_info);

    struct sigaction action;
    action.sa_handler = null_func;
    action.sa_flags = 0;
    sigfillset(&action.sa_mask);
    sigaction(SIGVTALRM, &action, NULL);

    // Some extensions expect SIGALRM to be defined, because MRI does.
    // We'll just use a noop for it.
    action.sa_handler = null_func;
    sigaction(SIGALRM, &action, NULL);

    // Ignore sigpipe.
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, NULL);

#ifdef USE_EXECINFO
    // If we have execinfo, setup some crash handlers
    if(!getenv("DISABLE_SEGV")) {
      action.sa_handler = segv_handler;
      sigaction(SIGSEGV, &action, NULL);
      sigaction(SIGBUS,  &action, NULL);
      sigaction(SIGILL,  &action, NULL);
      sigaction(SIGFPE,  &action, NULL);
      sigaction(SIGABRT, &action, NULL);
    }
#endif  // USE_EXEC_INFO
#else
    signal(SIGTERM, quit_handler);
#endif  // ifndef RBX_WINDOWS
  }
}
