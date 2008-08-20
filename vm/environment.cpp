/* An Environment is the toplevel class for Rubinius. It manages multiple
 * VMs, as well as imports C data from the process into Rubyland. */

#include "environment.hpp"
#include "config.hpp" // HACK rename to config_parser.hpp
#include "compiled_file.hpp"
#include "probes.hpp"
#include "builtin/string.hpp"
#include "builtin/array.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace rubinius {
  Environment::Environment() {
    state = new VM();
    state->probe = new TaskProbe;
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

      std::cout << "Loading: " << line << std::endl;
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
    std::ifstream stream(file.c_str());
    if(!stream) throw std::runtime_error("Unable to open file to run");

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") throw std::runtime_error("Invalid file");

    // TODO check version number
    cf->execute(state);
  }
}
