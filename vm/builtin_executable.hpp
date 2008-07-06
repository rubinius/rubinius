#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include "vmexecutable.hpp"

namespace rubinius {

  class MemoryPointer;

  class Executable : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = ExecutableType;

    OBJECT __ivars__; // slot
    SYMBOL primitive; // slot
    FIXNUM serial; // slot
    VMExecutable* executable;

    void initialize(STATE);
    bool execute(STATE, Task* cur, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}


#endif
