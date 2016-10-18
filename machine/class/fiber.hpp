#ifndef RBX_BUILTIN_FIBER
#define RBX_BUILTIN_FIBER

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/exception.hpp"
#include "class/lookup_table.hpp"
#include "class/object.hpp"
#include "class/string.hpp"
#include "class/thread.hpp"

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
    attr_accessor(exception, Exception);
    attr_accessor(locals, LookupTable);
    attr_accessor(pid, Fixnum);
    attr_accessor(stack_size, Fixnum);
    attr_accessor(thread_name, String);
    attr_accessor(fiber_id, Fixnum);
    attr_accessor(source, String);
    attr_accessor(thread, Thread);

  private:
    attr_field(start_time, uint64_t);

    attr_field(vm, VM*);
    attr_field(invoke_context, VM*);
    attr_field(restart_context, VM*);

    std::atomic<Status> status_;
    std::atomic<bool> wakeup_;

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Fiber* obj) {
      obj->block(nil<Object>());
      obj->exception(nil<Exception>());
      obj->locals(LookupTable::create(state));
      obj->pid(Fixnum::from(0));
      obj->stack_size(Fixnum::from(state->shared().config.machine_fiber_stack_size.value));
      obj->thread_name(String::create(state, state->vm()->name().c_str()));
      obj->fiber_id(Fixnum::from(++Fiber::fiber_ids_));
      obj->source(nil<String>());
      obj->thread(state->vm()->thread());
      obj->start_time(get_current_time());
      obj->vm(NULL);
      obj->invoke_context(state->vm());
      obj->restart_context(state->vm());
      obj->status(eCreated);
      obj->clear_wakeup();
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

    // Rubinius.primitive :fiber_s_list
    static Array* s_list(STATE);

    // Rubinius.primitive :fiber_s_main
    static Fiber* s_main(STATE);

    // Rubinius.primitive :fiber_s_count
    static Fixnum* s_count(STATE);

    bool root_p();

    Status status() {
      return status_;
    }

    void status(Status status) {
      status_ = status;
    }

    void wakeup() {
      wakeup_ = true;
    }

    void clear_wakeup() {
      wakeup_ = false;
    }

    bool wakeup_p() {
      return wakeup_;
    }

    void unpack_arguments(STATE, Arguments& args);
    Object* return_value(STATE);

    void start(STATE, Arguments& args);
    void restart(STATE);
    void cancel(STATE);
    void suspend_and_continue(STATE);

    // Rubinius.primitive :fiber_status
    String* status(STATE);

    // Rubinius.primitive :fiber_resume
    Object* resume(STATE, Arguments& args);

    // Rubinius.primitive :fiber_transfer
    Object* transfer(STATE, Arguments& args);

    // Rubinius.primitive :fiber_dispose
    Object* dispose(STATE);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
