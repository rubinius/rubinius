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
#include "builtin/task.hpp"
#include "builtin/taskprobe.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace rubinius {

  Environment::Environment() {
    state = new VM(VM::default_bytes, false);
    TaskProbe* probe = TaskProbe::create(state);
    state->probe.set(probe->parse_env(NULL) ? probe : (TaskProbe*)Qnil);
  }

  Environment::~Environment() {
    delete state;
  }

  void Environment::enable_preemption() {
    state->setup_preemption();
  }

  void Environment::load_config_argv(int argc, char** argv) {
    for(int i=1; i < argc; i++) {
      char* arg = argv[i];
      if(strncmp(arg, "-X", 2) == 0) {
        state->user_config->import_line(arg + 2);
        continue;
      }

      if(arg[0] != '-' || strcmp(arg, "--") == 0) return;
    }
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
        ary->set(state, which_arg++, String::create(state, arg)->taint());
      }
    }

    state->set_const("ARGV", ary);
  }

  void Environment::load_directory(std::string dir) {
    std::string path = dir + "/.load_order.txt";
    std::ifstream stream(path.c_str());
    if(!stream) {
      throw std::runtime_error("Unable to load directory, .load_order.txt is missing");
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

    state->user_config->import_stream(stream);
  }

  void Environment::boot_vm() {
    state->boot();
  }

  void Environment::run_file(std::string file) {
    if(!state->probe->nil_p()) state->probe->load_runtime(state, file);

    std::ifstream stream(file.c_str());
    if(!stream) throw std::runtime_error("Unable to open file to run");

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") throw std::runtime_error("Invalid file");

    // TODO check version number
    cf->execute(state);

    if(!G(current_task)->exception()->nil_p()) {
      // Reset the context so we can show the backtrace
      // HACK need to use write barrier aware stuff?
      Exception* exc = G(current_task)->exception();
      G(current_task)->active(state, exc->context());

      std::ostringstream msg;

      msg << "exception detected at toplevel: ";
      if(!exc->message()->nil_p()) {
        msg << exc->message()->c_str();
      }
      msg << " (" << exc->klass()->name()->c_str(state) << ")";
      Assertion::raise(msg.str().c_str());
    }

    delete cf;
  }

}
