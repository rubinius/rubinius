#include "arguments.hpp"
#include "builtin/access_variable.hpp"
#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/exception.hpp"
#include "builtin/packed_object.hpp"
#include "builtin/symbol.hpp"
#include "object_utils.hpp"
#include "object_memory.hpp"
#include "ontology.hpp"

namespace rubinius {

  void AccessVariable::init(STATE) {
    // HACK test superclass of AccessVariable
    GO(access_variable).set(ontology::new_class(state,
          "AccessVariable", G(executable), G(rubinius)));
    G(access_variable)->set_object_type(state, AccessVariableType);
  }

  AccessVariable* AccessVariable::allocate(STATE) {
    AccessVariable* av = state->new_object<AccessVariable>(G(access_variable));
    av->inliners_ = 0;
    av->prim_index_ = -1;
    av->custom_call_site_ = false;
    av->set_executor(AccessVariable::access_execute);
    return av;
  }

  Object* AccessVariable::access_read_regular_ivar(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                         Arguments& args) {
    AccessVariable* access = as<AccessVariable>(exec);
    if(unlikely(args.total() != 0)) {
      Exception::argument_error(state, 0, args.total());
      return NULL;
    }

    Object* recv = args.recv();
    return recv->get_ivar(state, access->name());
  }

  Object* AccessVariable::access_write_regular_ivar(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                         Arguments& args) {
    AccessVariable* access = as<AccessVariable>(exec);
    if(unlikely(args.total() != 1)) {
      Exception::argument_error(state, 1, args.total());
      return NULL;
    }

    Object* recv = args.recv();

    if(CBOOL(recv->frozen_p(state))) {
      Exception::frozen_error(state, call_frame, recv);
      return 0;
    }

    return recv->set_ivar(state, access->name(), args.get_argument(0));
  }

  /* Run when an AccessVariable is executed. Uses the details in exec
   * to access instance variables of args.recv() */
  Object* AccessVariable::access_execute(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                         Arguments& args) {
    AccessVariable* access = as<AccessVariable>(exec);
    Object* const self = args.recv();

    /* The writer case. */
    if(access->write()->true_p()) {
      if(CBOOL(self->frozen_p(state))) {
        Exception::frozen_error(state, call_frame, self);
        return 0;
      }

      if(args.total() != 1) {
        Exception::argument_error(state, 1, args.total());
        return NULL;
      }

      if(kind_of<Class>(mod) && self->reference_p()) {
        // Promote this to use a direct accessor
        if(TypeInfo* ti = state->memory()->find_type_info(self)) {
          TypeInfo::Slots::iterator it = ti->slots.find(access->name()->index());
          if(it != ti->slots.end()) {
            // Found one!
            access->set_executor(ti->slot_accessors[it->second]);
            ti->set_field(state, self, it->second, args.get_argument(0));
            return args.get_argument(0);
          }
        }

        /* Fall through, handle it as a normal ivar. */
        access->set_executor(access_write_regular_ivar);
      }

      self->set_ivar(state, access->name(), args.get_argument(0));
      return args.get_argument(0);
    }

    /* The read case. */
    if(args.total() != 0) {
      Exception::argument_error(state, 0, args.total());
      return NULL;
    }

    if(kind_of<Class>(mod) && self->reference_p()) {
      // Promote this to use a direct accessor
      if(TypeInfo* ti = state->memory()->find_type_info(self)) {
        TypeInfo::Slots::iterator it = ti->slots.find(access->name()->index());
        if(it != ti->slots.end()) {
          // Found one!
          access->set_executor(ti->slot_accessors[it->second]);
          return ti->get_field(state, self, it->second);
        }
      }
      // Ok, its a table ivar, setup fast access for next time.
      access->set_executor(access_read_regular_ivar);
    }

    return self->get_ivar(state, access->name());
  }
} // rubinius
