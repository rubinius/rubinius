#ifndef RBX_ENVIRONMENT_HPP
#define RBX_ENVIRONMENT_HPP

#include <string>
#include <stdexcept>

#include "vm.hpp"
#include "state.hpp"
#include "config_parser.hpp"
#include "configuration.hpp"

#include "memory/root.hpp"

#include "util/thread.hpp"

namespace rubinius {

  namespace memory {
    class FinalizerThread;
  }

  class ConfigParser;
  class QueryAgent;
  class SignalThread;

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

    utilities::thread::SpinLock fork_exec_lock_;
    utilities::thread::Mutex halt_lock_;

    memory::FinalizerThread* finalizer_thread_;

    std::string system_prefix_;
    std::string runtime_path_;

    memory::TypedRoot<Object*>* loader_;

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

    uint64_t signature() {
      return signature_;
    }

    utilities::thread::SpinLock& fork_exec_lock() {
      return fork_exec_lock_;
    }

    std::string& runtime_path() {
      return runtime_path_;
    }

    void set_loader(Object* loader) {
      loader_->set(loader);
    }

    Object* loader() {
      return loader_->get();
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
    void check_io_descriptors();
    void copy_argv(int argc, char** argv);
    void log_argv();
    void load_vm_options(int argc, char** argv);
    void load_argv(int argc, char** argv);
    void load_core(STATE, std::string root);
    void load_platform_conf(std::string dir);
    void load_conf(std::string path);
    void load_string(std::string str);
    void run_file(STATE, std::string path);
    void expand_config_value(std::string& cvar, const char* var, const char* value);
    void set_tmp_path();
    void set_username();
    void set_codedb_paths();
    void set_pid();
    void set_console_path();
    void boot();

    void after_exec(STATE);
    void after_fork_child(STATE);

    void halt(STATE, int exit_code);
    void atexit();

    void start_finalizer(STATE);

    void start_logging(STATE);
    void restart_logging(STATE);
    void stop_logging(STATE);
  };
}

#endif
