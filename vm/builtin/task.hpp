#ifndef RBX_BUILTIN_TASK_HPP
#define RBX_BUILTIN_TASK_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "vmmethod.hpp" // opcode
#include "jit_state.h" // jit_state

namespace rubinius {

  class Probe;
  class MethodContext;
  class Channel;
  class Exception;
  class TaskProbe;
  class Tuple;

  class ArgumentError : public VMException {
  public:
    size_t expected;
    size_t given;

    ArgumentError(size_t e, size_t g) : expected(e), given(g) { }
  };

  class Task;

  class Task : public Object {
  public:
    const static size_t fields = 0;
    const static object_type type = TaskType;

    OBJECT self; // slot
    Tuple* literals; // slot
    MethodContext* active; // slot
    MethodContext* home; // slot

    /* globals */
    Exception* exception; // slot
    int call_flags;
    opcode blockargs;

    /* Internal data */
    STATE;
    Message* msg;
    TaskProbe *probe;
    struct jit_state js;

    /* Data */
    Channel* debug_channel; // slot
    Channel* control_channel; // slot

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

    MethodContext* generate_context(OBJECT recv, CompiledMethod* meth);
    void restore_context(MethodContext* ctx);
    void make_active(MethodContext* ctx);
    void execute();
    void execute_interp();
    void import_arguments(MethodContext* ctx, Message& msg);
    bool passed_arg_p(size_t pos);

    void methctx_reference(MethodContext* ctx);
    void set_ip(int ip);
    int  current_ip();
    void cache_ip();

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
    void raise_exception(Exception *exc);
    void activate_method(Message& msg);
    bool send_message(Message& msg);
    bool send_message_slowly(Message& msg);
    Module* current_module();

    Executable* locate_method_on(OBJECT obj, SYMBOL sel, OBJECT priv);
    void simple_return(OBJECT val);
    void primitive_return(OBJECT val, Message& msg);
    void yield_debugger();
    bool check_serial(OBJECT obj, SYMBOL sel, int ser);
    void check_interrupts();

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
    void tragic_failure(Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

    class Halt : public std::runtime_error {
    public:
      Halt(const char* str) : std::runtime_error(str) { }
    };
  };
}

#endif
