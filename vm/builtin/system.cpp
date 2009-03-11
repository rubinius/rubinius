#include <vector>
#include <cerrno>

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cstring>
#include <sstream>

#include <sys/wait.h>
#include <unistd.h>

#include "vm/call_frame.hpp"

#include "vm/object_utils.hpp"
#include "vm/vm.hpp"

#include "compiled_file.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"
#include "config_parser.hpp"

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
#include "builtin/selector.hpp"
#include "builtin/taskprobe.hpp"
#include "builtin/float.hpp"

#include "builtin/system.hpp"
#include "signal.hpp"

#include "instruments/stats.hpp"

namespace rubinius {


/* Primitives */
  //
  // HACK: remove this when performance is better and compiled_file.rb
  // unmarshal_data method works.
  Object* System::compiledfile_load(STATE, String* path, Object* version) {
    if(!state->probe->nil_p()) {
      state->probe->load_runtime(state, std::string(path->c_str()));
    }

    std::ifstream stream(path->c_str());
    if(!stream) {
      std::ostringstream msg;
      msg << "unable to open file to run: " << path->c_str();
      Exception::io_error(state, msg.str().c_str());
    }

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") {
      std::ostringstream msg;
      msg << "Invalid file: " << path->c_str();
      Exception::io_error(state, msg.str().c_str());
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

  /* @todo Improve error messages */
  Object* System::vm_exec(VM* state, String* path, Array* args)
  {
    std::size_t argc = args->size();

    /* execvp() requires a NULL as last element */
    std::vector<char*> argv((argc + 1), NULL);

    for (std::size_t i = 0; i < argc; ++i) {
      /* strdup should be OK. Trying to exec with strings containing NUL == bad. --rue */
      argv[i] = ::strdup(as<String>(args->get(state, i))->c_str());
    }

    (void) ::execvp(path->c_str(), &argv[0]); /* std::vector is contiguous. --rue */

    /* execvp() returning means it failed. */
    Exception::errno_error(state, "execvp() failed!");

    return Qnil;
  }

  Object* System::vm_wait_pid(STATE, Fixnum* pid_obj, Object* no_hang) {
    pid_t input_pid = pid_obj->to_native();
    int options = 0;
    int status;
    pid_t pid;

    if(no_hang == Qtrue) {
      options |= WNOHANG;
    }

  retry:

    {
      GlobalLock::UnlockGuard lock(state->global_lock());
      pid = waitpid(input_pid, &status, options);
    }

    if(pid == -1) {
      if(errno == ECHILD) return Qfalse;
      if(errno == EINTR)  goto retry;

      // TODO handle other errnos?
      return Qfalse;
    }

    if(no_hang == Qtrue && pid == 0) {
      return Qnil;
    }

    Object* output;
    if(WIFEXITED(status)) {
      output = Fixnum::from(WEXITSTATUS(status));
    } else {
      output = Qnil;
    }

    if(input_pid > 0) {
      return output;
    }

    return Tuple::from(state, 2, output, Fixnum::from(pid));
  }

  Object* System::vm_exit(STATE, Fixnum* code) {
    state->thread_state()->raise_exit(code);
    return NULL;
  }

  Fixnum* System::vm_fork(VM* state)
  {
    int result = ::fork();

    if ( -1 == result ) {
      Exception::errno_error(state, "fork() failed!");
    }

    /*  @todo EVFLAG_FORKCHECK should remove need for any maintenance
     *        here, but is there any other re-initialisation needed?
     */

    return Fixnum::from(result);
  }

  Object* System::vm_gc_start(STATE, Object* tenure) {
    // Ignore tenure for now
    state->om->collect_young_now = true;
    state->om->collect_mature_now = true;
    return Qnil;
  }

  Object* System::vm_get_config_item(STATE, String* var) {
    ConfigParser::Entry* ent = state->user_config->find(var->c_str());
    if(!ent) return Qnil;

    if(ent->is_number()) {
      return Fixnum::from(atoi(ent->value.c_str()));
    } else if(ent->is_true()) {
      return Qtrue;
    }

    return String::create(state, ent->value.c_str());
  }

  Object* System::vm_get_config_section(STATE, String* section) {
    ConfigParser::EntryList* list;

    list = state->user_config->get_section(section->byte_address());

    Array* ary = Array::create(state, list->size());
    for(size_t i = 0; i < list->size(); i++) {
      String* var = String::create(state, list->at(i)->variable.c_str());
      String* val = String::create(state, list->at(i)->value.c_str());

      ary->set(state, i, Tuple::from(state, 2, var, val));
    }

    return ary;
  }

  Object* System::vm_reset_method_cache(STATE, Symbol* name) {
    // 1. clear the global cache
    state->global_cache->clear(name);
    // 2. clear the send site caches
    Selector::clear_by_name(state, name);
    return name;
  }

  /** @todo Double-check it is OK to drop the first frame, which
   *        /should/ be the #raise. --rue
   *
   *  @todo Could possibly capture the system backtrace at this
   *        point. --rue
   */
  Array* System::vm_backtrace(VM* state, Fixnum* skip, CallFrame* calling_environment) {
    CallFrame* call_frame = calling_environment;

    for(native_int i = skip->to_native(); call_frame && i > 0; --i) {
      call_frame = call_frame->previous;
    }

    Array* bt = Array::create(state, 5);

    while(call_frame) {
      // Ignore synthetic frames
      if(call_frame->cm) {
        bt->append(state, Location::create(state, call_frame));
      }

      call_frame = call_frame->previous;
    }

    return bt;
  }

/** @todo Fix, Task is gone. --rue */
//  Object* System::vm_profiler_instrumenter_start(STATE) {
//    G(current_task)->enable_profiler();
//    return Qtrue;
//  }

/** @todo Fix, Task is gone. --rue */
//  LookupTable* System::vm_profiler_instrumenter_stop(STATE) {
//    return G(current_task)->disable_profiler();
//  }

  Object* System::vm_write_error(STATE, String* str) {
    std::cerr << str->c_str() << std::endl;
    return Qnil;
  }

  Object* System::vm_jit_info(STATE) {
    if(!state->config.jit_enabled) {
      return Qnil;
    }

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Integer::from(state, state->stats.jit_timing));
    ary->set(state, 1, Integer::from(state, state->stats.jitted_methods));

    return ary;
  }

  Object* System::vm_stats_gc_clear(STATE) {
#ifdef RBX_GC_STATS
    stats::GCStats::clear();
#endif
    return Qnil;
  }

  Object* System::vm_stats_gc_info(STATE) {
#ifdef RBX_GC_STATS
    return stats::GCStats::get()->to_ruby(state);
#else
    return Qnil;
#endif
  }

  Object* System::vm_watch_signal(STATE, Fixnum* sig) {
    SignalThread* thr = state->shared.signal_thread();
    if(thr) {
      thr->add_signal(sig->to_native());
      return Qtrue;
    } else {
      return Qfalse;
    }
  }

  Object* System::vm_time(STATE) {
    return Integer::from(state, time(0));
  }

}
