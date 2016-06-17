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
  class VM;

  class Fiber : public Object {
  public:
    const static object_type type = FiberType;

    static std::atomic<uint32_t> fiber_ids_;

    enum Status {
      eCreated,
      eRunning,
      eYielding,
      eTransfer,
      eDead
    };

    attr_accessor(block, Object);
    attr_accessor(arguments, Array);
    attr_accessor(value, Object);
    attr_accessor(exception, Exception);
    attr_accessor(locals, LookupTable);
    attr_accessor(pid, Fixnum);
    attr_accessor(stack_size, Fixnum);
    attr_accessor(thread_name, String);
    attr_accessor(fiber_id, Fixnum);
    attr_accessor(source, String);

  private:
    attr_field(start_time, uint64_t);

    attr_field(vm, VM*);
    attr_field(invoke_context, VM*);

    std::atomic<Status> status_;

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Fiber* obj) {
      obj->block(nil<Object>());
      obj->arguments(nil<Array>());
      obj->value(nil<Object>());
      obj->exception(nil<Exception>());
      obj->locals(LookupTable::create(state));
      obj->pid(Fixnum::from(0));
      obj->stack_size(Fixnum::from(state->shared().config.machine_fiber_stack_size.value));
      obj->thread_name(String::create(state, state->vm()->name().c_str()));
      obj->fiber_id(Fixnum::from(++Fiber::fiber_ids_));
      obj->source(nil<String>());
      obj->status(eCreated);
      obj->start_time(get_current_time());
      obj->vm(NULL);
      obj->invoke_context(state->vm());
    }

    static void finalize(STATE, Fiber* fib);
    static void* run(void*);

    static Fiber* create(STATE, VM* vm);

    // Rubinius.primitive :fiber_new
    static Fiber* create(STATE, Object* self, Object* stack_size, Object* block);

    // Rubinius.primitive :fiber_s_current
    static Fiber* current(STATE);

    // Rubinius.primitive :fiber_s_yield
    static Object* s_yield(STATE, Arguments& args);

    Status status() {
      return status_;
    }

    void status(Status status) {
      status_ = status;
    }

    bool root_p();

    void unpack_arguments(STATE, Arguments& args);
    Object* return_value(STATE);

    void start(STATE, Arguments& args);
    void restart(STATE);
    void suspend(STATE);

    // Rubinius.primitive :fiber_status
    String* status(STATE);

    // Rubinius.primitive :fiber_resume
    Object* resume(STATE, Arguments& args);

    // Rubinius.primitive :fiber_transfer
    Object* transfer(STATE, Arguments& args);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
