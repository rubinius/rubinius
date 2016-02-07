#include "builtin/bignum.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/native_method.hpp"

#include "capi/ruby.h"
#include "object_utils.hpp"

using namespace rubinius;

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

  VALUE rb_int_positive_pow(long x, unsigned long y) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    State* state = env->state();
    Integer* base = Integer::from(state, x);
    Integer* exp  = Integer::from(state, y);
    if(Fixnum* base_fix = try_as<Fixnum>(base)) {
      if(Fixnum* exp_fix = try_as<Fixnum>(exp)) {
        return env->get_handle(base_fix->pow(state, exp_fix));
      } else {
        return env->get_handle(base_fix->pow(state, as<Bignum>(exp)));
      }
    } else {
       if(Fixnum* exp_fix = try_as<Fixnum>(exp)) {
        return env->get_handle(as<Bignum>(base)->pow(state, exp_fix));
      } else {
        return env->get_handle(as<Bignum>(base)->pow(state, as<Bignum>(exp)));
      }
    }
  }
}
