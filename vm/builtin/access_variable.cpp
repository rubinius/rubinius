#include "prelude.hpp"
#include "builtin/access_variable.hpp"
#include "builtin/class.hpp"
#include "builtin/task.hpp"
#include "builtin/executable.hpp"
#include "builtin/symbol.hpp"

#include "objectmemory.hpp"
#include "message.hpp"

namespace rubinius {

  void AccessVariable::init(STATE) {
    // HACK test superclass of AccessVariable
    GO(access_variable).set(state->new_class("AccessVariable", G(executable),
          AccessVariable::fields));
    G(access_variable)->set_object_type(state, AccessVariableType);
  }

  AccessVariable* AccessVariable::allocate(STATE) {
    AccessVariable* av = (AccessVariable*)state->new_object(G(access_variable));
    av->set_executor(AccessVariable::access_execute);
    return av;
  }

  /* Run when an AccessVariable is executed. Uses the details in msg.method
   * to access instance variables of msg.recv */
  bool AccessVariable::access_execute(STATE, Executable* meth, Task* task, Message& msg) {
    AccessVariable* access = as<AccessVariable>(meth);
    native_int idx = access->name()->index();

    /* The writer case. */
    if(access->write()->true_p()) {
      if(msg.args() != 1) {
        assert(0 && "implement raise exception");
      }
      /* We might be trying to access a field, so check there first. */
      TypeInfo* ti = state->om->find_type_info(msg.recv);
      if(ti) {
        TypeInfo::Slots::iterator it = ti->slots.find(idx);
        if(it != ti->slots.end()) {
          ti->set_field(state, msg.recv, it->second, msg.get_argument(0));
          task->primitive_return(msg.get_argument(0), msg);
          return false;
        }
      }

      /* Fall through, handle it as a normal ivar. */
      msg.recv->set_ivar(state, access->name(), msg.get_argument(0));
      task->primitive_return(msg.get_argument(0), msg);
      return false;
    }

    /* The read case. */
    if(msg.args() != 0) {
      assert(0 && "implement raise exception");
    } else {
      /* We might be trying to access a field, so check there first. */
      TypeInfo* ti = state->om->find_type_info(msg.recv);
      if(ti) {
        TypeInfo::Slots::iterator it = ti->slots.find(idx);
        if(it != ti->slots.end()) {
          task->primitive_return(ti->get_field(state, msg.recv, it->second), msg);
          return false;
        }
      }

      /* Get a normal ivar. */
      task->primitive_return(msg.recv->get_ivar(state, access->name()), msg);
    }

    return false;
  }
}
