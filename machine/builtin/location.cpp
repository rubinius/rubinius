#include "memory.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/location.hpp"
#include "builtin/native_method.hpp"
#include "builtin/string.hpp"

namespace rubinius {
  void Location::bootstrap(STATE) {
    GO(location).set(state->memory()->new_class<Class, Location>(
          state, G(rubinius), "Location"));
  }

  Location* Location::allocate(STATE, Object* self) {
    return state->memory()->new_object<Location>(state, as<Class>(self));
  }

  Location* Location::create(STATE, CallFrame* call_frame, bool include_variables)
  {
    if(NativeMethodFrame* nmf = call_frame->native_method_frame()) {
      return create(state, nmf);
    }

    Location* loc = allocate(state, G(location));

    loc->method_module(state, call_frame->module());
    loc->receiver(state, call_frame->self());
    loc->method(state, call_frame->compiled_code);
    loc->ip(state, Fixnum::from(call_frame->ip()));
    loc->flags(state, Fixnum::from(0));

    if(call_frame->is_block_p(state)) {
      loc->name(state, call_frame->top_scope(state)->method()->name());
      loc->set_is_block(state);
    } else {
      loc->name(state, call_frame->name());
    }

    MachineCode* mcode = call_frame->compiled_code->machine_code();
    if(mcode && mcode->jitted_p()) {
      loc->set_is_jit(state);
    }

    if(include_variables) {
      // Use promote_scope because it can figure out of the generated
      // VariableScope should be isolated by default (true atm for JITd
      // frames)
      loc->variables(state, call_frame->promote_scope(state));
    } else {
      loc->variables(state, nil<VariableScope>());
    }

    loc->constant_scope(state, call_frame->constant_scope());

    return loc;
  }

  Location* Location::of_closest_ruby_method(STATE) {
    if(CallFrame* frame = state->vm()->get_ruby_frame(1)) {
      return Location::create(state, frame, false);
    }

    return nil<Location>();
  }

  Location* Location::create(STATE, NativeMethodFrame* nmf) {
    NativeMethod* nm = try_as<NativeMethod>(nmf->get_object(nmf->method()));
    if(!nm) return 0;

    Location* loc = state->memory()->new_object<Location>(state, G(location));
    if(Module* mod = try_as<Module>(nmf->get_object(nmf->module()))) {
      loc->method_module(state, mod);
    }

    loc->receiver(state, nmf->get_object(nmf->receiver()));

    loc->method(state, nm);
    loc->ip(state, Fixnum::from(-1));
    loc->flags(state, Fixnum::from(2));

    loc->name(state, nm->name());
    return loc;
  }

  Array* Location::from_call_stack(STATE, bool include_vars, bool on_ip, ssize_t up) {
    CallFrame* frame = state->vm()->call_frame();

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    if(!frame) return nil<Array>();

    size_t count = 0;

    CallFrame* base = frame;
    while(base) {
      count++;
      base = base->previous;
    }

    Array* array = Array::create(state, count);
    bool first = true;

    while(frame) {
      if(first) {
        if(!frame->compiled_code) continue;

        first = false;
        if(Location* location = Location::create(state, frame, include_vars)) {
          if(on_ip) location->set_ip_on_current(state);
          array->append(state, location);
        }
      } else {
        if(frame->compiled_code) {
          array->append(state, Location::create(state, frame, include_vars));
        } else if(NativeMethodFrame* nmf = frame->native_method_frame()) {
          if(Location* location = Location::create(state, nmf)) {
            array->append(state, location);
          }
        }
      }

      frame = frame->previous;
    }

    return array;
  }

  Array* Location::mri_backtrace(STATE, ssize_t up) {
    CallFrame* frame = state->vm()->call_frame();

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    if(!frame) return nil<Array>();

    size_t count = 0;

    CallFrame* base = frame;
    while(base) {
      count++;
      base = base->previous;
    }

    Array* array = Array::create(state, count);

    while(frame) {
      if(frame->compiled_code && !frame->compiled_code->core_method(state)) {
        Symbol* name;
        Object* block = cFalse;
        Fixnum* line = Fixnum::from(frame->line(state));

        if(frame->block_p()) {
          block = cTrue;
          name = frame->top_scope(state)->method()->name();
        } else {
          Symbol* current_name = frame->name();
          Symbol* method_name  = frame->compiled_code->name();

          if(current_name->nil_p()) {
            if(method_name->nil_p()) {
              name = state->symbol("<unknown>");
            } else {
              name = method_name;
            }
          } else {
            name = current_name;
          }
        }

        array->append(state,
            Tuple::from(state, 4, frame->compiled_code, line, block, name));
      }

      frame = frame->previous;
    }

    return array;
  }
}
