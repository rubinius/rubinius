#include "prelude.hpp"
#include "object.hpp"
#include "virtual.hpp"

namespace rubinius {
  ObjectCallback::ObjectCallback(STATE) : state(state) { }
  ObjectCallback::~ObjectCallback() { }
}
