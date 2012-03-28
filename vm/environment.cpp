#include "config.h"
#include "prelude.hpp"
#include "environment.hpp"
#include "config_parser.hpp"
#include "compiled_file.hpp"
#include "objectmemory.hpp"

#include "vm/exception.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/nativemethod.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#if RBX_LLVM_API_VER == 208
#include <llvm/System/Threading.h>
#elif RBX_LLVM_API_VER == 209
#include <llvm/Support/Threading.h>
#endif
#include <llvm/Support/ManagedStatic.h>
#endif

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

#include "signal.hpp"
#include "object_utils.hpp"

#include "inline_cache.hpp"

#include "agent.hpp"

#include "instruments/tooling.hpp"

#include "on_stack.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#ifdef RBX_WINDOWS
#include "windows_compat.h"
#else
#include <sys/utsname.h>
#include <dlfcn.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>

namespace rubinius {

  // Used by the segfault reporter. Calculated up front to avoid
  // crashing inside the crash handler.
  static struct utsname machine_info;
  static char report_path[1024];
  static const char* report_file_name = ".rubinius_last_error";

  Environment::Environment(int argc, char** argv)
    : argc_(argc)
    , argv_(argv)
    , signature_(0)
    , version_(0)
    , agent(0)
  {
#ifdef ENABLE_LLVM
    if(!llvm::llvm_start_multithreaded()) {
      assert(0 && "llvm doesn't support threading!");
    }

#ifdef LLVM_JIT_DEBUG
    if(getenv("RBX_GDBJIT")) {
      llvm::JITEmitDebugInfo = true;
    }
#endif
#endif

    VM::init_stack_size();

    shared = new SharedState(this, config, config_parser);
    root_vm = shared->new_vm();
    state = new State(root_vm);

    uname(&machine_info);

    // Calculate the report_path
    if(char* home = getenv("HOME")) {
      char* path = report_path;

      memcpy(path, home, strlen(home));
      path += strlen(home);

      *path++ = '/';

      memcpy(path, report_file_name, strlen(report_file_name));
      path += strlen(report_file_name);

      *path = 0;
    } else {
      // We check and ignore the report_path if it's 'empty'
      report_path[0] = 0;
    }
  }

