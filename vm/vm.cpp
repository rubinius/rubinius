#include "vm.hpp"
#include "objectmemory.hpp"
#include "objects.hpp"

namespace rubinius {
  VM::VM(size_t bytes) {
    om = new ObjectMemory(bytes);
    bootstrap_ontology();
  }

  VM::~VM() {
    delete om;
  }

  Symbol* VM::symbol(const char* str, size_t size) {
    return (Symbol*)globals.symbols->lookup(this, str, size);
  }

  OBJECT VM::new_struct(Class* cls, size_t bytes) {
    size_t fields = bytes / sizeof(OBJECT);
    if(fields % sizeof(OBJECT) != 0) fields++;

    return om->new_object_bytes(cls, fields);
  }

  void VM::add_cleanup(Class*, void (*func)(STATE, OBJECT obj)) {

  }
};
