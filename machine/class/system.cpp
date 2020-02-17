#include "arguments.hpp"
#include "raise_reason.hpp"

#include "class/array.hpp"
#include "class/autoload.hpp"
#include "class/bignum.hpp"
#include "class/block_environment.hpp"
#include "class/channel.hpp"
#include "class/class.hpp"
#include "class/compact_lookup_table.hpp"
#include "class/lexical_scope.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/io.hpp"
#include "class/location.hpp"
#include "class/lookup_table.hpp"
#include "class/method_table.hpp"
#include "class/native_method.hpp"
#include "class/thread.hpp"
#include "class/tuple.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/system.hpp"
#include "class/unwind_state.hpp"
#include "class/variable_scope.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"
#include "config_parser.hpp"
#include "dtrace/dtrace.h"
#include "environment.hpp"
#include "helpers.hpp"
#include "lookup_data.hpp"
#include "memory.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "signals.hpp"
#include "thread_phase.hpp"
#include "windows_compat.h"

#include "memory/collector.hpp"
#include "memory/visitor.hpp"

#include "diagnostics/machine.hpp"

#include "util/sha1.h"
#include "util/timing.h"
#include "logger.hpp"

#include "paths.h"

#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <signal.h>
#include <sstream>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#ifndef RBX_WINDOWS
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <dlfcn.h>
#endif

#include "missing/setproctitle.h"

namespace rubinius {

