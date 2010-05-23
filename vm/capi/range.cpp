#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_range_new(VALUE begin, VALUE end, int exclude_end) {
    VALUE exclude_end_handle = exclude_end ? Qtrue : Qfalse;
    return rb_funcall(rb_cRange, rb_intern("new"), 3, begin, end, exclude_end_handle);
  }

  VALUE rb_range_beg_len(VALUE range, long* begp, long* lenp, long len, int err) {
    long beg, end, b, e;

    if(!rb_obj_is_kind_of(range, rb_cRange)) return Qfalse;

    beg = b = NUM2LONG(rb_funcall(range, rb_intern("begin"), 0));
    end = e = NUM2LONG(rb_funcall(range, rb_intern("end"), 0));

    if(beg < 0) {
        beg += len;
        if(beg < 0) goto out_of_range;
    }
    if(err == 0 || err == 2) {
        if(beg > len) goto out_of_range;
        if(end > len) end = len;
    }
    if(end < 0) end += len;
    if(!RTEST(rb_funcall(range, rb_intern("exclude_end?"), 0))) end++;    /* include end point */
    len = end - beg;
    if(len < 0) len = 0;

    *begp = beg;
    *lenp = len;
    return Qtrue;

  out_of_range:
    if(err) {
        rb_raise(rb_eRangeError, "%ld..%ld out of range",
                 b, e);
    }
    return Qnil;
  }
}
