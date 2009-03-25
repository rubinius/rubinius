#include "capi/capi.hpp"
#include "capi/ruby.h"

extern "C" {
  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
  }
}
