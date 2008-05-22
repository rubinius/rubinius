#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "message.hpp"

#include <stdexcept>

namespace rubinius {

  class Primitives;

  typedef OBJECT (Primitives::*primitive_func)(STATE, Message& msg);
#define CALL_PRIM(obj, ptr) ((obj)->*(ptr))

  class Primitives {
  public:
    static primitive_func resolve_primitive(STATE, SYMBOL name);
#include "gen/primitives_declare.hpp"
  };

  class PrimitiveFailed : public VMException {
  };

}

#endif
