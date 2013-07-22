#include "builtin/system.hpp"

namespace rubinius {
  Object* System::vm_ruby18_p(STATE) {
    return cTrue;
  }

  Object* System::vm_ruby21_p(STATE) {
    return cFalse;
  }
}
