#ifndef RBX_BUILTIN_FIBER
#define RBX_BUILTIN_FIBER

#include "fiber_data.hpp"
#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/object.hpp"

namespace rubinius {
  class LookupTable;

  class Fiber : public Object {
  public:
    const static object_type type = FiberType;

    enum Status {
      eNotStarted, eSleeping, eRunning, eDead
    };

    attr_accessor(starter, Object);
    attr_accessor(value, Array);
    attr_accessor(prev, Fiber);
    attr_accessor(exception, Exception);
    attr_accessor(locals, LookupTable);
    attr_accessor(dead, Object);
    attr_accessor(stack_size, Fixnum);

  private:
    attr_field(status, Status);
    attr_field(root, bool);
    attr_field(data, FiberData*);

  public:
    bool root_p() const {
      return root();
    }

    CallFrame* call_frame(STATE) const {
      if(!data()) Exception::raise_fiber_error(state, "corrupt Fiber");

      return data()->call_frame();
    }

    void set_call_frame(STATE, CallFrame* call_frame) {
      if(!data()) Exception::raise_fiber_error(state, "corrupt Fiber");

      data()->set_call_frame(call_frame);
    }

    void sleep(STATE) {
      if(!data()) Exception::raise_fiber_error(state, "corrupt Fiber");

      data()->set_call_frame(state->vm()->call_frame());
      status(eSleeping);
    }

    void run(STATE) {
      if(!data()) Exception::raise_fiber_error(state, "corrupt Fiber");

      state->vm()->set_current_fiber(this);
      state->vm()->set_call_frame(data()->call_frame());
      data()->set_call_frame(NULL);
      status(eRunning);
    }

    fiber_context_t* ucontext() const {
      return data()->machine();
    }

    memory::VariableRootBuffers& variable_root_buffers() {
      return data()->variable_root_buffers();
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
      obj->status(eNotStarted);
      obj->root(false);
      obj->data(NULL);
    }

    // Rubinius.primitive :fiber_new
    static Fiber* create(STATE, Object* self, Object* stack_size, Object* callable);
    static void start_on_stack();

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
