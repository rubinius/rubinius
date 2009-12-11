#include "helpers.hpp"
#include "builtin/object.hpp"
#include "call_frame.hpp"
#include "builtin/autoload.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/lookuptable.hpp"
#include "global_cache.hpp"
#include "objectmemory.hpp"
#include "builtin/tuple.hpp"
#include "builtin/system.hpp"
#include "builtin/thread.hpp"
#include "builtin/channel.hpp"
#include "builtin/global_cache_entry.hpp"
#include "builtin/methodtable.hpp"

#include "vm.hpp"
#include "object_utils.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "lookup_data.hpp"

namespace rubinius {
  namespace Helpers {
    void add_method(STATE, CallFrame* call_frame, Module* mod, Symbol* name, CompiledMethod* method) {
      method->scope(state, call_frame->static_scope());
      method->serial(state, Fixnum::from(0));
      mod->method_table()->store(state, name, method, G(sym_public));
      state->global_cache->clear(mod, name);

      if(Class* cls = try_as<Class>(mod)) {
        method->formalize(state, false);

        object_type type = (object_type)cls->instance_type()->to_native();
        TypeInfo* ti = state->om->type_info[type];
        if(ti) {
          method->specialize(state, ti);
        }
      }
    }

    void attach_method(STATE, CallFrame* call_frame, Object* recv, Symbol* name, CompiledMethod* method) {
      if(Module* mod = try_as<Module>(recv)) {
        StaticScope* ss = StaticScope::create(state);
        ss->module(state, mod);
        ss->parent(state, method->scope());
        method->scope(state, ss);
      } else {
        /* Push the current scope down. */
        method->scope(state, call_frame->static_scope());
      }

      add_method(state, call_frame, recv->metaclass(state), name, method);
    }

    Object* const_get(STATE, Module* mod, Symbol* name, bool* found) {
      Object* res;

      *found = false;

      while(!mod->nil_p()) {
        res = mod->get_const(state, name, found);
        if(*found) return res;

        // Don't stop when you see Object, because we need to check any
        // includes into Object as well, and they're found via superclass
        mod = mod->superclass();
      }

      return Qnil;
    }

    Object* const_get(STATE, CallFrame* call_frame, Symbol* name, bool* found) {
      StaticScope *cur;
      Object* result;

      *found = false;

      // Ok, this has to be explained or it will be considered black magic.
      // The scope chain always ends with an entry at the top that contains
      // a parent of nil, and a module of Object. This entry is put in
      // regardless of lexical scoping, it's the default scope.
      //
      // When looking up a constant, we don't want to consider the default
      // scope (ie, Object) initially because we need to lookup up
      // the superclass chain first, because falling back on the default.
      //
      // The rub comes from the fact that if a user explicitely opens up
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
      cur = call_frame->static_scope();
      while(!cur->nil_p()) {
        // Detect the toplevel scope (the default) and get outta dodge.
        if(cur->top_level_p(state)) break;

        result = cur->module()->get_const(state, name, found);
        if(*found) return result;

        cur = cur->parent();
      }

      // Now look up the superclass chain.
      Module* mod = call_frame->static_scope()->module();
      while(!mod->nil_p()) {
        result = mod->get_const(state, name, found);
        if(*found) return result;

        mod = mod->superclass();
      }

      // Lastly, check Object specificly
      result = G(object)->get_const(state, name, found);
      if(*found) return result;

      return Qnil;
    }

    Object* const_missing(STATE, Module* under, Symbol* sym, CallFrame* call_frame) {
      Array* args = Array::create(state, 1);
      args->set(state, 0, sym);
      return under->send(state, call_frame, G(sym_const_missing), args);
    }

    /** @todo Remove redundancy between this and sends. --rue */
    Object* locate_method_on(STATE, CallFrame* call_frame, Object* recv, Symbol* name, Object* priv) {
      LookupData lookup(recv, recv->lookup_begin(state));
      lookup.priv = (priv == Qtrue);

      Dispatch dis(name);

      if(!GlobalCache::resolve(state, dis.name, dis, lookup)) {
        return (Tuple*)Qnil;
      }

      return Tuple::from(state, 2, dis.method, dis.module);
    }

    Class* open_class(STATE, CallFrame* call_frame, Object* super, Symbol* name, bool* created) {
      Module* under;

      if(call_frame->static_scope()->nil_p()) {
        under = G(object);
      } else {
        under = call_frame->static_scope()->module();
      }

      return open_class(state, call_frame, under, super, name, created);
    }

    static Class* add_class(STATE, Module* under, Object* super, Symbol* name) {
      if(super->nil_p()) super = G(object);
      Class* cls = Class::create(state, as<Class>(super));

      if(under == G(object)) {
        cls->name(state, name);
      } else {
        cls->set_name(state, under, name);
      }

      under->set_const(state, name, cls);

      return cls;
    }

    static Class* check_superclass(STATE, CallFrame* call_frame, Class* cls, Object* super) {
      if(super->nil_p()) return cls;
      if(cls->true_superclass(state) != super) {
        std::ostringstream message;
        message << "Superclass mismatch: given "
                << as<Module>(super)->name()->c_str(state)
                << " but previously set to "
                << cls->true_superclass(state)->name()->c_str(state);
        Exception* exc =
          Exception::make_type_error(state, Class::type, super, message.str().c_str());
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }

      return cls;
    }

    Class* open_class(STATE, CallFrame* call_frame, Module* under, Object* super, Symbol* name, bool* created) {
      bool found;

      *created = false;

      Object* obj = under->get_const(state, name, &found);
      if(found) {
        TypedRoot<Object*> sup(state, super);

        if(Autoload* autoload = try_as<Autoload>(obj)) {
          obj = autoload->resolve(state, call_frame);

          // Check if an exception occurred
          if(!obj) return NULL;
        }

        return check_superclass(state, call_frame, as<Class>(obj), sup.get());
      }

      *created = true;
      return add_class(state, under, super, name);
    }

    Module* open_module(STATE, CallFrame* call_frame, Symbol* name) {
      Module* under = G(object);

      if(!call_frame->static_scope()->nil_p()) {
        under = call_frame->static_scope()->module();
      }

      return open_module(state, call_frame, under, name);
    }

    Module* open_module(STATE, CallFrame* call_frame, Module* under, Symbol* name) {
      Module* module;
      bool found;

      Object* obj = const_get(state, under, name, &found);

      if(found) {
        if(Autoload* autoload = try_as<Autoload>(obj)) {
          obj = autoload->resolve(state, call_frame);
        }

        return as<Module>(obj);
      }

      module = Module::create(state);

      module->set_name(state, under, name);
      under->set_const(state, name, module);

      return module;
    }

    void yield_debugger(STATE, CallFrame* call_frame) {
      Channel* chan;

      state->set_call_frame(call_frame);

      chan = try_as<Channel>(G(vm)->get_ivar(state,
            state->symbol("@debug_channel")));

      if(!chan) return;

      Channel* control = state->thread->control_channel();

      if(control->nil_p()) {
        control = Channel::create(state);
        state->thread->control_channel(state, control);
      }

      sassert(chan->has_readers_p());

      chan->send(state, state->thread.get());
      control->receive(state, call_frame);
    }
  }
}
