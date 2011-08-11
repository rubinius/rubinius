#include "builtin/object.hpp"
#include "builtin/system.hpp"
#include "builtin/regexp.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  static int mri_ruby_debug;
  static int mri_ruby_verbose;

  int* mri_global_debug() {
    if(rb_gv_get("$DEBUG") == Qtrue) {
      mri_ruby_debug = 1;
    } else {
      mri_ruby_debug = 0;
    }
    return &mri_ruby_debug;
  }

  int* mri_global_verbose() {
    if(rb_gv_get("$VERBOSE") == Qtrue) {
      mri_ruby_verbose = 1;
    } else {
      mri_ruby_verbose = 0;
    }
    return &mri_ruby_verbose;
  }

  void rb_free_global(VALUE global_handle) {
    capi::Handle* handle = capi::Handle::from(global_handle);
    if(CAPI_REFERENCE_P(handle) && handle->object()->reference_p()) {
      handle->deref();
    }
  }

  void rb_global_variable(VALUE* address) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    capi::Handle** loc = reinterpret_cast<capi::Handle**>(address);
    env->state()->shared.add_global_handle_location(loc);
  }

  void rb_gc_register_address(VALUE* address) {
    rb_global_variable(address);
  }

  void rb_gc_unregister_address(VALUE* address) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    capi::Handle** loc = reinterpret_cast<capi::Handle**>(address);
    env->state()->shared.del_global_handle_location(loc);
  }

  VALUE rb_gv_get(const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    long len;

    len = strlen(name);
    if ((len == 1 && name[0] == '~') ||
        (len == 2 && name[0] == '$' && name[1] == '~')) {
      return env->get_handle(Regexp::last_match_result(env->state(),
        Fixnum::from(0), Fixnum::from(0), env->current_call_frame()));
    }
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

  VALUE rb_f_global_variables() {
    return rb_funcall(rb_mKernel, rb_intern("global_variables"), 0);
  }

  void rb_define_readonly_variable(const char* name, VALUE* addr) {
    // This is pretty much wrong, because when name is accessed, the VALUE
    // at addr should be retrieved. We're going to just do it once, since almost
    // no one uses this (just SWIG, and sets it before it uses it anyway)
    rb_gv_set(name, *addr);
  }

  void rb_set_kcode(const char *code) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    System::vm_set_kcode(env->state(), String::create(env->state(), code));
  }
}
