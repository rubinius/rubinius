#ifndef RBX_BUILTIN_MEMORYPOINTER_HPP
#define RBX_BUILTIN_MEMORYPOINTER_HPP

namespace rubinius {
  class MemoryPointer : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = MemPtrType;

    void* pointer;

    static MemoryPointer* create(STATE, void* ptr);

    OBJECT get_field(STATE, int offset, int type);
    void   set_field(STATE, int offset, int type, OBJECT val);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
    };
  };
}

#endif
