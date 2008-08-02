#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "message.hpp"

#include "builtin/array.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"

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
      return task->stack->at(task->sp - (args - index - 1));
    } else {
      throw new Assertion("message not setup properly");
    }
  }

  void Message::combine_with_splat(STATE, Task* task, Array* splat) {
    arguments = Array::create(state, splat->size() + args);

    import_arguments(state, task, args);
    for(size_t i = 0, n = args; i < args; i++, n++) {
      arguments->set(state, n, splat->get(state, i));
    }

    args += splat->size();
  }

  void Message::import_arguments(STATE, Task* task, size_t args) {
    if(!arguments) {
      arguments = Array::create(state, args);
    }

    size_t stack_pos = args - 1;

    for(size_t i = 0; i < args; i++, stack_pos--) {
      arguments->set(state, i, task->stack->at(task->sp - stack_pos));
    }

    this->args = args;
  }

}
