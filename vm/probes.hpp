#ifndef RBX_PROBES_HPP
#define RBX_PROBES_HPP

#include "message.hpp"

namespace rubinius {
  class Task;
  class Module;
  class CompiledMethod;

  class TaskProbe {
  public:
    bool verbose;

    TaskProbe();

    void start_method(Task* task, Message& msg);
    void added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth);
    void lookup_failed(Task* task, Message& msg);
  };
}
#endif
