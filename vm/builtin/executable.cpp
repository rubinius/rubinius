#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/task.hpp"
#include "builtin/symbol.hpp"

#include "message.hpp"
#include "objectmemory.hpp"

namespace rubinius {

  void Executable::init(STATE) {
    GO(executable).set(state->new_class("Executable", G(object)));
    G(executable)->set_object_type(state, ExecutableType);
  }

  Executable* Executable::allocate(STATE, Object* self) {
    Executable* executable = state->new_object<Executable>(G(executable));
    executable->primitive(state, (Symbol*)Qnil);
    executable->serial(state, Fixnum::from(0));

    executable->set_executor(Executable::default_executor);

    if(kind_of<Class>(self)) {
      state->om->set_class(executable, self);
    }

    return executable;
  }

  Object* Executable::default_executor(STATE, CallFrame* call_frame, Message& msg) {
    msg.unshift_argument2(state, msg.recv, msg.name);
    msg.name = state->symbol("call");
    msg.recv = msg.method;
    msg.lookup_from = msg.recv->lookup_begin(state);
    return msg.send(state, call_frame);
  }
}
