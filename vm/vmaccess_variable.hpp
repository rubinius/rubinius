#ifndef RBX_VMACCESS_VARIABLE_HPP
#define RBX_VMACCESS_VARIABLE_HPP

#include "vm/vmexecutable.hpp"

namespace rubinius {
  class VMAccessVariable : public VMExecutable {
  public:
    VMAccessVariable();

    static VMAccessVariable* create(STATE);
    static bool executor(STATE, VMExecutable* meth, Task* task, Message& msg);
  };
}

#endif
