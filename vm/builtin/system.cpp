#include <vector>
#include <cerrno>

#include <unistd.h>

#include "vm/object.hpp"
#include "vm/vm.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

#include "builtin/system.hpp"


namespace rubinius {


/* Primitives */

  Object* Object::yield_gdb(STATE, Object* obj) {
    obj->show(state);
    Exception::assertion_error(state, "yield_gdb called and not caught");
    return obj;
  }

  /* TODO: Improve error messages */
  Object* System::vm_exec(VM* state, String* path, Array* args)
  {
    std::size_t argc = args->size();

    /* execvp() requires a NULL as last element */
    std::vector<char*> argv((argc + 1), NULL);

    for (std::size_t i = 0; i < argc; ++i) {
      argv[i] = as<String>(args->get(state, i))->c_str();
    }

    (void) ::execvp(path->c_str(), &argv[0]);

    /* execvp() returning means it failed. */
    Exception::errno_error(state, "execvp() failed!");

    return Qnil;
  }

  OBJECT Object::vm_exit(STATE, FIXNUM code) {
    ::exit(code->to_native());
  }

  Fixnum* System::vm_fork(VM* state)
  {
    int result = ::fork();

    if ( -1 == result ) {
      Exception::errno_error(state, "fork() failed!");
    }

    /*  TODO: EVFLAG_FORKCHECK should remove need for any maintenance
     *        here, but is there any other re-initialisation needed?
     */

    return Fixnum::from(result);
  }

  OBJECT Object::vm_gc_start(STATE, OBJECT tenure) {
    // Ignore tenure for now
    state->om->collect_young_now = true;
    state->om->collect_mature_now = true;
    return Qnil;
  }

  OBJECT Object::vm_get_config_item(STATE, String* var) {
    ConfigParser::Entry* ent = state->user_config->find(var->c_str());
    if(!ent) return Qnil;

    if(ent->is_number()) {
      return Fixnum::from(atoi(ent->value.c_str()));
    }
    return String::create(state, ent->value.c_str());
  }

  OBJECT Object::vm_get_config_section(STATE, String* section) {
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

  OBJECT Object::vm_reset_method_cache(STATE, SYMBOL name) {
    // 1. clear the global cache
    state->global_cache->clear(name);
    // 2. clear the send site caches
    Selector::clear_by_name(state, name);
    return name;
  }

  OBJECT Object::vm_show_backtrace(STATE, MethodContext* ctx) {
    G(current_task)->print_backtrace(ctx);
    return Qnil;
  }

  OBJECT Object::vm_start_profiler(STATE) {
    G(current_task)->enable_profiler();
    return Qtrue;
  }

  OBJECT Object::vm_stop_profiler(STATE, String* path) {
    G(current_task)->disable_profiler(path->c_str());
    return path;
  }

  OBJECT Object::vm_write_error(STATE, String* str) {
    std::cerr << str->byte_address() << std::endl;
    return Qnil;
  }

}
