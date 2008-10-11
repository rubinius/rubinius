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

  IO* IO::create(STATE, int fd) {
    IO* io = (IO*)state->new_object(G(io));
    io->descriptor(state, Fixnum::from(fd));
    io->set_mode(state);
    io->ibuffer(state, IOBuffer::create(state));
    io->eof(state, Qfalse);
    io->lineno(state, Fixnum::from(0));
    return io;
  }

  IO* IO::allocate(STATE, OBJECT self) {
    IO* io = (IO*)state->new_object(G(io));
    io->descriptor(state, (FIXNUM)Qnil);
    io->mode(state, (FIXNUM)Qnil);
    io->ibuffer(state, IOBuffer::create(state));
    io->eof(state, Qfalse);
    io->lineno(state, Fixnum::from(0));

    // Ensure the instance's class is set (i.e. for subclasses of IO)
    io->klass(state, as<Class>(self));
    return io;
  }

  FIXNUM IO::open(STATE, String* path, FIXNUM mode, FIXNUM perm) {
    int fd = ::open(path->c_str(), mode->to_native(), perm->to_native());
    return Fixnum::from(fd);
  }

  OBJECT IO::reopen(STATE, IO* other) {
    native_int cur_fd   = to_fd();
    native_int other_fd = other->to_fd();

    if(dup2(other_fd, cur_fd) == -1) {
      Exception::errno_error(state, "reopen");
    }

    set_mode(state);
    if(IOBuffer* ibuf = try_as<IOBuffer>(ibuffer())) {
      ibuf->reset(state);
    }

    return Qtrue;
  }

  OBJECT IO::ensure_open(STATE) {
    if(descriptor_->nil_p()) {
      Exception::io_error(state, "uninitialized stream");
    } else if(to_fd() == -1) {
      Exception::io_error(state, "closed stream");
    }
    return Qnil;
  }

  OBJECT IO::connect_pipe(STATE, IO* lhs, IO* rhs) {
    int fds[2];
    if(pipe(fds) == -1) {
      Exception::errno_error(state, "creating pipe");
    }

    lhs->descriptor(state, Fixnum::from(fds[0]));
    rhs->descriptor(state, Fixnum::from(fds[1]));

    lhs->set_mode(state);
    rhs->set_mode(state);
    return Qtrue;
  }

  INTEGER IO::seek(STATE, INTEGER amount, FIXNUM whence) {
    ensure_open(state);

    off_t position = lseek(to_fd(), amount->to_long_long(), whence->to_native());

    if(position == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, position);
  }

  OBJECT IO::close(STATE) {
    ensure_open(state);

    if(::close(to_fd())) {
      Exception::errno_error(state);
    } else {
      // HACK todo clear any events for this IO
      descriptor(state, Fixnum::from(-1));
    }
    return Qnil;
  }

  native_int IO::to_fd() {
    return descriptor_->to_native();
  }

  void IO::set_mode(STATE) {
    int acc_mode = fcntl(to_fd(), F_GETFL);
    if(acc_mode < 0) {
      Exception::errno_error(state);
    }
    mode(state, Fixnum::from(acc_mode));
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
    ensure_open(state);

    native_int fd = to_fd();

    if(op == state->symbol("tty?")) {
      return isatty(fd) ? Qtrue : Qfalse;
    } else if(op == state->symbol("ttyname")) {
      return String::create(state, ttyname(fd));
    } else {
      return Qnil;
    }
  }

  /* IOBuffer methods */

  IOBuffer* IOBuffer::create(STATE, size_t bytes) {
    IOBuffer* buf = (IOBuffer*)state->new_object(G(iobuffer));
    buf->storage(state, ByteArray::create(state, bytes));
    buf->channel(state, Channel::create(state));
    buf->total(state, Fixnum::from(bytes));
    buf->reset(state);

    return buf;
  }

  IOBuffer* IOBuffer::allocate(STATE) {
    return create(state);
  }

  void IOBuffer::reset(STATE) {
    used(state, Fixnum::from(0));
    start(state, Fixnum::from(0));
    eof(state, Qfalse);
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
