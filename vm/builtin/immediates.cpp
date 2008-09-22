#include "builtin/immediates.hpp"

#include <iostream>

namespace rubinius {

  void FalseClass::Info::mark(OBJECT t, ObjectMark& mark) { }

  void FalseClass::Info::show(STATE, OBJECT self, int level) {
    std::cout << "'false'" << std::endl;
  }

  void TrueClass::Info::mark(OBJECT t, ObjectMark& mark) { }

  void TrueClass::Info::show(STATE, OBJECT self, int level) {
    std::cout << "'true'" << std::endl;
  }

  void NilClass::Info::mark(OBJECT t, ObjectMark& mark) { }

  void NilClass::Info::show(STATE, OBJECT self, int level) {
    std::cout << "nil" << std::endl;
  }

  void NilClass::Info::show_simple(STATE, OBJECT self, int level) {
    show(state, self, level);
  }
}
