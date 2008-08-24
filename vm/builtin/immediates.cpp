#include "builtin/immediates.hpp"

namespace rubinius {
  void FalseClass::Info::mark(OBJECT t, ObjectMark& mark) { }
  void TrueClass::Info::mark(OBJECT t, ObjectMark& mark) { }
  void NilClass::Info::mark(OBJECT t, ObjectMark& mark) { }
}
