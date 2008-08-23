/* The simple ByteArray class, used to implement String. */

#include "bytearray.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {

  void ByteArray::init(STATE) {
    GO(bytearray).set(state->new_class("ByteArray"));
    G(bytearray)->set_object_type(ByteArrayType);
  }

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

  ByteArray* ByteArray::allocate(STATE, INTEGER bytes) {
    return ByteArray::create(state, bytes->to_native());
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

  FIXNUM ByteArray::set_byte(STATE, INTEGER index, FIXNUM value) {
    native_int size = SIZE_OF_BODY(this);
    native_int idx = index->to_native();

    if(idx < 0 || idx >= size) {
      throw PrimitiveFailed();
    }

    this->bytes[idx] = value->to_native();
    return Fixnum::from(this->bytes[idx]);
  }

  INTEGER ByteArray::move_bytes(STATE, INTEGER start, INTEGER count, INTEGER dest) {
    native_int size = SIZE_OF_BODY(this);
    native_int src = start->to_native();
    native_int cnt = count->to_native();
    native_int dst = dest->to_native();

    if(src + cnt > size || dst + cnt > size || src < 0 || dst < 0 || cnt < 0) {
      throw PrimitiveFailed();
    }

    memmove(this->bytes + dst, this->bytes + src, cnt);

    return count;
  }

  ByteArray* ByteArray::fetch_bytes(STATE, INTEGER start, INTEGER count) {
    native_int size = SIZE_OF_BODY(this);
    native_int src = start->to_native();
    native_int cnt = count->to_native();

    if(src + cnt > size || src < 0 || cnt < 0) {
      throw PrimitiveFailed();
    }

    ByteArray* ba = ByteArray::create(state, cnt + 1);
    memcpy(ba->bytes, this->bytes + src, cnt);
    ba->bytes[cnt] = 0;

    return ba;
  }

  FIXNUM ByteArray::compare_bytes(STATE, ByteArray* other, INTEGER a, INTEGER b) {
    native_int size = SIZE_OF_BODY(this);
    native_int osize = SIZE_OF_BODY(other);
    native_int slim = a->to_native();
    native_int olim = b->to_native();

    if(slim < 0 || olim < 0) {
      throw PrimitiveFailed();
    }

    // clamp limits to actual sizes
    native_int m = size < slim ? size : slim;
    native_int n = osize < olim ? osize : olim;

    // only compare the shortest string
    native_int len = m < n ? m : n;

    native_int cmp = memcmp(this->bytes, other->bytes, len);

    // even if substrings are equal, check actual requested limits
    // of comparison e.g. "xyz", "xyzZ"
    if(cmp == 0) {
      if(m < n) {
        return Fixnum::from(-1);
      } else if(m > n) {
        return Fixnum::from(1);
      } else {
        return Fixnum::from(0);
      }
    } else {
      return cmp < 0 ? Fixnum::from(-1) : Fixnum::from(1);
    }
  }

  ByteArray* ByteArray::dup_into(STATE, ByteArray* other) {
    native_int size = SIZE_OF_BODY(this);
    native_int osize = SIZE_OF_BODY(other);

    memcpy(other->bytes, this->bytes, size < osize ? size : osize);

    return other;
  }
}
