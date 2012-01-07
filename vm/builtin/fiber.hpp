#ifndef RBX_BUILTIN_FIBER
#define RBX_BUILTIN_FIBER

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "prelude.hpp"
#include "builtin/object.hpp"

#include "fiber_data.hpp"

namespace rubinius {
  class Fiber : public Object {
  public:
    const static object_type type = FiberType;

    enum Status {
      eSleeping, eRunning, eDead
    };

  private:
    Object* starter_; // slot
    Array* value_; // slot
    Fiber* prev_; // slot
    Exception* exception_; // slot
    CallFrame* top_;
    Status status_;
    VM* vm_;

    bool root_;

    FiberData* data_;
  public:
    attr_accessor(starter, Object);
    attr_accessor(value, Array);
    attr_accessor(prev, Fiber);
    attr_accessor(exception, Exception)

    CallFrame* call_frame() {
      return top_;
    }

    void sleep(CallFrame* cf) {
      top_ = cf;
      status_ = eSleeping;
    }

    void run() {
      top_ = 0;
      status_ = eRunning;
    }

    fiber_context_t* ucontext() {
      return data_->machine();
    }

    VM* vm() {
      return vm_;
    }

    void* stack() {
      return data_->stack_address();
    }

    int stack_size() {
      return data_->stack_size();
    }

  public:
    static void init(STATE);

    // Rubinius.primitive :fiber_new
    static Fiber* create(STATE, Object* self, Integer* stack_size, Object* callable);
    static void start_on_stack();

    // Rubinius.primitive :fiber_s_current
    static Fiber* current(STATE);

    // Rubinius.primitive :fiber_resume
    Object* resume(STATE, Arguments& args, CallFrame* calling_environment);

    // Rubinius.primitive :fiber_transfer
    Object* transfer(STATE, Arguments& args, CallFrame* calling_environment);

    // Rubinius.primitive :fiber_s_yield
    static Object* s_yield(STATE, Arguments& args, CallFrame* calling_environment);

    static void finalize(STATE, Fiber* fib);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void mark(Object* t, ObjectMark& mark);
    };
  };
}

#endif
