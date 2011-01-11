#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_SCAN_ARGS
VALUE util_spec_rb_scan_args(VALUE self, VALUE _argv, VALUE spec, VALUE additional_ret_args) {
  int i, argc;
  int argv_len = RARRAY_LEN(_argv);
  VALUE argv[4]; // argument array for rb_scan_args
  VALUE args[4]; // return array for rb_scan_args
  VALUE return_array = rb_ary_new();

  VALUE reverse_argv = rb_ary_reverse(rb_ary_dup(_argv));
  for (i = 0; i < argv_len; i++) {
    argv[i] = rb_ary_pop(reverse_argv);
  }

  rb_scan_args(argv_len, argv, RSTRING_PTR(spec), &args[0], &args[1], &args[2], &args[3]);

  argc = argv_len + NUM2INT(additional_ret_args);
  for (i = 0; i < argc; i++) {
    rb_ary_push(return_array, args[i]);
  }
  return return_array;
}
#endif

void Init_util_spec() {
  VALUE cls = rb_define_class("CApiUtilSpecs", rb_cObject);

#ifdef HAVE_RB_SCAN_ARGS
  rb_define_method(cls, "rb_scan_args", util_spec_rb_scan_args, 3);
#endif
}

#ifdef __cplusplus
}
#endif
