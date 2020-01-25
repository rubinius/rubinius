#include "alloc.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "class/byte_array.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/string.hpp"
#include "class/tuple.hpp"

namespace rubinius {

  uintptr_t ByteArray::bytes_offset;

  void ByteArray::bootstrap(STATE) {
    GO(bytearray).set(state->memory()->new_class<Class, ByteArray>(
          state, G(rubinius), "ByteArray"));

    ByteArray* ba = ALLOCA(ByteArray);
    bytes_offset = (uintptr_t)&(ba->bytes) - (uintptr_t)ba;
  }

  ByteArray* ByteArray::create(STATE, intptr_t bytes) {
    if(bytes < 0) {
      Exception::raise_argument_error(state, "negative byte array size");
    } else if(bytes == 0) {
      bytes = 1;
    }

    ByteArray* ba =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), bytes);
    memset(ba->bytes, 0, ba->full_size() - bytes_offset);

    return ba;
  }

  ByteArray* ByteArray::create_pinned(STATE, intptr_t bytes) {
    if(bytes < 0) {
      Exception::raise_argument_error(state, "negative byte array size");
    } else if(bytes == 0) {
      bytes = 1;
    }

    ByteArray* ba =
      state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), bytes);
    memset(ba->bytes, 0, ba->full_size() - bytes_offset);

    ba->set_pinned(state);

    return ba;
  }

  ByteArray* ByteArray::copy_pinned(STATE, ByteArray* byte_array) {
    size_t bytes = byte_array->full_size() - bytes_offset;

    ByteArray* ba =
      state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), bytes);

    memcpy(ba->bytes, byte_array->bytes, bytes);

    ba->set_pinned(state);

    return ba;
  }

  ByteArray* ByteArray::allocate(STATE, Fixnum* bytes) {
    return ByteArray::create(state, bytes->to_native());
  }

  Fixnum* ByteArray::size(STATE) {
    return Fixnum::from(size());
  }

  Fixnum* ByteArray::get_byte(STATE, Fixnum* index) {
    intptr_t idx = index->to_native();

    if(idx < 0 || idx >= size()) {
      Exception::raise_object_bounds_exceeded_error(state, "index out of bounds");
    }

    return Fixnum::from(this->bytes[idx]);
  }

  Fixnum* ByteArray::set_byte(STATE, Fixnum* index, Fixnum* value) {
    intptr_t idx = index->to_native();

    if(idx < 0 || idx >= size()) {
      Exception::raise_object_bounds_exceeded_error(state, "index out of bounds");
    }

    this->bytes[idx] = value->to_native();
    return Fixnum::from(this->bytes[idx]);
  }

  Fixnum* ByteArray::move_bytes(STATE, Fixnum* start, Fixnum* count, Fixnum* dest) {
    intptr_t src = start->to_native();
    intptr_t cnt = count->to_native();
    intptr_t dst = dest->to_native();

    if(src < 0) {
      Exception::raise_object_bounds_exceeded_error(state, "start less than zero");
    } else if(dst < 0) {
      Exception::raise_object_bounds_exceeded_error(state, "dest less than zero");
    } else if(cnt < 0) {
      Exception::raise_object_bounds_exceeded_error(state, "count less than zero");
    } else if((dst + cnt) > size()) {
      Exception::raise_object_bounds_exceeded_error(state, "move is beyond end of bytearray");
    } else if((src + cnt) > size()) {
      Exception::raise_object_bounds_exceeded_error(state, "move is more than available bytes");
    }

    memmove(this->bytes + dst, this->bytes + src, cnt);

    return count;
  }

  ByteArray* ByteArray::fetch_bytes(STATE, Fixnum* start, Fixnum* count) {
    intptr_t src = start->to_native();
    intptr_t cnt = count->to_native();

    if(src < 0) {
      Exception::raise_object_bounds_exceeded_error(state, "start less than zero");
    } else if(cnt < 0) {
      Exception::raise_object_bounds_exceeded_error(state, "count less than zero");
    } else if((src + cnt) > size()) {
      Exception::raise_object_bounds_exceeded_error(state, "fetch is more than available bytes");
    }

    ByteArray* ba =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), cnt + 1);
    memcpy(ba->bytes, this->bytes + src, cnt);
    ba->bytes[cnt] = 0;

    return ba;
  }

  ByteArray* ByteArray::prepend(STATE, String* str) {
    ByteArray* ba = ByteArray::create(state, size() + str->byte_size());

    memcpy(ba->bytes, str->data()->bytes, str->byte_size());
    memcpy(ba->bytes + str->byte_size(), bytes, size());

    return ba;
  }

  ByteArray* ByteArray::reverse(STATE, Fixnum* o_start, Fixnum* o_total) {
    intptr_t start = o_start->to_native();
    intptr_t total = o_total->to_native();

    if(total <= 0 || start < 0 || start >= size()) return this;

    uint8_t* pos1 = this->bytes + start;
    uint8_t* pos2 = this->bytes + total - 1;
    uint8_t tmp;

    while(pos1 < pos2) {
      tmp = *pos1;
      *pos1++ = *pos2;
      *pos2-- = tmp;
    }

    return this;
  }

  Fixnum* ByteArray::compare_bytes(STATE, ByteArray* other, Fixnum* a, Fixnum* b) {
    intptr_t slim = a->to_native();
    intptr_t olim = b->to_native();

    if(slim < 0) {
      Exception::raise_object_bounds_exceeded_error(state,
          "bytes of self to compare is less than zero");
    } else if(olim < 0) {
      Exception::raise_object_bounds_exceeded_error(state,
          "bytes of other to compare is less than zero");
    }

    // clamp limits to actual sizes
    intptr_t m = size() < slim ? size() : slim;
    intptr_t n = other->size() < olim ? other->size() : olim;

    // only compare the shortest string
    intptr_t len = m < n ? m : n;

    intptr_t cmp = memcmp(this->bytes, other->bytes, len);

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

  Object* ByteArray::locate(STATE, String* pattern, Fixnum* start, Fixnum* max_o) {
    const uint8_t* pat = pattern->byte_address();
    intptr_t len = pattern->byte_size();
    intptr_t max = max_o->to_native();

    if(len == 0) return start;
    if(max == 0) return cNil;

    if(max > size()) max = size();
    max -= (len - 1);

    for(intptr_t i = start->to_native(); i < max; i++) {
      if(this->bytes[i] == pat[0]) {
        intptr_t j;
        // match the rest of the pattern string
        for(j = 1; j < len; j++) {
          if(this->bytes[i+j] != pat[j]) break;
        }

        // if the full pattern matched, return the index
        // of the end of the pattern in 'this'.
        if(j == len) return Fixnum::from(i + len);
      }
    }

    return cNil;
  }

  size_t ByteArray::Info::object_size(const ObjectHeader* obj) {
    const ByteArray *ba = static_cast<const ByteArray*>(obj);
    assert(ba);

    return ba->full_size();
  }
}
