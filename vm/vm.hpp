#ifndef RBX_VM_H
#define RBX_VM_H

#include "prelude.hpp"
#include "globals.hpp"
#include "symboltable.hpp"

namespace llvm {
  class Module;
}

namespace rubinius {

  class Exception;

  namespace event {
    class Loop;
  }

  class GlobalCache;
  class TaskProbe;
  class Primitives;
  class ObjectMemory;
  class TypeInfo;
  class Task;
  class MethodContext;
  class ContextCache;
  class String;
  class ConfigParser;

  struct Configuration {
    bool compile_up_front;
  };

  class VM {
    public:
    /* Data members */
    Globals globals;
    ObjectMemory* om;
    event::Loop* events;
    GlobalCache* global_cache;
    TaskProbe* probe;
    Primitives* primitives;
    Configuration config;
    SymbolTable symbols;
    ConfigParser *user_config;

    /* Used to implement a simple context cache */
    ContextCache* context_cache;

    bool wait_events;

    static const size_t default_bytes = 1048576;

    /* Inline methods */
    /* Prototypes */
    VM(size_t bytes = default_bytes);
    ~VM();

    void bootstrap_ontology();
    void bootstrap_symbol();
    void bootstrap_exceptions();
    void boot_threads();

    OBJECT new_object(Class* cls);
    Class* new_basic_class(OBJECT sup, size_t fields);
    Class* new_class(OBJECT sup, size_t fields);
    Class* new_class(const char* name);
    Class* new_class(const char* name, size_t fields);
    Class* new_class(const char* name, OBJECT sup, size_t fields);
    Class* new_class(const char* name, OBJECT sup, size_t fields, Module* under);
    Module* new_module(const char* name, Module* under = NULL);
    OBJECT new_struct(Class* cls, size_t bytes);
    Task* new_task();

    SYMBOL VM::symbol(const char* str);
    SYMBOL VM::symbol(String* str);
    SYMBOL VM::symbol(std::string str);

    void add_type_info(TypeInfo* ti);
    TypeInfo* find_type(int type);

    void init_ffi();
    Thread* current_thread();
    void collect();

    void return_value(OBJECT val);
    void run_best_thread();
    void queue_thread(Thread* thread);
    void activate_thread(Thread* thread);
    void raise_from_errno(const char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    OBJECT current_block();

    void inspect(OBJECT obj);
    void set_const(const char* name, OBJECT val);
    void set_const(Module* mod, const char* name, OBJECT val);

    llvm::Module* llvm_module();
    void llvm_cleanup();

    void print_backtrace();

  };
};

#endif
