#include "class/object.hpp"
#include "util/thread.hpp"
#include "memory.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;


static utilities::thread::ThreadData<memory::ObjectMark*> _current_mark;

namespace rubinius {
  namespace capi {
    void set_current_mark(memory::ObjectMark* mark) {
      _current_mark.set(mark);
    }

    memory::ObjectMark* current_mark() {
      return _current_mark.get();
    }
  }
}

extern "C" {
  VALUE rb_gc_start() {
    rb_gc();
    return Qnil;
  }

  void rb_gc() {
    VALUE gc_class_handle = rb_const_get(rb_cObject, rb_intern("GC"));
    rb_funcall(gc_class_handle, rb_intern("start"), 0);
  }

  void rb_gc_force_recycle(VALUE val) {
    // NOTHING. We don't support this and never will.
  }

  void rb_gc_mark(VALUE ptr) {
    if(REFERENCE_P(ptr)) {
      MemoryHandle* handle = MemoryHandle::from(ptr);
      Object* obj = handle->object();
      if(obj->reference_p()) {
        obj = capi::current_mark()->call(obj);
        if(obj) handle->object(obj);
      }
    }
  }

  void rb_gc_mark_locations(VALUE *start, VALUE *end) {
    VALUE *v = start;
    while (v < end) {
      rb_gc_mark(*v++);
    }
  }

  /* In MRI, this function marks an object if it can be determined
   * to be in the heap.
   */
  void rb_gc_mark_maybe(VALUE ptr) {
    rb_gc_mark(ptr);
  }

  void rb_memerror() {
    // MRI raises a NoMemError here, but we're going to just
    // print out and error and exit.
    std::cerr << "[FATAL] Out of memory. Game Over." << std::endl;
    exit(EXIT_FAILURE);
  }

  VALUE rb_gc_enable() {
    VALUE gc_class_handle = rb_const_get(rb_cObject, rb_intern("GC"));
    return rb_funcall(gc_class_handle, rb_intern("enable"), 0);
  }

  VALUE rb_gc_disable() {
    VALUE gc_class_handle = rb_const_get(rb_cObject, rb_intern("GC"));
    return rb_funcall(gc_class_handle, rb_intern("disable"), 0);
  }
}
