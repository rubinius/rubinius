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
  class SignalThread;
  class FinalizerThread;

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

    SignalThread* signal_thread_;
    FinalizerThread* finalizer_thread_;

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
    void copy_argv(int argc, char** argv);
    void load_vm_options(int argc, char** argv);
    void load_argv(int argc, char** argv);
    void load_kernel(std::string root);
    void load_directory(std::string dir);
    void load_platform_conf(std::string dir);
    void load_conf(std::string path);
    void load_string(std::string str);
    void run_file(std::string path);
    void expand_config_value(config::String& cvar, const char* var, const char* value);
    void set_tmp_path();
    void set_username();
    void set_pid();
    void set_fsapi_path();
    void load_tool();
    void run_from_filesystem();
    void boot_vm();

    void before_exec(STATE);
    void after_exec(STATE);

    void halt(STATE);
    void halt_and_exit(STATE);
    int exit_code(STATE);

    void create_fsapi(STATE);
    void remove_fsapi(STATE);

    void start_finalizer(STATE);

    void start_signals(STATE);
    void stop_signals(STATE);

    void start_logging(STATE);
    void stop_logging(STATE);

    void start_jit(STATE);
    void stop_jit(STATE);
  };

}

#endif
