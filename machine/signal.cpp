#include "config.h"
#include "release.h"
#include "vm.hpp"
#include "state.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include "on_stack.hpp"
#include "signal.hpp"
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

#include "capi/capi.hpp"

#include "diagnostics/machine.hpp"

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

  SignalThread::SignalThread(STATE, VM* vm)
    : shared_(state->shared())
    , vm_(vm)
    , system_exit_(false)
    , exit_code_(0)
    , queue_index_(0)
    , process_index_(0)
  {
    state->set_vm(vm_);

    signal_thread_ = this;
    install_default_handlers(state);

    NativeMethod::init_thread(state);
  }

  VM* SignalThread::new_vm(STATE) {
    VM* vm = state->shared().thread_nexus()->new_vm(&state->shared(), "rbx.system");
    vm->set_kind(memory::ManagedThread::eSystem);
    return vm;
  }

  void SignalThread::set_exit_code(Object* exit_code) {
    if(Fixnum* fix = try_as<Fixnum>(exit_code)) {
      exit_code_ = fix->to_native();
    } else {
      exit_code_ = -1;
    }
  }

  void SignalThread::signal_handler(int signal) {
    signal_thread_->queue_signal(signal);
  }

  void SignalThread::queue_signal(int signal) {
    if(system_exit_) return;

    vm()->metrics()->signals_received++;

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
    Thread::create(state, vm());

    for(int i = 0; i < pending_signal_size_; i++) {
      pending_signals_[i] = 0;
    }

    queue_index_ = process_index_ = 0;

    watch_lock_.init();
    lock_.init();
    condition_.init();
  }

  void SignalThread::start(STATE) {
    initialize(state);

    if(state->shared().config.log_lifetime.value) {
      state->shared().signals()->print_machine_info(logger::write);
      state->shared().signals()->print_process_info(logger::write);

      logger::write("process: boot stats: " \
          "fields %lldus " \
          "main thread: %lldus " \
          "memory: %lldus " \
          "ontology: %lldus " \
          "platform: %lldus",
          state->shared().boot_metrics()->fields_us,
          state->shared().boot_metrics()->main_thread_us,
          state->shared().boot_metrics()->memory_us,
          state->shared().boot_metrics()->ontology_us,
          state->shared().boot_metrics()->platform_us);
    }

    run(state);
  }

  void SignalThread::restart(STATE) {
    vm_ = SignalThread::new_vm(state);

    initialize(state);

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, state->shared().config.machine_thread_stack_size);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    if(int error = pthread_create(&vm_->os_thread(), &attrs,
          SignalThread::run, (void*)this))
    {
      logger::fatal("%s: %s: create thread failed", strerror(error), vm_->name().c_str());
      ::abort();
    }

    pthread_attr_destroy(&attrs);
  }

  void SignalThread::after_fork_child(STATE) {
    restart(state);
  }

  void SignalThread::stop(STATE) {
    for(std::list<int>::iterator i = watched_signals_.begin();
        i != watched_signals_.end();
        ++i) {
      signal(*i, SIG_DFL);
    }
  }

  void* SignalThread::run(void* ptr) {
    SignalThread* thread = reinterpret_cast<SignalThread*>(ptr);
    VM* vm = thread->vm();

    State state_obj(vm), *state = &state_obj;

    vm->set_stack_bounds(state->shared().config.machine_thread_stack_size.value);
    vm->set_current_thread();

    RUBINIUS_THREAD_START(
        const_cast<RBX_DTRACE_CHAR_P>(vm->name().c_str()), vm->thread_id(), 1);

    vm->metrics()->start_reporting(state);

    NativeMethod::init_thread(state);

    int exit_code = 0;

    // TODO: clean up and unify with main() exception handling.
    try {
      thread->run(state);
    } catch(Assertion *e) {
      std::cout << "VM Assertion:" << std::endl;
      std::cout << "  " << e->reason << std::endl << std::endl;
      e->print_backtrace();

      std::cout << std::endl << "Ruby backtrace:" << std::endl;
      state->vm()->print_backtrace();
      delete e;
      exit_code = 1;
    } catch(RubyException &e) {
      std::cout << "Ruby Exception hit toplevel:\n";
      // Prints Ruby backtrace, and VM backtrace if captured
      e.show(state);
      exit_code = 1;
    } catch(TypeError &e) {

      /* TypeError's here are dealt with specially so that they can deliver
       * more information, such as _why_ there was a type error issue.
       *
       * This has the same name as the RubyException TypeError (run `5 + "string"`
       * as an example), but these are NOT the same - this exception is raised
       * internally when cNil gets passed to an array method, for instance, when
       * an array was expected.
       */
      std::cout << "Type Error detected:" << std::endl;
      TypeInfo* wanted = state->vm()->find_type(e.type);

      if(!e.object->reference_p()) {
        std::cout << "  Tried to use non-reference value " << e.object;
      } else {
        TypeInfo* was = state->vm()->find_type(e.object->type_id());
        std::cout << "  Tried to use object of type " <<
          was->type_name << " (" << was->type << ")";
      }

      std::cout << " as type " << wanted->type_name << " (" <<
        wanted->type << ")" << std::endl;

      e.print_backtrace();

      std::cout << "Ruby backtrace:" << std::endl;
      state->vm()->print_backtrace();
      exit_code = 1;
    } catch(VMException &e) {
      std::cout << "Unknown VM exception detected:" << std::endl;
      e.print_backtrace();
      exit_code = 1;
    } catch(std::exception& e) {
      std::cout << "C++ exception detected: " << e.what() << std::endl;
      exit_code = 1;
    } catch(...) {
      std::cout << "Unknown C++ exception detected at top level" << std::endl;
      exit_code = 1;
    }

    // We caught an exception, so exit directly here.
    exit(exit_code);
  }

  void SignalThread::run(STATE) {
    state->vm()->unmanaged_phase(state);

#ifndef RBX_WINDOWS
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif

    while(!system_exit_) {
      int signal = pending_signals_[process_index_];
      pending_signals_[process_index_] = 0;

      /* GCC 4.8.2 can't tell that this code is equivalent.
       *   process_index_ = ++process_index_ % pending_signal_size_;
       */
      ++process_index_;
      process_index_ %= pending_signal_size_;

      // TODO: block fork(), exec() on signal handler thread
      if(signal > 0) {
        ManagedPhase managed(state);

        vm()->metrics()->signals_processed++;

        Array* args = Array::create(state, 1);
        args->set(state, 0, Fixnum::from(signal));

        if(!G(rubinius)->send(state, state->symbol("received_signal"), args, cNil)) {
          if(state->thread_state()->raise_reason() == cException ||
              state->thread_state()->raise_reason() == cExit)
          {
            Exception* exc = state->thread_state()->current_exception();
            state->thread_state()->clear_raise();

            Array* args = Array::create(state, 1);
            args->set(state, 0, exc);

            state->shared().env()->loader()->send(state,
                state->symbol("handle_exception"), args, cNil);

            system_exit_ = true;

            break;
          }
        }
      } else {
        thread::Mutex::LockGuard guard(lock_);

        if(queue_index_ != process_index_) continue;
        condition_.wait(lock_);
      }
    }

    state->shared().env()->halt(state, exit_code_);
  }

  void SignalThread::print_machine_info(logger::PrintFunction function) {
    function("node: info: %s %s", machine_info.nodename, machine_info.version);
  }

#define RBX_PROCESS_INFO_LEN    256

  void SignalThread::print_process_info(logger::PrintFunction function) {
    const char* llvm_version;
    const char* jit_status;

    llvm_version = RBX_LLVM_VERSION;

    if(CBOOL(signal_thread_->shared().env()->state->globals().jit.get()->enabled_p(
            signal_thread_->shared().env()->state)))
    {
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

    function("process: info: %s", process_info);
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
    ThreadList* threads = shared_.thread_nexus()->threads();

    for(ThreadList::iterator i = threads->begin(); i != threads->end(); ++i) {
      VM* vm = (*i)->as_vm();
      if(!vm) continue;

      bool first = true;
      CallFrame* frame = vm->call_frame();

      while(frame) {
        if(first) {
          logger::fatal("--- %s %d backtrace ---", vm->kind_name(), vm->thread_id());
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

  void SignalThread::install_default_handlers(STATE) {
#ifndef RBX_WINDOWS
    // Get the machine info.
    uname(&machine_info);

    state->shared().nodename.assign(machine_info.nodename);

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
