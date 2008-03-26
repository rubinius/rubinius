#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  VM::VM(size_t bytes) {
    om = new ObjectMemory(bytes);
    bootstrap_ontology();
  }

  VM::~VM() {
    delete om;
  }
};
