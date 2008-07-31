#include "builtin/exception.hpp"

#include "vm.hpp"

namespace rubinius {
  Exception* Exception::create(STATE) {
    return (Exception*)state->new_object(G(exception));
  }
}
