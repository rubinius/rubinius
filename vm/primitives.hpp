#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "object.hpp"
#include "executor.hpp"

#include <stdexcept>

namespace rubinius {

  class Primitives;
  class Message;

  enum PrimitiveRuntimeCode {
    kPrimitiveFailed = 0
  };

  class Primitives {
  public:
    static OBJECT failure() {
      return reinterpret_cast<OBJECT>(kPrimitiveFailed);
    }

    /*
     * The primitive generator emits one 'executor' function per
     * primitive. This simply checks the argument types and then
     * calls the C++ code that implements the primitive.
     * See VMMethod::execute for the version that handles 'regular'
     * Ruby code.
     */
    static executor resolve_primitive(STATE, SYMBOL name);
    static ExecuteStatus unknown_primitive(STATE, Task* task, Message& msg);
#include "gen/primitives_declare.hpp"
  };
}

#endif
