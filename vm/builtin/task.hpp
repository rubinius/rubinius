#ifndef RBX_BUILTIN_TASK_HPP
#define RBX_BUILTIN_TASK_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "vmmethod.hpp" // opcode
#include "jit_state.h" // jit_state

#include "builtin/contexts.hpp"
#include "builtin/compiledmethod.hpp"

namespace rubinius {

  namespace profiler {
    class Profiler;
  }

  class MethodContext;
  class Channel;
  class Exception;
  class TaskProbe;
  class Tuple;
  class LookupTable;
  class LookupTableAssociation;
  class ExceptionPoint;

  class Task;

  /* Task is a place to hang the current information about what is running.
   * There is always exactly one Task per thread, but there may be
   * arbitrarily-many threads.
   */

  class Task : public Object {
  public:
    const static object_type type = TaskType;

  private:
    MethodContext* active_; // slot

    /* globals */
    Exception* exception_;  // slot
    TaskProbe *probe_;      // slot

    /* Data */
    Channel* debug_channel_;   // slot
    Channel* control_channel_; // slot

  public:
    // @todo fix up data members that aren't slots
    /* internal data */
    STATE;
    struct jit_state js;
    Message* msg;
    profiler::Profiler* profiler;
    ExceptionPoint* current_ep;

    /* globals */
    int call_flags;
    opcode blockargs;

  public:
    /* accessors */

    attr_accessor(active, MethodContext);
    attr_accessor(exception, Exception);
    attr_accessor(probe, TaskProbe);
    attr_accessor(debug_channel, Channel);
    attr_accessor(control_channel, Channel);

    /* interface */

    static void init(STATE);
    static Task* create(STATE, Object* recv, CompiledMethod* meth);
    // We actually use the value CompiledMethod::tramp_stack_size by default
    // inside the method though.
    static Task* create(STATE, size_t stack_size = 0);

    // Ruby.primitive :task_current
    static Task* current(STATE);

    // Ruby.primitive? :task_set_current
    static Object* set_current(STATE, Executable* exec, CallFrame* call_frame, Message& msg);

    // Ruby.primitive :task_control_channel
    Channel* get_control_channel(STATE);

    // Ruby.primitive :task_debug_channel
    Channel* get_debug_channel(STATE);

    // Ruby.primitive :task_current_context
    MethodContext* current_context(STATE);

    // Ruby.primitive :task_call_object
    Object* call_object(STATE, Object* recv, Symbol* meth, Array* args);

    // Ruby.primitive? :task_dup
    Object* task_dup(STATE, Executable* exec, CallFrame* call_frame, Message& msg);

    void execute();
    void execute_interp();
    bool passed_arg_p(size_t pos);

    void methctx_reference(MethodContext* ctx);
    void set_ip(int ip);
    int  current_ip();

    LookupTableAssociation* const_get_association(CallFrame* call_frame, Symbol* name, bool* found);
    Object* const_get(Module* under, Symbol* name, bool* found);
    Object* const_get(CallFrame* call_frame, Symbol* name, bool* found);
    void   const_set(Module* under, Symbol* sym, Object* val);
    void   const_set(CallFrame* call_frame, Symbol* sym, Object* val);

    void attach_method(CallFrame* call_frame, Object* obj, Symbol* name, CompiledMethod* meth);
    void add_method(CallFrame* call_frame, Module* obj, Symbol* name, CompiledMethod* meth);
    Class* open_class(CallFrame* call_frame, Object* super, Symbol* name, bool* created);
    Class* open_class(Module* under, Object* super, Symbol* name, bool* created);

    /** Reopen existing or create new Module by name in the current lexical scope. */
    Module* open_module(CallFrame* call_frame, Symbol* name);
    /** Reopen existing or create new Module by name under the given enclosing Module. */
    Module* open_module(Module* under, Symbol* name);

    // Ruby.primitive :task_raise
    Task* raise(STATE, Exception *exc);

    void raise_exception(Exception *exc);
    Object* send_message(CallFrame*, Message& msg);
    Object* send_message_slowly(CallFrame*, Message& msg);
    Module* current_module();

    Tuple* locate_method_on(Object* obj, Symbol* sel, Object* priv);

    void primitive_return(Object* val, Message& msg);
    void yield_debugger();
    bool check_serial(Object* obj, Symbol* sel, int ser);

    Object* execute_stream(CallFrame* call_frame, opcode* stream);
    void push(Object* val);
    Object* pop();
    Object* stack_top();
    Object* stack_at(size_t pos);
    int  calculate_sp();
    Object** current_stack();

    /* Locals manipulation */
    void set_local(int pos, Object* val);
    Object* get_local(int pos);

    void print_stack();
    void print_backtrace(MethodContext* ctx = 0);
    void tragic_failure(Message& msg);

    void enable_profiler();
    LookupTable* disable_profiler();

    // Add +ctx+ to the context chain by setting the active context
    // to +ctx+'s sender and making +ctx+ active.

    void literals(STATE, Tuple* tup) {
      active_->cm()->literals(state, tup);
    }

    const Tuple* literals() const {
      return active_->cm()->literals();
    }

    Tuple* literals() {
      return active_->cm()->literals();
    }

    const MethodContext* home() const {
      return active_->home();
    }

    MethodContext* home() {
      return active_->home();
    }

    const Object* self() const {
      return home()->self();
    }

    Object* self() {
      return home()->self();
    }

    void self(STATE, Object* obj) {
      active_->self(state, obj);
    }

    void make_active(MethodContext* ctx) {
      ctx->sender(state, active_);
      active(state, ctx);
    }

  private:
    // Restore the state of the active contexts sender
    void restore_sender();

    // Set +ctx+ the active context
    void restore_context(MethodContext* ctx);

  public:
    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void mark(Object*, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
    };

    class Halt : public std::runtime_error {
    public:
      Halt(const char* str) : std::runtime_error(str) { }
    };

    class WaitOnEvents {
    };
  };
}

#endif
