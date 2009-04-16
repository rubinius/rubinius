#include "builtin/bignum.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  long rb_num2long(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to Integer");
    } else if(Fixnum* fix = try_as<Fixnum>(object)) {
      return fix->to_long();
    } else if(Bignum* big = try_as<Bignum>(object)) {
      return big->to_long();
    } else if(Float* flt = try_as<Float>(object)) {
      return (long)flt->val;
    }

    obj = rb_funcall(obj, rb_intern("to_int"), 0);
    return rb_num2long(obj);
  }

  unsigned long rb_num2ulong(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(Bignum* big = try_as<Bignum>(object)) {
      return big->to_ulong();
    }

    return (unsigned long)rb_num2long(obj);
  }

  VALUE INT2NUM(int number) {
    return capi_native2num<int>(number);
  }

  VALUE LONG2NUM(long int number) {
    return capi_native2num<long int>(number);
  }

  VALUE UINT2NUM(unsigned int number) {
    return capi_native2num<unsigned int>(number);
  }

  VALUE ULONG2NUM(unsigned long number) {
    return capi_native2num<unsigned long>(number);
  }

  VALUE rb_cstr2inum(const char* string, int base) {
    return rb_str2inum(rb_str_new2(string), base);
  }

  VALUE rb_ll2inum(long long val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Bignum::from(env->state(), val));
  }

  VALUE rb_ull2inum(unsigned long long val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Bignum::from(env->state(), val));
  }

  VALUE rb_num_coerce_bin(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_cNumeric, rb_intern("num_coerce_bin"), 3, x, y, func);
  }

  VALUE rb_num_coerce_cmp(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_cNumeric, rb_intern("num_coerce_cmp"), 3, x, y, func);
  }
}
