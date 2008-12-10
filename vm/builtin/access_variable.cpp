#include "vm/object_utils.hpp"
#include "prelude.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/class.hpp"
#include "builtin/task.hpp"
#include "builtin/executable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/exception.hpp"

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
    AccessVariable* av = state->new_object<AccessVariable>(G(access_variable));
    av->set_executor(AccessVariable::access_execute);
    return av;
  }

  /* Run when an AccessVariable is executed. Uses the details in msg.method
   * to access instance variables of msg.recv */
  ExecuteStatus AccessVariable::access_execute(STATE, Task* task, Message& msg) {
    AccessVariable* access = as<AccessVariable>(msg.method);

    /* The writer case. */
    if(access->write()->true_p()) {
      if(msg.args() != 1) {
        Exception::argument_error(state, 1, msg.args());
      }

      /* Fall through, handle it as a normal ivar. */
      msg.recv->set_ivar(state, access->name(), msg.get_argument(0));
      task->primitive_return(msg.get_argument(0), msg);
      return cExecuteContinue;
    }

    /* The read case. */
    if(msg.args() != 0) {
      Exception::argument_error(state, 0, msg.args());
    } else {
      task->primitive_return(msg.recv->get_ivar(state, access->name()), msg);
    }

    return cExecuteContinue;
  }
}
