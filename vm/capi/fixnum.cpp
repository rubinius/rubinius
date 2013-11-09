#include "capi/ruby.h"

extern "C" {
  unsigned long rb_fix2uint(VALUE obj) {
    unsigned long num = rb_num2ulong(obj);

    if((long)num < 0) {
      rb_raise(rb_eRangeError, "integer too small to convert into unsigned int");
    }

    if((unsigned int)num != num) {
      rb_raise(rb_eRangeError, "integer too big to convert into unsigned int");
    }

    return num;
  }

  long rb_fix2int(VALUE obj) {
    long num = rb_num2long(obj);

    if((int)num != num) {
      rb_raise(rb_eRangeError, "integer too big to convert into unsigned int");
    }

    return num;
  }
}