  void System::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_class"),
                             state->symbol("vm_open_class"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_class_under"),
                             state->symbol("vm_open_class_under"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_module"),
                             state->symbol("vm_open_module"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_module_under"),
                             state->symbol("vm_open_module_under"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("add_defn_method"),
                             state->symbol("vm_add_method"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("attach_method"),
                             state->symbol("vm_attach_method"));

    System::attach_primitive(state,
                             as<Module>(G(rubinius)->get_const(state, "Type")), true,
                             state->symbol("object_singleton_class"),
                             state->symbol("vm_object_singleton_class"));

  }

  void System::attach_primitive(STATE, Module* mod, bool meta,
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

    tbl->store(state, name, nil<String>(), oc, nil<LexicalScope>(),
        Fixnum::from(0), G(sym_public));
  }

/* Primitives */

  Object* System::yield_gdb(STATE, Object* obj) {
    obj->show(state);
    Exception::raise_assertion_error(state, "yield_gdb called and not caught");
    return obj;
  }

  void exec_sh_fallback(STATE, const char* c_str, size_t c_len) {
    char* s = const_cast<char*>(c_str);
    bool use_sh = false;

    for(;*s;s++) {
      if(*s != ' ' && !ISALPHA(*s) && strchr("*?{}[]<>()~&|\\$;'`\"\n\t\r\f\v",*s)) {
        use_sh = true;
        break;
      }
    }

    if(use_sh) {
      execl("/bin/sh", "sh", "-c", c_str, (char*)0);
    } else {
      size_t max_spaces = (c_len / 2) + 2;
      char** args = new char*[max_spaces];

      // Now put nulls for spaces into c_str and assign each bit
      // to args to create the array of char*s that execv wants.

      s = const_cast<char*>(c_str);
      const char* s_end = c_str + c_len;
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
      if(idx > 0) {
        for(int i = 0; i < 5; i++) {
          if(::execvp(args[0], args) < 0) {
            if(errno != EAGAIN) break;
          }
        }
      }

      // If we failed, clean up the args.
      delete[] args;
    }
  }

  class ExecCommand {
    char* command_;
    size_t command_size_;
    size_t argc_;
    char** argv_;

    char* make_string(STATE, String* source) {
      const char* src = source->c_str_null_safe(state);
      size_t len = strnlen(src, source->byte_size());
      char* str = new char[len + 1];

      memcpy(str, src, len);
      str[len] = 0;

      return str;
    }

  public:
    ExecCommand(STATE, String* command)
      : argc_(0)
      , argv_(nullptr)
    {
      command_ = make_string(state, command);
      command_size_ = command->byte_size();
    }

    ExecCommand(STATE, String* command, Array* args) {
      command_ = make_string(state, command);
      command_size_ = command->byte_size();

      argc_ = args->size();
      argv_ = nullptr;

      if(argc_ > 0) {
        argv_ = new char*[argc_ + 1];

        /* execvp() requires a NULL as last element */
        argv_[argc_] = nullptr;

        for(size_t i = 0; i < argc_; i++) {
          /* POSIX guarantees that execvp does not modify the characters to
           * which the argv pointers point, despite the argument not being
           * declared as const char *const[].
           */
          argv_[i] = make_string(state, as<String>(args->get(state, i)));
        }
      }
    }

    ~ExecCommand() {
      if(argc_ > 0 && argv_) {
        for(size_t i = 0; i < argc_; i++) {
          delete[] argv_[i];
        }

        delete[] argv_;
      }

      delete[] command_;
    }

    char* command() {
      return command_;
    }

    size_t command_size() {
      return command_size_;
    }

    size_t argc() {
      return argc_;
    }

    char** argv() {
      return argv_;
    }
  };

  static void redirect_file_descriptor(int from, int to) {
    if(dup2(to, from) < 0) return;

    int flags = fcntl(from, F_GETFD);
    if(flags < 0) return;

    fcntl(from, F_SETFD, flags & ~FD_CLOEXEC);
  }

  Object* System::vm_spawn_setup(STATE, Object* spawn_state) {
    if(LookupTable* table = try_as<LookupTable>(spawn_state)) {
      if(Array* env = try_as<Array>(
            table->fetch(state, state->symbol("env"))))
      {
        intptr_t size = env->size();
        for(intptr_t i = 0; i < size; i += 2) {
          const char* key = as<String>(env->get(state, i))->c_str_null_safe(state);
          Object* value = env->get(state, i + 1);

          if(value->nil_p()) {
            unsetenv(key);
          } else {
            setenv(key, as<String>(value)->c_str_null_safe(state), 1);
          }
        }
      }

      if(Fixnum* pgrp = try_as<Fixnum>(
            table->fetch(state, state->symbol("pgroup"))))
      {
        setpgid(0, pgrp->to_native());
      }

      if(Fixnum* mask = try_as<Fixnum>(
            table->fetch(state, state->symbol("umask"))))
      {
        umask(mask->to_native());
      }

      if(String* str = try_as<String>(
            table->fetch(state, state->symbol("chdir"))))
      {
        const char* dir = str->c_str_null_safe(state);
        if(chdir(dir) < 0) {
          logger::error("%s: spawn: failed to change directory: %s",
              strerror(errno), dir);
        }
      }

      if(CBOOL(table->has_key(state, state->symbol("close_others")))) {
        Class* fd_class = (Class*) G(io)->get_const(state, "FileDescriptor");
        Fixnum* max_fd = (Fixnum*)fd_class->send(state, state->symbol("max_fd"));
        int max = max_fd->to_native();
        int flags;

        for(int fd = STDERR_FILENO + 1; fd < max; fd++) {
          if((flags = fcntl(fd, F_GETFD)) >= 0) {
            fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
          }
        }
      }

      if(Array* assign = try_as<Array>(
            table->fetch(state, state->symbol("assign_fd"))))
      {
        intptr_t size = assign->size();
        for(intptr_t i = 0; i < size; i += 4) {
          int from = as<Fixnum>(assign->get(state, i))->to_native();
          int to = IO::open_with_cloexec(state,
              as<String>(assign->get(state, i + 1))->c_str(state),
              as<Fixnum>(assign->get(state, i + 2))->to_native(),
              as<Fixnum>(assign->get(state, i + 3))->to_native());

          redirect_file_descriptor(from, to);
        }
      }

      if(Array* redirect = try_as<Array>(
            table->fetch(state, state->symbol("redirect_fd"))))
      {
        intptr_t size = redirect->size();
        for(intptr_t i = 0; i < size; i += 2) {
          int from = as<Fixnum>(redirect->get(state, i))->to_native();
          int to = as<Fixnum>(redirect->get(state, i + 1))->to_native();

          redirect_file_descriptor(from, to < 0 ? (-to + 1) : to);
        }
      }
    }

    return cNil;
  }

  static int fork_exec(STATE, int errors_fd) {
    StopPhase locked(state);

    // Hold the logger lock to avoid racing the logger
    logger::lock();

    // If execvp() succeeds, we'll read EOF and know.
    fcntl(errors_fd, F_SETFD, FD_CLOEXEC);

    int pid = ::fork();

    logger::unlock();

    if(pid == 0) {
      // We're in the child...
      state->machine()->after_fork_child(state);
    }

    return pid;
  }

  Object* System::vm_spawn(STATE, Object* spawn_state, String* path,
                           Array* args)
  {
    OnStack<1> os(state, spawn_state);

    /* Setting up the command and arguments may raise an exception so do it
     * before everything else.
     */
    ExecCommand exe(state, path, args);

    int errors[2];

    if(pipe(errors) != 0) {
      Exception::raise_errno_error(state, "error setting up pipes", errno, "pipe(2)");
      return nullptr;
    }

    int pid = fork_exec(state, errors[1]);

    // error
    if(pid == -1) {
      close(errors[0]);
      close(errors[1]);

      Exception::raise_errno_error(state, "error forking", errno, "fork(2)");
      return nullptr;
    }

    if(pid == 0) {
      // in child
      close(errors[0]);

      // Setup ENV, redirects, groups, etc. in the child before exec().
      vm_spawn_setup(state, spawn_state);

      /* Reset all signal handlers to the defaults, so any we setup in
       * Rubinius won't leak through. We need to use sigaction() here since
       * signal() provides no control over SA_RESTART and can use the wrong
       * value causing blocking I/O methods to become uninterruptable.
       */
      for(int i = 1; i < NSIG; i++) {
        struct sigaction action;

        action.sa_handler = SIG_DFL;
        action.sa_flags = 0;
        sigfillset(&action.sa_mask);

        sigaction(i, &action, nullptr);
      }

      int exec_errno = 0;

      if(exe.argc()) {
        for(int i = 0; i < 5; i++) {
          if(::execvp(exe.command(), exe.argv()) < 0) {
            exec_errno = errno;
            if(errno != EAGAIN) break;
          }
        }
      } else {
        exec_sh_fallback(state, exe.command(), exe.command_size());
        exec_errno = errno;
      }

      /* execvp() returning means it failed. */
      std::ostringstream command_line;
      command_line << exe.command();
      for(size_t i = 0; i < exe.argc(); i++) {
        command_line << " " << exe.argv()[i];
      }
      logger::error("%s: spawn: exec failed: %s",
          strerror(errno), command_line.str().c_str());

      int error_no = errno;
      if(write(errors[1], &error_no, sizeof(int)) < 0) {
        logger::error("%s: spawn: writing error status", strerror(errno));
      }
      close(errors[1]);

      /* Return saved errno to parent since bugs in the logger or other
         code after the call to execvp could overwrite errno.
      */
      exit(exec_errno);
    }

    int error_no = 0;
    int exec_errno = 0;
    ssize_t size;

    {
      UnmanagedPhase unmanaged(state);

      close(errors[1]);

      if(state->configuration()->log_lifetime.value) {
        const std::regex& filter = state->configuration()->log_filter();

        if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
          logger::write("process: spawn: %d: %s, %s, %s:%d",
              pid, exe.command(),
              state->name().c_str(),
              call_frame->file(state)->cpp_str(state).c_str(),
              call_frame->line(state));
        } else {
          logger::write("process: spawn: %d: %s, %s",
              pid, exe.command(),
              state->name().c_str());
        }
      }

      while((size = read(errors[0], &error_no, sizeof(int))) < 0) {
        switch(errno) {
        case EAGAIN:
        case EINTR:
          continue;
        default:
          logger::error("%s: spawn: reading error status", strerror(errno));
          break;
        }
      }
      close(errors[0]);

      if(size != 0) {
        int status, options = 0;

        waitpid(pid, &status, options);

        if(WIFEXITED(status)) {
          exec_errno = WEXITSTATUS(status);
        }
      }
    }

    if(size != 0) {
      Exception::raise_errno_error(state, "execvp(2) failed", exec_errno);
      return nullptr;
    }

    return Fixnum::from(pid);
  }

  Object* System::vm_backtick(STATE, String* str) {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return Primitives::failure();
#else
    /* Setting up the command may raise an exception so do it before
     * everything else.
     */
    ExecCommand exe(state, str);

    int errors[2], output[2];

    if(pipe(errors) != 0) {
      Exception::raise_errno_error(state, "error setting up pipes", errno, "pipe(2)");
      return nullptr;
    }

    if(pipe(output) != 0) {
      Exception::raise_errno_error(state, "error setting up pipes", errno, "pipe(2)");
      return nullptr;
    }

    int pid = fork_exec(state, errors[1]);

    // error
    if(pid == -1) {
      close(errors[0]);
      close(errors[1]);
      close(output[0]);
      close(output[1]);

      Exception::raise_errno_error(state, "error forking", errno, "fork(2)");
      return nullptr;
    }

    if(pid == 0) {
      close(errors[0]);
      close(output[0]);

      dup2(output[1], STDOUT_FILENO);
      close(output[1]);

      /* Reset all signal handlers to the defaults, so any we setup in
       * Rubinius won't leak through. We need to use sigaction() here since
       * signal() provides no control over SA_RESTART and can use the wrong
       * value causing blocking I/O methods to become uninterruptable.
       */
      for(int i = 1; i < NSIG; i++) {
        struct sigaction action;

        action.sa_handler = SIG_DFL;
        action.sa_flags = 0;
        sigfillset(&action.sa_mask);

        sigaction(i, &action, nullptr);
      }

      exec_sh_fallback(state, exe.command(), exe.command_size());
      int exec_errno = errno;

      /* execvp() returning means it failed. */
      logger::error("%s: backtick: exec failed: %s",
          strerror(errno), exe.command());

      int error_no = errno;
      if(write(errors[1], &error_no, sizeof(int)) < 0) {
        logger::error("%s: backtick: writing error status", strerror(errno));
      }
      close(errors[1]);

      exit(exec_errno);
    }

    int error_no = 0;
    int exec_errno = 0;
    ssize_t size;

    {
      UnmanagedPhase unmanaged(state);

      close(errors[1]);
      close(output[1]);

      if(state->configuration()->log_lifetime.value) {
        const std::regex& filter = state->configuration()->log_filter();

        if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
          logger::write("process: backtick: %d: %s, %s, %s:%d",
              pid, exe.command(),
              state->name().c_str(),
              call_frame->file(state)->cpp_str(state).c_str(),
              call_frame->line(state));
        } else {
          logger::write("process: backtick: %d: %s, %s",
              pid, exe.command(),
              state->name().c_str());
        }
      }

      while((size = read(errors[0], &error_no, sizeof(int))) < 0) {
        switch(errno) {
        case EAGAIN:
        case EINTR:
          continue;
        default:
          logger::error("%s: backtick: reading error status", strerror(errno));
          break;
        }
      }
      close(errors[0]);

      if(size != 0) {
        int status, options = 0;

        waitpid(pid, &status, options);

        if(WIFEXITED(status)) {
          exec_errno = WEXITSTATUS(status);
        }

        close(output[0]);
      }
    }

    if(size != 0) {
      Exception::raise_errno_error(state, "execvp(2) failed", exec_errno);
      return nullptr;
    }

    std::string buf;

    {
      UnmanagedPhase unmanaged(state);

      for(;;) {
        char raw_buf[1024];
        ssize_t bytes = read(output[0], raw_buf, 1023);

        if(bytes < 0) {
          switch(errno) {
            case EAGAIN:
            case EINTR:
              {
                ManagedPhase managed(state);
                if(state->thread_interrupted_p()) {
                  close(output[0]);
                  return nullptr;
                }
              }
              continue;
            default:
              close(output[0]);
              {
                ManagedPhase managed(state);
                Exception::raise_errno_error(state, "reading child data", errno, "read(2)");
              }
          }
        }

        if(bytes == 0) {
          break;
        }
        buf.append(raw_buf, bytes);
      }

      close(output[0]);
    }

    return Tuple::from(state, 2, Fixnum::from(pid),
                       String::create(state, buf.c_str(), buf.size()));
#endif  // RBX_WINDOWS
  }

  Object* System::vm_exec(STATE, String* path, Array* args) {
    StopPhase locked(state);

    /* Setting up the command and arguments may raise an exception so do it
     * before everything else.
     */
    ExecCommand exe(state, path, args);

    if(state->configuration()->log_lifetime.value) {
      const std::regex& filter = state->configuration()->log_filter();

      if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
        logger::write("process: exec: %s, %s, %s:%d", exe.command(),
            state->name().c_str(),
            call_frame->file(state)->cpp_str(state).c_str(),
            call_frame->line(state));
      } else {
        logger::write("process: exec: %s, %s", exe.command(),
            state->name().c_str());
      }
    }

    void* old_handlers[NSIG];

    /* Reset all signal handlers to the defaults, so any we setup in Rubinius
     * won't leak through. We need to use sigaction() here since signal()
     * provides no control over SA_RESTART and can use the wrong value causing
     * blocking I/O methods to become uninterruptable.
     */
    for(int i = 1; i < NSIG; i++) {
      struct sigaction action;
      struct sigaction old_action;

      action.sa_handler = SIG_DFL;
      action.sa_flags = 0;
      sigfillset(&action.sa_mask);

      sigaction(i, &action, &old_action);
      old_handlers[i] = (void*)old_action.sa_handler;
    }

    if(exe.argc()) {
      for(int i = 0; i < 5; i++) {
        if(::execvp(exe.command(), exe.argv()) < 0) {
          if(errno != EAGAIN) break;
        }
      }
    } else {
      exec_sh_fallback(state, exe.command(), exe.command_size());
    }

    int erno = errno;

    // Hmmm, execvp failed, we need to recover here.

    for(int i = 1; i < NSIG; i++) {
      struct sigaction action;

      action.sa_handler = (void(*)(int))old_handlers[i];
      action.sa_flags = 0;
      sigfillset(&action.sa_mask);

      sigaction(i, &action, nullptr);
    }

    /* execvp() returning means it failed. */
    Exception::raise_errno_error(state, "execvp(2) failed", erno);

    return nullptr;
  }

