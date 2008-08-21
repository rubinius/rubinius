#include "prelude.hpp"
#include "objectmemory.hpp"
#include "vm/vmaccess_variable.hpp"
#include "builtin/access_variable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"

namespace rubinius {

  /* Create a new VMAccessVariables.
   *
   * NOTE: There is no state in VMAccessVariable, it's just a place
   * to hang the executor off, so we just reuse the same
   * object for all usages */
  VMAccessVariable* VMAccessVariable::create(STATE) {
    static VMAccessVariable* object = NULL;
    if(!object) {
      object = new VMAccessVariable();
    }

    return object;
  }

  VMAccessVariable::VMAccessVariable() {
    execute = VMAccessVariable::executor;
  }

  /* Run when an AccessVariable is executed. Uses the details in msg.method
   * to access instance variables of msg.recv */
  bool VMAccessVariable::executor(STATE, VMExecutable* meth, Task* task, Message& msg) {
    AccessVariable* access = as<AccessVariable>(msg.method);
    native_int idx = access->name->index();

    /* The writer case. */
    if(access->write->true_p()) {
      if(msg.args != 1) {
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
      msg.recv->set_ivar(state, access->name, msg.get_argument(0));
      task->primitive_return(msg.get_argument(0), msg);
      return false;
    }

    /* The read case. */
    if(msg.args != 0) {
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
      task->primitive_return(msg.recv->get_ivar(state, access->name), msg);
    }

    return false;
  }
}
