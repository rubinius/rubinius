#include "arguments.hpp"
#include "call_frame.hpp"
#include "memory.hpp"
#include "metrics.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fiber.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/object.hpp"

#include "memory/gc.hpp"

#include <ostream>
#include <string>

namespace rubinius {
  std::atomic<uint32_t> Fiber::fiber_ids_;

  void Fiber::bootstrap(STATE) {
    GO(fiber).set(state->memory()->new_class<Class, Fiber>(
          state, G(rubinius), "Fiber"));

    fiber_ids_.store(0);

#ifdef RBX_FIBER_ENABLED
    G(fiber)->set_const(state, "ENABLED", cTrue);
#else
    G(fiber)->set_const(state, "ENABLED", cFalse);
#endif
  }

  double Fiber::run_time() {
    return timer::time_elapsed_seconds(start_time());
  }

  Fiber* Fiber::current(STATE) {
#ifdef RBX_FIBER_ENABLED
    Fiber* fib = state->vm()->current_fiber.get();

    // Lazily allocate a root fiber.
    if(fib->nil_p()) {
      fib = state->memory()->new_object<Fiber>(state, G(fiber));
      fib->root(true);
      fib->status(Fiber::eRunning);

      fib->data(state->vm()->new_fiber_data(true, fib->stack_size()->to_native()));
      fib->data()->set_call_frame(state->vm()->call_frame());

      state->memory()->native_finalizer(state, fib,
          (memory::FinalizerFunction)&Fiber::finalize);

      state->vm()->current_fiber.set(fib);
      state->vm()->root_fiber.set(fib);
    }

    return fib;
#else
    Exception::raise_not_implemented_error(state, "Fibers not supported on this platform");
#endif
  }

  void Fiber::start_on_stack() {
#ifdef RBX_FIBER_ENABLED
    VM* vm = VM::current();
    State state_obj(vm), *state = &state_obj;

    Fiber* fib = Fiber::current(state);

    // Reset the current fiber again to reset the stack limits so
    // we can properly detect stack overflows
    vm->set_current_fiber(fib);

    Array* result = nil<Array>();
    Object* obj = fib->starter()->send(state, G(sym_call), fib->value(), cNil, false);

    // GC has run! Don't use stack vars!

    fib = Fiber::current(state);
    fib->status(Fiber::eDead);
    fib->dead(cTrue);
    fib->set_call_frame(state, 0);

    if(state->shared().config.machine_fiber_log_lifetime.value) {
      logger::write("fiber: exit: %s, %d, %fs",
          fib->thread_name()->c_str(state),
          fib->fiber_id()->to_native(), fib->run_time());
    }

    Fiber* dest = fib->prev();

    // If this fiber has already been cleaned up, just ignore this
    if(!dest->data()) return;

    assert(!dest->nil_p());

    // Box this up so it's in a standard format at the point
    // of returning, so we can deal with it in the same way
    // as *args from #yield, #resume, and #transfer
    if(obj) {
      result = Array::create(state, 1);
      result->set(state, 0, obj);
    } else {
      if(state->vm()->thread_state()->raise_reason() == cException) {
        dest->exception(state, state->vm()->thread_state()->current_exception());
      }
    }

    vm->metrics().system.fibers_destroyed++;

    dest->run(state);
    dest->value(state, result);

    dest->data()->switch_and_orphan(state, fib->data());

    // TODO: CallFrame: return from this function

    rubinius::bug("returning from Fiber::start_on_stack");
#else
    rubinius::bug("Fibers not supported on this platform");
#endif
  }

  Fiber* Fiber::create(STATE, Object* self, Object* stack_size, Object* callable) {
#ifdef RBX_FIBER_ENABLED
    Fiber* fib = state->memory()->new_object<Fiber>(state, as<Class>(self));
    fib->starter(state, callable);

    if(Fixnum* size = try_as<Fixnum>(stack_size)) {
      state->vm()->validate_stack_size(state, size->to_native());
      fib->stack_size(state, size);
    }

    if(state->shared().config.machine_fiber_log_lifetime.value) {
      std::string& filter = state->shared().config.machine_fiber_log_filter.value;

      if(CallFrame* call_frame = state->vm()->get_filtered_frame(state, filter)) {
        std::ostringstream source;

        source << call_frame->file(state)->cpp_str(state).c_str()
          << ":" << call_frame->line(state);

        logger::write("fiber: new: %s, %d, %s",
            fib->thread_name()->c_str(state),
            fib->fiber_id()->to_native(), source.str().c_str());

        fib->source(state, String::create(state, source.str().c_str()));
      } else {
        logger::write("fiber: new: %s, %d",
            fib->thread_name()->c_str(state), fib->fiber_id()->to_native());
      }
    }

    state->vm()->metrics().system.fibers_created++;

    state->memory()->native_finalizer(state, fib,
        (memory::FinalizerFunction)&Fiber::finalize);

    return fib;
#else
    Exception::raise_not_implemented_error(state, "Fibers not supported on this platform");
#endif
  }

