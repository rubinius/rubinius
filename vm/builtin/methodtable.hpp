#ifndef RBX_BUILTIN_METHODTABLE_HPP
#define RBX_BUILTIN_METHODTABLE_HPP

#include "builtin/lookuptable.hpp"

namespace rubinius {
  class MethodTable : public LookupTable {
    public:
    const static object_type type = MethodTableType;

    static MethodTable* create(STATE);

    // Ruby.primitive :methodtable_allocate
    static MethodTable* allocate(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
