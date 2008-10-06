#include "builtin/io.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/bytearray.hpp"
#include "primitives.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

#include <fcntl.h>
#include <iostream>

namespace rubinius {
  void IO::init(STATE) {
    GO(io).set(state->new_class("IO", G(object), IO::fields));
    G(io)->set_object_type(state, IOType);

    GO(iobuffer).set(state->new_class("Buffer", G(object), IOBuffer::fields, G(io)));
    G(iobuffer)->set_object_type(state, IOBufferType);
  }

  IOBuffer* IOBuffer::create(STATE, size_t bytes) {
    IOBuffer* buf = (IOBuffer*)state->new_object(G(iobuffer));
    buf->storage(state, ByteArray::create(state, bytes));
    buf->channel(state, Channel::create(state));
    buf->total(state, Fixnum::from(bytes));
    buf->used(state, Fixnum::from(0));
    buf->start(state, Fixnum::from(0));
    buf->eof(state, Qfalse);

    return buf;
  }

  IOBuffer* IOBuffer::allocate(STATE) {
    return create(state);
  }

  IO* IO::create(STATE, int fd) {
    IO* io = (IO*)state->new_object(G(io));
    io->descriptor(state, Fixnum::from(fd));
    return io;
  }

  FIXNUM IO::open(STATE, String* path, FIXNUM mode, FIXNUM perm) {
    int fd = ::open(path->c_str(), mode->to_native(), perm->to_native());
    return Fixnum::from(fd);
  }

  INTEGER IO::seek(STATE, INTEGER amount, FIXNUM whence) {
    int fd = descriptor_->to_native();
    off_t position;

    if(fd == -1) {
      Exception::io_error(state, "closed stream");
    }

    position = lseek(fd, amount->to_long_long(), whence->to_native());

    if(position == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, position);
  }

  OBJECT IO::close(STATE) {
    int fd = descriptor_->to_native();
    if(fd == -1) {
      Exception::io_error(state, "closed stream");
    } else if(::close(fd)) {
      Exception::errno_error(state);
    } else {
      // HACK todo clear any events for this IO
      descriptor(state, Fixnum::from(-1));
    }
    return Qnil;
  }

  void IO::initialize(STATE, int fd, char* mode) {
    this->descriptor(state, Fixnum::from(fd));
    this->mode(state, String::create(state, mode));
  }

  native_int IO::to_fd() {
    return descriptor_->to_native();
  }

  OBJECT IO::write(STATE, String* buf) {
    ssize_t cnt = ::write(this->to_fd(), buf->data()->bytes, buf->size());

    if(cnt == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, cnt);
  }

  OBJECT IO::blocking_read(STATE, FIXNUM bytes) {
    String* str = String::create(state, bytes);

    ssize_t cnt = ::read(this->to_fd(), str->data()->bytes, bytes->to_native());
    if(cnt == -1) {
      Exception::errno_error(state);
    } else if(cnt == 0) {
      return Qnil;
    }

    str->num_bytes(state, Fixnum::from(cnt));

    return str;
  }

  OBJECT IO::query(STATE, SYMBOL op) {
    native_int fd = this->to_fd();

    if(fd < 0) {
      Exception::io_error(state, "closed stream");
    }

    if(op == state->symbol("tty?")) {
      return isatty(fd) ? Qtrue : Qfalse;
    } else if(op == state->symbol("ttyname")) {
      return String::create(state, ttyname(fd));
    } else {
      return Qnil;
    }
  }

  void IOBuffer::read_bytes(STATE, size_t bytes) {
    used(state, Fixnum::from(used_->to_native() + bytes));
  }

  char* IOBuffer::byte_address() {
    return (char*)storage_->bytes;
  }

  size_t IOBuffer::left() {
    return total_->to_native() - used_->to_native();
  }

  char* IOBuffer::at_unused() {
    char* start = (char*)storage_->bytes;
    start += used_->to_native();
    return start;
  }
};
