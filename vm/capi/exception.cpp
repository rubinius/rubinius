#include "builtin/exception.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;

extern "C" {
  VALUE rb_exc_new(VALUE etype, const char *ptr, long len) {
    return rb_funcall(etype, rb_intern("new"), 1, rb_str_new(ptr, len));
  }

  VALUE rb_exc_new2(VALUE etype, const char *s) {
    return rb_exc_new(etype, s, strlen(s));
  }

  VALUE rb_exc_new3(VALUE etype, VALUE str) {
    StringValue(str);
    return rb_funcall(etype, rb_intern("new"), 1, str);
  }

  void rb_exc_raise(VALUE exc_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Exception *exc = capi::c_as<Exception>(env->get_object(exc_handle));
    capi::capi_raise_backend(exc);

    rubinius::bug("rb_exc_raise failed");
    exit(1);  // compiler snack.
  }

  VALUE rb_errinfo() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(env->state()->thread_state()->current_exception());
  }

  void rb_set_errinfo(VALUE err) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Exception* exc = 0;
    Object* object = env->get_object(err);

    if(object->nil_p()) {
      exc = nil<Exception>();
    } else {
      exc = capi::c_as<Exception>(object);
    }

    env->state()->thread_state()->set_current_exception(exc);
  }
}
