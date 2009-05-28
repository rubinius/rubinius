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
#endif

#include "signal.hpp"
#include "object_utils.hpp"

#include "native_thread.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace rubinius {

  Environment::Environment(int argc, char** argv) {
    config_parser.process_argv(argc, argv);

    config_parser.update_configuration(config);

    shared = new SharedState(config, config_parser);

    state = shared->new_vm();
    state->initialize();

    if(config.print_config > 1) {
      std::cout << "========= Configuration =========\n";
      config.print(true);
      std::cout << "=================================\n";
    } else if(config.print_config) {
      config.print();
    }
  }

  Environment::~Environment() {
    VM::discard(state);
    SharedState::discard(shared);
  }

  void Environment::enable_preemption() {
    state->setup_preemption();
  }

  static void null_func(int sig) {}

  void Environment::start_signals() {
    struct sigaction action;
    action.sa_handler = null_func;
    action.sa_flags = SA_RESTART;
    sigfillset(&action.sa_mask);
    sigaction(7, &action, NULL);

    state->set_run_signals(true);
    shared->set_signal_handler(new SignalHandler(state));
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

  void Environment::boot_vm() {
    state->boot();

    TaskProbe* probe = TaskProbe::create(state);
    state->probe.set(probe->parse_env(NULL) ? probe : (TaskProbe*)Qnil);
  }

  void Environment::run_file(std::string file) {
    if(!state->probe->nil_p()) state->probe->load_runtime(state, file);

    std::ifstream stream(file.c_str());
    if(!stream) throw std::runtime_error("Unable to open file to run");

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
        msg << exc->message()->c_str();
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

}
