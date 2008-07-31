#include "builtin.hpp"

namespace rubinius {
  void NormalObject::Info::mark(OBJECT t, ObjectMark& mark) { }
  void FalseClass::Info::mark(OBJECT t, ObjectMark& mark) { }
  void TrueClass::Info::mark(OBJECT t, ObjectMark& mark) { }
  void NilClass::Info::mark(OBJECT t, ObjectMark& mark) { }
}
