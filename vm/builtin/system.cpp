#include <vector>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <string.h>
#include <sstream>
#include <signal.h>

#include "vm/config.h"

#ifdef RBX_WINDOWS
#include "windows_compat.h"
#else
#include <sys/resource.h>
#include <sys/wait.h>
#include <pwd.h>
#include <dlfcn.h>
#endif

#include <unistd.h>

#include "vm/call_frame.hpp"
#include "vm/helpers.hpp"

#include "vm/object_utils.hpp"
#include "vm/vm.hpp"

#include "compiled_file.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"
#include "config_parser.hpp"

#include "arguments.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/bignum.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/location.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/float.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/io.hpp"
#include "builtin/thread.hpp"

#include "builtin/channel.hpp"

#include "builtin/constantscope.hpp"
#include "builtin/block_environment.hpp"

#include "builtin/system.hpp"
#include "signal.hpp"
#include "lookup_data.hpp"

#include "configuration.hpp"

#include "inline_cache.hpp"

#include "agent.hpp"

#include "windows_compat.h"
#include "util/sha1.h"

#include "instruments/tooling.hpp"

#include "gc/walker.hpp"

#include "on_stack.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#include "llvm/jit_compiler.hpp"
#endif

#ifdef OS_X_10_5
#include <mach/mach.h>
#endif

#include "missing/setproctitle.h"

namespace rubinius {

  void System::bootstrap_methods(STATE) {
    GCTokenImpl gct;

    System::attach_primitive(state, gct,
                             G(rubinius), true,
                             state->symbol("open_class"),
                             state->symbol("vm_open_class"));

    System::attach_primitive(state, gct,
                             G(rubinius), true,
                             state->symbol("open_class_under"),
                             state->symbol("vm_open_class_under"));

    System::attach_primitive(state, gct,
                             G(rubinius), true,
                             state->symbol("open_module"),
                             state->symbol("vm_open_module"));

    System::attach_primitive(state, gct,
                             G(rubinius), true,
                             state->symbol("open_module_under"),
                             state->symbol("vm_open_module_under"));

    System::attach_primitive(state, gct,
                             G(rubinius), true,
                             state->symbol("add_defn_method"),
                             state->symbol("vm_add_method"));

    System::attach_primitive(state, gct,
                             G(rubinius), true,
                             state->symbol("attach_method"),
                             state->symbol("vm_attach_method"));

    System::attach_primitive(state, gct,
                             as<Module>(G(rubinius)->get_const(state, "Type")), true,
                             state->symbol("object_singleton_class"),
                             state->symbol("vm_object_singleton_class"));

  }

  void System::attach_primitive(STATE, GCToken gct, Module* mod, bool meta,
                                Symbol* name, Symbol* prim)
  {
    MethodTable* tbl;

    if(meta) {
      tbl = mod->singleton_class(state)->method_table();
    } else {
      tbl = mod->method_table();
    }

    Executable* oc = Executable::allocate(state, cNil);
    oc->primitive(state, prim);
    oc->resolve_primitive(state);

    tbl->store(state, gct, name, oc, G(sym_public));
  }

/* Primitives */
  //
  // HACK: remove this when performance is better and compiled_file.rb
  // unmarshal_data method works.
  Object* System::compiledfile_load(STATE, String* path,
                                    Integer* signature, Integer* version)
  {
    std::stringstream stream;
    std::ifstream file(path->c_str(state), std::ios::ate|std::ios::binary);

    if(!file) {
      return Primitives::failure();
    }

    std::streampos length = file.tellg();
    std::vector<char> buffer(length);
    file.seekg(0, std::ios::beg);
    file.read(&buffer[0], length);
    stream.rdbuf()->pubsetbuf(&buffer[0], length);

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") {
      delete cf;
      return Primitives::failure();
    }

    uint64_t sig = signature->to_ulong_long();
    if((sig > 0 && cf->signature != sig)
        || cf->version != version->to_native()) {
      delete cf;
      return Primitives::failure();
    }

    Object *body = cf->body(state);

