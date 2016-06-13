#ifndef RBX_BUILTIN_FIBER
#define RBX_BUILTIN_FIBER

#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

#include "instruments/timing.hpp"

#include <stdint.h>
#include <atomic>

namespace rubinius {
  class LookupTable;

  class Fiber : public Object {
  public:
    const static object_type type = FiberType;

    static std::atomic<uint32_t> fiber_ids_;

    enum Status {
      eNotStarted,
      eSleeping,
      eRunning,
      eDead
    };

    attr_accessor(starter, Object);
    attr_accessor(value, Array);
    attr_accessor(prev, Fiber);
    attr_accessor(exception, Exception);
    attr_accessor(locals, LookupTable);
    attr_accessor(dead, Object);
    attr_accessor(stack_size, Fixnum);
    attr_accessor(thread_name, String);
    attr_accessor(fiber_id, Fixnum);
    attr_accessor(source, String);

  private:
    attr_field(status, Status);
    attr_field(root, bool);
    attr_field(start_time, uint64_t);

  public:
    bool root_p() const {
      return root();
    }

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Fiber* obj) {
      obj->starter(nil<Object>());
      obj->value(nil<Array>());
      obj->prev(nil<Fiber>());
      obj->exception(nil<Exception>());
      obj->locals(nil<LookupTable>());
      obj->dead(nil<Object>());
      obj->stack_size(Fixnum::from(state->shared().config.machine_fiber_stack_size.value));
      obj->thread_name(String::create(state, state->vm()->name().c_str()));
      obj->fiber_id(Fixnum::from(++Fiber::fiber_ids_));
      obj->source(nil<String>());
      obj->status(eNotStarted);
      obj->root(false);
      obj->start_time(get_current_time());
    }

    // Rubinius.primitive :fiber_new
    static Fiber* create(STATE, Object* self, Object* stack_size, Object* callable);

    double run_time();

    // Rubinius.primitive :fiber_s_current
    static Fiber* current(STATE);

    // Rubinius.primitive :fiber_resume
    Object* resume(STATE, Arguments& args);

    // Rubinius.primitive :fiber_transfer
    Object* transfer(STATE, Arguments& args);

    // Rubinius.primitive :fiber_s_yield
    static Object* s_yield(STATE, Arguments& args);

    static void finalize(STATE, Fiber* fib);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void mark(Object* t, memory::ObjectMark& mark);
    };
  };
}

#endif
