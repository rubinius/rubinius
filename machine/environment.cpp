#include "config.h"
#include "paths.h"
#include "c_api.hpp"
#include "defines.hpp"
#include "environment.hpp"
#include "config_parser.hpp"
#include "memory.hpp"
#include "exception.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/code_db.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/jit.hpp"
#include "class/module.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/thread.hpp"

#include "diagnostics/codedb.hpp"
#include "diagnostics/machine.hpp"

#include "logger.hpp"

#include "memory/collector.hpp"

#include "signals.hpp"
#include "object_utils.hpp"

#include "on_stack.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#ifdef RBX_WINDOWS
#include "windows_compat.h"
#else
#include <dlfcn.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <pwd.h>
#include <dirent.h>

#include "missing/setproctitle.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/sysctl.h>
#endif

namespace rubinius {

  Environment::Environment(int argc, char** argv, Machine* m)
    : argc_(argc)
    , argv_(0)
    , _machine_(m)
    , _nodename_()
    , _username_()
    , _pid_()
  {
    String::init_hash();

    set_nodename();
    set_username();
    set_pid();

    copy_argv(argc, argv);
    ruby_init_setproctitle(argc, argv);

    state = root_vm = _machine_->threads()->create_thread_state("ruby.main");
    root_vm->set_main_thread();

    size_t stack_size = 0;
    struct rlimit rlim;

    if(getrlimit(RLIMIT_STACK, &rlim) == 0) {
      stack_size = rlim.rlim_cur;
    }

    root_vm->set_stack_bounds(stack_size);
    root_vm->set_current_thread();
  }

  void Environment::initialize() {
    load_vm_options(argc_, argv_);

    check_io_descriptors();

    NativeMethod::init_thread(state);

    start_logging(state);
    log_argv();

    if(_machine_->configuration()->log_config.value) {
      std::string options;

      config_parser.parsed_options(options);
      if(options.size()) {
        logger::write("config: %s", options.c_str());
      }
    }
  }

  Environment::~Environment() {
    for(int i = 0; i < argc_; i++) {
      delete[] argv_[i];
    }
    delete[] argv_;
  }

  void cpp_exception_bug() {
    logger::fatal("[BUG: Uncaught C++ exception]");
    logger::fatal("Please report this with the following backtrace to " \
        "https://github.com/rubinius/rubinius/issues");

    rubinius::bug();
  }

  void Environment::setup_cpp_terminate() {
    // Install a better terminate function to tell the user
    // there was a rubinius bug.
    std::set_terminate(cpp_exception_bug);
  }

  static void assign_io_descriptor(std::string dir, int std_fd, const char* desc) {
    std::string path = dir + desc;

    int fd = open(path.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0600);
    dup2(fd, std_fd);
    unlink(path.c_str());
  }

  void Environment::check_io_descriptors() {
    std::string dir = _machine_->configuration()->tmp_path.value;

    if(fcntl(STDIN_FILENO, F_GETFD) < 0 && errno == EBADF) {
      assign_io_descriptor(dir, STDIN_FILENO, "stdin");
    }

    if(fcntl(STDOUT_FILENO, F_GETFD) < 0 && errno == EBADF) {
      assign_io_descriptor(dir, STDOUT_FILENO, "stdout");
    }

    if(fcntl(STDERR_FILENO, F_GETFD) < 0 && errno == EBADF) {
      assign_io_descriptor(dir, STDERR_FILENO, "stderr");
    }
  }

  void Environment::start_logging(STATE) {
    logger::logger_level level = logger::eWarn;

    if(!_machine_->configuration()->log_level.value.compare("fatal")) {
      level = logger::eFatal;
    } else if(!_machine_->configuration()->log_level.value.compare("error")) {
      level = logger::eError;
    } else if(!_machine_->configuration()->log_level.value.compare("warn")) {
      level = logger::eWarn;
    } else if(!_machine_->configuration()->log_level.value.compare("info")) {
      level = logger::eInfo;
    } else if(!_machine_->configuration()->log_level.value.compare("debug")) {
      level = logger::eDebug;
    }

    if(!_machine_->configuration()->log_target.value.compare("syslog")) {
      logger::open(logger::eSyslog, level, RBX_PROGRAM_NAME);
    } else if(!_machine_->configuration()->log_target.value.compare("console")) {
      logger::open(logger::eConsoleLogger, level, RBX_PROGRAM_NAME);
    } else {
      expand_config_value(_machine_->configuration()->log_target.value, "$TMPDIR", _machine_->configuration()->tmp_path);
      expand_config_value(_machine_->configuration()->log_target.value, "$PROGRAM_NAME", RBX_PROGRAM_NAME);
      expand_config_value(_machine_->configuration()->log_target.value, "$USER", _username_.c_str());

      logger::open(logger::eFileLogger, level,
          _machine_->configuration()->log_target.value.c_str(),
          _machine_->configuration()->log_limit.value,
          _machine_->configuration()->log_archives.value,
          _machine_->configuration()->log_access.value);
    }
  }

