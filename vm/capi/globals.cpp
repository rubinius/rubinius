#include "builtin/system.hpp"
#include "builtin/object.hpp"
#include "builtin/regexp.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "objectmemory.hpp"

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

  VALUE mri_global_rb_rs() {
    return rb_gv_get("$/");
  }

  VALUE mri_global_rb_output_rs() {
    return rb_gv_get("$\\");
  }

  VALUE mri_global_rb_output_fs() {
    return rb_gv_get("$,");
  }

  VALUE mri_global_rb_default_rs() {
    VALUE rs = rb_str_new2("\n");
    OBJ_FREEZE(rs);
    return rs;
  }

  void rb_lastline_set(VALUE obj) {
    rb_thread_local_aset(rb_thread_current(), rb_intern("$_"), obj);
  }

  VALUE rb_lastline_get(void) {
    return rb_thread_local_aref(rb_thread_current(), rb_intern("$_"));
  }

  void rb_free_global(VALUE global_handle) {
    capi::Handle* handle = capi::Handle::from(global_handle);
    if(REFERENCE_P(handle) && handle->object()->reference_p()) {
      handle->deref();
    }
  }

  void capi_gc_register_address(VALUE* address, const char* file, int line) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    capi::Handle** loc = reinterpret_cast<capi::Handle**>(address);
    env->state()->memory()->add_global_capi_handle_location(env->state(), loc, file, line);
  }

  void rb_gc_unregister_address(VALUE* address) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    capi::Handle** loc = reinterpret_cast<capi::Handle**>(address);
    env->state()->memory()->del_global_capi_handle_location(env->state(), loc);
  }

  VALUE rb_gv_get(const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    long len;

    len = strlen(name);
    if((len == 1 && name[0] == '~') ||
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
    rb_gv_set(name, *addr);

    VALUE Globals = rb_const_get(rb_mRubinius, rb_intern("Globals"));
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    rb_funcall(Globals, rb_intern("read_only"), 1, env->get_handle(prefixed_by(env->state(), '$', rb_intern(name))));
  }

  void rb_set_kcode(const char *code) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    System::vm_set_kcode(env->state(), String::create(env->state(), code));
  }
}
