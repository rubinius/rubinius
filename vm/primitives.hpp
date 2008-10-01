#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "object.hpp"
#include "message.hpp"
#include "executor.hpp"

#include <stdexcept>

namespace rubinius {

  class Primitives;

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
    static bool unknown_primitive(STATE, Executable* exec, Task* task, Message& msg);
#include "gen/primitives_declare.hpp"
  };

  // Don't make this a subclass of VMException. All VMException's automatically
  // have the C++ backtrace attached to them. Doing so for this very commonly
  // raised exception increases the runtime by 15% in a simple test (and we
  // never use the C++ backtrace of PrimitiveFailed)
  class PrimitiveFailed {
  public:
    static void raise();
  };

}

#endif
