
#include "builtin/object.hpp"
#include "call_frame.hpp"
#include "builtin/autoload.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/class.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/lookuptable.hpp"
#include "global_cache.hpp"
#include "objectmemory.hpp"
#include "builtin/tuple.hpp"
#include "builtin/system.hpp"
#include "builtin/thread.hpp"
#include "builtin/channel.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/location.hpp"
#include "builtin/exception.hpp"

#include "vm.hpp"
#include "object_utils.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "lookup_data.hpp"

#include "on_stack.hpp"

#include "helpers.hpp"

namespace rubinius {
  namespace Helpers {
    Object* const_get_under(STATE, Module* mod, Symbol* name, ConstantMissingReason* reason, Object* filter, bool replace_autoload) {
      *reason = vNonExistent;

      while(!mod->nil_p()) {
        Object* result = mod->get_const(state, name, G(sym_public), reason, false, replace_autoload);
        if(*reason == vFound) {
          if(result != filter) return result;
          *reason = vNonExistent;
        }
        if(*reason == vPrivate) break;

        // Don't stop when you see Object, because we need to check any
        // includes into Object as well, and they're found via superclass
        mod = mod->superclass();
      }

      state->vm()->set_constant_missing_reason(*reason);
      return cNil;
    }

    Object* const_get(STATE, CallFrame* call_frame, Symbol* name, ConstantMissingReason* reason, Object* filter, bool replace_autoload) {
      ConstantScope *cur;
      Object* result;

      *reason = vNonExistent;

      call_frame = call_frame->top_ruby_frame();

      // Ok, this has to be explained or it will be considered black magic.
      // The scope chain always ends with an entry at the top that contains
      // a parent of nil, and a module of Object. This entry is put in
      // regardless of lexical scoping, it's the fallback scope (the default
      // scope). This is not case when deriving from BasicObject, which is
      // explained later.
      //
      // When looking up a constant, we don't want to consider the fallback
      // scope (ie, Object) initially because we need to lookup up
      // the superclass chain first, because falling back on the default.
      //
      // The rub comes from the fact that if a user explicitly opens up
      // Object in their code, we DO consider it. Like:
      //
      // class Idiot
      //   A = 2
      // end
      //
      // class ::Object
      //   A = 1
      //   class Stupid < Idiot
      //     def foo
      //       p A
      //     end
      //   end
      // end
      //
      // In this code, when A is looked up, Object must be considering during
      // the scope walk, NOT during the superclass walk.
      //
      // So, in this case, foo would print "1", not "2".
      //
      // As indicated above, the fallback scope isn't used when the superclass
      // chain directly rooted from BasicObject. To determine this is the
      // case, we record whether Object is seen when looking up the superclass
      // chain. If Object isn't seen, this means we are directly deriving from
      // BasicObject.

      cur = call_frame->constant_scope();
      while(!cur->nil_p()) {
        // Detect the toplevel scope (the default) and get outta dodge.
        if(cur->top_level_p(state)) break;

        result = cur->module()->get_const(state, name, G(sym_private), reason, false, replace_autoload);
        if(*reason == vFound) {
          if(result != filter) return result;
          *reason = vNonExistent;
        }

        cur = cur->parent();
      }

      // Now look up the superclass chain.
      Module *fallback = G(object);

      cur = call_frame->constant_scope();
      if(!cur->nil_p()) {
        bool object_seen = false;
        Module* mod = cur->module();
        while(!mod->nil_p()) {
          if(mod == G(object)) {
            object_seen = true;
          }
          if(!object_seen && mod == G(basicobject)) {
            fallback = NULL;
          }

          result = mod->get_const(state, name, G(sym_private), reason, false, replace_autoload);
          if(*reason == vFound) {
            if(result != filter) return result;
            *reason = vNonExistent;
          }

          mod = mod->superclass();
        }
      }

      // Lastly, check the fallback scope (=Object) specifically if needed
      if(fallback) {
        result = fallback->get_const(state, name, G(sym_private), reason, true, replace_autoload);
        if(*reason == vFound) {
          if(result != filter) return result;
          *reason = vNonExistent;
        }
      }

      return cNil;
    }

    Object* const_missing_under(STATE, Module* under, Symbol* sym, CallFrame* call_frame) {
      Array* args = Array::create(state, 1);
      args->set(state, 0, sym);
      return under->send(state, call_frame, G(sym_const_missing), args);
    }

    Object* const_missing(STATE, Symbol* sym, CallFrame* call_frame) {
      Module* under;

      call_frame = call_frame->top_ruby_frame();

      ConstantScope* scope = call_frame->constant_scope();
      if(scope->nil_p()) {
        under = G(object);
      } else {
        under = scope->module();
      }

      Array* args = Array::create(state, 1);
      args->set(state, 0, sym);
      return under->send(state, call_frame, G(sym_const_missing), args);
    }

