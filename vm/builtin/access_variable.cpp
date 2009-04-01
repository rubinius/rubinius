#include "vm/object_utils.hpp"
#include "prelude.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/exception.hpp"

#include "objectmemory.hpp"
#include "dispatch.hpp"
#include "arguments.hpp"

namespace rubinius {

  void AccessVariable::init(STATE) {
    // HACK test superclass of AccessVariable
    GO(access_variable).set(state->new_class("AccessVariable", G(executable)));
    G(access_variable)->set_object_type(state, AccessVariableType);
  }

  AccessVariable* AccessVariable::allocate(STATE) {
    AccessVariable* av = state->new_object<AccessVariable>(G(access_variable));
    av->set_executor(AccessVariable::access_execute);
    return av;
  }

  /* Run when an AccessVariable is executed. Uses the details in msg.method
   * to access instance variables of args.recv() */
  Object* AccessVariable::access_execute(STATE, CallFrame* call_frame, Dispatch& msg,
                                         Arguments& args) {
    AccessVariable* access = as<AccessVariable>(msg.method);

    /* The writer case. */
    if(access->write()->true_p()) {
      if(args.total() != 1) {
        Exception::argument_error(state, 1, args.total());
      }

      /* Fall through, handle it as a normal ivar. */
      args.recv()->set_ivar(state, access->name(), args.get_argument(0));
      return args.get_argument(0);
    }

    /* The read case. */
    if(args.total() != 0) {
      Exception::argument_error(state, 0, args.total());
      return NULL;
    } else {
      return args.recv()->get_ivar(state, access->name());
    }
  }
}