  Object* System::vm_waitpid(STATE, Fixnum* pid_obj, Object* no_hang) {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return Primitives::failure();
#else
    pid_t input_pid = pid_obj->to_native();
    int options = WNOHANG;
    int status;
    pid_t pid;
    bool request_nohang = CBOOL(no_hang);

  retry:

    {
      UnmanagedPhase unmanaged(state);
      pid = waitpid(input_pid, &status, options);
    }

    if(pid == 0) {
      if(request_nohang) {
        return cNil;
      } else {
        goto retry;
      }
    }

    if(pid == -1) {
      if(errno == ECHILD) return cFalse;

      if(errno == EINTR) {
        if(state->thread_interrupted_p()) return nullptr;
        goto retry;
      }

      // TODO handle other errnos?
      return cFalse;
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
    state->unwind_state()->raise_exit(code);
    return nullptr;
  }

  Fixnum* System::vm_fork(STATE) {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return force_as<Fixnum>(Primitives::failure());
#else
    if(Fiber* fiber = state->fiber()) {
      if(!fiber->nil_p() && !fiber->root_p()) {
        Exception::raise_runtime_error(state, "fork() not allowed from Fiber");
      }
    }

    int pid = -1;

    {
      StopPhase locked(state);

      state->machine()->before_fork(state);

      // Hold the logger lock to avoid racing the logger
      logger::lock();

      pid = ::fork();

      logger::unlock();

      if(pid > 0) {
        // We're in the parent...
        state->machine()->after_fork_parent(state);

        if(state->configuration()->log_lifetime.value) {
          const std::regex& filter = state->configuration()->log_filter();

          if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
            logger::write("process: fork: child: %d, %s, %s:%d", pid,
                state->name().c_str(),
                call_frame->file(state)->cpp_str(state).c_str(),
                call_frame->line(state));
          } else {
            logger::write("process: fork: child: %d, %s", pid,
                state->name().c_str());
          }
        }
      }
    }

    if(pid == 0) {
      // We're in the child...
      state->machine()->after_fork_child(state);

      // In the child, the PID is nil in Ruby.
      return nil<Fixnum>();
    } else if(pid < 0) {
      // We errored...
      Exception::raise_errno_error(state, "fork(2) failed");
      return nullptr;
    }

    // The compiler can't understand the covering above, so...
    return Fixnum::from(pid);
#endif
  }

