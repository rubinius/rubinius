#ifndef RBX_TYPE_INFO_HPP
#define RBX_TYPE_INFO_HPP

#include "prelude.hpp"
#include "object.hpp"

namespace rubinius {
  class VM;
  class Class;

  /* TypeInfo contains varies operations that are registered by types */
  class TypeInfo {
    public:
    VM* state;
    object_type type;

    TypeInfo(Class* cls);
    virtual ~TypeInfo() { }
    virtual void cleanup(OBJECT obj) { }
    virtual void mark(OBJECT obj) { }
  };
}

#endif
