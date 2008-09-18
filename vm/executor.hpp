#ifndef RBX_EXECUTOR_HPP
#define RBX_EXECUTOR_HPP

namespace rubinius {
  class VM;
  class Executable;
  class Task;
  class Message;

  typedef bool (*executor)(VM*, Executable*, Task*, Message& msg);
}

#endif
