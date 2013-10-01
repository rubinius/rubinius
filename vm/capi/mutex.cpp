#include "object_memory.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_mutex_new() {
    return capi_fast_call(rb_cMutex, rb_intern("new"), 0);
  }

  VALUE rb_mutex_locked_p(VALUE self) {
    return capi_fast_call(self, rb_intern("locked?"), 0);
  }

  VALUE rb_mutex_trylock(VALUE self) {
    return capi_fast_call(self, rb_intern("try_lock"), 0);
  }

  VALUE rb_mutex_lock(VALUE self) {
    return capi_fast_call(self, rb_intern("lock"), 0);
  }

  VALUE rb_mutex_unlock(VALUE self) {
    return capi_fast_call(self, rb_intern("unlock"), 0);
  }

  VALUE rb_mutex_sleep(VALUE self, VALUE timeout) {
    return capi_fast_call(self, rb_intern("sleep"), 1, timeout);
  }

  VALUE rb_mutex_synchronize(VALUE self, VALUE (*func)(VALUE farg), VALUE arg) {
    rb_mutex_lock(self);
    return rb_ensure((VALUE (*)(ANYARGS))func, arg, (VALUE (*)(ANYARGS))rb_mutex_unlock, self);
  }
}

