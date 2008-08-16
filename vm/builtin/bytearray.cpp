/* The simple ByteArray class, used to implement String. */

#include "bytearray.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  ByteArray* ByteArray::create(STATE, size_t bytes) {
    return (ByteArray*)state->om->new_object_bytes(G(bytearray), bytes);
  }

  void ByteArray::Info::mark(OBJECT t, ObjectMark& mark) { }

  char* ByteArray::to_chars(STATE) {
    native_int sz = this->size(state)->to_native();
    char* str = (char*)(this->bytes);
    char* out = ALLOC_N(char, sz);

    memcpy(out, str, sz);

    return out;
  }

  INTEGER ByteArray::size(STATE) {
    return Fixnum::from(SIZE_OF_BODY(this));
  }

  FIXNUM ByteArray::get_byte(STATE, INTEGER index) {
    native_int size = SIZE_OF_BODY(this);
    native_int idx = index->to_native();

    if(idx < 0 || idx >= size) {
      throw PrimitiveFailed();
    }

    return Fixnum::from(this->bytes[idx]);
  }
}
