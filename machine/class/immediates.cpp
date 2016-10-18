#include "class/immediates.hpp"

#include <iostream>

namespace rubinius {

  void FalseClass::Info::mark(Object* t, memory::ObjectMark& mark) { }

  void FalseClass::Info::show(STATE, Object* self, int level) {
    std::cout << "'false'" << std::endl;
  }

  void TrueClass::Info::mark(Object* t, memory::ObjectMark& mark) { }

  void TrueClass::Info::show(STATE, Object* self, int level) {
    std::cout << "'true'" << std::endl;
  }

  void NilClass::Info::mark(Object* t, memory::ObjectMark& mark) { }

  void NilClass::Info::show(STATE, Object* self, int level) {
    std::cout << "nil" << std::endl;
  }

  void NilClass::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }
}
