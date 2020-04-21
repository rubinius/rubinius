#include "config.h"
#include "release.h"
#include "thread_state.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include "on_stack.hpp"
#include "signals.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/lexical_scope.hpp"
#include "class/fixnum.hpp"
#include "class/jit.hpp"
#include "class/module.hpp"
#include "class/native_method.hpp"
#include "class/object.hpp"
#include "class/string.hpp"
#include "class/thread.hpp"
#include "class/unwind_state.hpp"

#include "capi/capi.hpp"

#include "diagnostics/machine.hpp"

#include "dtrace/dtrace.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <string>

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

#include <dlfcn.h>
#include <sys/utsname.h>
#include <sys/select.h>

namespace rubinius {
  Signals* Signals::signal_handler_ = nullptr;

  Signals::SignalThread::SignalThread(
      STATE, std::mutex& lock, std::condition_variable& condition)
    : MachineThread(state, "rbx.signals", MachineThread::eLarge)
    , queue_index_(0)
    , process_index_(0)
    , lock_(lock)
    , condition_(condition)
  {
  }

  void Signals::SignalThread::initialize(STATE) {
    Thread::create(state, thread_state());

    for(int i = 0; i < pending_signal_size_; i++) {
      pending_signals_[i] = 0;
    }

    queue_index_ = process_index_ = 0;
  }

  void Signals::SignalThread::wakeup(STATE) {
    MachineThread::wakeup(state);

    while(thread_running_p()) {
      condition_.notify_one();
    }
  }

  void Signals::SignalThread::stop(STATE) {
    MachineThread::stop_thread(state);
  }

  void Signals::SignalThread::run(STATE) {
    logger::info("signals: worker thread starting");

    state->unmanaged_phase();

    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);

    while(!thread_exit_) {
      {
        std::unique_lock<std::mutex> lock(lock_);
        condition_.wait(lock);
      }

      if(thread_exit_) break;

      int signal = pending_signals_[process_index_];
      pending_signals_[process_index_] = 0;

      process_index_ = ++process_index_ % pending_signal_size_;

      MachineException::guard(state, false, [&]{
          // TODO: block fork(), exec() on signal handler thread
          if(signal > 0) {
            ManagedPhase managed(state);

            thread_state()->metrics()->signals_processed++;

            Array* args = Array::create(state, 1);
            args->set(state, 0, Fixnum::from(signal));

            if(!G(rubinius)->send(state, state->symbol("received_signal"), args, cNil)) {
              if(state->unwind_state()->raise_reason() == cException ||
                  state->unwind_state()->raise_reason() == cSystemExit)
              {
                Exception* exc = state->unwind_state()->current_exception();
                state->unwind_state()->clear_raise();

                Array* args = Array::create(state, 1);
                args->set(state, 0, exc);

                state->machine_state()->loader()->send(state,
                    state->symbol("handle_exception"), args, cNil);

                thread_exit_ = true;
              }
            }
          }
        });
    }

    logger::info("signals: worker thread exited");
  }

  void Signals::SignalThread::queue_signal(int signal) {
    ThreadState::current()->metrics()->signals_received++;

    {
      std::lock_guard<std::mutex> guard(lock_);

      pending_signals_[queue_index_] = signal;
      queue_index_ = ++queue_index_ % pending_signal_size_;

      condition_.notify_one();
    }
  }

  Signals::Signals()
    : signal_thread_(nullptr)
    , watch_lock_()
    , watched_signals_()
  {
    Signals::signal_handler_ = this;
    install_default_handlers();
  }

  void Signals::start(STATE) {
    if(!signal_thread_) {
      signal_thread_ = new Signals::SignalThread(state, lock_, condition_);
      signal_thread_->start(state);
    }
  }

  void Signals::stop(STATE) {
    for(std::list<int>::iterator i = watched_signals_.begin();
        i != watched_signals_.end();
        ++i) {
      signal(*i, SIG_DFL);
    }

    if(signal_thread_) {
      signal_thread_->stop(state);
      signal_thread_ = nullptr;
    }
  }

  void Signals::signal_handler(int signal) {
    if(SignalThread* handler = Signals::signal_handler_->signal_thread()) {
      handler->queue_signal(signal);
    }
  }

  void Signals::after_fork_child(STATE) {
    new(&lock_) std::mutex;
    new(&condition_) std::condition_variable;

    if(signal_thread_) {
      delete signal_thread_;
      signal_thread_ = nullptr;
    }

    start(state);
  }

  void Signals::print_machine_info(logger::PrintFunction function) {
    ThreadState* state = ThreadState::current();

    function("node: info: %s %s",
        state->environment()->machine_info()->nodename,
        state->environment()->machine_info()->version);
  }