  void Environment::restart_logging(STATE) {
    logger::close();
    start_logging(state);
  }

  void Environment::copy_argv(int argc, char** argv) {
    argv_ = new char* [argc+1];
    argv_[argc] = 0;

    for(int i = 0; i < argc; i++) {
      size_t size = strlen(argv[i]) + 1;
      argv_[i] = new char[size];
      strncpy(argv_[i], argv[i], size);
    }
  }

  void Environment::log_argv() {
    std::ostringstream args;

    for(int i = 0; argv_[i]; i++) {
      if(i > 0) args << " ";
      args << argv_[i];
    }

    logger::write("command line: %s", args.str().c_str());
  }

  static void read_config_file(FILE* fp, ConfigParser& config_parser) {
#define RBX_CONFIG_FILE_LINE_MAX    256

    char buf[RBX_CONFIG_FILE_LINE_MAX];
    while(fgets(buf, RBX_CONFIG_FILE_LINE_MAX, fp)) {
      int size = strlen(buf);
      if(buf[size-1] == '\n') buf[size-1] = '\0';
      if(strncmp(buf, "-X", 2) == 0) {
        config_parser.import_line(buf + 2);
      }
    }
  }

  void Environment::load_vm_options(int argc, char**argv) {
    /* We parse -X options from three sources in the following order:
     *
     *  1. The file $HOME/.rbxconfig if $HOME is defined.
     *  2. The file .rbxconfig in the current working directory.
     *  3. The RBXOPT environment variable.
     *  4. The command line options.
     *
     * This order permits environment and command line options to override
     * "application" configuration. Likewise, command line options can override
     * environment configuration.
     */

    char* home = getenv("HOME");
    if(home) {
      std::string config_path(home);
      config_path += "/.rbxconfig";

      if(FILE* fp = fopen(config_path.c_str(), "r")) {
        read_config_file(fp, config_parser);
      }
    }

    // Configuration file.
    if(FILE* fp = fopen(".rbxconfig", "r")) {
      read_config_file(fp, config_parser);
    } else if(FILE* fp = fopen(".rbxrc", "r")) {
      std::cerr << "Use of config file .rbxrc is deprecated, use .rbxconfig." << std::endl;
      read_config_file(fp, config_parser);
    }

    // Environment.
    char* rbxopt = getenv("RBXOPT");
    if(rbxopt) {
      char *e, *b = rbxopt = strdup(rbxopt);
      char *s = b + strlen(rbxopt);

      while(b < s) {
        while(*b && isspace(*b)) b++;

        e = b;
        while(*e && !isspace(*e)) e++;

        if(e - b > 0) {
          if(strncmp(b, "-X", 2) == 0) {
            *e = 0;
            config_parser.import_line(b + 2);
          }
          b = e + 1;
        }
      }

      free(rbxopt);
    }

    // Command line.
    for(int i=1; i < argc; i++) {
      char* arg = argv[i];

      if(strcmp(arg, "--") == 0) {
        break;
      }

      if(strncmp(arg, "-X", 2) == 0) {
        config_parser.import_line(arg + 2);

      /* If we hit the first non-option, break out so in the following
       * command line, the first 'rbx' doesn't consume '-Xprofile':
       *
       *   rbx bundle exec rbx -Xprofile blah
       */
      } else if(arg[0] != '-') {
        break;
      }
    }

    config_parser.update_configuration(_machine_->configuration());

    set_tmp_path();
    set_console_path();
    set_codedb_paths();
  }

  void Environment::expand_config_value(std::string& cvar,
      const char* var, const char* value)
  {
    size_t index = cvar.find(var);

    if(index != std::string::npos) {
      cvar.replace(index, strlen(var), value);
    }
  }

  void Environment::set_tmp_path() {
    if(!_machine_->configuration()->tmp_path.value.compare("$TMPDIR")) {
      std::ostringstream path;
      const char* tmp = getenv("TMPDIR");

      if(tmp) {
        path << tmp;
        if(tmp[strlen(tmp)-1] != '/') path << "/";
      } else {
        path << "/tmp/";
      }

      _machine_->configuration()->tmp_path.value.assign(path.str());
    }
  }

