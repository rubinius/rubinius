#ifndef RBX_EXECUTOR_HPP
#define RBX_EXECUTOR_HPP

namespace rubinius {
  class VM;
  class Task;
  class Message;

  enum ExecuteStatus {
    cExecuteContinue = 0,
    cExecuteRestart
  };

  typedef ExecuteStatus (*executor)(VM*, Task*, Message& msg);
}

#endif
