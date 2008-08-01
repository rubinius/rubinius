#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "object.hpp"
#include "message.hpp"
#include "vmexecutable.hpp"
#include <stdexcept>

namespace rubinius {

  class Primitives;

  class Primitives {
  public:
    static executor resolve_primitive(STATE, SYMBOL name);
    static bool unknown_primitive(STATE, VMExecutable* exec, Task* task, Message& msg);
#include "gen/primitives_declare.hpp"
  };

  class PrimitiveFailed : public VMException {
  };

}

#endif
