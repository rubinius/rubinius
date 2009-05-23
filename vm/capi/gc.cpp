#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_gc_start() {
    VALUE gc_class_handle = rb_const_get(rb_cObject, rb_intern("GC"));
    rb_funcall(gc_class_handle, rb_intern("start"), 0);
    return Qnil;
  }

  void rb_gc_mark(VALUE ptr) {
    Handle* handle = Handle::from(ptr);
    if(handle->object()->reference_p()) {
      Object* res = VM::current_state()->current_mark.call(handle->object());
      if(res) {
        handle->set_object(res);
      }
    }
  }
}
