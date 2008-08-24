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

  Message::Message(STATE, Array* ary) :
      task(NULL), argument_start(0), send_site(NULL), name(NULL),
      recv(Qnil), block(Qnil), splat(Qnil), current_self(Qnil),
      args(0), stack(0), priv(false), lookup_from(NULL), 
      method(NULL), module(NULL) {
    this->state = state;
    arguments = ary;
    args = arguments->size();
  }

  Message::Message(STATE) :
      state(state), arguments(NULL), task(NULL),
      argument_start(0), send_site(NULL), name(NULL),
      recv(Qnil), block(Qnil), splat(Qnil), current_self(Qnil),
      args(0), stack(0), priv(false), lookup_from(NULL), 
      method(NULL), module(NULL) { }

  OBJECT Message::get_argument(size_t index) {
    if(arguments) {
      return arguments->get(state, index);
    } else if(task){
      return task->active->stack_back(args - index - 1);
    } else {
      throw Assertion("message not setup properly");
    }
  }

  void Message::combine_with_splat(STATE, Task* task, Array* splat) {
    arguments = Array::create(state, splat->size() + args);

    import_arguments(state, task, args);
    size_t stack_args = args;
    args += splat->size();

    for(size_t i = 0, n = stack_args; i < args; i++, n++) {
      arguments->set(state, n, splat->get(state, i));
    }
  }

  void Message::import_arguments(STATE, Task* task, size_t args) {
    this->task = task;
    if(!arguments) {
      arguments = Array::create(state, args);
    }

    size_t stack_pos = args - 1;

    for(size_t i = 0; i < args; i++, stack_pos--) {
      OBJECT arg = task->active->stack_back(stack_pos);
      arguments->set(state, i, arg);
    }

    this->args = args;
  }

  void Message::unshift_argument(STATE, OBJECT val) {
    if(arguments) {
      args++;
      arguments->unshift(state, val);
      return;
    }

    arguments = Array::create(state, args);

    size_t stack_pos = args - 1;

    arguments->set(state, 0, val);

    for(size_t i = 1; i <= args; i++, stack_pos--) {
      OBJECT arg = task->active->stack_back(stack_pos);
      arguments->set(state, i, arg);
    }

    args++;
  }

}
