#include "builtin.hpp"

namespace rubinius {
  void Executable::initialize(STATE) {
    if(CompiledMethod* cm = try_as<CompiledMethod>(this)) {
      // Side effect updates compiled as well.
      cm->vmmethod(state);
    } else {
      throw std::runtime_error("Invalid Executable object.\n");
    }
  }

  bool Executable::execute(STATE, Task* task, Message& msg) {
    if(!executable) initialize(state);
    return executable->execute(state, task, msg);
  }
}
