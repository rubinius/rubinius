#include "config.h"
#include "paths.h"
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

#include "logger.hpp"

#include "memory/immix_marker.hpp"
#include "memory/finalizer.hpp"

#include "signal.hpp"
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

#ifdef __FreeBSD__
#include <sys/sysctl.h>
#endif

namespace rubinius {

  Environment::Environment(int argc, char** argv)
    : argc_(argc)
    , argv_(0)
    , fork_exec_lock_()
    , halt_lock_()
    , finalizer_thread_(NULL)
    , loader_(NULL)
  {
    String::init_hash();

    copy_argv(argc, argv);
    ruby_init_setproctitle(argc, argv);

    shared = new SharedState(this, config, config_parser);

    load_vm_options(argc_, argv_);

    check_io_descriptors();

    root_vm = shared->thread_nexus()->new_vm(shared, "ruby.main");
    root_vm->set_main_thread();
    shared->set_root_vm(root_vm);

    size_t stack_size = 0;
    struct rlimit rlim;

    if(getrlimit(RLIMIT_STACK, &rlim) == 0) {
      stack_size = rlim.rlim_cur;
    }

    root_vm->set_stack_bounds(stack_size);
    root_vm->set_current_thread();

    state = new State(root_vm);

    loader_ = new memory::TypedRoot<Object*>(state);

    NativeMethod::init_thread(state);

    start_logging(state);
    log_argv();

    if(config.system_log_config.value) {
      std::string options;

      config_parser.parsed_options(options);
      if(options.size()) {
        logger::write("config: %s", options.c_str());
      }
    }
  }

  Environment::~Environment() {
    delete finalizer_thread_;

    VM::discard(state, root_vm);
    delete shared;
    delete state;

    for(int i = 0; i < argc_; i++) {
      delete[] argv_[i];
    }
    delete[] argv_;

    stop_logging(state);
  }

