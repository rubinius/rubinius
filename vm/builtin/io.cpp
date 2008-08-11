#include "builtin/io.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void IO::init(STATE) {
    GO(iobuffer).set(state->new_class("Buffer", G(object), IOBuffer::fields, G(io)));
  }

  IOBuffer* IOBuffer::create(STATE, size_t bytes) {
    IOBuffer* buf = (IOBuffer*)state->new_object(G(iobuffer));
    SET(buf, storage, ByteArray::create(state, bytes));
    SET(buf, total, Fixnum::from(bytes));
    SET(buf, used, Fixnum::from(0));

    return buf;
  }

  IO* IO::create(STATE, int fd) {
    IO* io = (IO*)state->new_object(G(io));
    SET(io, descriptor, Integer::from(state, fd));
    return io;
  }

  void IO::initialize(STATE, int fd, char* mode) {
    SET(this, descriptor, Integer::from(state, fd));
    SET(this, mode, String::create(state, mode));
  }

  native_int IO::to_fd() {
    return descriptor->to_nint();
  }

  void IOBuffer::read_bytes(size_t bytes) {
    used = Fixnum::from(used->n2i() + bytes);
  }

  char* IOBuffer::byte_address() {
    return (char*)storage->bytes;
  }

  size_t IOBuffer::left() {
    return total->n2i() - used->n2i();
  }

  char* IOBuffer::at_unused() {
    char* start = (char*)storage->bytes;
    start += used->n2i();
    return start;
  }
};