  Object* System::vm_gc_start(STATE, Object* force) {
    // force is set if this is being called by the core library (for instance
    // in File#ininitialize). If we decided to ignore some GC.start calls
    // by usercode trying to be clever, we can use force to know that we
    // should NOT ignore it.
    if(CBOOL(force) || state->configuration()->collector_honor_start) {
      state->collector()->collect_requested(state,
          "collector: request to collect from managed code");
    }
    return cNil;
  }

  Object* System::vm_get_config_item(STATE, String* var) {
    ConfigParser::Entry* ent = state->environment()->user_variables()->find(var->c_str(state));
    if(!ent) return cNil;

    if(ent->is_number()) {
      return Integer::from_cppstr(state, ent->value, 10);
    } else if(ent->is_true()) {
      return cTrue;
    }

    return String::create(state, ent->value.c_str(), ent->value.size());
  }

  Object* System::vm_get_config_section(STATE, String* section) {
    ConfigParser::EntryList* list;

    list = state->environment()->user_variables()->get_section(
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

  Object* System::vm_reset_method_cache(STATE, Module* mod, Symbol* name) {
    mod->reset_method_cache(state, name);

    state->metrics()->cache_resets++;

    return cTrue;
  }

   /*  @todo Could possibly capture the system backtrace at this
   *        point. --rue
   */
  Array* System::vm_backtrace(STATE, Fixnum* skip) {
    return Location::from_call_stack(state, skip->to_native());
  }

  Array* System::vm_mri_backtrace(STATE, Fixnum* skip) {
    return Location::mri_backtrace(state, skip->to_native());
  }


  Object* System::vm_show_backtrace(STATE) {
    state->call_frame()->print_backtrace(state);
    return cNil;
  }

  Object* System::vm_write_error(STATE, String* str) {
    std::cerr << str->c_str(state) << std::endl;
    return cNil;
  }

  Object* System::vm_watch_signal(STATE, Fixnum* sig, Object* ignored) {
    if(Signals* signals = state->machine()->signals()) {
      intptr_t i = sig->to_native();

      if(i < 0) {
        signals->add_signal_handler(state, -i, Signals::eDefault);
      } else if(i > 0) {
        signals->add_signal_handler(state, i,
            CBOOL(ignored) ? Signals::eIgnore : Signals::eCustom);
      }

      return cTrue;
    } else {
      return cFalse;
    }
  }

  Object* System::vm_signal_thread(STATE) {
    return state->machine()->signals()->signal_thread()->thread_state()->thread();
  }

  Object* System::vm_time(STATE) {
    return Integer::from(state, time(0));
  }

  static inline double tv_to_dbl(struct timeval* tv) {
    return (double)tv->tv_sec + ((double)tv->tv_usec / 1000000.0);
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

    uint64_t sys = 0;
    uint64_t usr = 0;
    thread_cpu_usage(&usr, &sys);

    ary->set(state, 4, Float::create(state, (double)usr / 1000000.0));
    ary->set(state, 5, Float::create(state, (double)sys / 1000000.0));

    return ary;
#endif
  }

  Class* System::vm_open_class(STATE, Symbol* name, Object* sup,
                               LexicalScope* scope)
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
    ConstantMissingReason reason = vNonExistent;

    Object* obj = under->get_const(state, name, G(sym_private), &reason);
    if(reason == vFound) {
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
        //                Fixnum::from(0));
        state->raise_exception(exc);
        return nullptr;
      }

      return cls;
    }

    if(super->nil_p()) super = G(object);

    return Class::create(state, as<Class>(super), under, name);
  }

