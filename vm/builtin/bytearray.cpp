/* The simple ByteArray class, used to implement String. */

#include "vm.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

#include "object_utils.hpp"

namespace rubinius {

  void ByteArray::init(STATE) {
    GO(bytearray).set(state->new_class_under("ByteArray", G(rubinius)));
    G(bytearray)->set_object_type(state, ByteArrayType);
    G(bytearray)->name(state, state->symbol("Rubinius::ByteArray"));
  }

  ByteArray* ByteArray::create(STATE, size_t bytes) {
    size_t body = bytes;
    ByteArray* ba = state->om->new_object_bytes<ByteArray>(G(bytearray), body);
    ba->full_size_ = body;
    memset(ba->bytes, 0, bytes);
    return ba;
  }

  ByteArray* ByteArray::create_pinned(STATE, size_t bytes) {
    size_t body = bytes;
    ByteArray* ba = state->om->new_object_bytes_mature<ByteArray>(G(bytearray), body);
    ba->full_size_ = body;
    memset(ba->bytes, 0, bytes);
    assert(ba->pin());

    return ba;
  }

  size_t ByteArray::Info::object_size(const ObjectHeader* obj) {
    const ByteArray *ba = reinterpret_cast<const ByteArray*>(obj);
    assert(ba);

    return ba->full_size_;
  }

  void ByteArray::Info::mark(Object* t, ObjectMark& mark) {
    // @todo implement
  }

  char* ByteArray::to_chars(STATE) {
    native_int sz = this->size(state)->to_native();
    char* str = (char*)(this->bytes);
    char* out = ALLOC_N(char, sz);

    std::memcpy(out, str, sz);

    return out;
  }

  ByteArray* ByteArray::allocate(STATE, Integer* bytes) {
    return ByteArray::create(state, bytes->to_native());
  }

  Integer* ByteArray::size(STATE) {
    return Integer::from(state, size());
  }

  Fixnum* ByteArray::get_byte(STATE, Integer* index) {
    native_int idx = index->to_native();

    if(idx < 0 || (unsigned)idx >= size()) {
      Exception::object_bounds_exceeded_error(state, "index out of bounds");
    }

    return Fixnum::from(this->bytes[idx]);
  }

  Fixnum* ByteArray::set_byte(STATE, Integer* index, Fixnum* value) {
    native_int idx = index->to_native();

    if(idx < 0 || (unsigned)idx >= size()) {
      Exception::object_bounds_exceeded_error(state, "index out of bounds");
    }

    this->bytes[idx] = value->to_native();
    return Fixnum::from(this->bytes[idx]);
  }

  Integer* ByteArray::move_bytes(STATE, Integer* start, Integer* count, Integer* dest) {
    native_int src = start->to_native();
    native_int cnt = count->to_native();
    native_int dst = dest->to_native();

    if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    } else if(dst < 0) {
      Exception::object_bounds_exceeded_error(state, "dest less than zero");
    } else if(cnt < 0) {
      Exception::object_bounds_exceeded_error(state, "count less than zero");
    } else if((unsigned)(dst + cnt) > size()) {
      Exception::object_bounds_exceeded_error(state, "move is beyond end of bytearray");
    } else if((unsigned)(src + cnt) > size()) {
      Exception::object_bounds_exceeded_error(state, "move is more than available bytes");
    }

    std::memmove(this->bytes + dst, this->bytes + src, cnt);

    return count;
  }

  ByteArray* ByteArray::fetch_bytes(STATE, Integer* start, Integer* count) {
    native_int src = start->to_native();
    native_int cnt = count->to_native();

    if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    } else if(cnt < 0) {
      Exception::object_bounds_exceeded_error(state, "count less than zero");
    } else if((unsigned)(src + cnt) > size()) {
      Exception::object_bounds_exceeded_error(state, "fetch is more than available bytes");
    }

    ByteArray* ba = ByteArray::create(state, cnt + 1);
    std::memcpy(ba->bytes, this->bytes + src, cnt);
    ba->bytes[cnt] = 0;

    return ba;
  }

  Fixnum* ByteArray::compare_bytes(STATE, ByteArray* other, Integer* a, Integer* b) {
    native_int slim = a->to_native();
    native_int olim = b->to_native();

    if(slim < 0) {
      Exception::object_bounds_exceeded_error(state,
          "bytes of self to compare is less than zero");
    } else if(olim < 0) {
      Exception::object_bounds_exceeded_error(state,
          "bytes of other to compare is less than zero");
    }

    // clamp limits to actual sizes
    size_t m = size() < (unsigned)slim ? size() : slim;
    size_t n = other->size() < (unsigned)olim ? other->size() : olim;

    // only compare the shortest string
    native_int len = m < n ? m : n;

    native_int cmp = std::memcmp(this->bytes, other->bytes, len);

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
    size_t len = pattern->size();
    size_t max = (size_t)max_o->to_native();

    if(len == 0) return start;

    if(max == 0 || max > size()) max = size();

    max -= (len - 1);

    for(size_t i = (size_t)start->to_native(); i < max; i++) {
      if(this->bytes[i] == pat[0]) {
        size_t j;
        // match the rest of the pattern string
        for(j = 1; j < len; j++) {
          if(this->bytes[i+j] != pat[j]) break;
        }

        // if the full pattern matched, return the index
        // of the end of the pattern in 'this'.
        if(j == len) return Integer::from(state, i + len);
      }
    }

    return Qnil;
  }
}
