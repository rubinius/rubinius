#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

namespace rubinius {

  class MemoryPointer;

  class Executable : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = ExecutableType;

    OBJECT __ivars__; // slot
    SYMBOL primitive; // slot
    FIXNUM serial; // slot
    MemoryPointer* compiled; // slot readonly

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

}


#endif
