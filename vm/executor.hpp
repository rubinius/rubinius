#ifndef RBX_EXECUTOR_HPP
#define RBX_EXECUTOR_HPP

namespace rubinius {
  class VM;
  class Executable;
  class Module;
  class Arguments;
  struct CallFrame;
  class Object;

  enum ExecuteStatus {
    cExecuteContinue = 0,
    cExecuteRestart
  };

  typedef Object* (*executor)(VM*, CallFrame*, Executable* exec, Module* mod,
                              Arguments& args);

  typedef Object* (*InvokePrimitive)(VM*, CallFrame*, Object**, int);
}

#endif
