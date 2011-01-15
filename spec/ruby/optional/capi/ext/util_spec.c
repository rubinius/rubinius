#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_SCAN_ARGS
VALUE util_spec_rb_scan_args(VALUE self, VALUE argv, VALUE fmt, VALUE expected, VALUE acc) {
  int i, result, argc = RARRAY_LEN(argv);
  VALUE args[4], failed, a1, a2, a3, a4;

  failed = rb_intern("failed");
  a1 = a2 = a2 = a4 = failed;

  for(i = 0; i < argc; i++) {
    args[i] = rb_ary_entry(argv, i);
  }

  result = rb_scan_args(argc, args, RSTRING_PTR(fmt), &a1, &a2, &a3, &a4);

  switch(NUM2INT(expected)) {
  case 4:
    rb_ary_unshift(acc, a4);
  case 3:
    rb_ary_unshift(acc, a3);
  case 2:
    rb_ary_unshift(acc, a2);
  case 1:
    rb_ary_unshift(acc, a1);
    break;
  default:
    rb_raise(rb_eException, "unexpected number of arguments returned by rb_scan_args");
  }

  return INT2NUM(result);
}
#endif

void Init_util_spec() {
  VALUE cls = rb_define_class("CApiUtilSpecs", rb_cObject);

#ifdef HAVE_RB_SCAN_ARGS
  rb_define_method(cls, "rb_scan_args", util_spec_rb_scan_args, 4);
#endif
}

#ifdef __cplusplus
}
#endif