  void Environment::set_nodename() {
    uname(&_machine_info_);
    _nodename_.assign(_machine_info_.nodename);
  }

  void Environment::set_username() {
    struct passwd *user_passwd = getpwuid(getuid());

    _username_.assign(user_passwd->pw_name);
  }

  void Environment::set_pid() {
    _pid_.assign(std::to_string(getpid()));
  }

  void Environment::set_console_path() {
    std::string path(_machine_->configuration()->console_path.value);

    expand_config_value(path, "$TMPDIR", _machine_->configuration()->tmp_path);
    expand_config_value(path, "$PROGRAM_NAME", RBX_PROGRAM_NAME);
    expand_config_value(path, "$USER", _username_.c_str());

    _machine_->configuration()->console_path.value.assign(path);
  }

  void Environment::set_codedb_paths() {
    if(_machine_->configuration()->codedb_core_path.value.size() == 0) {
      _machine_->configuration()->codedb_core_path.value.assign(system_prefix() + RBX_CODEDB_PATH);
    }

    std::string cache_path(_machine_->configuration()->codedb_cache_path.value);

    expand_config_value(cache_path, "$TMPDIR", _machine_->configuration()->tmp_path);
    expand_config_value(cache_path, "$PROGRAM_NAME", RBX_PROGRAM_NAME);
    expand_config_value(cache_path, "$USER", _username_.c_str());

    _machine_->configuration()->codedb_cache_path.value.assign(cache_path);
  }

  void Environment::load_command_line(STATE) {
    String* str = 0;
    Encoding* enc = Encoding::default_external(state);

    Array* os_ary = Array::create(state, argc_);
    for(int i = 0; i < argc_; i++) {
      str = String::create(state, argv_[i]);
      str->encoding(state, enc);
      os_ary->set(state, i, str);
    }

    G(rubinius)->set_const(state, "OS_ARGV", os_ary);

    char buf[MAXPATHLEN];
    str = String::create(state, getcwd(buf, MAXPATHLEN));
    str->encoding(state, enc);
    G(rubinius)->set_const(state, "OS_STARTUP_DIR", str);

    str = String::create(state, argv_[0]);
    str->encoding(state, enc);
    G(object)->set_const(state, "ARG0", str);

    Array* ary = Array::create(state, argc_ - 1);
    int which_arg = 0;
    bool skip_xflags = true;

    for(int i=1; i < argc_; i++) {
      char* arg = argv_[i];

      if(strcmp(arg, "--") == 0) {
        skip_xflags = false;
      } else if(strncmp(arg, "-X", 2) == 0) {
        if(skip_xflags) continue;
      } else if(arg[1] != '-') {
        skip_xflags = false;
      }

      str = String::create(state, arg);
      str->taint(state);
      str->encoding(state, enc);
      ary->set(state, which_arg++, str);
    }

    G(object)->set_const(state, "ARGV", ary);

    // Now finish up with the config
    if(_machine_->configuration()->print_config > 1) {
      std::cout << "========= Configuration =========\n";
      _machine_->configuration()->print(true);
      std::cout << "=================================\n";
    } else if(_machine_->configuration()->print_config) {
      _machine_->configuration()->print();
    }

    _machine_->c_api()->set_use_capi_lock(_machine_->configuration()->capi_lock);
  }

  void Environment::load_platform_conf(std::string dir) {
    std::string path = dir + "/platform.conf";
    std::ifstream stream(path.c_str());
    if(!stream) {
      std::string error = "Unable to load " + path + ", it is missing";
      throw std::runtime_error(error);
    }

    config_parser.import_stream(stream);
  }

  void Environment::load_conf(std::string path) {
    std::ifstream stream(path.c_str());
    if(!stream) {
      std::string error = "Unable to load " + path + ", it is missing";
      throw std::runtime_error(error);
    }

    config_parser.import_stream(stream);
  }

  void Environment::load_string(std::string str) {
    config_parser.import_many(str);
  }

  void Environment::after_fork_child(STATE) {
    set_pid();

    restart_logging(state);
  }

