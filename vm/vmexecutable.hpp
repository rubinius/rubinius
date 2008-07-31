#ifndef RBX_VMEXECUTABLE_HPP
#define RBX_VMEXECUTABLE_HPP

#include "prelude.hpp"
#include "message.hpp"

namespace rubinius {

  class VMExecutable;

  typedef bool (*executor)(STATE, VMExecutable*, Task*, Message& msg);

  class VMExecutable {
  public:

    executor execute;

    virtual ~VMExecutable() { }
  };
}

#endif
