#include "prelude.hpp"
#include "object.hpp"
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
    arguments(ary),
    total_args(0),
    start_(0),
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
    caller(NULL) {
      this->state = state;
      arguments = ary;
      total_args = arguments->size();
    }

  Message::Message(STATE):
    state(state),
    arguments(NULL),
    total_args(0),
    start_(0),
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
    caller(NULL) { }

  void Message::use_from_task(Task* task, size_t args) {
    this->caller = task->active();
    this->total_args = args;
  }

  OBJECT Message::get_argument(size_t index) {
    if(arguments) {
      return arguments->get(state, start_ + index);
    } else if(caller) {
      /* recv
       * arg0
       * arg1
       * arg2
       *
       * total_args = 3, index = which you want
       *
       * back 3 is recv, - 1 to get to the first arg
       * - index to get to which argument you want
       */
      return caller->stack_back(total_args - start_ - index - 1);
    } else {
      Exception::assertion_error(state,
          "message arguments are not set up properly");
      return Qnil;  // unreachable, but makes compiler happy
    }
  }

  void Message::set_arguments(STATE, Array* args) {
    arguments = args;
    total_args = args->size();
    start_ = 0;
  }

  void Message::append_arguments(STATE, Task* task, Array* splat) {
    arguments = Array::create(state, splat->size() + total_args);

    for(size_t i = 0; i < splat->size(); i++) {
      arguments->set(state, i, splat->get(state, i));
    }

    import_arguments(state, task, total_args, splat->size());
    total_args += splat->size();
  }

  void Message::append_splat(STATE, Task* task, Array* splat) {
    arguments = Array::create(state, splat->size() + total_args);

    import_arguments(state, task, total_args);
    size_t stack_args = total_args;
    total_args += splat->size();

    for(size_t i = 0, n = stack_args; i < total_args; i++, n++) {
      arguments->set(state, n, splat->get(state, i));
    }
  }

  void Message::import_arguments(STATE, Task* task, size_t args, size_t offset) {
    this->caller = task->active();

    if(!arguments) {
      arguments = Array::create(state, args);
    }

    size_t stack_pos = start_ + args - 1;
    size_t end = offset + args;

    for(size_t i = offset; i < end; i++, stack_pos--) {
      OBJECT arg = caller->stack_back(stack_pos);
      arguments->set(state, i, arg);
    }

    total_args = args;
  }

  Array* Message::as_array(STATE) {
    if(arguments) {
      if(start_ == 0) return arguments;
      Array* ary = Array::create(state, args());
      for(size_t i = 0; i < args(); i++) {
        ary->set(state, i, get_argument(i));
      }

      return ary;
    }

    Array* ary = Array::create(state, args());

    for(size_t i = 0; i < args(); i++) {
      ary->set(state, i, get_argument(i));
    }

    return ary;
  }

  void Message::unshift_argument(STATE, OBJECT val) {
    if(arguments) {
      total_args++;
      if(start_ > 0) {
        arguments->set(state, --start_, val);
      } else {
        arguments->unshift(state, val);
      }
      return;
    }

    Array* ary = Array::create(state, args() + 1);

    ary->set(state, 0, val);

    for(size_t i = 0; i < args(); i++) {
      ary->set(state, i + 1, get_argument(i));
    }

    total_args = args() + 1;
    start_ = 0;
    arguments = ary;
  }

  void Message::unshift_argument2(STATE, OBJECT one, OBJECT two) {
    if(arguments) {
      total_args += 2;
      if(start_ > 1) {
        arguments->set(state, --start_, two);
        arguments->set(state, --start_, one);
       } else if(start_ > 0) {
        arguments->set(state, --start_, two);
        arguments->unshift(state, one);
      } else {
        arguments->unshift(state, two);
        arguments->unshift(state, one);
      }
      return;
    }

    Array* ary = Array::create(state, args() + 2);

    ary->set(state, 0, one);
    ary->set(state, 1, two);

    for(size_t i = 0; i < args(); i++) {
      ary->set(state, i + 2, get_argument(i));
    }

    total_args = args() + 2;
    start_ = 0;
    arguments = ary;
  }

  OBJECT Message::shift_argument(STATE) {
    OBJECT first = get_argument(0);
    start_++;
    return first;
  }

  OBJECT Message::current_self() {
    return caller->self();
  }

}
