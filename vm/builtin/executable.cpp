#include "prelude.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/task.hpp"
#include "builtin/symbol.hpp"

#include "message.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  template <>
    bool kind_of<Executable>(OBJECT obj) {
      if(obj->obj_type == Executable::type ||
         obj->obj_type == AccessVariableType ||
         obj->obj_type == CMethodType ||
         obj->obj_type == NativeFuncType ||
         obj->obj_type == NMethodType) {
        return true;
      }

      return false;
    }

  void Executable::init(STATE) {
    GO(executable).set(state->new_class("Executable", G(object), Executable::fields));
    G(executable)->set_object_type(state, ExecutableType);
  }

  Executable* Executable::allocate(STATE, OBJECT self) {
    Executable* executable = (Executable*)state->new_object(G(executable));
    executable->primitive(state, (SYMBOL)Qnil);
    executable->serial(state, Fixnum::from(0));

    executable->set_executor(Executable::default_executor);

    if(kind_of<Class>(self)) {
      state->om->set_class(executable, self);
    }

    return executable;
  }

  bool Executable::default_executor(STATE, Executable* exc, Task* task, Message& msg) {
    msg.unshift_argument2(state, msg.recv, msg.name);
    msg.name = state->symbol("call");
    msg.recv = exc;
    msg.lookup_from = msg.recv->lookup_begin(state);
    return task->send_message_slowly(msg);
  }
}
