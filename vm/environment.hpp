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
  class SignalHandler;
  class FinalizerHandler;

  /**
   * Thrown when unable to find Rubinius runtime directories.
   */
  class MissingRuntime : public std::runtime_error {
  public:
    MissingRuntime(const std::string& str)
      : std::runtime_error(str)
    {}
  };

  /**
   * Thrown when there is a bad signature on a kernel .rbc file.
   */
  class BadKernelFile : public std::runtime_error {
  public:
    BadKernelFile(const std::string& str)
      : std::runtime_error(str)
    {}
  };


  /**
   * The environment context under which Rubinius virtual machines are executed.
   *
   * Environment and configuration data is processed and stored in an Environment
   * instance, which uses this information to bootstrap the VM. It also stores
   * all runtime shared state.
   */

  class Environment {
    int argc_;
    char** argv_;

    /**
     * Digest of the runtime and configuration files to keep the runtime
     * and VM in sync.
     */
    uint64_t signature_;

    // The Ruby library version with which the .rbc file is compatible.
    int version_;

    SignalHandler* signal_handler_;
    FinalizerHandler* finalizer_handler_;

    std::string system_prefix_;

  public:
    SharedState* shared;
    VM* root_vm;
    State* state;

    ConfigParser  config_parser;
    Configuration config;

  public:
    Environment(int argc, char** argv);
    ~Environment();

    int argc() const {
      return argc_;
    }

    char** argv() const {
      return argv_;
    }

    void set_root_vm(VM* vm) {
      root_vm = vm;
      state->set_vm(vm);
    }

    void setup_cpp_terminate();

    std::string executable_name();
    std::string system_prefix();
    bool verify_paths(std::string prefix);
    bool load_signature(std::string dir);
    void load_vm_options(int argc, char** argv);
    void load_argv(int argc, char** argv);
    void load_kernel(std::string root);
    void load_directory(std::string dir);
    void load_platform_conf(std::string dir);
    void load_conf(std::string path);
    void load_string(std::string str);
    void run_file(std::string path);
    void load_tool();
    void run_from_filesystem();
    void boot_vm();

    void halt(STATE);
    void halt_and_exit(STATE);
    int exit_code(STATE);

    void start_signals();
    void start_finalizer();
    void start_agent(int port);
  };

}

#endif
