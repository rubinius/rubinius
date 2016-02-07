#include "arguments.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/exception.hpp"
#include "builtin/packed_object.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  void AccessVariable::bootstrap(STATE) {
    GO(access_variable).set(state->memory()->new_class<Class, AccessVariable>(
        state, G(executable), G(rubinius), "AccessVariable"));
  }

  AccessVariable* AccessVariable::allocate(STATE) {
    return state->memory()->new_object<AccessVariable>(state, G(access_variable));
  }

  Object* AccessVariable::access_read_regular_ivar(STATE, CallFrame* call_frame,
      Executable* exec, Module* mod, Arguments& args)
  {
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

    if(CBOOL(recv->frozen_p(state)) && CBOOL(recv->frozen_mod_disallowed(state))) {
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
      if(CBOOL(self->frozen_p(state)) && CBOOL(self->frozen_mod_disallowed(state))) {
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
