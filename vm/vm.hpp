#ifndef RBX_VM_H
#define RBX_VM_H

#include "prelude.hpp"
#include "globals.hpp"

namespace rubinius {

  namespace event {
    class Loop;
  }

  class GlobalCache;

  class VM {
    public:
    /* Data members */
    Globals globals;
    ObjectMemory* om;
    event::Loop* events;
    GlobalCache* global_cache;

    static const size_t default_bytes = 10240;

    /* Inline methods */
    /* Prototypes */
    VM(size_t bytes = default_bytes);
    ~VM();

    void bootstrap_ontology();
    void bootstrap_symbol();
    void bootstrap_exceptions();

    OBJECT new_object(Class* cls);
    Class* new_basic_class(OBJECT sup, size_t fields);
    Class* new_class(OBJECT sup, size_t fields);
    Class* new_class(char* name);
    Class* new_class(char* name, size_t fields);
    Class* new_class(char* name, OBJECT sup, size_t fields);
    Class* new_class(char* name, OBJECT sup, size_t fields, Module* under);
    Module* new_module(char* name, Module* under = NULL);
    SYMBOL symbol(const char *str, size_t len = 0);
    OBJECT new_struct(Class* cls, size_t bytes);

    void add_type_info(TypeInfo* ti);

    void init_ffi();
    OBJECT current_thread();
    void collect();

  };
};

#endif
