#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_INTEGER_PACK
static VALUE integer_spec_rb_integer_pack(VALUE self, VALUE words, VALUE numwords,
    VALUE wordsize, VALUE nails, VALUE flags)
{
  int result = rb_integer_pack((void*)RSTRING_PTR(words), FIX2INT(numwords),
      FIX2INT(wordsize), FIX2INT(nails), FIX2INT(flags));
  return INT2FIX(result);
}
#endif

void Init_integer_spec() {
#ifdef HAVE_RB_INTEGER_PACK
  rb_define_method(cls, "rb_integer_pack", integer_spec_rb_integer_pack, 5);
#endif
}

#ifdef __cplusplus
extern "C" {
#endif
