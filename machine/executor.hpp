#ifndef RBX_EXECUTOR_HPP
#define RBX_EXECUTOR_HPP

namespace rubinius {
  class ThreadState;
  class Executable;
  class Module;
  class Arguments;
  struct CallFrame;
  class Object;

  enum ExecuteStatus {
    cExecuteContinue = 0,
    cExecuteRestart
  };

  typedef Object* (*executor)(ThreadState*, Executable* exec, Module* mod, Arguments& args);
  typedef Object* (*InvokePrimitive)(ThreadState*, Object**, int);
}

#endif