    delete cf;
    return body;
  }

  Object* System::yield_gdb(STATE, Object* obj) {
    obj->show(state);
    Exception::assertion_error(state, "yield_gdb called and not caught");
    return obj;
  }

  Object* System::vm_exec(STATE, String* path, Array* args,
                          CallFrame* calling_environment) {

    const char* c_path = path->c_str_null_safe(state);
    size_t argc = args->size();

    char** argv = new char*[argc + 1];

    /* execvp() requires a NULL as last element */
    argv[argc] = NULL;

    for(size_t i = 0; i < argc; i++) {
      // POSIX guarantees that execvp does not modify the characters to which the argv
      // pointers point, despite the argument not being declared as const char *const[].
      argv[i] = const_cast<char*>(as<String>(args->get(state, i))->c_str_null_safe(state));
    }

    OnStack<2> os(state, path, args);
    // Some system (darwin) don't let execvp work if there is more
    // than one thread running.
    {
      // TODO: Make this guard unnecessary
      GCIndependent guard(state, calling_environment);
      state->shared().auxiliary_threads()->before_exec(state);
    }

    // TODO Need to stop and kill off any ruby threads!
    // We haven't run into this because exec is almost always called
    // after fork(), which pulls over just one thread anyway.

    void* old_handlers[NSIG];

    // Reset all signal handlers to the defaults, so any we setup in Rubinius
    // won't leak through. We need to use sigaction() here since signal()
    // provides no control over SA_RESTART and can use the wrong value causing
    // blocking I/O methods to become uninterruptable.
    for(int i = 1; i < NSIG; i++) {
      struct sigaction action;
      struct sigaction old_action;

      action.sa_handler = SIG_DFL;
      action.sa_flags = 0;
      sigfillset(&action.sa_mask);

      sigaction(i, &action, &old_action);
      old_handlers[i] = (void*)old_action.sa_handler;
    }

    (void)::execvp(c_path, argv);
    int erno = errno;

    // Hmmm, execvp failed, we need to recover here.

    for(int i = 1; i < NSIG; i++) {
      struct sigaction action;

      action.sa_handler = (void(*)(int))old_handlers[i];
      action.sa_flags = 0;
      sigfillset(&action.sa_mask);

      sigaction(i, &action, NULL);
    }

    delete[] argv;

    state->shared().auxiliary_threads()->after_exec(state);

    /* execvp() returning means it failed. */
    Exception::errno_error(state, "execvp(2) failed", erno);
    return NULL;
  }

  Object* System::vm_replace(STATE, String* str,
                             CallFrame* calling_environment)
  {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return Primitives::failure();
#else

    const char* c_str = str->c_str_null_safe(state);

    int fds[2];

    OnStack<1> os(state, str);

    if(pipe(fds) != 0) return Primitives::failure();

    {
      // TODO: Make this guard unnecessary
      GCIndependent guard(state, calling_environment);
      state->shared().auxiliary_threads()->before_fork(state);
    }

    pid_t pid = ::fork();

    // error
    if(pid == -1) {
      close(fds[0]);
      close(fds[1]);
      return Primitives::failure();
    }

    // child
    if(pid == 0) {
      close(fds[0]);
      dup2(fds[1], STDOUT_FILENO);
      close(fds[1]);

      // detect and decide to use sh or not.
      char* s = const_cast<char*>(c_str);
      bool use_sh = false;

      for(;*s;s++) {
        if(*s != ' ' && !ISALPHA(*s) && strchr("*?{}[]<>()~&|\\$;'`\"\n",*s)) {
          use_sh = true;
          break;
        }
      }

      if(use_sh) {
        execl("/bin/sh", "sh", "-c", c_str, (char*)0);
      } else {
        size_t c_size = str->byte_size();
        size_t max_spaces = (c_size / 2) + 2;
        char** args = new char*[max_spaces];

        // Now put nulls for spaces into c_str and assign each bit
        // to args to create the array of char*s that execv wants.

        s = const_cast<char*>(c_str);
        const char* s_end = c_str + c_size;
        int idx = 0;

        for(;;) {
          // turn the next group of spaces into nulls.
          while(s < s_end && *s == ' ') {
            *s = 0;
            s++;
          }

          // Hit the end, bail.
          if(s == s_end) break;

          // Write the address of the next chunk here.
          args[idx++] = s;

          // Skip to the next space
          while(s < s_end && *s != ' ') s++;
        }

        args[idx] = 0;

        // If we added anything, then exec, otherwise fall through and fail.
        if(idx > 0) execvp(args[0], args);
      }

      // bad news, shouldn't be here.
      std::cerr << "execvp failed: " << strerror(errno) << std::endl;
      exit(1);
    } else {
      state->shared().auxiliary_threads()->after_fork_parent(state);
    }

    close(fds[1]);

    std::string buf;
    for(;;) {

      ssize_t bytes = 0;
      char raw_buf[1024];
      {
        GCIndependent guard(state, calling_environment);
        bytes = read(fds[0], raw_buf, 1023);
      }

      if(bytes < 0) {
        switch(errno) {
          case EAGAIN:
          case EINTR:
            if(!state->check_async(calling_environment)) {
              close(fds[0]);
              return NULL;
            }
            continue;
          default:
            close(fds[0]);
            Exception::errno_error(state, "reading child data", errno, "read(2)");
        }
      }

      if(bytes == 0) {
        break;
      }
      buf.append(raw_buf, bytes);
    }

    close(fds[0]);

    return Tuple::from(state, 2, Fixnum::from(pid),
                       String::create(state, buf.c_str(), buf.size()));
#endif  // RBX_WINDOWS
  }

  Object* System::vm_wait_pid(STATE, Fixnum* pid_obj, Object* no_hang,
                              CallFrame* calling_environment)
  {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return Primitives::failure();
#else
    pid_t input_pid = pid_obj->to_native();
    int options = 0;
    int status;
    pid_t pid;

    if(no_hang == cTrue) {
      options |= WNOHANG;
    }

    typedef void (*rbx_sighandler_t)(int);

    rbx_sighandler_t hup_func;
    rbx_sighandler_t quit_func;
    rbx_sighandler_t int_func;

  retry:

    hup_func  = signal(SIGHUP, SIG_IGN);
    quit_func = signal(SIGQUIT, SIG_IGN);
    int_func  = signal(SIGINT, SIG_IGN);

    {
      GCIndependent guard(state, calling_environment);
      pid = waitpid(input_pid, &status, options);
    }

    signal(SIGHUP, hup_func);
    signal(SIGQUIT, quit_func);
    signal(SIGINT, int_func);

    if(pid == -1) {
      if(errno == ECHILD) return cFalse;
      if(errno == EINTR) {
        if(!state->check_async(calling_environment)) return NULL;
        goto retry;
      }

      // TODO handle other errnos?
      return cFalse;
    }

    if(no_hang == cTrue && pid == 0) {
      return cNil;
    }

    Object* output  = cNil;
    Object* termsig = cNil;
    Object* stopsig = cNil;

    if(WIFEXITED(status)) {
      output = Fixnum::from(WEXITSTATUS(status));
    } else if(WIFSIGNALED(status)) {
      termsig = Fixnum::from(WTERMSIG(status));
    } else if(WIFSTOPPED(status)){
      stopsig = Fixnum::from(WSTOPSIG(status));
    }

    return Tuple::from(state, 4, output, termsig, stopsig, Fixnum::from(pid));
#endif  // RBX_WINDOWS
  }

  Object* System::vm_exit(STATE, Fixnum* code) {
    state->vm()->thread_state()->raise_exit(code);
    return NULL;
  }

  Fixnum* System::vm_fork(STATE, GCToken gct, CallFrame* calling_environment)
  {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return force_as<Fixnum>(Primitives::failure());
#else
    int result = 0;

    {
      // TODO: Make this guard unnecessary
      GCIndependent guard(state, calling_environment);
      state->shared().auxiliary_threads()->before_fork(state);
    }

    /*
     * We have to bring all the threads to a safe point before we can
     * fork the process so any internal locks are unlocked before we fork
     */

    StopTheWorld stw(state, gct, calling_environment);

    // ok, now fork!
    result = ::fork();

    // We're in the child...
    if(result == 0) {
      /*  @todo any other re-initialisation needed? */

      state->vm()->thread->init_lock();
      state->shared().reinit(state);
      state->shared().om->on_fork(state);
      state->shared().auxiliary_threads()->after_fork_child(state);
    } else {
      {
        // TODO: Make this guard unnecessary
        GCIndependent guard(state, calling_environment);
        state->shared().auxiliary_threads()->after_fork_parent(state);
      }
    }

    if(result == -1) {
      Exception::errno_error(state, "fork(2) failed");
      return NULL;
    }

    return Fixnum::from(result);
#endif
  }

  Object* System::vm_gc_start(STATE, GCToken gct, Object* force, CallFrame* call_frame) {
    // force is set if this is being called by the kernel (for instance
    // in File#ininitialize). If we decided to ignore some GC.start calls
    // by usercode trying to be clever, we can use force to know that we
    // should NOT ignore it.
    if(CBOOL(force) || state->shared().config.gc_honor_start) {
      state->memory()->collect_young_now = true;
      state->memory()->collect_mature_now = true;
      state->vm()->collect_maybe(gct, call_frame);
    }
    return cNil;
  }

  Integer* System::vm_gc_count(STATE) {
    return Integer::from(state, state->memory()->gc_stats.young_collection_count.read() +
                                state->memory()->gc_stats.full_collection_count.read());
  }

  Integer* System::vm_gc_size(STATE) {
    return Integer::from(state, state->shared().config.gc_bytes * 2 +
                                state->memory()->immix_usage() +
                                state->memory()->loe_usage() +
                                state->memory()->code_usage() +
                                state->shared().symbols.bytes_used());
  }

  Integer* System::vm_gc_time(STATE) {
    return Integer::from(state, state->memory()->gc_stats.total_young_collection_time.read() +
                                state->memory()->gc_stats.total_full_collection_time.read());
  }

  Object* System::vm_get_config_item(STATE, String* var) {
    ConfigParser::Entry* ent = state->shared().user_variables.find(var->c_str(state));
    if(!ent) return cNil;

    if(ent->is_number()) {
      return Bignum::from_string(state, ent->value.c_str(), 10);
    } else if(ent->is_true()) {
      return cTrue;
    }

    return String::create(state, ent->value.c_str(), ent->value.size());
  }

  Object* System::vm_get_config_section(STATE, String* section) {
    ConfigParser::EntryList* list;

    list = state->shared().user_variables.get_section(
        reinterpret_cast<char*>(section->byte_address()));

    Array* ary = Array::create(state, list->size());
    for(size_t i = 0; i < list->size(); i++) {
      std::string variable = list->at(i)->variable;
      std::string value    = list->at(i)->value;
      String* var = String::create(state, variable.c_str(), variable.size());
      String* val = String::create(state, value.c_str(), value.size());

      ary->set(state, i, Tuple::from(state, 2, var, val));
    }

    delete list;

    return ary;
  }

  Object* System::vm_reset_method_cache(STATE, Symbol* name) {
    // 1. clear the global cache
    state->vm()->global_cache()->clear(state, name);

    state->shared().ic_registry()->clear(state, name);
    return name;
  }

   /*  @todo Could possibly capture the system backtrace at this
   *        point. --rue
   */
  Array* System::vm_backtrace(STATE, Fixnum* skip, Object* inc_vars,
                              CallFrame* calling_environment) {
    CallFrame* call_frame = calling_environment;

    bool include_vars = CBOOL(inc_vars);

    for(native_int i = skip->to_native(); call_frame && i > 0; --i) {
      call_frame = call_frame->previous;
    }

    return Location::from_call_stack(state, call_frame, include_vars);
  }

  Array* System::vm_mri_backtrace(STATE, Fixnum* skip,
                              CallFrame* calling_environment) {
    CallFrame* call_frame = calling_environment;

    for(native_int i = skip->to_native(); call_frame && i > 0; --i) {
      call_frame = call_frame->previous;
    }

    return Location::mri_backtrace(state, call_frame);
  }


  Object* System::vm_show_backtrace(STATE, CallFrame* calling_environment) {
    calling_environment->print_backtrace(state);
    return cNil;
  }

  Object* System::vm_tooling_available_p(STATE) {
#ifdef RBX_PROFILER
    return state->shared().tool_broker()->available(state) ? cTrue : cFalse;
#else
    return cFalse;
#endif
  }

  Object* System::vm_tooling_active_p(STATE) {
    return state->vm()->tooling() ? cTrue : cFalse;
  }

  Object* System::vm_tooling_enable(STATE) {
    state->shared().tool_broker()->enable(state);
    return cTrue;
  }

  Object* System::vm_tooling_disable(STATE) {
    return state->shared().tool_broker()->results(state);
  }

  Object* System::vm_load_tool(STATE, String* str) {
    std::string path = std::string(str->c_str(state)) + ".";

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
        return Tuple::from(state, 2, cFalse, String::create(state, dlerror()));
      }
    }

    void* sym = dlsym(handle, "Tool_Init");
    if(!sym) {
      dlclose(handle);
      return Tuple::from(state, 2, cFalse, String::create(state, dlerror()));
    } else {
      typedef int (*init_func)(rbxti::Env* env);
      init_func init = (init_func)sym;

      if(!init(state->vm()->tooling_env())) {
        dlclose(handle);
        return Tuple::from(state, 2, cFalse, String::create(state, path.c_str(), path.size()));
      }
    }

    return Tuple::from(state, 1, cTrue);
  }

  Object* System::vm_write_error(STATE, String* str) {
    std::cerr << str->c_str(state) << std::endl;
    return cNil;
  }

  Object* System::vm_jit_info(STATE) {
    if(state->shared().config.jit_disabled) return cNil;

#ifdef ENABLE_LLVM
    LLVMState* ls = LLVMState::get(state);

    Array* ary = Array::create(state, 5);
    ary->set(state, 0, Integer::from(state, ls->jitted_methods()));
    ary->set(state, 1, Integer::from(state, ls->code_bytes()));
    ary->set(state, 2, Integer::from(state, ls->time_spent));
    ary->set(state, 3, Integer::from(state, ls->accessors_inlined()));
    ary->set(state, 4, Integer::from(state, ls->uncommons_taken()));

    return ary;
#else
    return cNil;
#endif
  }

  Object* System::vm_watch_signal(STATE, Fixnum* sig, Object* ignored) {
    SignalHandler* h = state->shared().signal_handler();
    if(h) {
      native_int i = sig->to_native();
      if(i < 0) {
        h->add_signal(state, -i, SignalHandler::eDefault);
      } else if(i > 0) {
        h->add_signal(state, i, ignored == cTrue ? SignalHandler::eIgnore : SignalHandler::eCustom);
      }

      return cTrue;
    } else {
      return cFalse;
    }
  }

  Object* System::vm_time(STATE) {
    return Integer::from(state, time(0));
  }

