#ifndef RBX_BUILTIN_FIBER
#define RBX_BUILTIN_FIBER

#include "vm/config.h"

#ifdef HAS_UCONTEXT
#define FIBER_ENABLED
#include <ucontext.h>
typedef ucontext_t fiber_context_t;
#else
struct fiber_context_t {
  int dummy;
};
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtin/object.hpp"

namespace rubinius {
  class Fiber : public Object {
  public:
    const static object_type type = FiberType;

    enum Status {
      eSleeping, eRunning, eDead
    };

  private:
    Object* starter_; // slot
    Object* value_; // slot
    Fiber* prev_; // slot
    CallFrame* top_;
    Status status_;
    VM* state_;

    bool root_;
    void* stack_;
    int stack_size_;
    fiber_context_t* context_;

  public:
    attr_accessor(starter, Object);
    attr_accessor(value, Object);
    attr_accessor(prev, Fiber);

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
      return context_;
    }

    VM* state() {
      return state_;
    }

    void* stack() {
      return stack_;
    }

    int stack_size() {
      return stack_size_;
    }

  public:
    static void init(STATE);

    // Ruby.primitive :fiber_new
    static Fiber* create(STATE, Integer* stack_size, Object* callable);
    static void start_on_stack();

    // Ruby.primitive :fiber_s_current
    static Fiber* current(STATE);

    // Ruby.primitive :fiber_resume
    Object* resume(STATE, Arguments& args, CallFrame* calling_environment);

    // Ruby.primitive :fiber_s_yield
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
