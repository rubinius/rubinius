#ifndef RBX_BUILTIN_METHODTABLE_HPP
#define RBX_BUILTIN_METHODTABLE_HPP

namespace rubinius {
  class MethodTable : public LookupTable {
    public:
    const static object_type type = MTType;
    static MethodTable* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
