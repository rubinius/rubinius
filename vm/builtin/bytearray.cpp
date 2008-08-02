/* The simple ByteArray class, used to implement String. */

#include "bytearray.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  ByteArray* ByteArray::create(STATE, size_t bytes) {
    return (ByteArray*)state->om->new_object_bytes(G(bytearray), bytes);
  }

  void ByteArray::Info::mark(OBJECT t, ObjectMark& mark) { }

  INTEGER ByteArray::size(STATE) {
    return Object::i2n(SIZE_OF_BODY(this));
  }
}