  Module* System::vm_open_module(STATE, Symbol* name, LexicalScope* scope) {
    Module* under = G(object);

    if(!scope->nil_p()) {
      under = scope->module();
    }

    return vm_open_module_under(state, name, under);
  }

  Module* System::vm_open_module_under(STATE, Symbol* name, Module* under) {
    ConstantMissingReason reason = vNonExistent;

    Object* obj = under->get_const(state, name, G(sym_private), &reason);

    if(reason == vFound) return as<Module>(obj);

    Module* module = Module::create(state);

    module->set_name(state, name, under);
    under->set_const(state, name, module);

    return module;
  }

  static Tuple* find_method(STATE, Module* lookup_begin, Symbol* name, Symbol* min_visibility) {
    // Use cUndef for the self type so protected checks never pass
    // and work as expected.
    LookupData lookup(cUndef, lookup_begin, min_visibility);

    Dispatch dispatch(name);

    if(!dispatch.resolve(state, name, lookup)) {
      return nil<Tuple>();
    }

    return Tuple::from(state, 2, dispatch.method, dispatch.module);
  }

  Tuple* System::vm_find_method(STATE, Object* recv, Symbol* name) {
    return find_method(state, recv->lookup_begin(state), name, G(sym_private));
  }

  Tuple* System::vm_find_public_method(STATE, Object* recv, Symbol* name) {

    return find_method(state, recv->lookup_begin(state), name, G(sym_public));
  }

