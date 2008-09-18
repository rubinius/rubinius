#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "object.hpp"
#include "message.hpp"
#include "executor.hpp"

#include <stdexcept>


namespace rubinius {

  class Primitives;

  class Primitives {
  public:
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

  class PrimitiveFailed : public VMException {
  public:
    static void raise();
  };

}

#endif
