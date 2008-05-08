#ifndef RBX_ENVIRONMENT_HPP
#define RBX_ENVIRONMENT_HPP

#include "builtin.hpp"

#include <string>
#include <stdexcept>

namespace rubinius {

  class Environment {
    VM* state;
  public:

    Environment();
    ~Environment();

    void load_argv(int argc, char** argv);
    void load_directory(std::string dir);
    void run_file(std::string path);
  };
}

#endif