  Object* Fiber::resume(STATE, Arguments& args) {
#ifdef RBX_FIBER_ENABLED
    if(!data()) {
      data(state->vm()->new_fiber_data(stack_size()->to_native()));
    }

    if(status() == Fiber::eDead || data()->dead_p()) {
      Exception::raise_fiber_error(state, "dead fiber called");
    }

    if(!prev()->nil_p()) {
      Exception::raise_fiber_error(state, "double resume");
    }

    if(data()->thread() && data()->thread() != state->vm()) {
      Exception::raise_fiber_error(state, "cross thread fiber resuming is illegal");
    }

    Array* val = args.as_array(state);
    value(state, val);

    Fiber* cur = Fiber::current(state);
    prev(state, cur);

    cur->sleep(state);

    run(state);

    data()->switch_to(state, cur->data());

    // Back here when someone yields back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);

    // TODO: clean up this and the following conditional.
    if(state->vm()->thread_interrupted_p(state)) {
      return NULL;
    }

    if(!cur->exception()->nil_p()) {
      state->raise_exception(cur->exception());
      cur->exception(state, nil<Exception>());
      return NULL;
    }

    Array* ret = cur->value();

    if(ret->nil_p()) return cNil;

    switch(ret->size()) {
    case 0:  return cNil;
    case 1:  return ret->get(state, 0);
    default: return ret;
    }
#else
    Exception::raise_not_implemented_error(state, "Fibers not supported on this platform");
#endif
  }

  Object* Fiber::transfer(STATE, Arguments& args) {
#ifdef RBX_FIBER_ENABLED
    if(!data()) {
      data(state->vm()->new_fiber_data(stack_size()->to_native()));
    }

    if(status() == Fiber::eDead || data()->dead_p()) {
      Exception::raise_fiber_error(state, "dead fiber called");
    }

    if(data()->thread() && data()->thread() != state->vm()) {
      Exception::raise_fiber_error(state, "cross thread fiber resuming is illegal");
    }

    Array* val = args.as_array(state);
    value(state, val);

    Fiber* cur = Fiber::current(state);
    Fiber* root = state->vm()->root_fiber.get();
    assert(root);

    prev(state, root);

    cur->sleep(state);

    run(state);

    data()->switch_to(state, cur->data());

    // Back here when someone transfers back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);

    if(!cur->exception()->nil_p()) {
      state->raise_exception(cur->exception());
      cur->exception(state, nil<Exception>());
      return 0;
    }

    Array* ret = cur->value();

    if(ret->nil_p()) return cNil;

    switch(ret->size()) {
    case 0:  return cNil;
    case 1:  return ret->get(state, 0);
    default: return ret;
    }
#else
    Exception::raise_not_implemented_error(state, "Fibers not supported on this platform");
#endif
  }

  Object* Fiber::s_yield(STATE, Arguments& args) {
#ifdef RBX_FIBER_ENABLED
    Fiber* cur = Fiber::current(state);
    Fiber* dest_fib = cur->prev();

    assert(cur != dest_fib);

    if(cur->root()) {
      Exception::raise_fiber_error(state, "can't yield from root fiber");
    }

    cur->prev(state, nil<Fiber>());

    Array* val = args.as_array(state);
    dest_fib->value(state, val);

    cur->sleep(state);

    dest_fib->run(state);

    dest_fib->data()->switch_to(state, cur->data());

    // Back here when someone yields back to us!
    // Beware here, because the GC has probably run so GC pointers on the C++ stack
    // can't be accessed.

    cur = Fiber::current(state);

    Array* ret = cur->value();

    if(ret->nil_p()) return cNil;

    switch(ret->size()) {
    case 0:  return cNil;
    case 1:  return ret->get(state, 0);
    default: return ret;
    }
#else
    Exception::raise_not_implemented_error(state, "Fibers not supported on this platform");
#endif
  }


  void Fiber::finalize(STATE, Fiber* fib) {
#ifdef RBX_FIBER_ENABLED
    if(state->shared().config.machine_fiber_log_finalizer.value) {
      logger::write("fiber: finalizer: %s, %d",
          fib->thread_name()->c_str(state), fib->fiber_id()->to_native());
    }

    if(!fib->data()) return;
    fib->data()->orphan(state);

    delete fib->data();
    fib->data(NULL);
#endif
  }

  void Fiber::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);
    Fiber* fib = force_as<Fiber>(obj);
    FiberData* data = fib->data();
    if(!data || data->dead_p()) return;
    data->set_mark();
  }
}

