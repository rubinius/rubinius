#include "vm.hpp"
#include "objectmemory.hpp"
#include "message.hpp"

#include "builtin/array.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"
#include "builtin/contexts.hpp"

namespace rubinius {

  Message::Message(STATE, Array* ary):
    state(state),
    total_args(0),
    send_site(NULL),
    name(NULL),
    recv(Qnil),
    block(Qnil),
    stack(0),
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
    stack(0),
    priv(false),
    lookup_from(NULL),
    method(NULL),
    module(NULL),
    method_missing(false),
    caller_(NULL) { }

  void Message::set_arguments(STATE, Array* args) {
    use_array(args);
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

  void Message::use_from_task(Task* task, size_t args) {
    set_args(args);
    stack_args_ = caller_->stack_back_position(args - 1);
    arguments_ = stack_args_;
  }

}