  Object* System::vm_add_method(STATE, Symbol* name,
                                Object* method,
                                LexicalScope* scope, Object* vis)
  {
    Module* mod = scope->for_method_definition();

    CompiledCode* cc = try_as<CompiledCode>(method);
    if(cc) {
      cc->scope(state, scope);
      cc->serial(state, Fixnum::from(0));
      mod->add_method(state, name, nil<String>(), cc, scope);
    } else {
      mod->add_method(state, name, as<String>(method), cNil, scope);
    }

    if(!cc) return method;

    if(Class* cls = try_as<Class>(mod)) {
      OnStack<5> o2(state, mod, cc, scope, vis, cls);

      if(!cc->internalize(state)) return 0;

      object_type type = (object_type)cls->instance_type()->to_native();
      TypeInfo* ti = state->memory()->type_info[type];
      if(ti) {
        cc->specialize(state, ti);
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

      Tuple* lits = cc->literals();
      for(intptr_t i = 0; i < lits->num_fields(); i++) {
        if(Symbol* sym = try_as<Symbol>(lits->at(state, i))) {
          if(CBOOL(sym->is_ivar_p(state))) {
            if(!ary->includes_p(state, sym)) ary->append(state, sym);
          }
        }
      }
    }

    return cc;
  }

  Object* System::vm_attach_method(STATE, Symbol* name,
                                   Object* method,
                                   LexicalScope* scope, Object* recv)
  {
    Module* mod = recv->singleton_class(state);

    if(CompiledCode* cc = try_as<CompiledCode>(method)) {
      cc->scope(state, scope);
      cc->serial(state, Fixnum::from(0));
      mod->add_method(state, name, nil<String>(), cc, scope);
    } else {
      mod->add_method(state, name, as<String>(method), cNil, scope);
    }

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
      return sc->singleton();
    }

    return cNil;
  }

  Object* System::vm_object_respond_to(STATE, Object* obj, Symbol* name, Object* include_private) {
    return obj->respond_to(state, name, include_private);
  }

  Object* System::vm_object_equal(STATE, Object* a, Object* b) {
    return RBOOL(a->equal_p(b));
  }

  Object* System::vm_object_kind_of(STATE, Object* obj, Module* mod) {
    return RBOOL(obj->kind_of_p(state, mod));
  }

  Object* System::vm_global_serial(STATE) {
    return Fixnum::from(state->memory()->global_serial());
  }

  Object* System::vm_inc_global_serial(STATE) {
    if(state->configuration()->serial_debug) {
      std::cerr << std::endl
                << "global serial increased from "
                << state->memory()->global_serial()
                << std::endl;

      state->call_frame()->print_backtrace(state, std::cerr, 6, true);
    }

    return Fixnum::from(state->memory()->inc_global_serial());
  }

  Object* System::vm_deoptimize_all(STATE, Object* o_disable) {
    int total = 0;

    /* TODO: GC: use MemoryVisitor
    memory::ObjectWalker walker(state->memory());
    memory::GCData gc_data(state);

    // Seed it with the root objects.
    walker.seed(gc_data);

    Object* obj = walker.next();

    // bool disable = CBOOL(o_disable);

    while(obj) {
      if(CompiledCode* code = try_as<CompiledCode>(obj)) {
        if(MachineCode* mcode = code->machine_code()) {
          // mcode->deoptimize(state, code, disable);
        }
        total++;
      }

      obj = walker.next();
    }
    */

    return Integer::from(state, total);
  }

  Object* System::vm_raise_exception(STATE, Exception* exc) {
    state->raise_exception(exc);

    if(state->configuration()->log_exceptions.value) {
      logger::write("exception: %s: %s",
          exc->class_object(state)->module_name()->cpp_str(state).c_str(), exc->message_c_str(state));
    }

    return nullptr;
  }

