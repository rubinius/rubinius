#include "prelude.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/task.hpp"
#include "builtin/symbol.hpp"

#include "message.hpp"
#include "objectmemory.hpp"

namespace rubinius {

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

  ExecuteStatus Executable::default_executor(STATE, Task* task, Message& msg) {
    msg.unshift_argument2(state, msg.recv, msg.name);
    msg.name = state->symbol("call");
    msg.recv = msg.method;
    msg.lookup_from = msg.recv->lookup_begin(state);
    return task->send_message_slowly(msg);
  }
}
