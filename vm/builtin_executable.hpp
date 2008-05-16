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

    void initialize(STATE);
    void execute(STATE, Task* cur, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}


#endif
