#ifndef RBX_ENVIRONMENT_HPP
#define RBX_ENVIRONMENT_HPP

#include <string>
#include <stdexcept>

#include "vm.hpp"
#include "config_parser.hpp"
#include "configuration.hpp"

namespace rubinius {

  class ConfigParser;

  class Environment {
  public:
    SharedState* shared;
    VM* state;

    ConfigParser  config_parser;
    Configuration config;

    Environment(int argc, char** argv);
    ~Environment();

    void load_config_argv(int argc, char** argv);
    void load_argv(int argc, char** argv);
    void load_directory(std::string dir);
    void load_platform_conf(std::string dir);
    void run_file(std::string path);
    void enable_preemption();
    void boot_vm();
    int exit_code();
    void start_signal_thread();
  };

}

#endif
