#ifndef RBX_PROBES_HPP
#define RBX_PROBES_HPP

#include "message.hpp"
#include "vmmethod.hpp"

namespace rubinius {
  class Task;
  class Module;
  class CompiledMethod;

  union TaskProbeFlags {
    struct {
      unsigned int StartMethod        : 1;
      unsigned int AddMethod          : 1;
      unsigned int LookupFailed       : 1;
      unsigned int ExecuteInstruction : 1;
    };
    uint32_t all_flags;

    void set_all_flags();
    bool exists(std::string options, const char* option);
    void parse(std::string options);
  };

  class TaskProbe {
  public:
    union TaskProbeFlags flags;

    TaskProbe();

    void start_method(Task* task, Message& msg);
    void added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth);
    void lookup_failed(Task* task, Message& msg);
    void execute_instruction(Task* task, MethodContext* ctx, opcode op);
  };
}
#endif
