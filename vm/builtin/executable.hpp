#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "vmexecutable.hpp"

namespace rubinius {
  class MemoryPointer;

  class Executable : public Object {
    public:
    const static size_t fields = 3;
    const static object_type type = ExecutableType;

    SYMBOL primitive; // slot
    FIXNUM serial; // slot
    VMExecutable* executable;

    bool execute(STATE, Task* task, Message& msg) {
      return executable->execute(state, executable, task, msg);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  template <>
    static bool kind_of<Executable>(OBJECT obj) {
      if(obj->obj_type == Executable::type ||
         obj->obj_type == CMethodType ||
         obj->obj_type == AccessVariableType) {
        return true;
      }

      return false;
    }
}

#endif