  void Environment::missing_core(const char* message) {
    std::cerr << std::endl;
    std::cerr << message << std::endl << std::endl;
    std::cerr << "Rubinius was configured to find the directories relative to:" << std::endl;
    std::cerr << std::endl << "  " << RBX_PREFIX_PATH << std::endl << std::endl;
    std::cerr << "Set the environment variable RBX_PREFIX_PATH to the directory";
    std::cerr << std::endl;
    std::cerr << "that is the prefix of the following runtime directories:" << std::endl;
    std::cerr << std::endl;
    std::cerr << "      BIN_PATH: " << RBX_BIN_PATH << std::endl;
    std::cerr << "  RUNTIME_PATH: " << RBX_RUNTIME_PATH << std::endl;
    std::cerr << "   CODEDB_PATH: " << RBX_CODEDB_PATH << std::endl;
    std::cerr << "     CORE_PATH: " << RBX_CORE_PATH << std::endl;
    std::cerr << "      LIB_PATH: " << RBX_LIB_PATH << std::endl;
    std::cerr << "     SITE_PATH: " << RBX_SITE_PATH << std::endl;
    std::cerr << "   VENDOR_PATH: " << RBX_VENDOR_PATH << std::endl;
    std::cerr << "     GEMS_PATH: " << RBX_GEMS_PATH << std::endl;
    std::cerr << std::endl;

    exit(1);
  }

  /**
   * Loads the runtime core library files stored in runtime/core. This method
   * is called after the VM has completed bootstrapping, and is ready to load
   * Ruby code.
   */
  void Environment::load_core(STATE) {
    MachineException::guard(state, false, [&]{
        CodeDB::open(state,
            _machine_->configuration()->codedb_core_path.value,
            _machine_->configuration()->codedb_cache_path.value);
      });
  }

  std::string Environment::executable_name() {
    char name[PATH_MAX];
    memset(name, 0, PATH_MAX);

#ifdef __APPLE__
    uint32_t size = PATH_MAX;
    if(_NSGetExecutablePath(name, &size) == 0) {
      return name;
    } else if(realpath(argv_[0], name)) {
      return name;
    }
#elif defined(__FreeBSD__) || defined(__NetBSD__)
    size_t size = PATH_MAX;
    int oid[4];

    oid[0] = CTL_KERN;
#if defined(__FreeBSD__)
    oid[1] = KERN_PROC;
    oid[2] = KERN_PROC_PATHNAME;
    oid[3] = getpid();
#else
    oid[1] = KERN_PROC_ARGS;
    oid[2] = getpid();
    oid[3] = KERN_PROC_PATHNAME;
#endif

    if(sysctl(oid, 4, name, &size, 0, 0) == 0) {
      return name;
    } else if(realpath(argv_[0], name)) {
      return name;
    }
#elif defined(__linux__)
    {
      if(readlink("/proc/self/exe", name, PATH_MAX) >= 0) {
        return name;
      } else if(realpath(argv_[0], name)) {
        return name;
      }
    }
#else
    if(realpath(argv_[0], name)) {
      return name;
    }
#endif

    return argv_[0];
  }

  bool Environment::verify_paths(std::string prefix, std::string& failure_reason) {
    struct stat st;

    std::string codedb = prefix + RBX_CODEDB_PATH;
    if(stat(codedb.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) {
      failure_reason.assign("the CodeDB path is invalid: ").append(codedb);
      return false;
    }

    std::string cache = codedb + "/cache";
    if(stat(cache.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) {
      failure_reason.assign("the CodeDB cache path is invalid: ").append(cache);
      return false;
    }

    std::string source = codedb + "/source";
    if(stat(source.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) {
      failure_reason.assign("the CodeDB source path is invalid: ").append(source);
      return false;
    }

    std::string bin = prefix + RBX_BIN_PATH;
    if(stat(bin.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) {
      failure_reason.assign("the bin path is invalid: ").append(bin);
      return false;
    }

    return true;
  }

  std::string Environment::system_prefix() {
    if(!system_prefix_.empty()) return system_prefix_;

    std::string failure_reason;

    // 1. Check if our configure prefix is overridden by the environment.
    const char* path = getenv("RBX_PREFIX_PATH");
    if(path && verify_paths(path, failure_reason)) {
      system_prefix_ = path;
      return path;
    }

    // 2. Check if our configure prefix is valid.
    path = RBX_PREFIX_PATH;
    if(verify_paths(path, failure_reason)) {
      system_prefix_ = path;
      return path;
    }

    // 3. Check if we can derive paths from the executable name.
    // TODO: For Windows, substitute '/' for '\\'
    std::string name = executable_name();
    size_t exe = name.rfind('/');

    if(exe != std::string::npos) {
      std::string prefix = name.substr(0, exe - strlen(RBX_BIN_PATH));
      if(verify_paths(prefix, failure_reason)) {
        system_prefix_ = prefix;
        return prefix;
      }
    }

    std::string error("Unable to find Rubinius runtime directories: ");
    error.append(failure_reason);

    missing_core(error.c_str());
  }
}
