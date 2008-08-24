#ifndef RBX_VMNATIVEFUNCTION_VARIABLE_HPP
#define RBX_VMNATIVEFUNCTION_VARIABLE_HPP

#include "vm/vmexecutable.hpp"

namespace rubinius {
  class VMNativeFunction : public VMExecutable {
  public:
    VMNativeFunction();

    static VMNativeFunction* create(STATE);
    static bool executor(STATE, VMExecutable* meth, Task* task, Message& msg);
  };
}

#endif
