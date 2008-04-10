#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include "cpu.hpp"

namespace rubinius {

  Message::Message(STATE, Array* ary) {
    this->state = state;
    arguments = ary;
  }

  OBJECT Message::get_argument(size_t index) {
    return arguments->get(state, index);
  }
}
