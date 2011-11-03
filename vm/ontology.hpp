#ifndef RBX_VM_ONTOLOGPY_HPP
#define RBX_VM_ONTOLOGPY_HPP

namespace rubinius {
  class Class;
  class Module;

  namespace ontology {
    /// Create an uninitialized Class object
    Class* new_basic_class(STATE, Class* sup);

    /// Create a Class of name +name+ as an Object subclass
    Class* new_class(STATE, const char* name);

    /// Create a Class of name +name+ as a subclass of +super_class+
    Class* new_class(STATE, const char* name, Class* super_class,
                     Module* under = 0);

    /// Create a Class of name +name+ under +under+
    Class* new_class_under(STATE, const char* name, Module* under);

    Module* new_module(STATE, const char* name, Module* under = NULL);
  }
}

#endif
