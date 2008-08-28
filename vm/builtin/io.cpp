#include "builtin/io.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "primitives.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

#include <fcntl.h>

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

  FIXNUM IO::open(STATE, String* path, FIXNUM mode, FIXNUM perm) {
    int fd = ::open(path->c_str(), mode->to_native(), perm->to_native());
    return Fixnum::from(fd);
  }

  OBJECT IO::close(STATE) {
    int fd = descriptor->to_native();
    if(fd == -1) {
      throw PrimitiveFailed();
    } else if(::close(fd)) {
      throw PrimitiveFailed();
    } else {
      // HACK todo clear any events for this IO
      SET(this, descriptor, Fixnum::from(-1));
    }
    return Qnil;
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

  OBJECT IO::blocking_read(STATE, FIXNUM bytes) {
    String* str = String::allocate(state, bytes);

    ssize_t cnt = ::read(this->to_fd(), str->data->bytes, bytes->to_native());
    if(cnt == -1) {
      throw PrimitiveFailed();
    } else if(cnt == 0) {
      return Qnil;
    }

    SET(str, num_bytes, Fixnum::from(cnt));

    return str;
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
