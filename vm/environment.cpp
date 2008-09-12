/* An Environment is the toplevel class for Rubinius. It manages multiple
 * VMs, as well as imports C data from the process into Rubyland. */
#include "prelude.hpp"
#include "environment.hpp"
#include "config.hpp" // HACK rename to config_parser.hpp
#include "compiled_file.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/task.hpp"
#include "builtin/exception.hpp"
#include "builtin/taskprobe.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace rubinius {
  Environment::Environment() {
    state = new VM();
    TaskProbe* probe = TaskProbe::create(state);
    state->probe.set(probe->parse_env(NULL) ? probe : (TaskProbe*)Qnil);
  }

  Environment::~Environment() {
    delete state;
  }

  void Environment::load_argv(int argc, char** argv) {
    state->set_const("ARG0", String::create(state, argv[0]));

    Array* ary = Array::create(state, argc - 1);
    for(int i = 0; i < argc - 1; i++) {
      ary->set(state, i, String::create(state, argv[i + 1]));
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

      String* message = String::create(state,
          "exception detected at toplevel: ");
      if(!exc->message()->nil_p()) {
        message->append(state, exc->message());
      }
      message->append(state, " (");
      message->append(state, exc->klass()->name()->to_str(state));
      message->append(state, ")");
      Assertion::raise(message->c_str());
    }
  }

}
