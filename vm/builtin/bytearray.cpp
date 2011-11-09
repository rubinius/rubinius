/* The simple ByteArray class, used to implement String. */

#include <stdint.h>

#include "vm.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

namespace rubinius {

  void ByteArray::init(STATE) {
    GO(bytearray).set(ontology::new_class_under(state,
                        "ByteArray", G(rubinius)));
    G(bytearray)->set_object_type(state, ByteArrayType);
    G(bytearray)->name(state, state->symbol("Rubinius::ByteArray"));
  }

  ByteArray* ByteArray::create(STATE, native_int bytes) {
    assert(bytes >= 0 && bytes < INT32_MAX);

    size_t body = bytes;
    ByteArray* ba = state->vm()->new_object_bytes<ByteArray>(G(bytearray), body);

    if(unlikely(!ba)) {
      Exception::memory_error(state);
    }

    ba->full_size_ = body;
    memset(ba->bytes, 0, bytes);
    return ba;
  }

  ByteArray* ByteArray::create_pinned(STATE, native_int bytes) {
    assert(bytes >= 0 && bytes < INT32_MAX);

    size_t body = bytes;
    ByteArray* ba = state->memory()->new_object_bytes_mature<ByteArray>(state, G(bytearray), body);
    if(unlikely(!ba)) {
      Exception::memory_error(state);
    }

    if(!ba->pin()) {
      rubinius::bug("unable to allocate pinned ByteArray");
    }

    ba->full_size_ = body;
    memset(ba->bytes, 0, bytes);

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

  ByteArray* ByteArray::allocate(STATE, Fixnum* bytes) {
    native_int size = bytes->to_native();
    if(size < 0) {
      Exception::argument_error(state, "negative byte array size");
    } else if (size > INT32_MAX) {
      Exception::argument_error(state, "too large byte array size");
    }
    return ByteArray::create(state, size);
  }

  Fixnum* ByteArray::size(STATE) {
    return Fixnum::from(size());
  }

  Fixnum* ByteArray::get_byte(STATE, Fixnum* index) {
    native_int idx = index->to_native();

    if(idx < 0 || idx >= size()) {
      Exception::object_bounds_exceeded_error(state, "index out of bounds");
    }

    return Fixnum::from(this->bytes[idx]);
  }

  Fixnum* ByteArray::set_byte(STATE, Fixnum* index, Fixnum* value) {
    native_int idx = index->to_native();

    if(idx < 0 || idx >= size()) {
      Exception::object_bounds_exceeded_error(state, "index out of bounds");
    }

    this->bytes[idx] = value->to_native();
    return Fixnum::from(this->bytes[idx]);
  }

  Fixnum* ByteArray::move_bytes(STATE, Fixnum* start, Fixnum* count, Fixnum* dest) {
    native_int src = start->to_native();
    native_int cnt = count->to_native();
    native_int dst = dest->to_native();

    if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    } else if(dst < 0) {
      Exception::object_bounds_exceeded_error(state, "dest less than zero");
    } else if(cnt < 0) {
      Exception::object_bounds_exceeded_error(state, "count less than zero");
    } else if((dst + cnt) > size()) {
      Exception::object_bounds_exceeded_error(state, "move is beyond end of bytearray");
    } else if((src + cnt) > size()) {
      Exception::object_bounds_exceeded_error(state, "move is more than available bytes");
    }

    memmove(this->bytes + dst, this->bytes + src, cnt);

    return count;
  }

  ByteArray* ByteArray::fetch_bytes(STATE, Fixnum* start, Fixnum* count) {
    native_int src = start->to_native();
    native_int cnt = count->to_native();

    if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    } else if(cnt < 0) {
      Exception::object_bounds_exceeded_error(state, "count less than zero");
    } else if((src + cnt) > size()) {
      Exception::object_bounds_exceeded_error(state, "fetch is more than available bytes");
    }

    ByteArray* ba = ByteArray::create(state, cnt + 1);
    memcpy(ba->bytes, this->bytes + src, cnt);
    ba->bytes[cnt] = 0;

    return ba;
  }

  Fixnum* ByteArray::compare_bytes(STATE, ByteArray* other, Fixnum* a, Fixnum* b) {
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
    native_int m = size() < slim ? size() : slim;
    native_int n = other->size() < olim ? other->size() : olim;

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
}
