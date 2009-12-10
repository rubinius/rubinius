/* An Environment is the toplevel class for Rubinius. It manages multiple
 * VMs, as well as imports C data from the process into Rubyland. */
#include "prelude.hpp"
#include "environment.hpp"
#include "config_parser.hpp"
#include "compiled_file.hpp"

#include "vm/exception.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/taskprobe.hpp"

#ifdef ENABLE_LLVM
#include "llvm/jit.hpp"
#include <llvm/System/Threading.h>
#endif

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

#include "signal.hpp"
#include "object_utils.hpp"

#include "native_thread.hpp"
#include "inline_cache.hpp"

#include "agent.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace rubinius {

  Environment::Environment()
    : agent(0)
  {
#ifdef ENABLE_LLVM
    assert(llvm::llvm_start_multithreaded() && "llvm doesn't support threading!");
#endif

    shared = new SharedState(config, config_parser);
    state = shared->new_vm();
  }

  Environment::~Environment() {
    VM::discard(state);
    SharedState::discard(shared);
  }

  void Environment::load_config_argv(int argc, char** argv) {
    config_parser.process_argv(argc, argv);
    config_parser.update_configuration(config);

    if(config.print_config > 1) {
      std::cout << "========= Configuration =========\n";
      config.print(true);
      std::cout << "=================================\n";
    } else if(config.print_config) {
      config.print();
    }
  }

  void cpp_exception_bug() {
    std::cerr << "[BUG] Uncaught C++ internal exception\n";
    std::cerr << "So sorry, it appears that you've encountered an internal\n";
    std::cerr << "bug. Please report this on the rubinius issue tracker.\n";
    std::cerr << "Include the following backtrace in the issue:\n\n";

    rubinius::abort();
  }

  void Environment::setup_cpp_terminate() {
    // Install a better terminate function to tell the user
    // there was a rubinius bug.
    std::set_terminate(cpp_exception_bug);
  }

  void Environment::enable_preemption() {
    state->setup_preemption();
  }

  static void null_func(int sig) {}

#ifdef USE_EXECINFO
  static void segv_handler(int sig) {
    static int crashing = 0;
    void *array[32];
    size_t size;

    // So we don't recurse!
    if(crashing) exit(101);

    crashing = 1;

    // print out all the frames to stderr
    static const char msg[] = "Error: signal ";
    if(write(2, msg, 14) == 0) exit(101);

    switch(sig) {
    case SIGSEGV:
      if(write(2, "SIGSEGV\n", 8) == 0) exit(101);
      break;
    case SIGBUS:
      if(write(2, "SIGBUS\n", 7) == 0) exit(101);
      break;
    case SIGILL:
      if(write(2, "SIGILL\n", 7) == 0) exit(101);
      break;
    case SIGABRT:
      if(write(2, "SIGABRT\n", 8) == 0) exit(101);
      break;
    case SIGFPE:
      if(write(2, "SIGFPE\n", 7) == 0) exit(101);
      break;
    default:
      if(write(2, "UNKNOWN\n", 8) == 0) exit(101);
      break;
    }

    // Try to get the output to flush...
    if(write(2, "\n\n", 2) == 0) exit(101);

    // get void*'s for all entries on the stack
    size = backtrace(array, 32);

    backtrace_symbols_fd(array, size, 2);

    // Try to get the output to flush...
    if(write(2, "\n\n", 2) == 0) exit(101);

    exit(100);
  }
