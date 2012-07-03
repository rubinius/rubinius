#include "builtin/object.hpp"
#include "util/thread.hpp"
#include "objectmemory.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;


static utilities::thread::ThreadData<ObjectMark*> _current_mark;

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
      env->state()->vm()->om->collect_young_now = true;
      env->state()->vm()->om->collect_mature_now = true;
      env->state()->shared().gc_soon();
    }
  }

  void rb_gc_force_recycle(VALUE val) {
    // NOTHING. We don't support this and never will.
  }

  void rb_gc_mark(VALUE ptr) {
    Handle* handle = Handle::from(ptr);
    if(REFERENCE_P(handle) && handle->object()->reference_p()) {
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
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Handle* handle = Handle::from(ptr);

    if(capi::Handle::valid_handle_p(env->state(), handle)) {
      rb_gc_mark(ptr);
    }
  }

  void rb_memerror() {
    // MRI raises a NoMemError here, but we're going to just
    // print out and error and exit.
    std::cerr << "[FATAL] Out of memory. Game Over." << std::endl;
    exit(EXIT_FAILURE);
  }
}
