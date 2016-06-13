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
#include <regex>
#include <string>

namespace rubinius {
  std::atomic<uint32_t> Fiber::fiber_ids_;

  void Fiber::bootstrap(STATE) {
    GO(fiber).set(state->memory()->new_class<Class, Fiber>(state, "Fiber"));

    fiber_ids_.store(0);
  }

  double Fiber::run_time() {
    return timer::time_elapsed_seconds(start_time());
  }

  Fiber* Fiber::current(STATE) {
    return nil<Fiber>();
  }

  Fiber* Fiber::create(STATE, Object* self, Object* stack_size, Object* callable) {
    Fiber* fib = state->memory()->new_object<Fiber>(state, as<Class>(self));
    fib->starter(state, callable);

    if(Fixnum* size = try_as<Fixnum>(stack_size)) {
      state->vm()->validate_stack_size(state, size->to_native());
      fib->stack_size(state, size);
    }

    if(state->shared().config.machine_fiber_log_lifetime.value) {
      const std::regex& filter = state->shared().config.machine_fiber_log_filter();

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
  }

  Object* Fiber::resume(STATE, Arguments& args) {
    return cNil;
  }

  Object* Fiber::transfer(STATE, Arguments& args) {
    return cNil;
  }

  Object* Fiber::s_yield(STATE, Arguments& args) {
    return cNil;
  }


  void Fiber::finalize(STATE, Fiber* fib) {
    if(state->shared().config.machine_fiber_log_finalizer.value) {
      logger::write("fiber: finalizer: %s, %d",
          fib->thread_name()->c_str(state), fib->fiber_id()->to_native());
    }
  }

  void Fiber::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);
    // Fiber* fib = force_as<Fiber>(obj);
  }
}
