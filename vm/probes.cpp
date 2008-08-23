
#include <iostream>
#include "probes.hpp"
#include "builtin/task.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/symbol.hpp"
#include "vm/message.hpp"
#include "builtin/string.hpp"

namespace rubinius {
  TaskProbe::TaskProbe() {
    verbose = !!getenv("PROBE");
  }

  void TaskProbe::start_method(Task* task, Message& msg) {
    if(!verbose) return;
    std::cout << "[Sending: '" << msg.send_site->name->to_str(task->state)->byte_address() <<
      "']" << std::endl;
  }
  
  void TaskProbe::lookup_failed(Task* task, Message& msg) {
    if(!verbose) return;
    std::cout << "[Unable to find: '" << msg.send_site->name->to_str(task->state)->byte_address() <<
      "']" << std::endl;
  }
    
  void TaskProbe::added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth) {
    if(!verbose) return;
    std::cout << "[Added method '" << name->to_str(task->state)->byte_address() << "']" << std::endl;
  }
}

