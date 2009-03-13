#include "vm.hpp"
#include "objectmemory.hpp"
#include "message.hpp"

#include "builtin/array.hpp"
#include "builtin/executable.hpp"
#include "builtin/tuple.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"

#include <sstream>

namespace rubinius {

  Message::Message(STATE, SendSite* ss, Symbol* name, Object* recv, CallFrame* call_frame,
                   size_t arg_count, Object* block, bool priv, Module* lookup_from) :
    state(state),
    arguments_array(NULL),
    total_args(arg_count),
    send_site(ss),
    name(name),
    recv(recv),
    block(block),
    priv(priv),
    lookup_from(lookup_from),
    method(static_cast<Executable*>(Qnil)),
    module(static_cast<Module*>(Qnil)),
    method_missing(false),
    caller_(call_frame)
  {
    if(total_args > 0) {
      stack_args_ = call_frame->stack_back_position(total_args - 1);
      arguments_ = stack_args_;
    } else {
      arguments_ = stack_args_ = NULL;
    }
  }

  Message::Message(STATE, Symbol* name, Object* recv, size_t arg_count,
                   Object* block, Module* lookup_from) :
    state(state),
    arguments_array(NULL),
    total_args(arg_count),
    stack_args_(NULL),
    arguments_(NULL),
    send_site(NULL),
    name(name),
    recv(recv),
    block(block),
    priv(false),
    lookup_from(lookup_from),
    method(static_cast<Executable*>(Qnil)),
    module(static_cast<Module*>(Qnil)),
    method_missing(false),
    caller_(NULL) { }

  /* @todo: is this used anywhere but test_nativefunction? */
  Message::Message(STATE, Array* ary):
    state(state),
    total_args(0),
    send_site(NULL),
    name(NULL),
    recv(Qnil),
    block(Qnil),
    priv(false),
    lookup_from(NULL),
    method(NULL),
    module(NULL),
    method_missing(false),
    caller_(NULL) {
      use_array(ary);
    }

  Message::Message(STATE):
    state(state),
    arguments_array(NULL),
    total_args(0),
    send_site(NULL),
    name(NULL),
    recv(Qnil),
    block(Qnil),
    priv(false),
    lookup_from(NULL),
    method(NULL),
    module(NULL),
    method_missing(false),
    caller_(NULL) { }

  void Message::set_arguments(STATE, Array* args) {
    use_array(args);
  }

  void Message::set_stack_args(int count, Object** args) {
    stack_args_ = arguments_ = args;
    total_args = count;
  }

  void Message::append_arguments(STATE, Array* splat) {
    Array* args = Array::create(state, splat->size() + total_args);

    for(size_t i = 0; i < splat->size(); i++) {
      args->set(state, i, splat->get(state, i));
    }

    for(size_t i = 0, n = splat->size(); i < total_args; i++, n++) {
      args->set(state, n, get_argument(i));
    }

    use_array(args);
  }

  void Message::append_splat(STATE, Array* splat) {
    Array* args = Array::create(state, splat->size() + total_args);

    for(size_t i = 0; i < total_args; i++) {
      args->set(state, i, get_argument(i));
    }

    for(size_t i = 0, n = total_args; i < splat->size(); i++, n++) {
      args->set(state, n, splat->get(state, i));
    }

    use_array(args);
  }

  Array* Message::as_array(STATE) {
    if(arguments_array) {
      return arguments_array;
    }

    Array* ary = Array::create(state, args());

    for(size_t i = 0; i < args(); i++) {
      ary->set(state, i, get_argument(i));
    }

    return ary;
  }

  void Message::unshift_argument(STATE, Object* val) {
    if(arguments_array) {
      arguments_array->unshift(state, val);

      // Repoint internal things since we manipulated the array
      use_array(arguments_array);
      return;
    }

    Array* ary = Array::create(state, args() + 1);

    ary->set(state, 0, val);

    for(size_t i = 0; i < args(); i++) {
      ary->set(state, i + 1, get_argument(i));
    }

    use_array(ary);
  }

  void Message::unshift_argument2(STATE, Object* one, Object* two) {
    if(arguments_array) {
      arguments_array->unshift(state, two);
      arguments_array->unshift(state, one);

      use_array(arguments_array);
      return;
    }

    Array* ary = Array::create(state, args() + 2);

    ary->set(state, 0, one);
    ary->set(state, 1, two);

    for(size_t i = 0; i < args(); i++) {
      ary->set(state, i + 2, get_argument(i));
    }

    use_array(ary);
  }

  Object* Message::shift_argument(STATE) {
    total_args--;
    if(arguments_array) {
      Object* first = arguments_array->shift(state);

      use_array(arguments_array);

      return first;
    } else {
      return shift_stack_args();
    }
  }

  Object* Message::current_self() {
    return caller_->self();
  }

  /* Only called if send_message can't locate anything to run, which pretty
   * much never happens, since it means even method_missing wasn't available. */
  static void tragic_failure(STATE, Message& msg) {
    std::stringstream ss;
    ss << "unable to locate any method '" << msg.send_site->name()->c_str(state) <<
      "' from '" << msg.lookup_from->name()->c_str(state) << "'";

    Exception::assertion_error(state, ss.str().c_str());
  }

  Object* Message::send(STATE, CallFrame* call_frame) {
    Symbol* original_name = name;
    if(!GlobalCacheResolver::resolve(state, *this)) {
      method_missing = true;
      name = G(sym_method_missing);
      priv = true; // lets us look for method_missing anywhere
      if(!GlobalCacheResolver::resolve(state, *this)) {
        tragic_failure(state, *this);
        return NULL;
      }
    }

    if(method_missing) {
      unshift_argument(state, original_name);
    }

    return method->execute(state, call_frame, *this);
  }
}
