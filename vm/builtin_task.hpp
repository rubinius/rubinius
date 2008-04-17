#ifndef RBX_BUILTIN_TASK_HPP
#define RBX_BUILTIN_TASK_HPP

#include "prelude.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "objects.hpp"
#include "cpu.hpp"

namespace rubinius {
  class Task;

  class Task : public BuiltinType {
  public:
    const static size_t fields = 0;
    const static object_type type = TaskType;

    /* 'registers' */
    size_t ip;
    size_t sp;

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

    static void init(STATE);
    static Task* create(STATE, OBJECT recv, CompiledMethod* meth);

    MethodContext* generate_context(STATE, OBJECT recv, CompiledMethod* meth);
    void execute();
    void methctx_reference(MethodContext* ctx);
    void cache_ip();
    OBJECT const_get_in_context(OBJECT obj);
    OBJECT const_set(OBJECT sym, OBJECT val);
    OBJECT const_set(OBJECT sym, OBJECT val, OBJECT under);
    OBJECT const_get_from(OBJECT sym, OBJECT under);
    OBJECT attach_method(OBJECT obj, OBJECT name, CompiledMethod* meth);
    OBJECT add_method(OBJECT obj, OBJECT name, CompiledMethod* meth);
    OBJECT perform_hook(OBJECT obj, OBJECT name, OBJECT val);
    Class* open_class(OBJECT super, OBJECT name, int* created);
    Class* open_class(OBJECT under, OBJECT super, OBJECT name, int* created);
    Module* open_module(OBJECT name, OBJECT parent);
    Module* open_module(OBJECT name);

    void raise_exception(Exception *exc);
    void activate_method(Message& msg);
    void send_message(Message& msg);
    void send(OBJECT recv, OBJECT name, size_t args, OBJECT block);
    Module* current_module();

    Executable* locate_method_on(OBJECT obj, OBJECT name, OBJECT priv);
    void simple_return(OBJECT val);
    void yield_debugger();
    OBJECT check_serial(OBJECT obj, OBJECT name, int ser);
    void check_interrupts();

    class Info : public TypeInfo {
    public:
      virtual void mark(Task* obj);
    };

  };
}

#endif
