#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  void rb_free_global(VALUE global_handle) {
    capi::Handle* handle = capi::Handle::from(global_handle);
    handle->deref();
  }

  /** @todo This impl is wrong. address itself needs to be registered and
   *        deref'd at GC time to mark the object. */
  void rb_global_variable(VALUE* address) {
    capi::Handle* handle = capi::Handle::from(*address);
    handle->ref();
  }

  VALUE rb_gv_get(const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE Globals = rb_const_get(rb_cObject, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]"),
                      1,
                      env->get_handle(prefixed_by("$", name)));
  }

  VALUE rb_gv_set(const char* name, VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE Globals = rb_const_get(rb_cObject, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]="),
                      2,
                      env->get_handle(prefixed_by("$", name)),
                      value);
  }
}