  void cpp_exception_bug() {
    logger::fatal("[BUG: Uncaught C++ exception]");
    logger::fatal("Please report this with the following backtrace to " \
        "https://github.com/rubinius/rubinius/issues");

    rubinius::abort();
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
    std::string dir = config.system_tmp.value;

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

  void Environment::stop_logging(STATE) {
    logger::close();
  }

  void Environment::start_finalizer(STATE) {
    finalizer_thread_ = new memory::FinalizerThread(state);
    finalizer_thread_->start(state);
  }

  void Environment::start_logging(STATE) {
    logger::logger_level level = logger::eWarn;

    if(!config.system_log_level.value.compare("fatal")) {
      level = logger::eFatal;
    } else if(!config.system_log_level.value.compare("error")) {
      level = logger::eError;
    } else if(!config.system_log_level.value.compare("warn")) {
      level = logger::eWarn;
    } else if(!config.system_log_level.value.compare("info")) {
      level = logger::eInfo;
    } else if(!config.system_log_level.value.compare("debug")) {
      level = logger::eDebug;
    }

    if(!config.system_log.value.compare("syslog")) {
      logger::open(logger::eSyslog, level, RBX_PROGRAM_NAME);
    } else if(!config.system_log.value.compare("console")) {
      logger::open(logger::eConsoleLogger, level, RBX_PROGRAM_NAME);
    } else {
      expand_config_value(config.system_log.value, "$TMPDIR", config.system_tmp);
      expand_config_value(config.system_log.value, "$PROGRAM_NAME", RBX_PROGRAM_NAME);
      expand_config_value(config.system_log.value, "$USER", shared->username.c_str());

      logger::open(logger::eFileLogger, level,
          config.system_log.value.c_str(),
          config.system_log_limit.value,
          config.system_log_archives.value,
          config.system_log_access.value);
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

    config_parser.update_configuration(config);

    set_tmp_path();
    set_username();
    set_pid();
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
    if(!config.system_tmp.value.compare("$TMPDIR")) {
      std::ostringstream path;
      const char* tmp = getenv("TMPDIR");

      if(tmp) {
        path << tmp;
        if(tmp[strlen(tmp)-1] != '/') path << "/";
      } else {
        path << "/tmp/";
      }

      config.system_tmp.value.assign(path.str());
    }
  }

  void Environment::set_username() {
    struct passwd *user_passwd = getpwuid(getuid());

    shared->username.assign(user_passwd->pw_name);
  }

  void Environment::set_pid() {
    std::ostringstream pid;
    pid << getpid();
    shared->pid.assign(pid.str());
  }

  void Environment::set_console_path() {
    std::string path(config.system_console_path.value);

    expand_config_value(path, "$TMPDIR", config.system_tmp);
    expand_config_value(path, "$PROGRAM_NAME", RBX_PROGRAM_NAME);
    expand_config_value(path, "$USER", shared->username.c_str());

    config.system_console_path.value.assign(path);
  }

  void Environment::set_codedb_paths() {
    if(config.codedb_core_path.value.size() == 0) {
      config.codedb_core_path.value.assign(system_prefix() + RBX_CODEDB_PATH);
    }

    std::string cache_path(config.codedb_cache_path.value);

    expand_config_value(cache_path, "$TMPDIR", config.system_tmp);
    expand_config_value(cache_path, "$PROGRAM_NAME", RBX_PROGRAM_NAME);
    expand_config_value(cache_path, "$USER", shared->username.c_str());

    config.codedb_cache_path.value.assign(cache_path);
  }

  void Environment::load_argv(int argc, char** argv) {
    String* str = 0;
    Encoding* enc = Encoding::default_external(state);

    Array* os_ary = Array::create(state, argc);
    for(int i = 0; i < argc; i++) {
      str = String::create(state, argv[i]);
      str->encoding(state, enc);
      os_ary->set(state, i, str);
    }

    G(rubinius)->set_const(state, "OS_ARGV", os_ary);

    char buf[MAXPATHLEN];
    str = String::create(state, getcwd(buf, MAXPATHLEN));
    str->encoding(state, enc);
    G(rubinius)->set_const(state, "OS_STARTUP_DIR", str);

    str = String::create(state, argv[0]);
    str->encoding(state, enc);
    state->vm()->set_const("ARG0", str);

    Array* ary = Array::create(state, argc - 1);
    int which_arg = 0;
    bool skip_xflags = true;

    for(int i=1; i < argc; i++) {
      char* arg = argv[i];

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

    state->vm()->set_const("ARGV", ary);

    // Now finish up with the config
    if(config.print_config > 1) {
      std::cout << "========= Configuration =========\n";
      config.print(true);
      std::cout << "=================================\n";
    } else if(config.print_config) {
      config.print();
    }

    state->shared().set_use_capi_lock(config.capi_lock);
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
    fork_exec_lock_.init();
    halt_lock_.init();

    set_pid();

    restart_logging(state);
  }

  void Environment::missing_core(const char* message) {
    std::cerr << std::endl;
    std::cerr << message << std::endl;
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

  void Environment::halt(STATE, int exit_code) {
    utilities::thread::Mutex::LockGuard guard(halt_lock_);

    state->shared().set_halting();

    if(state->shared().config.system_log_lifetime.value) {
      logger::write("process: exit: %s %d %lld %fs %fs",
          shared->pid.c_str(), exit_code,
          shared->codedb_metrics().load_count,
          timer::elapsed_seconds(shared->codedb_metrics().load_ns),
          shared->run_time());
    }

    {
      UnmanagedPhase unmanaged(state);
      shared->machine_threads()->shutdown(state);
    }

    shared->finalizer()->dispose(state);

    shared->thread_nexus()->lock(state, state->vm());

    shared->finalizer()->finish(state);

    if(!G(coredb)->nil_p()) G(coredb)->close(state);

    if(Memory* om = state->memory()) {
      if(memory::ImmixMarker* im = om->immix_marker()) {
        im->stop(state);
      }
    }

    NativeMethod::cleanup_thread(state);

    state->shared().signals()->stop(state);

    exit(exit_code);
  }

  /**
   * Loads the runtime core library files stored in runtime/core. This method
   * is called after the VM has completed bootstrapping, and is ready to load
   * Ruby code.
   */
  void Environment::load_core(STATE) {
    try {
      CodeDB::open(state,
          config.codedb_core_path.value,
          config.codedb_cache_path.value);
    } catch(RubyException& exc) {
      exc.show(state);
      exit(1);
    } catch(Assertion& exc) {
      exc.print_backtrace();
      exit(1);
    } catch(TypeError& exc) {
      exc.print_backtrace();
      exit(1);
    } catch(...) {
      std::cout << "Unknown C++ exception loading CodeDB" << std::endl;
      exit(1);
    }
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
#elif defined(__FreeBSD__)
    size_t size = PATH_MAX;
    int oid[4];

    oid[0] = CTL_KERN;
    oid[1] = KERN_PROC;
    oid[2] = KERN_PROC_PATHNAME;
    oid[3] = getpid();

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

  bool Environment::verify_paths(std::string prefix) {
    struct stat st;

    std::string codedb = prefix + RBX_CODEDB_PATH;
    if(stat(codedb.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

    std::string cache = codedb + "/cache";
    if(stat(cache.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) return false;

    std::string source = codedb + "/source";
    if(stat(source.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

    std::string bin = prefix + RBX_BIN_PATH;
    if(stat(bin.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

    return true;
  }

  std::string Environment::system_prefix() {
    if(!system_prefix_.empty()) return system_prefix_;

    // 1. Check if our configure prefix is overridden by the environment.
    const char* path = getenv("RBX_PREFIX_PATH");
    if(path && verify_paths(path)) {
      system_prefix_ = path;
      return path;
    }

    // 2. Check if our configure prefix is valid.
    path = RBX_PREFIX_PATH;
    if(verify_paths(path)) {
      system_prefix_ = path;
      return path;
    }

    // 3. Check if we can derive paths from the executable name.
    // TODO: For Windows, substitute '/' for '\\'
    std::string name = executable_name();
    size_t exe = name.rfind('/');

    if(exe != std::string::npos) {
      std::string prefix = name.substr(0, exe - strlen(RBX_BIN_PATH));
      if(verify_paths(prefix)) {
        system_prefix_ = prefix;
        return prefix;
      }
    }

    missing_core("unable to find Rubinius runtime directories.");
  }

  void Environment::boot() {
    std::string codedb_path = system_prefix() + RBX_CODEDB_PATH;

    {
      timer::StopWatch<timer::microseconds> timer(
          state->shared().boot_metrics().platform_us);

      load_platform_conf(codedb_path);
    }

    {
      timer::StopWatch<timer::microseconds> timer(
          state->shared().boot_metrics().memory_us);

      shared->om = new Memory(state->vm(), *shared);
    }

    shared->set_initialized();

    state->vm()->managed_phase(state);

    {
      timer::StopWatch<timer::microseconds> timer(
          state->shared().boot_metrics().fields_us);

      TypeInfo::auto_learn_fields(state);
    }

    {
      timer::StopWatch<timer::microseconds> timer(
          state->shared().boot_metrics().ontology_us);

      state->vm()->bootstrap_ontology(state);
    }

    start_finalizer(state);

    load_argv(argc_, argv_);

    // Start the main Ruby thread.
    Thread* main = 0;
    OnStack<1> os(state, main);

    {
      timer::StopWatch<timer::microseconds> timer(
          state->shared().boot_metrics().main_thread_us);

      main = Thread::create(state, state->vm(), Thread::main_thread);
      main->start_thread(state, Thread::run);
    }

    VM* vm = SignalThread::new_vm(state);
    vm->set_stack_bounds(state->vm()->stack_size());

    State main_state(vm);
    state->shared().start_signals(&main_state);

    state->shared().set_running();
  }
}