#define NANOSECONDS 1000000000
  Object* System::vm_sleep(STATE, GCToken gct, Object* duration,
                           CallFrame* calling_environment)
  {
    struct timespec ts = {0,0};
    bool use_timed_wait = true;

    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      ts.tv_sec = fix->to_native();
    } else if(Float* flt = try_as<Float>(duration)) {
      uint64_t nano = (uint64_t)(flt->val * NANOSECONDS);
      ts.tv_sec  =  (time_t)(nano / NANOSECONDS);
      ts.tv_nsec =    (long)(nano % NANOSECONDS);
    } else if(duration == G(undefined)) {
      use_timed_wait = false;
    } else {
      return Primitives::failure();
    }

    time_t start = time(0);

    if(use_timed_wait) {
      struct timeval tv = {0,0};
      gettimeofday(&tv, 0);

      uint64_t nano = ts.tv_nsec + tv.tv_usec * 1000;
      ts.tv_sec  += tv.tv_sec + nano / NANOSECONDS;
      ts.tv_nsec  = nano % NANOSECONDS;

      if(!state->park_timed(gct, calling_environment, &ts)) return NULL;
    } else {
      if(!state->park(gct, calling_environment)) return NULL;
    }

    if(!state->check_async(calling_environment)) return NULL;

    return Fixnum::from(time(0) - start);
  }

  Object* System::vm_check_interrupts(STATE, CallFrame* calling_environment) {
    if(state->check_async(calling_environment)) {
      return cNil;
    } else {
      return NULL;
    }
  }

  static inline double tv_to_dbl(struct timeval* tv) {
    return (double)tv->tv_sec + ((double)tv->tv_usec / 1000000.0);
  }

  static inline double to_dbl(long sec, long msec) {
    return (double)sec + ((double)msec / 1000000.0);
  }

  Array* System::vm_times(STATE) {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return force_as<Array>(Primitives::failure());
#else
    struct rusage buf;

    Array* ary = Array::create(state, 4);

    getrusage(RUSAGE_SELF, &buf);
    ary->set(state, 0, Float::create(state, tv_to_dbl(&buf.ru_utime)));
    ary->set(state, 1, Float::create(state, tv_to_dbl(&buf.ru_stime)));

    getrusage(RUSAGE_CHILDREN, &buf);
    ary->set(state, 2, Float::create(state, tv_to_dbl(&buf.ru_utime)));
    ary->set(state, 3, Float::create(state, tv_to_dbl(&buf.ru_stime)));

    // Get Thread info too
#if defined(OS_X_10_5)
    mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
    thread_basic_info_data_t info;
    kern_return_t err;

    mach_port_t port = mach_thread_self();
    err = thread_info(port, THREAD_BASIC_INFO, (thread_info_t)&info, &count);
    mach_port_deallocate(mach_task_self(), port);

    if(err == KERN_SUCCESS) {
      ary->set(state, 4, Float::create(state, to_dbl(
              info.user_time.seconds, info.user_time.microseconds)));
      ary->set(state, 5, Float::create(state, to_dbl(
              info.system_time.seconds, info.system_time.microseconds)));
    } else {
      ary->set(state, 4, ary->get(state, 0));
      ary->set(state, 5, ary->get(state, 1));
    }
#elif defined(RUSAGE_THREAD)
    getrusage(RUSAGE_THREAD, &buf);
    ary->set(state, 4, Float::create(state, tv_to_dbl(&buf.ru_utime)));
    ary->set(state, 5, Float::create(state, tv_to_dbl(&buf.ru_stime)));
#elif defined(_WIN32)
    FILETIME unused, unused2;
    FILETIME sys, usr;

    GetThreadTimes(GetCurrentThread(), &unused, &unused2, &sys, &user);

    ary->set(state, 4, Float::create(state, ((double)usr) / 10000));
    ary->set(state, 5, Float::create(state, ((double)sys) / 10000));
#else
    ary->set(state, 4, ary->get(state, 0));
    ary->set(state, 5, ary->get(state, 1));
#endif

    return ary;
#endif
  }

  Class* System::vm_open_class(STATE, Symbol* name, Object* sup,
                               ConstantScope* scope)
  {
    Module* under;

    if(scope->nil_p()) {
      under = G(object);
    } else {
      under = scope->module();
    }

    return vm_open_class_under(state, name, sup, under);
  }

  Class* System::vm_open_class_under(STATE, Symbol* name, Object* super,
                                     Module* under)
  {
    bool found = false;

    Object* obj = under->get_const(state, name, &found);
    if(found) {
      Class* cls = as<Class>(obj);
      if(super->nil_p()) return cls;

      if(cls->true_superclass(state) != super) {
        std::ostringstream message;
        message << "Superclass mismatch: given "
                << as<Module>(super)->debug_str(state)
                << " but previously set to "
                << cls->true_superclass(state)->debug_str(state);

        Exception* exc =
          Exception::make_type_error(state, Class::type, super,
                                     message.str().c_str());
        // exc->locations(state, System::vm_backtrace(state,
        //                Fixnum::from(0), call_frame));
        state->raise_exception(exc);
        return NULL;
      }

      return cls;
    }

    // Create the class.
    if(super->nil_p()) super = G(object);
    Class* cls = Class::create(state, as<Class>(super));

    cls->set_name(state, name, under);
    under->set_const(state, name, cls);

    return cls;
  }

  Module* System::vm_open_module(STATE, Symbol* name, ConstantScope* scope) {
    Module* under = G(object);

    if(!scope->nil_p()) {
      under = scope->module();
    }

    return vm_open_module_under(state, name, under);
  }

  Module* System::vm_open_module_under(STATE, Symbol* name, Module* under) {
    bool found;

    Object* obj = under->get_const(state, name, &found);

    if(found) return as<Module>(obj);

    Module* module = Module::create(state);

    module->set_name(state, name, under);
    under->set_const(state, name, module);

    return module;
  }

  static Tuple* find_method(STATE, Object* recv, Symbol* name, Symbol* min_visibility) {
    LookupData lookup(recv, recv->lookup_begin(state), min_visibility);

    Dispatch dis(name);

    if(!GlobalCache::resolve(state, name, dis, lookup)) {
      return nil<Tuple>();
    }

    return Tuple::from(state, 2, dis.method, dis.module);
  }

  Tuple* System::vm_find_method(STATE, Object* recv, Symbol* name) {
    return find_method(state, recv, name, G(sym_private));
  }

  Tuple* System::vm_find_public_method(STATE, Object* recv, Symbol* name) {
    return find_method(state, recv, name, G(sym_public));
  }

  Object* System::vm_add_method(STATE, GCToken gct, Symbol* name,
                                CompiledCode* method,
                                ConstantScope* scope, Object* vis)
  {
    Module* mod = scope->for_method_definition();

    method->scope(state, scope);
    method->serial(state, Fixnum::from(0));

    OnStack<4> os(state, mod, method, scope, vis);

    mod->add_method(state, gct, name, method);

    if(Class* cls = try_as<Class>(mod)) {
      OnStack<1> o2(state, cls);

      if(!method->internalize(state, gct)) {
        Exception::argument_error(state, "invalid bytecode method");
        return 0;
      }

      object_type type = (object_type)cls->instance_type()->to_native();
      TypeInfo* ti = state->memory()->type_info[type];
      if(ti) {
        method->specialize(state, ti);
      }
    }

    bool add_ivars = false;

    if(Class* cls = try_as<Class>(mod)) {
      add_ivars = !kind_of<SingletonClass>(cls) &&
                  cls->type_info()->type == Object::type;
    } else {
      add_ivars = true;
    }

    if(add_ivars) {
      Array* ary = mod->seen_ivars();
      if(ary->nil_p()) {
        ary = Array::create(state, 5);
        mod->seen_ivars(state, ary);
      }

      Tuple* lits = method->literals();
      for(native_int i = 0; i < lits->num_fields(); i++) {
        if(Symbol* sym = try_as<Symbol>(lits->at(state, i))) {
          if(CBOOL(sym->is_ivar_p(state))) {
            if(!ary->includes_p(state, sym)) ary->append(state, sym);
          }
        }
      }
    }

    vm_reset_method_cache(state, name);

    return method;
  }

  Object* System::vm_attach_method(STATE, GCToken gct, Symbol* name,
                                   CompiledCode* method,
                                   ConstantScope* scope, Object* recv)
  {
    Module* mod = recv->singleton_class(state);

    method->scope(state, scope);
    method->serial(state, Fixnum::from(0));

    OnStack<2> os(state, mod, method);

    mod->add_method(state, gct, name, method);

    vm_reset_method_cache(state, name);

    return method;
  }

  Class* System::vm_object_class(STATE, Object* obj) {
    return obj->class_object(state);
  }

  Object* System::vm_object_singleton_class(STATE, Object* obj) {
    if(obj->reference_p()) return obj->singleton_class(state);
    if(obj->true_p()) return G(true_class);
    if(obj->false_p()) return G(false_class);
    if(obj->nil_p()) return G(nil_class);
    return Primitives::failure();
  }

  Object* System::vm_singleton_class_object(STATE, Module* mod) {
    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      return sc->attached_instance();
    }

    return cNil;
  }

  Object* System::vm_object_respond_to(STATE, Object* obj, Symbol* name) {
    return obj->respond_to(state, name, cFalse);
  }

  Object* System::vm_object_equal(STATE, Object* a, Object* b) {
    return a == b ? cTrue : cFalse;
  }

  Object* System::vm_object_kind_of(STATE, Object* obj, Module* mod) {
    return obj->kind_of_p(state, mod) ? cTrue : cFalse;
  }

  Object* System::vm_inc_global_serial(STATE) {
    return Fixnum::from(state->shared().inc_global_serial(state));
  }

  Object* System::vm_jit_block(STATE, GCToken gct, BlockEnvironment* env,
                               Object* show)
  {
#ifdef ENABLE_LLVM
    LLVMState* ls = LLVMState::get(state);

    OnStack<2> os(state, env, show);

    MachineCode* mcode = env->machine_code(state, gct);

    jit::Compiler jit(ls);
    jit.compile_block(ls, env->compiled_code(), mcode);

    if(show->true_p()) {
      jit.show_machine_code();
    }
#endif

    return show;
  }

  Object* System::vm_deoptimize_inliners(STATE, Executable* exec) {
    exec->clear_inliners(state);
    return cTrue;
  }

  Object* System::vm_deoptimize_all(STATE, Object* o_disable) {
    ObjectWalker walker(state->memory());
    GCData gc_data(state->vm());

    // Seed it with the root objects.
    walker.seed(gc_data);

    Object* obj = walker.next();

    int total = 0;

    bool disable = CBOOL(o_disable);

    // TODO: this should be inside tooling
    bool tooling_interpreter = state->shared().tool_broker()->tooling_interpreter_p();

    while(obj) {
      if(CompiledCode* code = try_as<CompiledCode>(obj)) {
        if(MachineCode* mcode = code->machine_code()) {
          mcode->deoptimize(state, code, 0, disable);
          if(tooling_interpreter) {
            mcode->run = MachineCode::tooling_interpreter;
          } else {
            mcode->run = MachineCode::interpreter;
          }
        }
        total++;
      }

      obj = walker.next();
    }

    return Integer::from(state, total);
  }

  Object* System::vm_raise_exception(STATE, Exception* exc) {
    state->raise_exception(exc);
    return NULL;
  }

  Fixnum* System::vm_memory_size(STATE, Object* obj) {
    if(obj->reference_p()) {
      size_t bytes = obj->size_in_bytes(state->vm());
      if(Bignum* b = try_as<Bignum>(obj)) {
        bytes += b->managed_memory_size(state);
      }
      Object* iv = obj->ivars();
      if(LookupTable* lt = try_as<LookupTable>(iv)) {
        bytes += iv->size_in_bytes(state->vm());
        bytes += lt->values()->size_in_bytes(state->vm());
        bytes += (lt->entries()->to_native() * sizeof(LookupTableBucket));
      } else if(iv->reference_p()) {
        bytes += iv->size_in_bytes(state->vm());
      }
      return Fixnum::from(bytes);
    }

    return Fixnum::from(0);
  }

  Object* System::vm_throw(STATE, Object* dest, Object* value) {
    state->vm()->thread_state()->raise_throw(dest, value);
    return NULL;
  }

  Object* System::vm_catch(STATE, Object* dest, Object* obj,
                           CallFrame* call_frame)
  {
    LookupData lookup(obj, obj->lookup_begin(state), G(sym_protected));
    Dispatch dis(G(sym_call));
    Arguments args(G(sym_call), 1, &dest);
    args.set_recv(obj);

    OnStack<1> os(state, dest);
    Object* ret = dis.send(state, call_frame, lookup, args);

    if(!ret && state->vm()->thread_state()->raise_reason() == cCatchThrow) {
      if(state->vm()->thread_state()->throw_dest() == dest) {
        Object* val = state->vm()->thread_state()->raise_value();
        state->vm()->thread_state()->clear_return();
        return val;
      }
    }

    return ret;
  }

  Object* System::vm_set_class(STATE, Object* obj, Class* cls) {
    if(!obj->reference_p()) return Primitives::failure();
    if(obj->type_id() != cls->type_info()->type)
      return Primitives::failure();

    if(kind_of<PackedObject>(obj)) {
      if(obj->klass()->packed_size() != cls->packed_size())
        return Primitives::failure();
    }

    obj->klass(state, cls);
    return obj;
  }

  Object* System::vm_method_missing_reason(STATE) {
    switch(state->vm()->method_missing_reason()) {
    case ePrivate:
      return state->symbol("private");
    case eProtected:
      return state->symbol("protected");
    case eSuper:
      return state->symbol("super");
    case eVCall:
      return state->symbol("vcall");
    case eNormal:
      return state->symbol("normal");
    default:
      return state->symbol("none");
    }
  }

  Object* System::vm_extended_modules(STATE, Object* obj) {
    if(SingletonClass* sc = try_as<SingletonClass>(obj->klass())) {
      Array* ary = Array::create(state, 3);

      Module* mod = sc->superclass();
      while(IncludedModule* im = try_as<IncludedModule>(mod)) {
        ary->append(state, im->module());

        mod = mod->superclass();
      }

      return ary;
    }

    return cNil;
  }

  Symbol* System::vm_get_kcode(STATE) {
    switch(state->shared().kcode_page()) {
    case kcode::eEUC:
      return state->symbol("EUC");
    case kcode::eSJIS:
      return state->symbol("SJIS");
    case kcode::eUTF8:
      return state->symbol("UTF8");
    default:
      return state->symbol("NONE");
    }
  }

  Object* System::vm_set_kcode(STATE, String* what) {
    if(what->byte_size() < 1) {
      kcode::set(state, kcode::eAscii);
    } else {
      const char* str = what->c_str(state);

      switch(str[0]) {
      case 'E':
      case 'e':
        kcode::set(state, kcode::eEUC);
        break;
      case 'S':
      case 's':
        kcode::set(state, kcode::eSJIS);
        break;
      case 'U':
      case 'u':
        kcode::set(state, kcode::eUTF8);
        break;
      default:
        kcode::set(state, kcode::eAscii);
        break;
      }
    }

    return vm_get_kcode(state);
  }

  Object* System::vm_const_defined(STATE, Symbol* sym,
                                   CallFrame* calling_environment)
  {
    bool found;

    Object* res = Helpers::const_get(state, calling_environment, sym, &found);

    if(!found || (!LANGUAGE_18_ENABLED(state) && kind_of<Autoload>(res))) {
      return Primitives::failure();
    }

    return res;
  }

  Object* System::vm_const_defined_under(STATE, Module* under, Symbol* sym,
                                         Object* send_const_missing,
                                         CallFrame* calling_environment)
  {
    bool found;

    Object* res = Helpers::const_get_under(state, under, sym, &found);
    if(!found) {
      if(send_const_missing->true_p()) {
        res = Helpers::const_missing_under(state, under, sym,
                                           calling_environment);
      } else {
        res = Primitives::failure();
      }
    }

    return res;
  }

  Object* System::vm_check_callable(STATE, Object* obj, Symbol* sym,
                                    Object* self)
  {
    Module* mod = obj->lookup_begin(state);

    MethodTableBucket* entry;
    bool skip_vis_check = false;

    while(!mod->nil_p()) {
      entry = mod->method_table()->find_entry(state, sym);

      if(entry) {
        if(entry->undef_p(state)) return cFalse;
        if(!skip_vis_check) {
          if(entry->private_p(state)) return cFalse;
          if(entry->protected_p(state)) {
            if(!self->kind_of_p(state, mod)) return cFalse;
          }
        }

        // It's callable, ok, but see if we should see if it's just a stub
        // to change the visibility of another method.
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
        } else {
          return cTrue;
        }
      }

      mod = mod->superclass();
    }

    return cFalse;
  }

  Object* System::vm_check_super_callable(STATE, CallFrame* call_frame) {
    if(call_frame->native_method_p()) return cTrue;

    Module* mod = call_frame->module()->superclass();

    MethodTableBucket* entry;
    Symbol* sym = call_frame->original_name();

    while(!mod->nil_p()) {
      entry = mod->method_table()->find_entry(state, sym);

      if(entry) {
        if(entry->undef_p(state)) return cFalse;

        // It's callable, ok, but see if we should see if it's just a stub
        // to change the visibility of another method.
        if(!entry->method()->nil_p()) {
          return cTrue;
        }
      }

      mod = mod->superclass();
    }

    return cFalse;
  }

  String* System::vm_get_user_home(STATE, String* name) {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return force_as<String>(Primitives::failure());
#else
    struct passwd *pwd;
    String* home = 0;

    if((pwd = getpwnam(name->c_str(state)))) {
      home = String::create(state, pwd->pw_dir);
    } else {
      home = nil<String>();
    }

    endpwent();
    return home;
#endif
  }

  Object* System::vm_agent_start(STATE) {
    state->shared().start_agent(state);
    return cNil;
  }

  IO* System::vm_agent_loopback(STATE) {
    QueryAgent* agent = state->shared().start_agent(state);

    int sock = agent->loopback_socket();
    if(sock < 0) {
      if(!agent->setup_local()) return nil<IO>();

      sock = agent->loopback_socket();
    }

    agent->wakeup();
    // dup the descriptor so the lifetime of socket is properly controlled.
    return IO::create(state, dup(sock));
  }

  Object* System::vm_set_finalizer(STATE, Object* obj, Object* fin) {
    if(!obj->reference_p()) return cFalse;
    state->memory()->set_ruby_finalizer(obj, fin);
    return cTrue;
  }

  Object* System::vm_object_lock(STATE, GCToken gct, Object* obj,
                                 CallFrame* call_frame)
  {
    if(!obj->reference_p()) return Primitives::failure();
    state->set_call_frame(call_frame);

    switch(obj->lock(state, gct)) {
    case eLocked:
      return cTrue;
    case eLockTimeout:
    case eUnlocked:
    case eLockError:
      return Primitives::failure();
    case eLockInterrupted:
      {
        Exception* exc = state->vm()->interrupted_exception();
        assert(!exc->nil_p());
        state->vm()->clear_interrupted_exception();
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);
        return 0;
      }
    }

    return cNil;
  }

  Object* System::vm_object_uninterrupted_lock(STATE, GCToken gct, Object* obj,
                                 CallFrame* call_frame)
  {
    if(!obj->reference_p()) return Primitives::failure();
    state->set_call_frame(call_frame);

    switch(obj->lock(state, gct, 0, false)) {
    case eLocked:
      return cTrue;
    case eLockTimeout:
    case eUnlocked:
    case eLockError:
    case eLockInterrupted:
      return Primitives::failure();
    }

    return cNil;
  }

  Object* System::vm_object_lock_timed(STATE, GCToken gct, Object* obj, Integer* time,
                                       CallFrame* call_frame)
  {
    if(!obj->reference_p()) return Primitives::failure();
    state->set_call_frame(call_frame);

    switch(obj->lock(state, gct, time->to_native())) {
    case eLocked:
      return cTrue;
    case eLockTimeout:
      return cFalse;
    case eUnlocked:
    case eLockError:
      return Primitives::failure();
    case eLockInterrupted:
      {
        Exception* exc = state->vm()->interrupted_exception();
        assert(!exc->nil_p());
        state->vm()->clear_interrupted_exception();
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);
        return 0;
      }
      return 0;
    }

    return cNil;
  }

  Object* System::vm_object_trylock(STATE, GCToken gct, Object* obj,
                                    CallFrame* call_frame)
  {
    if(!obj->reference_p()) return Primitives::failure();
    state->set_call_frame(call_frame);
    if(obj->try_lock(state, gct) == eLocked) return cTrue;
    return cFalse;
  }

  Object* System::vm_object_locked_p(STATE, GCToken gct, Object* obj) {
    if(!obj->reference_p()) return cFalse;
    if(obj->locked_p(state, gct)) return cTrue;
    return cFalse;
  }

  Object* System::vm_object_unlock(STATE, GCToken gct, Object* obj,
                                   CallFrame* call_frame)
  {
    if(!obj->reference_p()) return Primitives::failure();
    state->set_call_frame(call_frame);

    if(obj->unlock(state, gct) == eUnlocked) return cNil;
    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->vm()->thread_id() << " unlock failed]" << std::endl;
    }
    return Primitives::failure();
  }

  Object* System::vm_memory_barrier(STATE) {
    atomic::memory_barrier();
    return cNil;
  }

  Object* System::vm_ruby18_p(STATE) {
    return LANGUAGE_18_ENABLED(state) ? cTrue : cFalse;
  }

  Object* System::vm_ruby19_p(STATE) {
    return LANGUAGE_19_ENABLED(state) ? cTrue : cFalse;
  }

  Object* System::vm_ruby20_p(STATE) {
    return LANGUAGE_20_ENABLED(state) ? cTrue : cFalse;
  }

  Object* System::vm_windows_p(STATE) {
#ifdef RBX_WINDOWS
    return cTrue;
#else
    return cFalse;
#endif
  }

  Object* System::vm_darwin_p(STATE) {
#ifdef RBX_DARWIN
    return cTrue;
#else
    return cFalse;
#endif
  }

  Object* System::vm_bsd_p(STATE) {
#ifdef RBX_BSD
    return cTrue;
#else
    return cFalse;
#endif
  }

  Object* System::vm_linux_p(STATE) {
#ifdef RBX_LINUX
    return cTrue;
#else
    return cFalse;
#endif
  }

  String* System::sha1_hash(STATE, String* str) {
    XSHA1_CTX ctx;
    XSHA1_Init(&ctx);
    XSHA1_Update(&ctx, str->byte_address(), str->byte_size());

    uint8_t digest[20];
    XSHA1_Finish(&ctx, digest);

    char buf[40];
    static const char hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'
    };

    for(int i = 0; i < 20; i++) {
      unsigned char byte = digest[i];
      buf[i + i]     = hex[byte >> 4];
      buf[i + i + 1] = hex[byte & 0x0f];
    }

    return String::create(state, buf, 40);
  }

  Tuple* System::vm_thread_state(STATE) {
    ThreadState* ts = state->vm()->thread_state();
    Tuple* tuple = Tuple::create(state, 5);

    Symbol* reason = 0;
    switch(ts->raise_reason()) {
    case cNone:
      reason = state->symbol("none");
      break;
    case cException:
      reason = state->symbol("exception");
      break;
    case cReturn:
      reason = state->symbol("return");
      break;
    case cBreak:
      reason = state->symbol("break");
      break;
    case cExit:
      reason = state->symbol("exit");
      break;
    case cCatchThrow:
      reason = state->symbol("catch_throw");
      break;
    case cThreadKill:
      reason = state->symbol("thread_kill");
      break;
    default:
      reason = state->symbol("unknown");
    }

    tuple->put(state, 0, reason);
    tuple->put(state, 1, ts->raise_value());
    tuple->put(state, 2, ts->destination_scope());
    tuple->put(state, 3, ts->current_exception());
    tuple->put(state, 4, ts->throw_dest());

    return tuple;
  }

  Object* System::vm_run_script(STATE, GCToken gct, CompiledCode* code,
                                CallFrame* calling_environment)
  {
    Dispatch msg(state->symbol("__script__"), G(object), code);
    Arguments args(state->symbol("__script__"), G(main), cNil, 0, 0);

    OnStack<1> os(state, code);

    code->internalize(state, gct, 0, 0);

#ifdef RBX_PROFILER
    if(unlikely(state->vm()->tooling())) {
      tooling::ScriptEntry me(state, code);
      return code->machine_code()->execute_as_script(state, code, calling_environment);
    } else {
      return code->machine_code()->execute_as_script(state, code, calling_environment);
    }
#else
    return code->machine_code()->execute_as_script(state, code, calling_environment);
#endif
  }

