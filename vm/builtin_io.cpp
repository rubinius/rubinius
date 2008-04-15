#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void IO::init(STATE) {
    G(iobuffer) = state->new_class("Buffer", G(object), IO::Buffer::fields, G(io));
  }

  IO::Buffer* IO::Buffer::create(STATE, size_t bytes) {
    IO::Buffer* buf = (IO::Buffer*)state->new_object(G(iobuffer));
    SET(buf, storage, ByteArray::create(state, bytes));
    SET(buf, total, Object::i2n(bytes));
    SET(buf, used, Object::i2n(0));

    return buf;
  }
};
