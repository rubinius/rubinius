#ifndef RBX_EXECUTOR_HPP
#define RBX_EXECUTOR_HPP

namespace rubinius {
  class State;
  class Executable;
  class Module;
  class Arguments;
  struct CallFrame;
  class Object;

  enum ExecuteStatus {
    cExecuteContinue = 0,
    cExecuteRestart
  };

  typedef Object* (*executor)(State*, Executable* exec, Module* mod, Arguments& args);
  typedef Object* (*InvokePrimitive)(State*, Object**, int);
}

#endif
