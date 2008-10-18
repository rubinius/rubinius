#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "executor.hpp"
#include "prelude.hpp"

#include <stdexcept>

namespace rubinius {

  /* Forwards */
  class Object;
  class Message;
  class Primitives;
  class Symbol;


  enum PrimitiveRuntimeCode {
    kPrimitiveFailed = 0
  };


  class Primitives {
  public:
    static Object* failure() {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    /*
     * The primitive generator emits one 'executor' function per
     * primitive. This simply checks the argument types and then
     * calls the C++ code that implements the primitive.
     * See VMMethod::execute for the version that handles 'regular'
     * Ruby code.
     */
    static executor resolve_primitive(STATE, Symbol* name);
    static ExecuteStatus unknown_primitive(STATE, Task* task, Message& msg);

#include "gen/primitives_declare.hpp"

  };
}

#endif
