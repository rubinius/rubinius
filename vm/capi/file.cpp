#include "builtin/string.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
    RFile* Handle::as_rfile(NativeMethodEnvironment* env) {
      if(type_ != cRFile) {
        RFile* rfile = new RFile;
        rfile->handle = as_value();
        rfile->fptr = as_rio(env);

        type_ = cRFile;
        as_.rfile = rfile;
      }

      return as_.rfile;
    }
  }
}

extern "C" {
  struct RFile* capi_rfile_struct(VALUE file) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return Handle::from(file)->as_rfile(env);
  }

  int rb_cloexec_dup(int fd) {
    return fcntl(fd, F_DUPFD_CLOEXEC, 3);
  }

  VALUE rb_file_open(const char* name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE n = env->get_handle(String::create(env->state(), name));
    VALUE m = env->get_handle(String::create(env->state(), mode));

    return rb_funcall(rb_cFile, rb_intern("open"), 2, n, m);
  }

  FILE *  rb_io_stdio_file(rb_io_t *fptr) {
    return fptr->f;
  }

  VALUE rb_file_path_value(volatile VALUE* obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    STATE = env->state();

    if(!kind_of<String>(env->get_object(*obj))) {
      *obj = rb_funcall(env->get_handle(G(type)), rb_intern("coerce_to_path"), 1, *obj);
    }

    return *obj;
  }

  VALUE rb_file_open_str(VALUE name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE m = env->get_handle(String::create(env->state(), mode));

    FilePathValue(name);
    return rb_funcall(rb_cFile, rb_intern("open"), 2, name, m);
  }
}
