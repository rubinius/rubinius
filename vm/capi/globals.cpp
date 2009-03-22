#include "builtin/object.hpp"

#include "capi/capi.hpp"

using namespace capi;

extern "C" {
  void rb_free_global(VALUE global_handle) {
    /* Failing silently is OK by MRI. */
    if(global_handle < 0) {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      env->delete_global(global_handle);
    }
  }

  /** @todo   Check logic. This alters the VALUE to be a global handler. --rue */
  void rb_global_variable(VALUE* address) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(*address);

    if(REFERENCE_P(object)) {
      *address = env->get_handle_global(object);
    }
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
