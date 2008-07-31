
#include "prelude.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  String* Fixnum::to_s(STATE) {
    std::stringstream sout;
    sout << n2i();
    return String::create(state, sout.str().c_str());
  }
}
