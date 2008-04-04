#ifndef RBX_VM_H
#define RBX_VM_H

#include "object.hpp"
#include "globals.hpp"

namespace rubinius {

  class ObjectMemory;

  class VM {
    public:
    /* Data members */
    Globals globals;
    ObjectMemory *om;

    /* Prototypes */
    VM(size_t bytes);
    ~VM();

    void bootstrap_ontology();
    void bootstrap_symbol();
    void bootstrap_exceptions();

    Class* new_basic_class(OBJECT sup, size_t fields);
    Class* new_class(OBJECT sup, size_t fields);
    Class* new_class(char* name, OBJECT sup, size_t fields);
    Class* new_class(char* name, OBJECT sup, size_t fields, Module* under);
    Module* new_module(char* name, Module* under = NULL);
    SYMBOL symbol(const char *str, size_t len = 0);
    OBJECT new_struct(Class* cls, size_t bytes);

    void add_cleanup(Class* cls, void (*func)(STATE, OBJECT obj));
  };
};

#define STATE rubinius::VM *state

#endif
