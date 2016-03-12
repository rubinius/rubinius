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

  private:
    Object* starter_; // slot
    Array* value_; // slot
    Fiber* prev_; // slot
    Exception* exception_; // slot
    LookupTable* locals_; // slot
    Object* dead_; // slot
    Status status_;

    bool root_;

    FiberData* data_;

  public:
    attr_accessor(starter, Object);
    attr_accessor(value, Array);
    attr_accessor(prev, Fiber);
    attr_accessor(exception, Exception);
    attr_accessor(locals, LookupTable);
    attr_accessor(dead, Object);

    bool root_p() const {
      return root_;
    }

    CallFrame* call_frame() const {
      if(!data_) return 0;
      return data_->call_frame();
    }

    void set_call_frame(CallFrame* call_frame) {
      if(data_) {
        data_->set_call_frame(call_frame);
      }
    }

    void sleep(CallFrame* call_frame) {
      if(call_frame && !data_) rubinius::bug("bad fiber");
      data_->set_call_frame(call_frame);
      status_ = eSleeping;
    }

    void run(STATE) {
      state->vm()->set_current_fiber(this);
      if(data_) {
        state->vm()->set_call_frame(data_->call_frame());
      }
      status_ = eRunning;
    }

    fiber_context_t* ucontext() const {
      return data_->machine();
    }

    FiberData* data() const {
      return data_;
    }

    void* stack_region() const {
      return data_->stack_address();
    }

    void* stack_end() const {
      return data_->stack_address();
    }

    void* stack_start() const {
      return (void*)((uintptr_t)stack_region() + stack_size());
    }

    int stack_size() const {
      return data_->stack_size();
    }

    memory::VariableRootBuffers& variable_root_buffers() {
      return data_->variable_root_buffers();
    }

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Fiber* obj) {
      obj->starter_ = nil<Object>();
      obj->value_ = nil<Array>();
      obj->prev_ = nil<Fiber>();
      obj->exception_ = nil<Exception>();
      obj->locals_ = nil<LookupTable>();
      obj->dead_ = nil<Object>();
      obj->status_ = eNotStarted;
      obj->root_ = false;
      obj->data_ = NULL;
    }

    // Rubinius.primitive :fiber_new
    static Fiber* create(STATE, Object* self, Object* callable);
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
