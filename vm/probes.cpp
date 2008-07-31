
#include <iostream>
#include "probes.hpp"
#include "builtin/task.hpp"

namespace rubinius {
  void TaskProbe::start_method(Task* task, Message& msg) {
    std::cout << "[Sending: '" << *msg.send_site->name->to_str(task->state) <<
      "']" << std::endl;
  }
  
  void TaskProbe::lookup_failed(Task* task, Message& msg) {
    std::cout << "[Unable to find: '" << *msg.send_site->name->to_str(task->state) <<
      "']" << std::endl;
  }
    
  void TaskProbe::added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth) {
    std::cout << "[Added method '" << *name->to_str(task->state) << "']" << std::endl;
  }
}

