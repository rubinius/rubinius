#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include "vmexecutable.hpp"

namespace rubinius {

  class MemoryPointer;

  class Executable : public Object {
    public:
    const static size_t fields = 4;
    const static object_type type = ExecutableType;

    OBJECT __ivars__; // slot
    SYMBOL primitive; // slot
    FIXNUM serial; // slot
    VMExecutable* executable;

    bool execute(STATE, Task* task, Message& msg) {
      return executable->execute(state, task, msg);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}


#endif
