#include "vm.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  Object* VM::ruby_lib_version() {
    return Fixnum::from(21);
  }
};
