#include <math.h>
#include <cmath>

#include "builtin/bignum.hpp"
#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_big2str(VALUE self, int base) {
    return rb_funcall(self, rb_intern("to_s"), 1, INT2FIX(base));
  }

  long rb_big2long(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to long");
    } else if(Bignum* big = try_as<Bignum>(object)) {
      if((size_t)mp_count_bits(big->mp_val()) > sizeof(long) * CHAR_BIT)
        rb_raise(rb_eRangeError, "bignum too big to convert into long");

      unsigned long val = big->to_ulong();
      if((long)val < 0 && (big->mp_val()->sign == MP_ZPOS || (long)val != LONG_MIN))
        rb_raise(rb_eRangeError, "bignum too big to convert into long");

      if(big->mp_val()->sign == MP_NEG)
        return -val;
      return val;
    }
    rb_raise(rb_eArgError, "parameter is not a Bignum");

    return 0;
  }

  unsigned long rb_big2ulong(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to unsigned long");
    } else if(Bignum* big = try_as<Bignum>(object)) {
      if((size_t)mp_count_bits(big->mp_val()) > sizeof(long) * CHAR_BIT)
        rb_raise(rb_eRangeError, "bignum too big to convert into long");

      unsigned long val = big->to_ulong();
      if(big->mp_val()->sign == MP_NEG) {
        if((long)val < 0)
          rb_raise(rb_eRangeError, "bignum out of range of unsigned long");
        return -val;
      }
      return val;
    }
    rb_raise(rb_eArgError, "parameter is not a Bignum");

    return 0;
  }

  long long rb_big2ll(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to unsigned long");
    } else if(Bignum* big = try_as<Bignum>(object)) {
      if((size_t)mp_count_bits(big->mp_val()) > sizeof(long long) * CHAR_BIT)
        rb_raise(rb_eRangeError, "bignum too big to convert into long long");

      long long val = big->to_ulong_long();
      if(big->mp_val()->sign == MP_NEG) return -val;
      return val;
    }
    rb_raise(rb_eArgError, "parameter is not a Bignum");

    return 0;
  }

  unsigned long long rb_big2ull(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to unsigned long");
    } else if(Bignum* big = try_as<Bignum>(object)) {
      if((size_t)mp_count_bits(big->mp_val()) > sizeof(unsigned long long) * CHAR_BIT)
        rb_raise(rb_eRangeError, "bignum too big to convert into unsigned long long");

      unsigned long long val = big->to_ulong_long();
      if(big->mp_val()->sign == MP_NEG) return -val;
      return val;
    }

    rb_raise(rb_eArgError, "parameter is not a Bignum");

    return 0;
  }

  double rb_big2dbl(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Bignum* big = c_as<Bignum>(env->get_object(obj));
    double d = big->to_double(env->state());
    if(std::isinf(d)) {
      rb_warn("Bignum out of Float range");
      if(big->mp_val()->sign == MP_NEG) {
        d = -HUGE_VAL;
      } else {
        d = HUGE_VAL;
      }
    }
    return d;
  }

  int rb_big_bytes_used(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Bignum* big = c_as<Bignum>(env->get_object(obj));

    return big->size(env->state())->to_native();
  }
}
