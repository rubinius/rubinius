#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include <fcntl.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    IO* capi_get_io(NativeMethodEnvironment* env, VALUE io_handle) {
      Handle* handle = Handle::from(io_handle);
      handle->flush(env);
      return c_as<IO>(handle->object());
    }

    void flush_cached_rio(NativeMethodEnvironment* env, Handle* handle) {
    }

    static const char* flags_modestr(int oflags)
    {
#ifdef O_BINARY
# define MODE_BINARY(a,b) ((oflags & O_BINARY) ? (b) : (a))
#else
# define MODE_BINARY(a,b) (a)
#endif
      int accmode = oflags & (O_RDONLY|O_WRONLY|O_RDWR);
      if (oflags & O_APPEND) {
        if (accmode == O_WRONLY) {
          return MODE_BINARY("a", "ab");
        }
        if (accmode == O_RDWR) {
          return MODE_BINARY("a+", "ab+");
        }
      }
      switch (oflags & (O_RDONLY|O_WRONLY|O_RDWR)) {
      case O_RDONLY:
        return MODE_BINARY("r", "rb");
      case O_WRONLY:
        return MODE_BINARY("w", "wb");
      case O_RDWR:
        return MODE_BINARY("r+", "rb+");
      }
      return NULL;
    }

    RIO* Handle::as_rio(NativeMethodEnvironment* env) {
      if(type_ != cRIO) {
        IO* io_obj = c_as<IO>(object());

        RIO* f = new RIO;
        f->handle = as_value();
        f->fd = io_obj->descriptor()->to_native();
        f->f = fdopen(f->fd, flags_modestr(io_obj->mode()->to_native()));
        // Disable all buffering so that it doesn't get out of sync with
        // the normal IO buffer.
        setvbuf(f->f, 0, _IONBF, 0);

        type_ = cRIO;
        as_.rio = f;

        flush_ = flush_cached_rio;

        env->state()->shared.global_handles()->move(this,
            env->state()->shared.cached_handles());
      }

      return as_.rio;
    }
  }
}
extern "C" {
  struct RIO* capi_rio_struct(VALUE io_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return Handle::from(io_handle)->as_rio(env);
  }

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

  int rb_io_wait_readable(int fd) {
    bool retry = false;

    switch(errno) {
    case EINTR:
#ifdef ERESTART
    case ERESTART:
#endif
      retry = true;
      break;

    case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
      break;

    default:
      // this is the MRI api. If errno is not on of these, say to the caller
      // "um, i guess nothing more to do?"
      return 0;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ready = 0;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env);

    while(!ready) {
      ready = select(fd+1, &fds, 0, 0, 0);
      if(!retry) break;
    }

    return 1;
  }

  int rb_io_wait_writable(int fd) {
    bool retry = false;

    switch(errno) {
    case EINTR:
#ifdef ERESTART
    case ERESTART:
#endif
      retry = true;
      break;

    case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
      break;

    default:
      // this is the MRI api. If errno is not on of these, say to the caller
      // "um, i guess nothing more to do?"
      return 0;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ready = 0;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env);

    while(!ready) {
      ready = select(fd+1, 0, &fds, 0, 0);
      if(!retry) break;
    }

    return 1;
  }

  void rb_thread_wait_fd(int fd) {
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ready = 0;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env);

    while(!ready) {
      ready = select(fd+1, &fds, &fds, 0, 0);
    }
  }

  void rb_io_set_nonblock(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    IO* io = c_as<IO>(env->get_object(io_handle));
    io->set_nonblock(env->state());
  }

  void rb_io_check_readable(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO* io = c_as<IO>(env->get_object(io_handle));

    if ((io->mode()->to_native() & O_ACCMODE) != O_RDONLY) {
      rb_raise(rb_eIOError, "not opened for reading");
    }
  }

  void rb_io_check_writable(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO* io = c_as<IO>(env->get_object(io_handle));

    if ((io->mode()->to_native() & O_ACCMODE) != O_WRONLY) {
      rb_raise(rb_eIOError, "not opened for writing");
    }
  }
}