#define HASH_TRIE_BASE_SHIFT  6

#if RBX_SIZEOF_LONG == 8
#define HASH_TRIE_BIT_WIDTH   6
#define HASH_TRIE_BIT_MASK    0x3f
#else
#define HASH_TRIE_BIT_WIDTH   5
#define HASH_TRIE_BIT_MASK    0x1f
#endif

  static inline size_t hash_trie_bit(Fixnum* hash, Fixnum* level) {
    native_int h = hash->to_native();
    native_int l = level->to_native();

    size_t width = HASH_TRIE_BIT_WIDTH;
    size_t mask = HASH_TRIE_BIT_MASK;
    size_t base = HASH_TRIE_BASE_SHIFT;
    size_t result = 1;

    return result << ((h >> (l * width + base)) & mask);
  }

  static inline int hash_trie_index(size_t m) {
#if RBX_SIZEOF_LONG == 8
    native_int sk5 = 0x5555555555555555;
    native_int sk3 = 0x3333333333333333;
    native_int skf0 = 0x0F0F0F0F0F0F0F0F;

    m -= (m >> 1) & sk5;
    m = (m & sk3) + ((m >> 2) & sk3);
    m = (m & skf0) + ((m >> 4) & skf0);
    m += m >> 8;
    m += m >> 16;
    m = (m + (m >> 32)) & 0xFF;
#else
    native_int sk5 = 0x55555555;
    native_int sk3 = 0x33333333;
    native_int skf0 = 0xF0F0F0F;

    m -= (m >> 1) & sk5;
    m = (m & sk3) + ((m >> 2) & sk3);
    m = (m & skf0) + ((m >> 4) & skf0);
    m += m >> 8;
    m = (m + (m >> 16)) & 0x3F;
#endif

    return m;
  }

  Fixnum* System::vm_hash_trie_item_index(STATE, Fixnum* hash,
                                           Fixnum* level, Integer* map)
  {
    size_t m = map->to_ulong();
    size_t b = hash_trie_bit(hash, level);

    if(m & b) {
      return Fixnum::from(hash_trie_index((b - 1) & m));
    } else {
      return nil<Fixnum>();
    }
  }

  Integer* System::vm_hash_trie_set_bitmap(STATE, Fixnum* hash,
                                           Fixnum* level, Integer* map)
  {
    size_t m = map->to_ulong();
    size_t b = hash_trie_bit(hash, level);

    return Integer::from(state, m | b);
  }

  Integer* System::vm_hash_trie_unset_bitmap(STATE, Fixnum* hash,
                                             Fixnum* level, Integer* map)
  {
    size_t m = map->to_ulong();
    size_t b = hash_trie_bit(hash, level);

    return Integer::from(state, m & ~b);
  }

  String* System::vm_get_module_name(STATE, Module* mod) {
    return mod->get_name(state);
  }

  Object* System::vm_set_module_name(STATE, Module* mod, Object* name, Object* under) {
    if(name->nil_p()) return cNil;

    if(under->nil_p()) under = G(object);
    mod->set_name(state, as<Symbol>(name), as<Module>(under));

    return cNil;
  }

  String* System::vm_set_process_title(STATE, String* title) {
    setproctitle("%s", title->c_str_null_safe(state));
    return title;
  }
}