  Environment::~Environment() {
    delete sig_handler_;

    VM::discard(state, root_vm);
    SharedState::discard(shared);
    delete state;
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

#ifndef RBX_WINDOWS
  static void null_func(int sig) {}
#endif

#ifdef USE_EXECINFO

  static void safe_write(int fd, const char* str, int len=0) {
    if(!len) len = strlen(str);
    if(write(fd, str, len) == 0) exit(101);
  }

  static void write_sig(int fd, int sig) {
    switch(sig) {
    case SIGSEGV:
      safe_write(fd, "SIGSEGV");
      break;
    case SIGBUS:
      safe_write(fd, "SIGBUS");
      break;
    case SIGILL:
      safe_write(fd, "SIGILL");
      break;
    case SIGABRT:
      safe_write(fd, "SIGABRT");
      break;
    case SIGFPE:
      safe_write(fd, "SIGFPE");
      break;
    default:
      safe_write(fd, "UNKNOWN");
      break;
    }
  }

  static void segv_handler(int sig) {
    static int crashing = 0;
    void* array[64];
    size_t size;

    // So we don't recurse!
    if(crashing) exit(101);

    crashing = 1;

    int fd = STDERR_FILENO;

    if(getenv("RBX_PAUSE_ON_CRASH")) {
      std::cerr << "\n========== CRASH (" << getpid();
      std::cerr << "), pausing for 60 seconds to attach debugger\n";
      sleep(60);
    }

    // If there is a report_path setup..
    if(report_path[0]) {
      fd = open(report_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      // If we can't open this path, use stderr.
      if(fd == -1) fd = STDERR_FILENO;
    }

    // print out all the frames to stderr
    static const char header[] = 
      "Rubinius Crash Report #rbxcrashreport\n\n"
      "Error: signal ";

    safe_write(fd, header, sizeof(header));
    write_sig(fd, sig);

    safe_write(fd, "\n\n[[Backtrace]]\n");

    // get void*'s for all entries on the stack
    size = backtrace(array, 64);

    backtrace_symbols_fd(array, size, fd);

    // Try to get the output to flush...
    safe_write(fd, "\n[[System Info]]\n");
    safe_write(fd, "sysname: ");
    safe_write(fd, machine_info.sysname);
    safe_write(fd, "\n");
    safe_write(fd, "nodename: ");
    safe_write(fd, machine_info.nodename);
    safe_write(fd, "\n");
    safe_write(fd, "release: ");
    safe_write(fd, machine_info.release);
    safe_write(fd, "\n");
    safe_write(fd, "version: ");
    safe_write(fd, machine_info.version);
    safe_write(fd, "\n");
    safe_write(fd, "machine: ");
    safe_write(fd, machine_info.machine);
    safe_write(fd, "\n");

    // If we didn't write to stderr, then close the file down and
    // write info to stderr about reporting the error.
    if(fd != STDERR_FILENO) {
      close(fd);
      safe_write(2, "\n---------------------------------------------\n");
      safe_write(2, "CRASH: A fatal error has occurred.\n\nBacktrace:\n");
      backtrace_symbols_fd(array, size, 2);
      safe_write(2, "\n\n");
      safe_write(2, "Wrote full error report to: ");
      safe_write(2, report_path);
      safe_write(2, "\nRun 'rbx report' to submit this crash report!\n");
    }

    exit(100);
  }
#endif

  static void quit_handler(int sig) {
    static const char msg[] = "Terminated: signal ";
    if(write(2, msg, sizeof(msg)) == 0) exit(1);

    switch(sig) {
    case SIGTERM:
      if(write(2, "SIGTERM\n", 7) == 0) exit(1);
      break;
#ifndef RBX_WINDOWS
    case SIGHUP:
      if(write(2, "SIGHUP\n", 6) == 0) exit(1);
      break;
    case SIGUSR1:
      if(write(2, "SIGUSR1\n", 7) == 0) exit(1);
      break;
    case SIGUSR2:
      if(write(2, "SIGUSR2\n", 7) == 0) exit(1);
      break;
#endif
    default:
      if(write(2, "UNKNOWN\n", 8) == 0) exit(1);
      break;
    }

    _exit(1);
  }

  void Environment::start_signals() {
#ifndef RBX_WINDOWS
    struct sigaction action;
    action.sa_handler = null_func;
    action.sa_flags = 0;
    sigfillset(&action.sa_mask);
    sigaction(SIGVTALRM, &action, NULL);
#endif

    state->vm()->set_run_signals(true);
    sig_handler_ = new SignalHandler(state);
    shared->set_signal_handler(sig_handler_);
    sig_handler_->run(state);

#ifndef RBX_WINDOWS
    // Ignore sigpipe.
    signal(SIGPIPE, SIG_IGN);

    // Some extensions expect SIGALRM to be defined, because MRI does.
    // We'll just use a noop for it.
    signal(SIGALRM, null_func);

    // If we have execinfo, setup some crash handlers
#ifdef USE_EXECINFO
    if(!getenv("DISABLE_SEGV")) {
      signal(SIGSEGV, segv_handler);
      signal(SIGBUS,  segv_handler);
      signal(SIGILL,  segv_handler);
      signal(SIGFPE,  segv_handler);
      signal(SIGABRT, segv_handler);

      // Force glibc to load the shared library containing backtrace()
      // now, so that we don't have to try and load it in the signal
      // handler.
      void* ary[1];
      backtrace(ary, 1);
    }
#endif  // USE_EXEC_INFO

    // Setup some other signal that normally just cause the process
    // to terminate so that we print out a message, then terminate.
    signal(SIGHUP,  quit_handler);
    signal(SIGUSR1, quit_handler);
    signal(SIGUSR2, quit_handler);
#endif  // ifndef RBX_WINDOWS

    signal(SIGTERM, quit_handler);
  }

  void Environment::load_vm_options(int argc, char**argv) {
    /* Parse -X options from RBXOPT environment variable.  We parse these
     * first to permit arguments passed directly to the VM to override
     * settings from the environment.
     */
    char* rbxopt = getenv("RBXOPT");
    if(rbxopt) {
      char *e, *b = rbxopt = strdup(rbxopt);
      char *s = b + strlen(rbxopt);

      while(b < s) {
        while(*b && isspace(*b)) b++;

        e = b;
        while(*e && !isspace(*e)) e++;

        int len;
        if((len = e - b) > 0) {
          if(strncmp(b, "-X", 2) == 0) {
            *e = 0;
            config_parser.import_line(b + 2);
          }
          b = e + 1;
        }
      }

      free(rbxopt);
    }

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
  }

  void Environment::load_argv(int argc, char** argv) {
    Array* os_ary = Array::create(state, argc);
    for(int i = 0; i < argc; i++) {
      os_ary->set(state, i, String::create(state, argv[i]));
    }

    G(rubinius)->set_const(state, "OS_ARGV", os_ary);

    char buf[MAXPATHLEN];
    G(rubinius)->set_const(state, "OS_STARTUP_DIR",
        String::create(state, getcwd(buf, MAXPATHLEN)));

    state->vm()->set_const("ARG0", String::create(state, argv[0]));

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

      ary->set(state, which_arg++, String::create(state, arg)->taint(state));
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

    if(config.agent_start > 0) {
      // if port_ is 1, the user wants to use a randomly assigned local
      // port which will be written to the temp file for console to pick
      // up.

      int port = config.agent_start;
      if(port == 1) port = 0;
      start_agent(port);
    }

    if(config.report_path.set_p()) {
      // Test that we can actually use this path
      int fd = open(config.report_path, O_RDONLY | O_CREAT, 0666);
      if(!fd) {
        std::cerr << "Unable to use " << config.report_path << " for crash reports.\n";
        std::cerr << "Unable to open path: " << strerror(errno) << "\n";

        // Don't use the home dir path even, just use stderr
        report_path[0] = 0;
      } else {
        close(fd);
        unlink(config.report_path);
        strncpy(report_path, config.report_path, sizeof(report_path) - 1);
      }
    }
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
      if(line.size() == 0) continue;

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
    if(cf->magic != "!RBIX") throw std::runtime_error("Invalid file");
    if((signature_ > 0 && cf->signature != signature_)
        || cf->version != version_) {
      throw BadKernelFile(file);
    }

    cf->execute(state);

    if(state->vm()->thread_state()->raise_reason() == cException) {
      Exception* exc = as<Exception>(state->vm()->thread_state()->current_exception());
      std::ostringstream msg;

      msg << "exception detected at toplevel: ";
      if(!exc->message()->nil_p()) {
        if(String* str = try_as<String>(exc->message())) {
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

    if(state->shared().config.ic_stats) {
      state->shared().ic_registry()->print_stats(state);
    }

    GCTokenImpl gct;

    // Handle an edge case where another thread is waiting to stop the world.
    if(state->shared().should_stop()) {
      state->checkpoint(gct, 0);
    }

    NativeMethod::cleanup_thread(state);

#ifdef ENABLE_LLVM
    LLVMState::shutdown(state);
#endif

    SignalHandler::shutdown();

    // Hold everyone.
    while(!state->stop_the_world());
    shared->om->run_all_io_finalizers(state);

    // TODO: temporarily disable to sort out finalizing Pointer objects
    // shared->om->run_all_finalizers(state);
  }

  /**
   * Returns the exit code to use when exiting the rbx process.
   */
  int Environment::exit_code(STATE) {

#ifdef ENABLE_LLVM
    if(LLVMState* ls = shared->llvm_state) {
      std::ostream& jit_log = ls->log();
      if(jit_log != std::cerr) {
        dynamic_cast<std::ofstream&>(jit_log).close();
      }
    }
#endif

    if(state->vm()->thread_state()->raise_reason() == cExit) {
      if(Fixnum* fix = try_as<Fixnum>(state->vm()->thread_state()->raise_value())) {
        return fix->to_native();
      } else {
        return -1;
      }
    }

    return 0;
  }

  void Environment::start_agent(int port) {
    agent = new QueryAgent(*shared, state);
    if(config.agent_verbose) agent->set_verbose();

    if(!agent->bind(port)) return;

    shared->set_agent(agent);

    agent->run();
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

    // Pull in the signature file; this helps control when .rbc files need to
    // be discarded and recompiled due to changes to the compiler since the
    // .rbc files were created.
    std::string sig_path = root + "/signature";
    std::ifstream sig_stream(sig_path.c_str());
    if(sig_stream) {
      sig_stream >> signature_;
      G(rubinius)->set_const(state, "Signature",
                       Integer::from(state, signature_));
      sig_stream.close();
    } else {
      std::string error = "Unable to load compiler signature file: " + sig_path;
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
      if(line.size() == 0) continue;

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

  /**
   * Runs rbx from the filesystem, loading the Ruby kernel files relative to
   * the supplied root directory.
   *
   * @param root The path to the Rubinius /runtime directory, which contains
   * the loader.rbc and kernel files.
   */
  void Environment::run_from_filesystem(std::string root) {
    int i = 0;
    state->vm()->set_root_stack(reinterpret_cast<uintptr_t>(&i),
                                VM::cStackDepthMax);

    load_platform_conf(root);
    load_vm_options(argc_, argv_);
    boot_vm();
    load_argv(argc_, argv_);

    state->vm()->initialize_config();

    load_tool();

    if(LANGUAGE_20_ENABLED(state)) {
      root += "/20";
    } else if(LANGUAGE_19_ENABLED(state)) {
      root += "/19";
    } else {
      root += "/18";
    }
    G(rubinius)->set_const(state, "RUNTIME_PATH", String::create(state, root.c_str(), root.size()));

    load_kernel(root);

    start_signals();
    run_file(root + "/loader.rbc");

    state->vm()->thread_state()->clear();

    Object* loader = G(rubinius)->get_const(state, state->symbol("Loader"));
    if(loader->nil_p()) {
      std::cout << "Unable to find loader!\n";
      exit(127);
    }

    OnStack<1> os(state, loader);

    Object* inst = loader->send(state, 0, state->symbol("new"));

    OnStack<1> os2(state, inst);

    inst->send(state, 0, state->symbol("main"));
  }
}
