#ifndef RBX_VM_H
#define RBX_VM_H

#include "prelude.hpp"
#include "globals.hpp"

namespace rubinius {

  class Exception;

  namespace event {
    class Loop;
  }

  class GlobalCache;
  class TaskProbe;
  class Primitives;

  class VM {
    public:
    /* Data members */
    Globals globals;
    ObjectMemory* om;
    event::Loop* events;
    GlobalCache* global_cache;
    TaskProbe* probe;
    Primitives* primitives;

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
    SYMBOL symbol(const char *str, size_t len = 0);
    OBJECT new_struct(Class* cls, size_t bytes);

    void add_type_info(TypeInfo* ti);
    TypeInfo* find_type(int type);

    void init_ffi();
    OBJECT current_thread();
    void collect();

    void return_value(OBJECT val);
    void run_best_thread();
    void queue_thread(Thread* thread);
    void activate_thread(Thread* thread);
    void raise_from_errno(char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    OBJECT current_block();

    void inspect(OBJECT obj);
    void set_const(const char* name, OBJECT val);
    void set_const(Module* mod, const char* name, OBJECT val);

  };
};

#endif
