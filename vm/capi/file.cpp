#include "builtin/string.hpp"
#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

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

  VALUE rb_file_open(const char* name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE n = env->get_handle(String::create(env->state(), name));
    VALUE m = env->get_handle(String::create(env->state(), mode));

    return rb_funcall(rb_cFile, rb_intern("open"), 2, n, m);
  }
}