#define RBX_PROCESS_INFO_LEN    256

  void Signals::print_process_info(logger::PrintFunction function) {
    const char* llvm_version;
    const char* jit_status;

    ThreadState* state = ThreadState::current();

    llvm_version = RBX_LLVM_VERSION;

    if(CBOOL(state->machine()->environment()->state->globals().jit.get()->enabled_p(
            state->machine()->environment()->state)))
    {
      jit_status = "JIT";
    } else {
      jit_status = "JIT disabled";
    }

    char process_info[RBX_PROCESS_INFO_LEN];

    snprintf(process_info, RBX_PROCESS_INFO_LEN, "%s %s %s %s %s %s %.8s %s %s",
        state->machine()->environment()->username().c_str(),
        RBX_PROGRAM_NAME, state->machine()->environment()->pid().c_str(),
        RBX_VERSION, RBX_RUBY_VERSION, RBX_RELEASE_DATE, RBX_BUILD_REV,
        llvm_version, jit_status);

    function("process: info: %s", process_info);
  }

  void Signals::add_signal_handler(STATE, int signal, HandlerType type) {
    std::lock_guard<std::mutex> lock(watch_lock_);

    struct sigaction action;

    if(type == eDefault) {
      action.sa_handler = SIG_DFL;
      watched_signals_.remove(signal);
    } else if(type == eIgnore) {
      action.sa_handler = SIG_IGN;
      watched_signals_.push_back(signal);
    } else {
      action.sa_handler = Signals::signal_handler;
      watched_signals_.push_back(signal);
    }

    action.sa_flags = 0;
    sigfillset(&action.sa_mask);

    sigaction(signal, &action, nullptr);
  }

  void Signals::print_backtraces() {
    ThreadState* state = ThreadState::current();

    state->threads()->each(state, [&](STATE, ThreadState* thr) {
        bool first = true;
        CallFrame* frame = thr->call_frame();

        while(frame) {
          if(first) {
            logger::fatal("--- %s %d backtrace ---", thr->kind_name(), thr->thread_id());
            first = false;
          }

          std::ostringstream stream;

          if(NativeMethodFrame* nmf = frame->native_method_frame()) {
            stream << static_cast<void*>(frame) << ": ";
            NativeMethod* nm = MemoryHandle::try_as<NativeMethod>(nmf->method());
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
                              ":" << (void*)obj->object_id(state)->to_native() << ">.";
                  }
                }
              } else if(IncludedModule* im = try_as<IncludedModule>(frame->module())) {
                stream <<  im->module()->debug_str(state) << "#";
              } else {
                Symbol* name;
                std::string mod_name;

                if(frame->module()->nil_p()) {
                  mod_name = frame->lexical_scope()->module()->debug_str(state);
                } else {
                  if((name = try_as<Symbol>(frame->module()->module_name()))) {
                    mod_name = name->debug_str(state);
                  } else if((name = try_as<Symbol>(
                            frame->lexical_scope()->module()->module_name()))) {
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

          logger::fatal(stream.str().c_str());

          frame = frame->previous;
        }
      });
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

  static void abandon_ship_handler(int sig, siginfo_t* info, void*) {
    struct sigaction action;

    // Unset our handler for this signal.
    memset(&action, 0, sizeof(action));
    sigfillset(&action.sa_mask);

    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    sigaction(sig, &action, nullptr);

    char* pause_env = getenv("RBX_PAUSE_ON_CRASH");

    if(pause_env) {
      long timeout = strtol(pause_env, nullptr, 10);
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
    Signals::signal_handler_->print_machine_info(logger::fatal);
    logger::fatal("--- end system info ---");

    logger::fatal("--- begin rubinius info ---");
    Signals::signal_handler_->print_process_info(logger::fatal);
    logger::fatal("--- end rubinius info ---");

    logger::fatal("--- begin system backtrace ---");
    for(i = 0; i < frames; i++) {
      logger::fatal("%s", symbols[i]);
    }
    logger::fatal("--- end system backtrace ---");

    logger::fatal("--- begin Ruby backtraces ---");
    Signals::signal_handler_->print_backtraces();
    logger::fatal("--- end Ruby backtraces ---");

    free(symbols);
    raise(sig);
  }
#endif

  void Signals::install_default_handlers() {
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    sigfillset(&action.sa_mask);

    action.sa_handler = null_func;
    action.sa_flags = 0;
    sigaction(SIGVTALRM, &action, nullptr);

    // Some extensions expect SIGALRM to be defined, because MRI does.
    // We'll just use a noop for it.
    action.sa_handler = null_func;
    sigaction(SIGALRM, &action, nullptr);

    // Ignore sigpipe.
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, nullptr);

#ifdef USE_EXECINFO
    // If we have execinfo, setup some crash handlers
    if(!getenv("DISABLE_SEGV")) {
      memset(&action, 0, sizeof(action));
      sigfillset(&action.sa_mask);

      action.sa_flags = SA_SIGINFO;
      action.sa_sigaction = abandon_ship_handler;

      sigaction(SIGSEGV, &action, nullptr);
      sigaction(SIGBUS,  &action, nullptr);
      sigaction(SIGILL,  &action, nullptr);
      sigaction(SIGFPE,  &action, nullptr);
      sigaction(SIGABRT, &action, nullptr);
    }
#endif  // USE_EXEC_INFO
  }
}
