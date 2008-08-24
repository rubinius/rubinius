#include "builtin/io.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void IO::init(STATE) {
    GO(io).set(state->new_class("IO", G(object), IO::fields));
    G(io)->set_object_type(IOType);

    GO(iobuffer).set(state->new_class("Buffer", G(object), IOBuffer::fields, G(io)));
    G(iobuffer)->set_object_type(IOBufferType);
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
    return descriptor->to_native();
  }

  OBJECT IO::write(STATE, String* buf) {
    ssize_t cnt = ::write(this->to_fd(), buf->data->bytes, buf->size());

    // TODO: RAISE_FROM_ERRNO
    if(cnt == -1) {
      throw std::runtime_error("IO::write primitive failed. (TODO RAISE_FROM_ERRNO)");
    }

    return Integer::from(state, cnt);
  }

  void IOBuffer::read_bytes(size_t bytes) {
    used = Fixnum::from(used->to_native() + bytes);
  }

  char* IOBuffer::byte_address() {
    return (char*)storage->bytes;
  }

  size_t IOBuffer::left() {
    return total->to_native() - used->to_native();
  }

  char* IOBuffer::at_unused() {
    char* start = (char*)storage->bytes;
    start += used->to_native();
    return start;
  }
};
