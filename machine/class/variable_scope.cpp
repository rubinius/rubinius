#include "memory.hpp"
#include "call_frame.hpp"

#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/fiber.hpp"
#include "class/object.hpp"
#include "class/system.hpp"
#include "class/tuple.hpp"
#include "class/variable_scope.hpp"

#include <mutex>

namespace rubinius {
  void VariableScope::bootstrap(STATE) {
    GO(variable_scope).set(state->memory()->new_class<Class, VariableScope>(
          state, G(rubinius), "VariableScope"));
  }

  void VariableScope::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(variable_scope), false,
                             state->symbol("method_visibility"),
                             state->symbol("variable_scope_method_visibility"));
  }

  VariableScope* VariableScope::of_sender(STATE) {
    if(CallFrame* frame = state->get_ruby_frame(1)) {
      return frame->promote_scope(state);
    }

    return nil<VariableScope>();
  }

  VariableScope* VariableScope::current(STATE) {
    if(CallFrame* call_frame = state->call_frame()) {
      if(!call_frame->native_method_p()) return call_frame->promote_scope(state);
    }

    return nil<VariableScope>();
  }

  VariableScope* VariableScope::allocate(STATE) {
    return state->memory()->new_object<VariableScope>(state, G(variable_scope));
  }

  VariableScope* VariableScope::synthesize(STATE, CompiledCode* method,
      Module* module, Object* parent, Object* self, Object* block, Tuple* locals)
  {
    VariableScope* scope =
      state->memory()->new_object<VariableScope>(state, G(variable_scope));

    scope->block(state, block);
    scope->module(state, module);
    scope->method(state, method);

    if(VariableScope* vs = try_as<VariableScope>(parent)) {
      scope->parent(state, vs);
    } else {
      scope->parent(state, nil<VariableScope>());
    }

    scope->heap_locals(state, locals);

    scope->self(state, self);
    scope->number_of_locals(locals->num_fields());

    return scope;
  }

  Tuple* VariableScope::local_variables(STATE) {
    Tuple* tup = Tuple::create(state, number_of_locals());

    for(int i = 0; i < number_of_locals(); i++) {
      tup->put(state, i, get_local(state, i));
    }

    return tup;
  }

  Object* VariableScope::set_local_prim(STATE, Fixnum* number, Object* object) {
    int num = number->to_int();

    if(num < 0) {
      Exception::raise_argument_error(state, "negative local index");
    } else if(num >= number_of_locals()) {
      Exception::raise_argument_error(state, "index larger than number of locals");
    }

    set_local(state, num, object);
    return cNil;
  }

  // bootstrap method, replaced with an attr_accessor in core library.
  Object* VariableScope::method_visibility(STATE) {
    return cNil;
  }

  Object* VariableScope::locked(STATE) {
    return RBOOL(locked_p());
  }

  Object* VariableScope::set_locked(STATE) {
    _flags_ |= CallFrame::cScopeLocked;
    VariableScope* parent = this->parent();
    while(parent && !parent->nil_p()) {
      parent->set_locked(state);
      parent = parent->parent();
    }
    return cNil;
  }

  void VariableScope::set_local_internal(STATE, int pos, Object* val) {
     if(isolated()) {
       heap_locals()->put(state, pos, val);
     } else {
       set_local(pos, val);
     }
   }

  void VariableScope::set_local(STATE, int pos, Object* val) {
    if(unlikely(locked_p())) {
      std::lock_guard<locks::spinlock_mutex> guard(_lock_);
      set_local_internal(state, pos, val);
    } else {
      set_local_internal(state, pos, val);
    }
  }

  void VariableScope::set_local(int pos, Object* val) {
    Object** ary = locals();

    ary[pos] = val;
  }

  Object* VariableScope::get_local_internal(STATE, int pos) {
     if(isolated()) {
       return heap_locals()->at(pos);
     } else {
       return get_local(pos);
     }
   }

  Object* VariableScope::get_local(STATE, int pos) {
    if(unlikely(locked_p())) {
      std::lock_guard<locks::spinlock_mutex> guard(_lock_);
      return get_local_internal(state, pos);
    } else {
      return get_local_internal(state, pos);
    }
  }

  Object* VariableScope::get_local(int pos) {
    Object** ary = locals();

    return ary[pos];
  }

  Object* VariableScope::top_level_visibility(STATE) {
    return RBOOL(top_level_visibility_p());
  }

  Object* VariableScope::script(STATE) {
    return RBOOL(script_p());
  }

  void VariableScope::flush_to_heap_internal(STATE) {
    if(isolated()) return;

    Tuple* new_locals = Tuple::create(state, number_of_locals());

    for(int i = 0; i < number_of_locals(); i++) {
      new_locals->put(state, i, locals()[i]);
    }

    heap_locals(state, new_locals);
    isolated(1);
  }

  void VariableScope::flush_to_heap(STATE) {
    if(unlikely(locked_p())) {
      std::lock_guard<locks::spinlock_mutex> guard(_lock_);
      flush_to_heap_internal(state);
      _flags_ &= ~CallFrame::cScopeLocked;
    } else {
      flush_to_heap_internal(state);
    }
  }

  void VariableScope::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    auto_mark(state, obj, f);

    VariableScope* vs = as<VariableScope>(obj);

    if(!vs->isolated_p()) {
      Object** ary = vs->locals();

      size_t locals = vs->number_of_locals();

      for(size_t i = 0; i < locals; i++) {
        f(state, &ary[i]);
      }
    }
  }

  void VariableScope::Info::before_visit(STATE, Object* obj,
      std::function<void (STATE, Object**)> f)
  {
    VariableScope* vs = as<VariableScope>(obj);

    if(!vs->isolated_p()) {
      Object** ary = vs->locals();

      size_t locals = vs->number_of_locals();

      for(size_t i = 0; i < locals; i++) {
        f(state, &ary[i]);
      }
    }
  }
}
