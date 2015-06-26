#include "config.h"
#include "release.h"
#include "vm.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "on_stack.hpp"
#include "signal.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/jit.hpp"
#include "builtin/module.hpp"
#include "builtin/native_method.hpp"
#include "builtin/string.hpp"
#include "builtin/thread.hpp"

#include "util/logger.hpp"

#include "dtrace/dtrace.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <iostream>
#include <string>

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

  SignalThread::SignalThread(STATE)
    : InternalThread(state, "rbx.signals", InternalThread::eSmall)
    , shared_(state->shared())
    , queue_index_(0)
    , process_index_(0)
  {
    signal_thread_ = this;
    install_default_handlers();
  }

  void SignalThread::signal_handler(int signal) {
    signal_thread_->queue_signal(signal);
  }

  void SignalThread::queue_signal(int signal) {
    if(thread_exit_) return;

    vm()->metrics().system.signals_received++;

    {
      thread::Mutex::LockGuard guard(lock_);

      pending_signals_[queue_index_] = signal;
      /* GCC 4.8.2 can't tell that this code is equivalent.
       *   queue_index_ = ++queue_index_ % pending_signal_size_;
       */
      ++queue_index_;
      queue_index_ %= pending_signal_size_;

      condition_.signal();
    }
  }

  void SignalThread::initialize(STATE) {
    InternalThread::initialize(state);

    Thread::create(state, vm());

    for(int i = 0; i < pending_signal_size_; i++) {
      pending_signals_[i] = 0;
    }

    queue_index_ = process_index_ = 0;

    watch_lock_.init();
    lock_.init();
    condition_.init();
  }

  void SignalThread::wakeup(STATE) {
    InternalThread::wakeup(state);

    {
      thread::Mutex::LockGuard guard(lock_);
      condition_.signal();
    }
  }

  void SignalThread::stop(STATE) {
    for(std::list<int>::iterator i = watched_signals_.begin();
        i != watched_signals_.end();
        ++i) {
      signal(*i, SIG_DFL);
    }

    InternalThread::stop(state);
  }

  void SignalThread::run(STATE) {
    GCTokenImpl gct;
    state->gc_independent(gct, 0);

#ifndef RBX_WINDOWS
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif

    while(!thread_exit_) {
      int signal = pending_signals_[process_index_];
      pending_signals_[process_index_] = 0;

      /* GCC 4.8.2 can't tell that this code is equivalent.
       *   process_index_ = ++process_index_ % pending_signal_size_;
       */
      ++process_index_;
      process_index_ %= pending_signal_size_;

      if(signal > 0) {
        GCDependent guard(state, 0);
        vm()->set_call_frame(0);

        vm()->metrics().system.signals_processed++;

        Array* args = 0;
        OnStack<1> os(state, args);

        args = Array::create(state, 1);
        args->set(state, 0, Fixnum::from(signal));

        if(!G(rubinius)->send(state, 0, state->symbol("received_signal"), args, cNil)) {
          if(state->thread_state()->raise_reason() == cException ||
              state->thread_state()->raise_reason() == cExit) {
            Array* args = 0;
            Exception* exc = 0;
            OnStack<2> os(state, args, exc);

            exc = state->thread_state()->current_exception();
            state->thread_state()->clear_raise();

            args = Array::create(state, 1);
            args->set(state, 0, exc);

            state->shared().env()->loader()->send(
                state, 0, state->symbol("handle_exception"), args, cNil);

            state->shared().env()->halt_and_exit(state);

            break;
          }
        }

        vm()->set_call_frame(0);
      } else {
        thread::Mutex::LockGuard guard(lock_);

        if(queue_index_ != process_index_) continue;
        condition_.wait(lock_);
      }

      vm()->set_call_frame(0);
    }
  }

  void SignalThread::print_machine_info(PrintFunction function) {
    function("node info: %s %s", machine_info.nodename, machine_info.version);
  }

#define RBX_PROCESS_INFO_LEN    256

  void SignalThread::print_process_info(PrintFunction function) {
    const char* llvm_version;
    const char* jit_status;

#if ENABLE_LLVM
    llvm_version = RBX_LLVM_VERSION;
#else
    llvm_version = "LLVM Disabled";
#endif

    if(CBOOL(signal_thread_->shared().env()->state->globals().jit.get()->enabled())) {
      jit_status = "JIT";
    } else {
      jit_status = "JIT disabled";
    }

    char process_info[RBX_PROCESS_INFO_LEN];

    snprintf(process_info, RBX_PROCESS_INFO_LEN, "%s %s %s %s %s %s %.8s %s %s",
        signal_thread_->shared().username.c_str(),
        RBX_PROGRAM_NAME, signal_thread_->shared().pid.c_str(),
        RBX_VERSION, RBX_RUBY_VERSION, RBX_RELEASE_DATE, RBX_BUILD_REV,
        llvm_version, jit_status);

    function("process info: %s", process_info);
  }

  void SignalThread::add_signal_handler(STATE, int signal, HandlerType type) {
    thread::SpinLock::LockGuard guard(watch_lock_);

#ifndef RBX_WINDOWS
    struct sigaction action;

    if(type == eDefault) {
      action.sa_handler = SIG_DFL;
      watched_signals_.remove(signal);
    } else if(type == eIgnore) {
      action.sa_handler = SIG_IGN;
      watched_signals_.push_back(signal);
    } else {
      action.sa_handler = signal_handler;
      watched_signals_.push_back(signal);
    }

    action.sa_flags = 0;
    sigfillset(&action.sa_mask);

    sigaction(signal, &action, NULL);
#endif
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

  void SignalThread::install_default_handlers() {
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
