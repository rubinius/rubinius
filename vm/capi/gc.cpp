#include "builtin/object.hpp"
#include "util/thread.hpp"
#include "objectmemory.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;


static thread::ThreadData<ObjectMark*> _current_mark;

namespace rubinius {
  namespace capi {
    void set_current_mark(ObjectMark* mark) {
      _current_mark.set(mark);
    }

    ObjectMark* current_mark() {
      return _current_mark.get();
    }
  }
}

extern "C" {
  VALUE rb_gc_start() {
    VALUE gc_class_handle = rb_const_get(rb_cObject, rb_intern("GC"));
    rb_funcall(gc_class_handle, rb_intern("start"), 0);
    return Qnil;
  }

  void rb_gc() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    // Normally ignore this. It's almost always a hack.
    if(getenv("RBX_RESPECT_RB_GC")) {
      env->state()->om->collect_young_now = true;
      env->state()->om->collect_mature_now = true;
      env->state()->interrupts.set_perform_gc();
    }
  }

  void rb_gc_mark(VALUE ptr) {
    Handle* handle = Handle::from(ptr);
    if(CAPI_REFERENCE_P(handle) && handle->object()->reference_p()) {
      Object* res = capi::current_mark()->call(handle->object());
      if(res) {
        handle->set_object(res);
      }
    }
  }

  /* In MRI, this function marks an object if it can be determined
   * to be in the heap.
   */
  void rb_gc_mark_maybe(VALUE ptr) {
    rb_gc_mark(ptr);
  }
}
