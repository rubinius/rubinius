#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
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

  bool Executable::resolve_primitive(STATE) {
    if(!primitive_->nil_p()) {
      if(Symbol* name = try_as<Symbol>(primitive_)) {
        set_executor(Primitives::resolve_primitive(state, name));
        return true;
      }
    }

    return false;
  }


  Object* Executable::default_executor(STATE, CallFrame* call_frame, Message& msg) {
    msg.arguments().unshift2(state, msg.recv, msg.name);
    msg.name = state->symbol("call");
    msg.recv = msg.method;
    msg.flush_lookup();
    return msg.send(state, call_frame);
  }
}
