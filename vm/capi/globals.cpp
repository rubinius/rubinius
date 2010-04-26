#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  void rb_free_global(VALUE global_handle) {
    capi::Handle* handle = capi::Handle::from(global_handle);
    if(CAPI_REFERENCE_P(handle) && handle->object()->reference_p()) {
      handle->deref();
    }
  }

  /** @todo This impl is wrong. address itself needs to be registered and
   *        deref'd at GC time to mark the object. */
  void rb_global_variable(VALUE* address) {
    capi::Handle* handle = capi::Handle::from(*address);
    if(CAPI_REFERENCE_P(handle) && handle->object()->reference_p()) {
      handle->ref();
    }
  }

  void rb_gc_register_address(VALUE* address) {
    rb_global_variable(address);
  }

  void rb_gc_unregister_address(VALUE* address) {
    capi::Handle* handle = capi::Handle::from(*address);
    if(CAPI_REFERENCE_P(handle) && handle->object()->reference_p()) {
      handle->deref();
    }
  }

  VALUE rb_gv_get(const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE Globals = rb_const_get(rb_mRubinius, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]"),
                      1,
                      env->get_handle(prefixed_by(env->state(), '$', rb_intern(name))));
  }

  VALUE rb_gv_set(const char* name, VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE Globals = rb_const_get(rb_mRubinius, rb_intern("Globals"));

    return rb_funcall(Globals,
                      rb_intern("[]="),
                      2,
                      env->get_handle(prefixed_by(env->state(), '$', rb_intern(name))),
                      value);
  }

  void rb_define_readonly_variable(const char* name, VALUE* addr) {
    // This is pretty much wrong, because when name is accessed, the VALUE
    // at addr should be retrieved. We're going to just do it once, since almost
    // no one uses this (just SWIG, and sets it before it uses it anyway)
    rb_gv_set(name, *addr);
  }
}
