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

  class Task;

  /* Task is a place to hang the current information about what is running.
   * There is always exactly one Task per thread, but there may be
   * arbitrarily-many threads.
   */

  class Task : public Object {
  public:
    const static size_t fields = 0;
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
    // TODO: fix up data members that aren't slots
    /* internal data */
    STATE;
    struct jit_state js;
    Message* msg;
    profiler::Profiler* profiler;

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
    static Task* create(STATE, OBJECT recv, CompiledMethod* meth);
    // We actually use the value CompiledMethod::tramp_stack_size by default
    // inside the method though.
    static Task* create(STATE, size_t stack_size = 0);

    // Ruby.primitive :task_current
    static Task* current(STATE);

    // Ruby.primitive :task_control_channel
    Channel* get_control_channel(STATE);

    // Ruby.primitive :task_debug_channel
    Channel* get_debug_channel(STATE);

    // Ruby.primitive :task_current_context
    MethodContext* current_context(STATE);

    // Ruby.primitive :task_call_object
    OBJECT call_object(STATE, OBJECT recv, SYMBOL meth, Array* args);

    void execute();
    void execute_interp();
    bool passed_arg_p(size_t pos);

    void methctx_reference(MethodContext* ctx);
    void set_ip(int ip);
    int  current_ip();

    OBJECT const_get(Module* under, SYMBOL name, bool* found);
    OBJECT const_get(SYMBOL name, bool* found);
    void   const_set(Module* under, SYMBOL sym, OBJECT val);
    void   const_set(SYMBOL sym, OBJECT val);

    void attach_method(OBJECT obj, SYMBOL name, CompiledMethod* meth);
    void add_method(Module* obj, SYMBOL name, CompiledMethod* meth);
    Class* open_class(OBJECT super, SYMBOL name, bool* created);
    Class* open_class(Module* under, OBJECT super, SYMBOL name, bool* created);
    Module* open_module(Module* under, SYMBOL name);
    Module* open_module(SYMBOL name);

    // Ruby.primitive :task_raise
    Task* raise(STATE, Exception *exc);

    void raise_exception(Exception *exc);
    bool send_message(Message& msg);
    bool send_message_slowly(Message& msg);
    bool execute_message(Message& msg);
    Module* current_module();

    Tuple* locate_method_on(OBJECT obj, SYMBOL sel, OBJECT priv);

    /** Returning from a NativeMethod and restoring sender. */
    void native_return(Object* return_value);

    void primitive_return(OBJECT val, Message& msg);
    void yield_debugger();
    bool check_serial(OBJECT obj, SYMBOL sel, int ser);

    void execute_stream(opcode* stream);
    void push(OBJECT val);
    OBJECT pop();
    OBJECT stack_top();
    OBJECT stack_at(size_t pos);
    int  calculate_sp();
    OBJECT* current_stack();

    /* Locals manipulation */
    void set_local(int pos, OBJECT val);
    OBJECT get_local(int pos);

    void print_stack();
    void print_backtrace(MethodContext* ctx = 0);
    void tragic_failure(Message& msg);

    void enable_profiler();
    void disable_profiler(char* results);

    // Add +ctx+ to the context chain by setting the active context
    // to +ctx+'s sender and making +ctx+ active.
    void make_active(MethodContext* ctx);

    Tuple* const literals() {
      return active_->cm()->literals();
    }

    void literals(STATE, Tuple* tup) {
      active_->cm()->literals(state, tup);
    }

    MethodContext* const home() {
      return active_->home();
    }

    Object* const self() {
      return home()->self();
    }

    void self(STATE, OBJECT obj) {
      active_->self(state, obj);
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
      virtual void show(STATE, OBJECT self, int level);
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
