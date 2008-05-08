#ifndef RBX_BUILTIN_TASK_HPP
#define RBX_BUILTIN_TASK_HPP

#include "objects.hpp"
#include "message.hpp"
#include "builtin_channel.hpp"

namespace rubinius {

  class ArgumentError : public VMException {
  public:
    size_t expected;
    size_t given;

    ArgumentError(size_t e, size_t g) : expected(e), given(g) { }
  };

  class Task;

  class Task : public BuiltinType {
  public:
    const static size_t fields = 0;
    const static object_type type = TaskType;

    /* 'registers' */
    size_t ip;
    int    sp;

    OBJECT self;
    Tuple* literals;
    MethodContext* active;

    /* globals */
    Exception* exception;
    int call_flags;
    int blockargs;

    /* Internal data */
    Tuple* stack;
    STATE;

    /* Optimization */
    opcode *ip_ptr;

    /* Data */
    Channel* debug_channel;
    Channel* control_channel;

    static void init(STATE);
    static Task* create(STATE, OBJECT recv, CompiledMethod* meth);
    static Task* create(STATE);

    MethodContext* generate_context(OBJECT recv, CompiledMethod* meth);
    void make_active(MethodContext* ctx);
    void execute();
    void import_arguments(MethodContext* ctx, Message& msg);
    bool passed_arg_p(size_t pos);

    void methctx_reference(MethodContext* ctx);
    void cache_ip();

    OBJECT const_get(Module* under, SYMBOL name, bool* found);
    OBJECT const_get(SYMBOL name, bool* found);
    void   const_set(Module* under, SYMBOL sym, OBJECT val);
    void   const_set(SYMBOL sym, OBJECT val);

    void attach_method(OBJECT obj, SYMBOL name, CompiledMethod* meth);
    void add_method(Module* obj, SYMBOL name, CompiledMethod* meth);
    bool perform_hook(OBJECT obj, SYMBOL name, OBJECT arg);
    Class* open_class(OBJECT super, SYMBOL name, bool* created);
    Class* open_class(Module* under, OBJECT super, SYMBOL name, bool* created);
    Module* open_module(Module* under, SYMBOL name);
    Module* open_module(SYMBOL name);

    void raise_exception(Exception *exc);
    void activate_method(Message& msg);
    void send_message(Message& msg);
    void send_message_slowly(Message& msg);
    Module* current_module();

    Executable* locate_method_on(OBJECT obj, SYMBOL sel, OBJECT priv);
    void simple_return(OBJECT val);
    void yield_debugger();
    bool check_serial(OBJECT obj, SYMBOL sel, int ser);
    void check_interrupts();

    void execute_stream(opcode* stream);
    void push(OBJECT val);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Task* obj);
    };
  };
}

#endif
