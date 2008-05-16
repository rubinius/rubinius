#ifndef RBX_PROBES_HPP
#define RBX_PROBES_HPP

namespace rubinius {
  class TaskProbe {
  public:
    void start_method(Task* task, Message& msg);
    void added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth);
    void lookup_failed(Task* task, Message& msg);
  };
}
#endif
