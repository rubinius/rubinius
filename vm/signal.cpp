#include "config.h"
#include "release.h"
#include "vm.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "signal.hpp"
#include "configuration.hpp"

#include "builtin/module.hpp"
#include "builtin/array.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/jit.hpp"
#include "builtin/module.hpp"
#include "builtin/module.hpp"
#include "builtin/native_method.hpp"
#include "builtin/string.hpp"
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

  static SignalThread* signal_thread_ = 0;

  // Used by the segfault reporter. Calculated up front to avoid
  // crashing inside the crash handler.
  static struct utsname machine_info;

  SignalThread::SignalThread(STATE, Configuration& config)
    : InternalThread(state, "rbx.signal")
    , shared_(state->shared())
    , target_(state->vm())
    , queued_signals_(0)
  {
    signal_thread_ = this;
    shared_.set_signal_handler(this);

    setup_default_handlers();
  }

  void SignalThread::initialize(STATE) {
    InternalThread::initialize(state);

    for(int i = 0; i < NSIG; i++) {
      pending_signals_[i] = 0;
    }

    worker_lock_.init();
    worker_cond_.init();
  }

  void SignalThread::wakeup(STATE) {
    InternalThread::wakeup(state);

    worker_cond_.signal();
  }

  void SignalThread::stop(STATE) {
    for(std::list<int>::iterator i = watched_signals_.begin();
        i != watched_signals_.end();
        ++i) {
      signal(*i, SIG_DFL);
    }

    InternalThread::stop(state);
  }

  void SignalThread::print_machine_info(PrintFunction function) {
    function("sysname: %s", machine_info.sysname);
    function("nodename: %s", machine_info.nodename);
    function("release: %s", machine_info.release);
    function("version: %s", machine_info.version);
    function("machine: %s", machine_info.machine);
  }


  void SignalThread::print_process_info(PrintFunction function) {
    function("user: %s", signal_thread_->shared().username.c_str());
    function("pid: %s", signal_thread_->shared().pid.c_str());
    function("program name: %s", RBX_PROGRAM_NAME);
    function("version: %s", RBX_VERSION);
    function("ruby version: %s", RBX_RUBY_VERSION);
    function("release date: %s", RBX_RELEASE_DATE);
    function("build revision: %s", RBX_BUILD_REV);
#if ENABLE_LLVM
    function("llvm version: %s", RBX_LLVM_VERSION);
#endif
    function("jit status: %s",
        CBOOL(signal_thread_->shared().env()->state->globals().jit.get()->enabled())
        ? "enabled" : "disabled");
  }

  void SignalThread::run(STATE) {
#ifndef RBX_WINDOWS
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif

    GCTokenImpl gct;

    metrics().init(metrics::eRubyMetrics);

    while(!thread_exit_) {
      {
        utilities::thread::Mutex::LockGuard lg(worker_lock_);
        if(thread_exit_) break;
        state->gc_independent(gct, 0);
        worker_cond_.wait(worker_lock_);
        // If we should exit now, don't try to become
        // dependent first but break and exit the thread
        if(thread_exit_) break;
      }
      state->gc_dependent(gct, 0);
      {
        utilities::thread::Mutex::LockGuard lg(worker_lock_);
        if(thread_exit_) break;
      }

      target_->wakeup(state, gct, 0);
    }
  }

  void SignalThread::signal_handler(int sig) {
    signal_thread_->handle_signal(sig);
  }

  void SignalThread::handle_signal(int sig) {
    if(thread_exit_) return;

    target_->metrics().system_metrics.os_signals_received++;

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

  void SignalThread::add_signal(STATE, int sig, HandlerType type) {
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
      action.sa_handler = signal_handler;
      watched_signals_.push_back(sig);
    }

    action.sa_flags = 0;
    sigfillset(&action.sa_mask);

    sigaction(sig, &action, NULL);
#endif
  }

  bool SignalThread::deliver_signals(STATE, CallFrame* call_frame) {
    queued_signals_ = 0;

    for(int i = 0; i < NSIG; i++) {
      if(pending_signals_[i] > 0) {
        pending_signals_[i] = 0;

        target_->metrics().system_metrics.os_signals_processed++;

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

  void SignalThread::print_backtraces() {
    STATE = shared_.env()->state;
    ThreadList* threads = shared_.threads();

    for(ThreadList::iterator i = threads->begin(); i != threads->end(); ++i) {
      VM* vm = (*i)->as_vm();
      if(!vm) continue;

      if(vm->saved_call_frame()) {
        utilities::logger::fatal("--- Thread %d backtrace ---", vm->thread_id());
      }

      for(CallFrame* frame = vm->saved_call_frame(); frame; frame = frame->previous) {
        std::ostringstream stream;

        if(NativeMethodFrame* nmf = frame->native_method_frame()) {
          stream << static_cast<void*>(frame) << ": ";
          NativeMethod* nm = try_as<NativeMethod>(nmf->get_object(nmf->method()));
          if(nm && nm->name()->symbol_p()) {
            stream << "capi:" << nm->name()->debug_str(state) << " at ";
            stream << nm->file()->c_str(state);
          } else {
            stream << "unknown capi";
          }
        } else if(frame->compiled_code) {
          if(frame->is_block_p(state)) {
            stream << "__block__";
          } else {
            if(SingletonClass* sc = try_as<SingletonClass>(frame->module())) {
              Object* obj = sc->singleton();

              if(Module* mod = try_as<Module>(obj)) {
                stream << mod->debug_str(state) << ".";
              } else {
                if(obj == G(main)) {
                  stream << "MAIN.";
                } else {
                  stream << "#<" << obj->class_object(state)->debug_str(state) <<
                            ":" << (void*)obj->id(state)->to_native() << ">.";
                }
              }
            } else if(IncludedModule* im = try_as<IncludedModule>(frame->module())) {
              stream <<  im->module()->debug_str(state) << "#";
            } else {
              Symbol* name;
              std::string mod_name;

              if(frame->module()->nil_p()) {
                mod_name = frame->constant_scope()->module()->debug_str(state);
              } else {
                if((name = try_as<Symbol>(frame->module()->module_name()))) {
                  mod_name = name->debug_str(state);
                } else if((name = try_as<Symbol>(
                          frame->constant_scope()->module()->module_name()))) {
                  mod_name = name->debug_str(state);
                } else {
                  mod_name = "<anonymous module>";
                }
              }
              stream << mod_name << "#";
            }

            Symbol* name = try_as<Symbol>(frame->name());
            if(name) {
              stream << name->debug_str(state);
            } else {
              stream << frame->compiled_code->name()->debug_str(state);
            }
          }

          stream << " in ";
          if(Symbol* file_sym = try_as<Symbol>(frame->compiled_code->file())) {
            stream << file_sym->debug_str(state) << ":" << frame->line(state);
          } else {
            stream << "<unknown>";
          }

          stream << " (+" << frame->ip();
          if(frame->is_inline_frame()) {
            stream << " inline";
          } else if(frame->jitted_p()) {
            stream << " jit";
          }
          stream << ")";
        }

        utilities::logger::fatal(stream.str().c_str());
      }
    }
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

  static void abandon_ship_handler(int sig) {
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
    logger::fatal("--- begin system info ---");
    signal_thread_->print_machine_info(logger::fatal);
    logger::fatal("--- end system info ---");

    logger::fatal("--- begin rubinius info ---");
    signal_thread_->print_process_info(logger::fatal);
    logger::fatal("--- end rubinius info ---");

    logger::fatal("--- begin system backtrace ---");
    for(i = 0; i < frames; i++) {
      logger::fatal("%s", symbols[i]);
    }
    logger::fatal("--- end system backtrace ---");

    logger::fatal("--- begin Ruby backtraces ---");
    signal_thread_->print_backtraces();
    logger::fatal("--- end Ruby backtraces ---");

    raise(sig);
  }
#endif

  void SignalThread::setup_default_handlers() {
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
      action.sa_handler = abandon_ship_handler;
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