    Class* open_class(STATE, GCToken gct, CallFrame* call_frame, Object* super, Symbol* name, bool* created) {
      Module* under;

      call_frame = call_frame->top_ruby_frame();

      if(call_frame->constant_scope()->nil_p()) {
        under = G(object);
      } else {
        under = call_frame->constant_scope()->module();
      }

      return open_class(state, gct, call_frame, under, super, name, created);
    }

    static Class* add_class(STATE, Module* under, Object* super, Symbol* name) {
      if(super->nil_p()) super = G(object);
      Class* cls = Class::create(state, as<Class>(super));

      cls->set_name(state, name, under);
      under->set_const(state, name, cls);

      return cls;
    }

    static Class* check_superclass(STATE, CallFrame* call_frame, Class* cls, Object* super) {
      if(super->nil_p()) return cls;
      if(cls->true_superclass(state) != super) {
        std::ostringstream message;
        message << "Superclass mismatch: given "
                << as<Module>(super)->debug_str(state)
                << " but previously set to "
                << cls->true_superclass(state)->debug_str(state);
        Exception* exc =
          Exception::make_type_error(state, Class::type, super, message.str().c_str());
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);
        return NULL;
      }

      return cls;
    }

    Class* open_class(STATE, GCToken gct, CallFrame* call_frame, Module* under, Object* super, Symbol* name, bool* created) {
      ConstantMissingReason reason;

      *created = false;

      Object* obj = under->get_const(state, name, G(sym_public), &reason);

      if(reason == vFound) {
        OnStack<4> os(state, under, super, name, obj);

        if(Autoload* autoload = try_as<Autoload>(obj)) {
          obj = autoload->resolve(state, gct, call_frame, under, true);

          // Check if an exception occurred
          if(!obj) return NULL;
        }

        // Autoload::resolve will return nil if code loading failed, in which
        // case we ignore the autoload.
        if(!obj->nil_p()) {
          return check_superclass(state, call_frame, as<Class>(obj), super);
        }
      }

      *created = true;
      return add_class(state, under, super, name);
    }

    Module* open_module(STATE, GCToken gct, CallFrame* call_frame, Symbol* name) {
      Module* under = G(object);

      call_frame = call_frame->top_ruby_frame();

      if(!call_frame->constant_scope()->nil_p()) {
        under = call_frame->constant_scope()->module();
      }

      return open_module(state, gct, call_frame, under, name);
    }

    Module* open_module(STATE, GCToken gct, CallFrame* call_frame, Module* under, Symbol* name) {
      Module* module;
      ConstantMissingReason reason;

      Object* obj = under->get_const(state, name, G(sym_public), &reason);

      if(reason == vFound) {
        OnStack<3> os(state, under, name, obj);

        if(Autoload* autoload = try_as<Autoload>(obj)) {
          obj = autoload->resolve(state, gct, call_frame, under, true);
        }

        // Check if an exception occurred
        if(!obj) return NULL;

        // Autoload::resolve will return nil if code loading failed, in which
        // case we ignore the autoload.
        if(!obj->nil_p()) {
          return as<Module>(obj);
        }
      }

      module = Module::create(state);

      module->set_name(state, name, under);
      under->set_const(state, name, module);

      return module;
    }

    bool yield_debugger(STATE, GCToken gct, CallFrame* call_frame, Object* bp) {
      Thread* cur = Thread::current(state);
      Thread* debugger = cur->debugger_thread();

      // Not for us, bail.
      if(debugger->nil_p()) {
        return true;
      }

      Channel* debugger_chan = debugger->control_channel();

      // Debugger not initialized? bail.
      if(debugger_chan->nil_p()) {
        std::cout << "no debugger channel\n";
        return true;
      }

      // If we're hitting here, clear any chance that step would be used
      // without being explicitly requested.
      state->vm()->clear_thread_step();

      state->set_call_frame(call_frame);

      Channel* my_control = cur->control_channel();

      // Lazily create our own control channel.
      if(my_control->nil_p()) {
        my_control = Channel::create(state);
        cur->control_channel(state, my_control);
      }

      Array* locs = Location::from_call_stack(state, call_frame, true, true);

      OnStack<1> os(state, my_control);

      debugger_chan->send(state, gct,
          Tuple::from(state, 4, bp, cur, my_control, locs), call_frame);

      // Block until the debugger wakes us back up.
      Object* ret = my_control->receive(state, gct, call_frame);

      // Do not access any locals other than ret beyond here unless you add OnStack<>
      // to them! The GC has probably run and moved things.

      // if ret is null, then receive was interrupted and there is an exception
      // to propagate.
      if(!ret) return false;

      // Process a few commands...
      if(ret == state->symbol("step")) {
        state->vm()->set_check_local_interrupts();
        state->vm()->set_thread_step();
      }

      // All done!
      return true;
    }
  }
}
