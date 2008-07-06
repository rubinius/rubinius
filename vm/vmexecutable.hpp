#ifndef RBX_VMEXECUTABLE_HPP
#define RBX_VMEXECUTABLE_HPP

namespace rubinius {
  class VMExecutable {
  public:

    virtual ~VMExecutable() { }
    virtual bool execute(STATE, Task* task, Message& msg) = 0;
  };
}

#endif