#endif

  static void quit_handler(int sig) {
    static const char msg[] = "Terminated: signal ";
    if(write(2, msg, sizeof(msg)) == 0) exit(1);

    switch(sig) {
    case SIGHUP:
      if(write(2, "SIGHUP\n", 6) == 0) exit(1);
      break;
    case SIGTERM:
      if(write(2, "SIGTERM\n", 7) == 0) exit(1);
      break;
    case SIGUSR1:
      if(write(2, "SIGUSR1\n", 7) == 0) exit(1);
      break;
    case SIGUSR2:
      if(write(2, "SIGUSR2\n", 7) == 0) exit(1);
      break;
    default:
      if(write(2, "UNKNOWN\n", 8) == 0) exit(1);
      break;
    }

    exit(1);
  }

  void Environment::start_signals() {
    struct sigaction action;
    action.sa_handler = null_func;
    action.sa_flags = SA_RESTART;
    sigfillset(&action.sa_mask);
    sigaction(NativeThread::cWakeupSignal, &action, NULL);

    state->set_run_signals(true);
    shared->set_signal_handler(new SignalHandler(state));

    // Ignore sigpipe.
    signal(SIGPIPE, SIG_IGN);

    // If we have execinfo, setup some crash handlers
#ifdef USE_EXECINFO
    if(!getenv("DISABLE_SEGV")) {
      signal(SIGSEGV, segv_handler);
      signal(SIGBUS,  segv_handler);
      signal(SIGILL,  segv_handler);
      signal(SIGFPE,  segv_handler);
      signal(SIGABRT, segv_handler);

      // Force glibc to load the shared library containing backtrace()
      // now, so that we don't have to try and load it in the signal
      // handler.
      void* ary[1];
      backtrace(ary, 1);
    }
#endif

    // Setup some other signal that normally just cause the process
    // to terminate so that we print out a message, then terminate.
    signal(SIGHUP,  quit_handler);
    signal(SIGTERM, quit_handler);
    signal(SIGUSR1, quit_handler);
    signal(SIGUSR2, quit_handler);
  }

  void Environment::load_argv(int argc, char** argv) {
    bool process_xflags = true;
    state->set_const("ARG0", String::create(state, argv[0]));

    Array* ary = Array::create(state, argc - 1);
    int which_arg = 0;
    for(int i=1; i < argc; i++) {
      char* arg = argv[i];

      if(arg[0] != '-' || strcmp(arg, "--") == 0) {
        process_xflags = false;
      }

      if(!process_xflags || strncmp(arg, "-X", 2) != 0) {
        ary->set(state, which_arg++, String::create(state, arg)->taint(state));
      }
    }

    state->set_const("ARGV", ary);
  }

  void Environment::load_directory(std::string dir) {
    std::string path = dir + "/load_order.txt";
    std::ifstream stream(path.c_str());
    if(!stream) {
      throw std::runtime_error("Unable to load directory, load_order.txt is missing");
    }

    while(!stream.eof()) {
      std::string line;
      stream >> line;
      stream.get(); // eat newline

      // skip empty lines
      if(line.size() == 0) continue;

      run_file(dir + "/" + line);
    }
  }

  void Environment::load_platform_conf(std::string dir) {
    std::string path = dir + "/platform.conf";
    std::ifstream stream(path.c_str());
    if(!stream) {
      std::string error = "Unable to load " + path + ", it is missing";
      throw std::runtime_error(error);
    }

    config_parser.import_stream(stream);
  }

  void Environment::load_conf(std::string path) {
    std::ifstream stream(path.c_str());
    if(!stream) {
      std::string error = "Unable to load " + path + ", it is missing";
      throw std::runtime_error(error);
    }

    config_parser.import_stream(stream);
  }

  void Environment::load_string(std::string str) {
    config_parser.import_many(str);
  }

  void Environment::boot_vm() {
    if(config.qa_port > 0) start_agent(config.qa_port);

    // Respect -Xint
    if(config.jit_force_off) {
      config.jit_enabled.set("no");
    }

    state->initialize();
    state->boot();

    TaskProbe* probe = TaskProbe::create(state);
    state->probe.set(probe->parse_env(NULL) ? probe : (TaskProbe*)Qnil);
  }

  void Environment::run_file(std::string file) {
    if(!state->probe->nil_p()) state->probe->load_runtime(state, file);

    std::ifstream stream(file.c_str());
    if(!stream) {
      std::string msg = std::string("Unable to open file to run: ");
      msg.append(file);
      throw std::runtime_error(msg);
    }

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") throw std::runtime_error("Invalid file");

    /** @todo Redundant? CompiledFile::execute() does this. --rue */
    state->thread_state()->clear_exception(true);

    // TODO check version number
    cf->execute(state);

    if(state->thread_state()->raise_reason() == cException) {
      Exception* exc = as<Exception>(state->thread_state()->raise_value());
      std::ostringstream msg;

      msg << "exception detected at toplevel: ";
      if(!exc->message()->nil_p()) {
        if(String* str = try_as<String>(exc->message())) {
          msg << str->c_str();
        } else {
          msg << "<non-string Exception message>";
        }
      } else if(Exception::argument_error_p(state, exc)) {
        msg << "given "
            << as<Fixnum>(exc->get_ivar(state, state->symbol("@given")))->to_native()
            << ", expected "
            << as<Fixnum>(exc->get_ivar(state, state->symbol("@expected")))->to_native();
      }
      msg << " (" << exc->klass()->name()->c_str(state) << ")";
      std::cout << msg.str() << "\n";
      exc->print_locations(state);
      Assertion::raise(msg.str().c_str());
    }

    delete cf;
  }

  void Environment::halt() {
    if(state->shared.config.ic_stats) {
      state->shared.ic_registry()->print_stats(state);
    }

#ifdef ENABLE_LLVM
    LLVMState::shutdown(state);
#endif

    state->shared.stop_the_world();
  }

  int Environment::exit_code() {
    if(state->thread_state()->raise_reason() == cExit) {
      if(Fixnum* fix = try_as<Fixnum>(state->thread_state()->raise_value())) {
        return fix->to_native();
      } else {
        return -1;
      }
    }

    return 0;
  }

  void Environment::start_agent(int port) {
    agent = new QueryAgent(*shared, port);
    if(config.qa_verbose) agent->set_verbose();
    agent->run();
  }
}
