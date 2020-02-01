#include "memory.hpp"
#include "machine_code.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "class/location.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"

#include "capi/capi.hpp"

#include "diagnostics/machine.hpp"
#include "diagnostics/timing.hpp"

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
    // TODO: instructions
    loc->ip(state, Fixnum::from(call_frame->ip()+1));
    loc->flags(state, Fixnum::from(0));

    if(call_frame->is_block_p(state)) {
      if(VariableScope* vs = try_as<VariableScope>(call_frame->top_scope(state))) {
        loc->name(state, vs->method()->name());
      } else {
        loc->name(state, call_frame->name());
      }
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

    loc->lexical_scope(state, call_frame->lexical_scope());

    return loc;
  }

  Location* Location::of_closest_ruby_method(STATE) {
    if(CallFrame* frame = state->get_ruby_frame(1)) {
      return Location::create(state, frame, false);
    }

    return Location::allocate(state, G(location));
  }

  Location* Location::create(STATE, NativeMethodFrame* nmf) {
    NativeMethod* nm = MemoryHandle::try_as<NativeMethod>(nmf->method());
    if(!nm) return Location::allocate(state, G(location));

    Location* loc = state->memory()->new_object<Location>(state, G(location));
    if(Module* mod = MemoryHandle::try_as<Module>(nmf->module())) {
      loc->method_module(state, mod);
    }

    loc->receiver(state, MemoryHandle::object(nmf->receiver()));

    loc->method(state, nm);
    loc->ip(state, Fixnum::from(-1));
    loc->flags(state, Fixnum::from(2));

    loc->name(state, nm->name());
    return loc;
  }

  Array* Location::debugging_call_stack(STATE) {
    CallFrame* base = state->call_frame();
    size_t count = 0;

    while(base) {
      count++;
      base = base->previous;
    }

    if(count == 0) return nil<Array>();

    Array* array = Array::create(state, count);
    bool first = true;

    for(CallFrame* frame = state->call_frame();
        frame;
        frame = frame->previous)
    {
      if(frame->compiled_code) {
        Location* location = Location::create(state, frame, true);

        if(first) {
          location->set_ip_on_current(state);
          first = false;
        }

        array->append(state, location);
      } else if(NativeMethodFrame* nmf = frame->native_method_frame()) {
        array->append(state, Location::create(state, nmf));
      }

    }

    return array;
  }

  Array* Location::from_call_stack(STATE, ssize_t up) {
    if(up < 0) rubinius::bug("negative skip frame value provided");

    timer::StopWatch<timer::microseconds> timer(
        state->metrics()->backtrace_us);
    state->metrics()->backtraces++;

    CallFrame* base = state->call_frame();
    CallFrame* start = base;
    size_t count = 0;

    while(base) {
      if(up-- > 0) {
        start = base = base->previous;
      } else {
        count++;
        base = base->previous;
      }
    }

    if(count == 0) return nil<Array>();

    Array* array = Array::create(state, count);

    for(CallFrame* frame = start; frame; frame = frame->previous) {
      if(frame->compiled_code) {
        array->append(state, Location::create(state, frame));
      } else if(NativeMethodFrame* nmf = frame->native_method_frame()) {
        array->append(state, Location::create(state, nmf));
      }
    }

    return array;
  }

  Array* Location::mri_backtrace(STATE, ssize_t up) {
    if(up < 0) rubinius::bug("negative skip frame value provided");

    timer::StopWatch<timer::microseconds> timer(
        state->metrics()->backtrace_us);
    state->metrics()->backtraces++;

    CallFrame* base = state->call_frame();
    CallFrame* start = base;
    size_t count = 0;

    while(base) {
      if(up-- > 0) {
        start = base = base->previous;
      } else {
        count++;
        base = base->previous;
      }
    }

    if(count == 0) return nil<Array>();

    Array* array = Array::create(state, count);

    for(CallFrame* frame = start; frame; frame = frame->previous) {
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
    }

    return array;
  }
}