  Fixnum* System::vm_memory_size(STATE, Object* obj) {
    if(obj->reference_p()) {
      size_t bytes = obj->size_in_bytes(state);
      if(Bignum* b = try_as<Bignum>(obj)) {
        bytes += b->managed_memory_size(state);
      }
      Object* iv = obj->ivars();
      if(LookupTable* lt = try_as<LookupTable>(iv)) {
        bytes += iv->size_in_bytes(state);
        bytes += lt->values()->size_in_bytes(state);
        bytes += (lt->entries()->to_native() * sizeof(LookupTableBucket));
      } else if(iv->reference_p()) {
        bytes += iv->size_in_bytes(state);
      }
      return Fixnum::from(bytes);
    }

    return Fixnum::from(0);
  }

  Object* System::vm_throw(STATE, Object* dest, Object* value) {
    state->unwind_state()->raise_throw(dest, value);
    return nullptr;
  }

  Object* System::vm_catch(STATE, Object* dest, Object* obj) {
    LookupData lookup(obj, obj->lookup_begin(state), G(sym_protected));
    Dispatch dispatch(G(sym_call));
    Arguments args(G(sym_call), 1, &dest);
    args.set_recv(obj);

    OnStack<1> os(state, dest);
    Object* ret = dispatch.send(state, lookup, args);

    if(!ret && state->unwind_state()->raise_reason() == cCatchThrow) {
      if(state->unwind_state()->throw_dest()->equal_p(dest)) {
        Object* val = state->unwind_state()->raise_value();
        state->unwind_state()->clear_return();
        return val;
      }
    }

    return ret;
  }

  Object* System::vm_set_class(STATE, Object* obj, Class* cls) {
    if(!obj->reference_p()) return Primitives::failure();
    if(obj->type_id() != cls->type_info()->type) {
      return Primitives::failure();
    }

    if(kind_of<PackedObject>(obj)) {
      if(obj->klass()->packed_size() != cls->packed_size()) {
        return Primitives::failure();
      }
    }

    obj->klass(state, cls);
    return obj;
  }

