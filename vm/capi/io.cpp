#include "vm.hpp"
#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "object_memory.hpp"
#include "primitives.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "windows_compat.h"

#include <fcntl.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    IO* capi_get_io(NativeMethodEnvironment* env, VALUE io_handle) {
      Handle* handle = Handle::from(io_handle);
      handle->flush(env);
      env->check_tracked_handle(handle, false);
      return c_as<IO>(handle->object());
    }

    static const char* flags_modestr(int oflags)
    {
#ifdef O_BINARY
# define MODE_BINARY(a,b) ((oflags & O_BINARY) ? (b) : (a))
#else
# define MODE_BINARY(a,b) (a)
#endif
      int accmode = oflags & (O_RDONLY|O_WRONLY|O_RDWR);
      if(oflags & O_APPEND) {
        if(accmode == O_WRONLY) {
          return MODE_BINARY("a", "ab");
        }
        if(accmode == O_RDWR) {
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
      IO* io_obj = c_as<IO>(object());

      if(type_ != cRIO) {
        env->shared().capi_ds_lock().lock();

        if(type_ != cRIO) {
          int fd = (int)io_obj->descriptor()->to_native();

          env->shared().capi_ds_lock().unlock();

          if(fd == -1) {
            char buf[RBX_STRERROR_BUFSIZE];
            char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
            rb_raise(rb_eIOError, "%s (%d)", err, errno);
          }

          FILE* f = fdopen(fd, flags_modestr(io_obj->mode()->to_native()));

          if(!f) {
            char buf[RBX_STRERROR_BUFSIZE];
            char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
            std::cerr << "Error convert fd (" << fd << ") to lowlevel IO: "
                      << err << " (" << errno << ")" << std::endl;

            env->shared().capi_ds_lock().unlock();

            rb_raise(rb_eTypeError,
                "unable to convert fd (%d) to lowlevel IO: %s (%d)",
                fd, err, errno);
          }

          RIO* rf = new RIO;
          rf->handle = as_value();
          rf->fd = fd;
          rf->f = f;
          rf->f2 = NULL;
          rf->stdio_file = NULL;
          rf->finalize = NULL;

          // Disable all buffering so that it doesn't get out of sync with
          // the normal IO buffer.
          setvbuf(rf->f, 0, _IONBF, 0);

          type_ = cRIO;
          as_.rio = rf;
        }

        env->shared().capi_ds_lock().unlock();
      }

      return as_.rio;
    }

    bool Handle::rio_close() {
      if(type_ != cRIO) return true;

      RIO* rio = as_.rio;

      if(rio->finalize) rio->finalize(rio, true);

      bool ok = (fclose(rio->f) == 0);
      rio->f = NULL;

      return ok;
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

  VALUE rb_io_addstr(VALUE io, VALUE str) {
    rb_io_write(io, str);
    return io;
  }

  VALUE rb_io_print(int argc, VALUE *argv, VALUE io) {
    return rb_funcall2(io, rb_intern("print"), argc, argv);
  }

  VALUE rb_io_printf(int argc, VALUE *argv, VALUE io) {
    return rb_funcall2(io, rb_intern("printf"), argc, argv);
  }

  VALUE rb_io_puts(int argc, VALUE *argv, VALUE io) {
    return rb_funcall2(io, rb_intern("puts"), argc, argv);
  }

  VALUE rb_io_write(VALUE io, VALUE str) {
    return rb_funcall(io, rb_intern("write"), 1, str);
  }

  VALUE rb_io_close(VALUE io) {
    return rb_funcall(io, rb_intern("close"), 0);
  }

  VALUE rb_io_binmode(VALUE io) {
    return rb_funcall(io, rb_intern("binmode"), 0);
  }

  int rb_io_fd(VALUE io_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    IO* io = c_as<IO>(env->get_object(io_handle));
    return io->descriptor()->to_native();
  }

  long rb_io_fread(char* ptr, int len, FILE* f) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    long ret;

    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);
      ret = fread(ptr, 1, len, f);
    }

    ENTER_CAPI(env->state());

    return ret;
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
      return Qfalse;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET((int_fd_t)fd, &fds);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);

      int ready = 0;
      while(!ready) {
        ready = select(fd+1, &fds, 0, 0, 0);
        if(!retry) break;
      }
    }

    ENTER_CAPI(env->state());

    return Qtrue;
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
      return Qfalse;
    }

    fd_set fds;

    FD_ZERO(&fds);
    FD_SET((int_fd_t)fd, &fds);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);

      int ready = 0;
      while(!ready) {
        ready = select(fd+1, 0, &fds, 0, 0);
        if(!retry) break;
      }
    }
    ENTER_CAPI(env->state());
    return Qtrue;
  }

  /*
   * rb_thread_wait_fd actually waits until a read is
   * available on the given fd
   */
  void rb_thread_wait_fd(int fd) {
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET((int_fd_t)fd, &fds);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);

      int ready = 0;
      while(!ready) {
        ready = select(fd+1, &fds, 0, 0, 0);
      }
    }
    ENTER_CAPI(env->state());
  }

  /*
   * rb_thread_fd_writable waits until the given fd
   * is available for writing
   */
  void rb_thread_fd_writable(int fd) {
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET((int_fd_t)fd, &fds);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);

      int ready = 0;
      while(!ready) {
        ready = select(fd+1, 0, &fds, 0, 0);
      }
    }

    ENTER_CAPI(env->state());
  }

  void rb_io_set_nonblock(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    IO* io = c_as<IO>(env->get_object(io_handle));
    io->set_nonblock(env->state());
  }

  VALUE rb_io_check_io(VALUE io) {
    return rb_check_convert_type(io, T_FILE, "IO", "to_io");
  }

  void rb_io_check_closed(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO* io = c_as<IO>(env->get_object(io_handle));

    if(io->descriptor()->to_native() == -1) {
      rb_raise(rb_eIOError, "closed stream");
    }
  }

  void rb_io_check_readable(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO* io = c_as<IO>(env->get_object(io_handle));
    int io_mode = io->mode()->to_native() & O_ACCMODE;
    if(!(O_RDONLY == io_mode || O_RDWR == io_mode)) {
      rb_raise(rb_eIOError, "not opened for reading");
    }
  }

  void rb_io_check_writable(rb_io_t* iot) {
    VALUE io_handle = iot->handle;
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO* io = c_as<IO>(env->get_object(io_handle));
    int io_mode = io->mode()->to_native() & O_ACCMODE;
    if(!(O_WRONLY == io_mode || O_RDWR == io_mode)) {
      rb_raise(rb_eIOError, "not opened for writing");
    }
  }

  void rb_update_max_fd(int fd) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO::update_max_fd(env->state(), fd);
  }

  void rb_fd_fix_cloexec(int fd) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    IO::new_open_fd(env->state(), fd);
  }
}
