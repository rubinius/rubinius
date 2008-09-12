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

  private:
    SYMBOL primitive_; // slot
    FIXNUM serial_;    // slot

  public:
    // TODO: fix up data members that aren't slots
    VMExecutable* executable;

  public:
    /* accessors */

    attr_accessor(primitive, Symbol);
    attr_accessor(serial, Fixnum);

    /* interface */

    static void init(STATE);

    bool execute(STATE, Task* task, Message& msg) {
      return executable->execute(state, executable, task, msg);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
