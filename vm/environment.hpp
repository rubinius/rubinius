#ifndef RBX_ENVIRONMENT_HPP
#define RBX_ENVIRONMENT_HPP

#include <string>
#include <stdexcept>

#include "vm.hpp"

namespace rubinius {

  class Environment {
  public:
    VM* state;

    Environment();
    ~Environment();

    void load_argv(int argc, char** argv);
    void load_directory(std::string dir);
    void load_platform_conf(std::string dir);
    void run_file(std::string path);
  };

}

#endif
