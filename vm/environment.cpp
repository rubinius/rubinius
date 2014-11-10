#include "config.h"
#include "signature.h"
#include "paths.h"
#include "prelude.hpp"
#include "environment.hpp"
#include "config_parser.hpp"
#include "compiled_file.hpp"
#include "object_memory.hpp"

#include "exception.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/jit.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/native_method.hpp"

#include "util/logger.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#if RBX_LLVM_API_VER == 208
#include <llvm/System/Threading.h>
#elif RBX_LLVM_API_VER == 209
#include <llvm/Support/Threading.h>
#endif
#include <llvm/Support/ManagedStatic.h>
#endif

#include "gc/immix_marker.hpp"
#include "gc/finalize.hpp"

#include "signal.hpp"
#include "object_utils.hpp"

#include "instruments/tooling.hpp"

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
#include <pwd.h>

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
    , signature_(0)
    , version_(0)
    , signal_handler_(NULL)
    , finalizer_handler_(NULL)
  {
#ifdef ENABLE_LLVM
#if RBX_LLVM_API_VER < 305
    if(!llvm::llvm_start_multithreaded()) {
      assert(0 && "llvm doesn't support threading!");
    }
#endif
#endif

    String::init_hash();

    VM::init_stack_size();

    copy_argv(argc, argv);
    ruby_init_setproctitle(argc, argv);

    shared = new SharedState(this, config, config_parser);

    load_vm_options(argc_, argv_);

    root_vm = shared->new_vm();
    root_vm->metrics()->init(metrics::eRubyMetrics);
    state = new State(root_vm);

    start_logging(state);
  }

  Environment::~Environment() {
    stop_logging(state);

    delete signal_handler_;
    delete finalizer_handler_;

    VM::discard(state, root_vm);
    SharedState::discard(shared);
    delete state;

    for(int i = 0; i < argc_; i++) {
      delete[] argv_[i];
    }
    delete[] argv_;
  }

  void cpp_exception_bug() {
    std::cerr << "[BUG] Uncaught C++ internal exception\n";
    std::cerr << "So sorry, it appears that you've encountered an internal\n";
    std::cerr << "bug. Please report this on the rubinius issue tracker.\n";
    std::cerr << "Include the following backtrace in the issue:\n\n";

    rubinius::abort();
  }

  void Environment::setup_cpp_terminate() {
    // Install a better terminate function to tell the user
    // there was a rubinius bug.
    std::set_terminate(cpp_exception_bug);
  }

  void Environment::start_jit(STATE) {
    utilities::thread::SpinLock::LockGuard lg(state->shared().llvm_state_lock());

    if(state->shared().config.jit_disabled) return;

#ifdef ENABLE_LLVM
    if(!state->shared().llvm_state) {
      state->shared().llvm_state = new LLVMState(state);
    }
#endif
  }

  void Environment::stop_logging(STATE) {
    utilities::logger::close();
  }

  void Environment::stop_jit(STATE) {
    utilities::thread::SpinLock::LockGuard lg(state->shared().llvm_state_lock());

    if(state->shared().config.jit_disabled) return;

#ifdef ENABLE_LLVM
    if(state->shared().llvm_state) {
      state->shared().llvm_state->stop(state);
    }
#endif
  }

  void Environment::start_signals(STATE) {
    state->vm()->set_run_signals(true);
    signal_handler_ = new SignalHandler(state, config);
  }

  void Environment::start_finalizer(STATE) {
    finalizer_handler_ = new FinalizerHandler(state);
  }

  void Environment::start_logging(STATE) {
    utilities::logger::logger_level level = utilities::logger::eWarn;

    if(!config.system_log_level.value.compare("fatal")) {
      level = utilities::logger::eFatal;
    } else if(!config.system_log_level.value.compare("error")) {
      level = utilities::logger::eError;
    } else if(!config.system_log_level.value.compare("warn")) {
      level = utilities::logger::eWarn;
    } else if(!config.system_log_level.value.compare("info")) {
      level = utilities::logger::eInfo;
    } else if(!config.system_log_level.value.compare("debug")) {
      level = utilities::logger::eDebug;
    }

    if(!config.system_log.value.compare("syslog")) {
      utilities::logger::open(utilities::logger::eSyslog, RBX_PROGRAM_NAME, level);
    } else if(!config.system_log.value.compare("console")) {
      utilities::logger::open(utilities::logger::eConsoleLogger, RBX_PROGRAM_NAME, level);
    } else {
      const char* place_holder = "$PROGRAM_NAME";
      size_t index = config.system_log.value.find(place_holder);

      if(index != std::string::npos) {
        config.system_log.value.replace(index, strlen(place_holder), RBX_PROGRAM_NAME);
      }

      int fd;

      if((fd = open(config.system_log.value.c_str(), O_CREAT, 0644)) > 0) {
        close(fd);
      } else if(errno == EACCES) {
        std::ostringstream path;

        if(const char* s = strrchr(config.system_log.value.c_str(), '/')) {
          path << (s + 1);
        } else {
          path << config.system_log.value.c_str();
        }

        config.system_log.value.assign(config.system_tmp.value + path.str());

        if((fd = open(config.system_log.value.c_str(), O_CREAT, 0644)) > 0) {
          close(fd);
        } else {
          std::cerr << RBX_PROGRAM_NAME << ": unable to open log: "
                    << config.system_log.value.c_str() << std::endl;
        }
      }

      utilities::logger::open(utilities::logger::eFileLogger,
          config.system_log.value.c_str(), level);
    }
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

  void Environment::load_vm_options(int argc, char**argv) {
    /* We parse -X options from three sources in the following order:
     *
     *  1. The file .rbxrc in the current working directory.
     *  2. The RBXOPT environment variable.
     *  3. The command line options.
     *
     * This order permits environment and command line ot override
     * "application" configuration. Likewise, command line can override
     * environment configuration.
     */

#define RBX_CONFIG_FILE_LINE_MAX    256

    // Configuration file.
    if(FILE* fp = fopen(".rbxrc", "r")) {
      char buf[RBX_CONFIG_FILE_LINE_MAX];
      while(fgets(buf, RBX_CONFIG_FILE_LINE_MAX, fp)) {
        int size = strlen(buf);
        if(buf[size-1] == '\n') buf[size-1] = '\0';
        if(strncmp(buf, "-X", 2) == 0) {
          config_parser.import_line(buf + 2);
        }
      }
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
    set_fsapi_path();
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

  void Environment::set_fsapi_path() {
    std::ostringstream path;
    struct passwd *user_passwd = getpwuid(getuid());

    if(!config.system_fsapi_path.value.compare("$TMPDIR")) {
      path << config.system_tmp.value;
    } else {
      std::string cpath = config.system_fsapi_path.value;

      path << cpath;
      if(cpath[cpath.size() - 1] != '/') path << "/";
    }

    path << "rbx-" << user_passwd->pw_name << "-" << getpid();

    shared->fsapi_path.assign(path.str());

    create_fsapi(state);
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

  void Environment::load_directory(std::string dir) {
    // Read the version-specific load order file.
    std::string path = dir + "/load_order.txt";
    std::ifstream stream(path.c_str());
    if(!stream) {
      std::string msg = "Unable to load directory, " + path + " is missing";
      throw std::runtime_error(msg);
    }

    while(!stream.eof()) {
      std::string line;
      stream >> line;
      stream.get(); // eat newline

      // skip empty lines
      if(line.empty()) continue;

      run_file(dir + "/" + line);
    }
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

  void Environment::boot_vm() {
    state->vm()->initialize_as_root();
  }

  void Environment::run_file(std::string file) {
    std::ifstream stream(file.c_str());
    if(!stream) {
      std::string msg = std::string("Unable to open file to run: ");
      msg.append(file);
      throw std::runtime_error(msg);
    }

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") {
      std::ostringstream msg;
      msg << "attempted to open a bytecode file with invalid magic identifier"
          << ": path: " << file << ", magic: " << cf->magic;
      throw std::runtime_error(msg.str().c_str());
    }
    if((signature_ > 0 && cf->signature != signature_)
        || cf->version != version_) {
      throw BadKernelFile(file);
    }

    cf->execute(state);

    if(state->vm()->thread_state()->raise_reason() == cException) {
      Exception* exc = as<Exception>(state->vm()->thread_state()->current_exception());
      std::ostringstream msg;

      msg << "exception detected at toplevel: ";
      if(!exc->reason_message()->nil_p()) {
        if(String* str = try_as<String>(exc->reason_message())) {
          msg << str->c_str(state);
        } else {
          msg << "<non-string Exception message>";
        }
      } else if(Exception::argument_error_p(state, exc)) {
        msg << "given "
            << as<Fixnum>(exc->get_ivar(state, state->symbol("@given")))->to_native()
            << ", expected "
            << as<Fixnum>(exc->get_ivar(state, state->symbol("@expected")))->to_native();
      }
      msg << " (" << exc->klass()->debug_str(state) << ")";
      std::cout << msg.str() << "\n";
      exc->print_locations(state);
      Assertion::raise(msg.str().c_str());
    }

    delete cf;
  }

  void Environment::before_exec(STATE) {
    remove_fsapi(state);
  }

  void Environment::after_exec(STATE) {
    create_fsapi(state);
  }

  void Environment::create_fsapi(STATE) {
    if(mkdir(shared->fsapi_path.c_str(), shared->config.system_fsapi_access) < 0) {
      utilities::logger::error("%s: unable to create FSAPI path", strerror(errno));
    }

    // The umask setting will override our permissions for mkdir().
    if(chmod(shared->fsapi_path.c_str(), shared->config.system_fsapi_access) < 0) {
      utilities::logger::error("%s: unable to set mode for FSAPI path", strerror(errno));
    }
  }

  void Environment::remove_fsapi(STATE) {
    if(rmdir(shared->fsapi_path.c_str()) < 0) {
      utilities::logger::error("%s: unable to remove FSAPI path", strerror(errno));
    }
  }

  void Environment::halt_and_exit(STATE) {
    halt(state);
    int code = exit_code(state);
#ifdef ENABLE_LLVM
    llvm::llvm_shutdown();
#endif
    exit(code);
  }

  void Environment::halt(STATE) {
    state->shared().tool_broker()->shutdown(state);
    if(ImmixMarker* im = state->memory()->immix_marker()) {
      im->shutdown(state);
    }

    stop_jit(state);

    GCTokenImpl gct;

    root_vm->set_call_frame(0);

    // Handle an edge case where another thread is waiting to stop the world.
    if(state->shared().should_stop()) {
      state->checkpoint(gct, 0);
    }

    {
      GCIndependent guard(state, 0);
      shared->auxiliary_threads()->shutdown(state);
      root_vm->set_call_frame(0);
    }

    shared->finalizer_handler()->finish(state, gct);

    root_vm->set_call_frame(0);

    // Hold everyone.
    while(!state->stop_the_world()) {
      state->checkpoint(gct, 0);
    }

    NativeMethod::cleanup_thread(state);

    remove_fsapi(state);
  }

  /**
   * Returns the exit code to use when exiting the rbx process.
   */
  int Environment::exit_code(STATE) {
    if(state->vm()->thread_state()->raise_reason() == cExit) {
      if(Fixnum* fix = try_as<Fixnum>(state->vm()->thread_state()->raise_value())) {
        return fix->to_native();
      } else {
        return -1;
      }
    }

    return 0;
  }

  /**
   * Loads the runtime kernel files stored in /runtime.
   * These files consist of the compiled Ruby /kernel code in .rbc files, which
   * are needed to bootstrap the Ruby kernel.
   * This method is called after the VM has completed bootstrapping, and is
   * ready to load Ruby code.
   *
   * @param root The path to the /runtime directory. All kernel loading is
   *             relative to this path.
   */
  void Environment::load_kernel(std::string root) {
    // Check that the index file exists; this tells us which sub-directories to
    // load, and the order in which to load them
    std::string index = root + "/index";
    std::ifstream stream(index.c_str());
    if(!stream) {
      std::string error = "Unable to load kernel index: " + root;
      throw std::runtime_error(error);
    }

    version_ = as<Fixnum>(G(rubinius)->get_const(
          state, state->symbol("RUBY_LIB_VERSION")))->to_native();

    // Load alpha
    run_file(root + "/alpha.rbc");

    while(!stream.eof()) {
      std::string line;

      stream >> line;
      stream.get(); // eat newline

      // skip empty lines
      if(line.empty()) continue;

      load_directory(root + "/" + line);
    }
  }

  void Environment::load_tool() {
    if(!state->shared().config.tool_to_load.set_p()) return;
    std::string path = std::string(state->shared().config.tool_to_load.value) + ".";

#ifdef _WIN32
    path += "dll";
#else
  #ifdef __APPLE_CC__
    path += "bundle";
  #else
    path += "so";
  #endif
#endif

    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if(!handle) {
      path = std::string(RBX_LIB_PATH) + "/" + path;

      handle = dlopen(path.c_str(), RTLD_NOW);
      if(!handle) {
        std::cerr << "Unable to load tool '" << path << "': " << dlerror() << "\n";
        return;
      }
    }

    void* sym = dlsym(handle, "Tool_Init");
    if(!sym) {
      std::cerr << "Failed to initialize tool '" << path << "': " << dlerror() << "\n";
    } else {
      typedef int (*init_func)(rbxti::Env* env);
      init_func init = (init_func)sym;

      if(!init(state->vm()->tooling_env())) {
        std::cerr << "Tool '" << path << "' reported failure to init.\n";
      }
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

  bool Environment::load_signature(std::string runtime) {
    std::string path = runtime;

    path += "/signature";

    std::ifstream signature(path.c_str());
    if(signature) {
      signature >> signature_;

      if(signature_ != RBX_SIGNATURE) return false;

      signature.close();

      return true;
    }

    return false;
  }

  bool Environment::verify_paths(std::string prefix) {
    struct stat st;

    std::string dir = prefix + RBX_RUNTIME_PATH;
    if(stat(dir.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

    if(!load_signature(dir)) return false;

    dir = prefix + RBX_BIN_PATH;
    if(stat(dir.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

    dir = prefix + RBX_KERNEL_PATH;
    if(stat(dir.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

    dir = prefix + RBX_LIB_PATH;
    if(stat(dir.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) return false;

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

    throw MissingRuntime("FATAL ERROR: unable to find Rubinius runtime directories.");
  }

  /**
   * Runs rbx from the filesystem. Searches for the Rubinius runtime files
   * according to the algorithm in find_runtime().
   */
  void Environment::run_from_filesystem() {
    int i = 0;
    state->vm()->set_root_stack(reinterpret_cast<uintptr_t>(&i),
                                VM::cStackDepthMax);

    std::string runtime = system_prefix() + RBX_RUNTIME_PATH;

    load_platform_conf(runtime);
    boot_vm();
    start_finalizer(state);

    load_argv(argc_, argv_);

    start_signals(state);
    state->vm()->initialize_config();

    load_tool();

    start_jit(state);

    G(rubinius)->set_const(state, "Signature", Integer::from(state, signature_));

    G(rubinius)->set_const(state, "RUNTIME_PATH", String::create(state,
                           runtime.c_str(), runtime.size()));

    load_kernel(runtime);
    shared->finalizer_handler()->start_thread(state);

    run_file(runtime + "/loader.rbc");

    state->vm()->thread_state()->clear();

    state->shared().start_console(state);
    state->shared().start_metrics(state);

    Object* loader = G(rubinius)->get_const(state, state->symbol("Loader"));
    if(loader->nil_p()) {
      rubinius::bug("Unable to find loader");
    }

    OnStack<1> os(state, loader);

    // Enable the JIT after the core library has loaded
    G(jit)->enable(state);

    Object* inst = loader->send(state, 0, state->symbol("new"));
    if(inst) {
      OnStack<1> os2(state, inst);

      inst->send(state, 0, state->symbol("main"));
    } else {
      rubinius::bug("Unable to instantiate loader");
    }
  }
}
