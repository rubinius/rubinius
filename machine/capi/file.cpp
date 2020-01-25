#include "class/string.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  RFile* MemoryHandle::get_rfile(STATE) {
    if(rfile_p()) {
      return reinterpret_cast<RFile*>(data());
    } else if(object_type_p()) {
      RFile* rfile = new RFile();
      set_rfile(rfile);
      return rfile;
    } else {
      Exception::raise_runtime_error(state, "C-API handle invalid reference as RFile");
    }
  }
}

extern "C" {
  struct RFile* capi_rfile_struct(VALUE file) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return MemoryHandle::from(file)->get_rfile(env->state());
  }

  int rb_cloexec_dup(int fd) {
    return fcntl(fd, F_DUPFD_CLOEXEC, 3);
  }

  VALUE rb_file_open(const char* name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE n = MemoryHandle::value(String::create_pinned(env->state(), name));
    VALUE m = MemoryHandle::value(String::create_pinned(env->state(), mode));

    return rb_funcall(rb_cFile, rb_intern("open"), 2, n, m);
  }

  FILE *  rb_io_stdio_file(rb_io_t *fptr) {
    return fptr->f;
  }

  VALUE rb_file_path_value(volatile VALUE* obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    STATE = env->state();

    if(!MemoryHandle::try_as<String>(*obj)) {
      *obj = rb_funcall(MemoryHandle::value(G(type)), rb_intern("coerce_to_path"), 1, *obj);
    }

    return *obj;
  }

  VALUE rb_file_open_str(VALUE name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE m = MemoryHandle::value(String::create(env->state(), mode));

    FilePathValue(name);
    return rb_funcall(rb_cFile, rb_intern("open"), 2, name, m);
  }
}