  Object* System::vm_method_missing_reason(STATE) {
    switch(state->method_missing_reason()) {
    case ePrivate:
      return G(sym_private);
    case eProtected:
      return G(sym_protected);
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

  Object* System::vm_constant_missing_reason(STATE) {
    switch(state->constant_missing_reason()) {
    case vPrivate:
      return G(sym_private);
    case vNonExistent:
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

  Object* System::vm_check_callable(STATE, Object* obj, Symbol* sym, Object* self) {
    LookupData lookup(self, obj->lookup_begin(state), G(sym_public));
    Dispatch dispatch(sym);

    Object* responds = RBOOL(dispatch.resolve(state, sym, lookup));
    if(!CBOOL(responds)) {
      LookupData lookup(obj, obj->lookup_begin(state), G(sym_private));
      Symbol* name = G(sym_respond_to_missing);
      Dispatch dispatch(name);

      Object* buf[2];
      buf[0] = name;
      buf[1] = G(sym_public);
      Arguments args(name, obj, 2, buf);
      responds = RBOOL(CBOOL(dispatch.send(state, lookup, args)));
    }
    return responds;
  }

  Object* System::vm_check_super_callable(STATE) {
    CallFrame* call_frame = state->call_frame();

    Module* start = call_frame->module()->superclass();
    Symbol* sym = call_frame->original_name();

    LookupData lookup(call_frame->self(), start, G(sym_private));
    Dispatch dispatch(sym);

    return RBOOL(dispatch.resolve(state, sym, lookup));
  }

#define GETPW_R_SIZE 2048

  String* System::vm_get_user_home(STATE, String* name) {
#ifdef RBX_WINDOWS
    // TODO: Windows
    return force_as<String>(Primitives::failure());
#else
    struct passwd pw;
    struct passwd *pwd;

    long len = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(len < 0) len = GETPW_R_SIZE;

retry:
    ByteArray* buf =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), len);

    int err = getpwnam_r(name->c_str_null_safe(state), &pw,
                         reinterpret_cast<char*>(buf->raw_bytes()), len, &pwd);
    if(err) {
      if(errno == ERANGE) {
        len *= 2;
        // Check for overflow
        if(len > 0) goto retry;
        Exception::raise_runtime_error(state, "getpwnam_r(3) buffer exceeds maximum size");
      }
      Exception::raise_errno_error(state, "retrieving user home directory",
          errno, "getpwnam_r(3)");
    }
    if(pwd) {
      return String::create(state, pwd->pw_dir);
    }
    return nil<String>();
#endif
  }

  Object* System::vm_set_finalizer(STATE, Object* obj, Object* fin) {
    if(!obj->reference_p()) return cFalse;
    state->collector()->managed_finalizer(state, obj, fin);
    return cTrue;
  }

  Object* System::vm_object_lock(STATE, Object* obj) {
    if(!obj->reference_p()) return Primitives::failure();

    obj->lock(state);

    return cNil;
  }

  Object* System::vm_object_trylock(STATE, Object* obj) {
    if(!obj->reference_p()) return Primitives::failure();

    return RBOOL(obj->try_lock(state));
  }

  Object* System::vm_object_locked_p(STATE, Object* obj) {
    if(!obj->reference_p()) return cFalse;

    return RBOOL(obj->locked_p(state));
  }

  Object* System::vm_object_lock_owned_p(STATE, Object* obj) {
    if(!obj->reference_p()) return cFalse;

    return RBOOL(obj->lock_owned_p(state));
  }

  Object* System::vm_object_unlock(STATE, Object* obj) {
    if(!obj->reference_p()) return Primitives::failure();

    obj->unlock(state);

    return cNil;
  }

  Object* System::vm_memory_barrier(STATE) {
    std::atomic_thread_fence(std::memory_order_acq_rel);
    return cNil;
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

  static const char sha1_hex[] = {
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
     'a', 'b', 'c', 'd', 'e', 'f'
  };

  String* System::sha1_hash(STATE, String* str) {
    XSHA1_CTX ctx;
    XSHA1_Init(&ctx);
    XSHA1_Update(&ctx, str->byte_address(), str->byte_size());

    uint8_t digest[20];
    XSHA1_Finish(&ctx, digest);

    char buf[40];

    for(int i = 0; i < 20; i++) {
      unsigned char byte = digest[i];
      buf[i + i]     = sha1_hex[byte >> 4];
      buf[i + i + 1] = sha1_hex[byte & 0x0f];
    }

    return String::create(state, buf, 40);
  }

  Tuple* System::vm_thread_state(STATE) {
    UnwindState* ts = state->unwind_state();
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
    case cSystemExit:
      reason = state->symbol("exit");
      break;
    case cCatchThrow:
      reason = state->symbol("catch_throw");
      break;
    case cThreadKill:
      reason = state->symbol("thread_kill");
      break;
    case cFiberCancel:
      reason = state->symbol("fiber_cancel");
      break;
    }

    tuple->put(state, 0, reason);
    tuple->put(state, 1, ts->raise_value());
    tuple->put(state, 2, ts->destination_scope());
    tuple->put(state, 3, ts->current_exception());
    tuple->put(state, 4, ts->throw_dest());

    return tuple;
  }

  Object* System::vm_run_script(STATE, CompiledCode* code) {
    Arguments args(state->symbol("__script__"), G(main), cNil, 0, 0);

    OnStack<1> os(state, code);

    code->internalize(state);

    return code->machine_code()->execute_as_script(state, code);
  }

#define HASH_TRIE_BIT_WIDTH   6
#define HASH_TRIE_BIT_MASK    0x3f

  static inline size_t hash_trie_bit(Fixnum* hash, Fixnum* level) {
    intptr_t h = hash->to_native();
    intptr_t l = level->to_native();

    size_t width = HASH_TRIE_BIT_WIDTH;
    size_t mask = HASH_TRIE_BIT_MASK;
    size_t result = 1;

    return result << ((h >> (l * width)) & mask);
  }

  static inline int hash_trie_index(size_t m) {
    uint64_t sk5 = 0x5555555555555555;
    uint64_t sk3 = 0x3333333333333333;
    uint64_t skf0 = 0x0F0F0F0F0F0F0F0F;

    m -= (m >> 1) & sk5;
    m = (m & sk3) + ((m >> 2) & sk3);
    m = (m & skf0) + ((m >> 4) & skf0);
    m += m >> 8;
    m += m >> 16;
    m = (m + (m >> 32)) & 0xFF;

    return m;
  }

  Fixnum* System::vm_hash_trie_item_index(STATE, Fixnum* hash,
                                           Fixnum* level, Integer* map)
  {
    uint64_t m = map->to_ulong();
    uint64_t b = hash_trie_bit(hash, level);

    if(m & b) {
      return Fixnum::from(hash_trie_index((b - 1) & m));
    } else {
      return nil<Fixnum>();
    }
  }

  Integer* System::vm_hash_trie_set_bitmap(STATE, Fixnum* hash,
                                           Fixnum* level, Integer* map)
  {
    uint64_t m = map->to_ulong();
    uint64_t b = hash_trie_bit(hash, level);

    return Integer::from(state, m | b);
  }

  Integer* System::vm_hash_trie_unset_bitmap(STATE, Fixnum* hash,
                                             Fixnum* level, Integer* map)
  {
    uint64_t m = map->to_ulong();
    uint64_t b = hash_trie_bit(hash, level);

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

  Object* System::vm_dtrace_fire(STATE, String* payload) {
#if HAVE_DTRACE
    if(RUBINIUS_RUBY_PROBE_ENABLED()) {
      char* bytes = reinterpret_cast<char*>(payload->byte_address());
      RUBINIUS_RUBY_PROBE(
          const_cast<RBX_DTRACE_CHAR_P>(bytes),
          payload->byte_size());
      return cTrue;
    }
    return cFalse;
#else
    return cNil;
#endif
  }
}
