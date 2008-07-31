#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void IO::init(STATE) {
    GO(iobuffer).set(state->new_class("Buffer", G(object), IOBuffer::fields, G(io)));
  }

  IOBuffer* IOBuffer::create(STATE, size_t bytes) {
    IOBuffer* buf = (IOBuffer*)state->new_object(G(iobuffer));
    SET(buf, storage, ByteArray::create(state, bytes));
    SET(buf, total, Object::i2n(bytes));
    SET(buf, used, Object::i2n(0));

    return buf;
  }

  IO* IO::create(STATE, int fd) {
    IO* io = (IO*)state->new_object(G(io));
    SET(io, descriptor, Object::i2n(state, fd));
    return io;
  }

  void IO::initialize(STATE, int fd, char* mode) {
    SET(this, descriptor, Object::i2n(state, fd));
    SET(this, mode, String::create(state, mode));
  }
};
