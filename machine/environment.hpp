#ifndef RBX_ENVIRONMENT_HPP
#define RBX_ENVIRONMENT_HPP

#include <string>
#include <stdexcept>

#include "machine.hpp"
#include "thread_state.hpp"
#include "config_parser.hpp"
#include "configuration.hpp"
#include "spinlock.hpp"

#include <mutex>

#include <sys/utsname.h>

namespace rubinius {

  namespace memory {
    class Collector;
  }

  class ConfigParser;

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

    std::string system_prefix_;

    Machine* _machine_;

    struct utsname _machine_info_;

    std::string _nodename_;
    std::string _username_;
    std::string _pid_;

  public:
    ThreadState* root_vm;
    ThreadState* state;

    ConfigParser  config_parser;

  private:
    void set_nodename();
    void set_username();
    void set_pid();

  public:
    Environment(int argc, char** argv, Machine* m);
    ~Environment();

    void initialize();

    int argc() const {
      return argc_;
    }

    char** argv() const {
      return argv_;
    }

    ConfigParser* user_variables() {
      // TODO: Machine
      return &config_parser;
    }

    const std::string& nodename() const {
      return _nodename_;
    }

    const std::string& username() const {
      return _username_;
    }

    const std::string& pid() const {
      return _pid_;
    }

    struct utsname* const machine_info() {
      return &_machine_info_;
    }

    void set_root_vm(ThreadState* vm) {
      root_vm = vm;
      // TODO: VM
      // state->set_vm(vm);
    }

    void setup_cpp_terminate();

    std::string executable_name();
    std::string system_prefix();
    bool verify_paths(std::string prefix, std::string& failure_reason);
    void check_io_descriptors();
    void copy_argv(int argc, char** argv);
    void log_argv();
    void load_vm_options(int argc, char** argv);
    void load_command_line(STATE);
    void load_core(STATE);
    void load_platform_conf(std::string dir);
    void load_conf(std::string path);
    void load_string(std::string str);
    void expand_config_value(std::string& cvar, const char* var, const char* value);
    void set_tmp_path();
    void set_codedb_paths();
    void set_console_path();

    void after_fork_child(STATE);

    NORETURN(void missing_core(const char* message));

    void atexit();

    void start_logging(STATE);
    void restart_logging(STATE);
  };
}

#endif
