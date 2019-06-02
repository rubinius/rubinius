#include "memory.hpp"

#include "memory/thca.hpp"

namespace rubinius {
  namespace memory {
    Object* OpenTHCA::allocate(STATE, native_int bytes, object_type type) {
      return state->memory()->new_object(state, bytes, type);
    }

    Object* ClosedTHCA::allocate(STATE, native_int bytes, object_type type) {
      return state->memory()->new_object(state, bytes, type);
    }

    Object* IsolatedTHCA::allocate(STATE, native_int bytes, object_type type) {
      return state->memory()->new_object(state, bytes, type);
    }
  }
}
