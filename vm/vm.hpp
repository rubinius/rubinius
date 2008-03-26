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
    Class *new_basic_class(OBJECT sup, size_t fields);
    Class *new_class(OBJECT sup, size_t fields);
  };
};

#define STATE rubinius::VM *state

#endif
