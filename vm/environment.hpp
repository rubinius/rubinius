#ifndef RBX_ENVIRONMENT_HPP
#define RBX_ENVIRONMENT_HPP

#include <string>
#include <stdexcept>

#include "vm.hpp"
#include "config_parser.hpp"
#include "configuration.hpp"

namespace rubinius {

  class ConfigParser;
  class QueryAgent;

  // throw when there is a bad signature on a kernel .rbc file.
  class BadKernelFile : public std::runtime_error {
  public:
    BadKernelFile(const std::string& str)
      : std::runtime_error(str)
    {}
  };

  class Environment {
    int argc_;
    char** argv_;

    uint64_t signature_;

  public:
    SharedState* shared;
    VM* state;
    QueryAgent* agent;

    ConfigParser  config_parser;
    Configuration config;

  public:
    Environment(int argc, char** argv);
    ~Environment();

    void setup_cpp_terminate();

    void load_config_argv(int argc, char** argv);
    void load_argv(int argc, char** argv);
    void load_kernel(std::string root);
    void load_directory(std::string dir);
    void load_platform_conf(std::string dir);
    void load_conf(std::string path);
    void load_string(std::string str);
    void run_file(std::string path);
    void run_from_filesystem(std::string root);
    void boot_vm();

    void halt();
    int exit_code();
    void start_signals();
    void start_agent(int port);
  };

}

#endif
