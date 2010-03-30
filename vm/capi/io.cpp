#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  void rb_eof_error() {
    rb_raise(rb_eEOFError, "end of file reached");
  }

  VALUE rb_io_write(VALUE io, VALUE str) {
    return rb_funcall(io, rb_intern("write"), 1, str);
  }

  int rb_io_fd(VALUE io_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    IO* io = c_as<IO>(env->get_object(io_handle));
    return io->descriptor()->to_native();
  }

  void rb_io_wait_readable(int fd) {
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ready = 0;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock& lock = env->state()->global_lock();
    lock.unlock();

    while(!ready) {
      ready = select(fd+1, &fds, 0, 0, 0);
    }

    lock.lock();
  }

  void rb_io_wait_writable(int fd) {
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ready = 0;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock& lock = env->state()->global_lock();
    lock.unlock();

    while(!ready) {
      ready = select(fd+1, 0, &fds, 0, 0);
    }

    lock.lock();
  }

  void rb_thread_wait_fd(int fd) {
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ready = 0;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock& lock = env->state()->global_lock();
    lock.unlock();

    while(!ready) {
      ready = select(fd+1, &fds, &fds, 0, 0);
    }

    lock.lock();
  }

  void rb_io_set_nonblock(rb_io_t* dummy) {
    VALUE io_handle = (VALUE)dummy;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    IO* io = c_as<IO>(env->get_object(io_handle));
    io->set_nonblock(env->state());
  }
}
