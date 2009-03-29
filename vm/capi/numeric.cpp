#include "builtin/bignum.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  /** Shares impl. with the other NUM2*, change all if modifying. */
  int NUM2INT(VALUE num_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* number = env->get_object(num_handle);

    if(Fixnum* fix = try_as<Fixnum>(number)) {
      return fix->to_int();
    }
    else if(Bignum* big = try_as<Bignum>(number)) {
      return big->to_int();
    }
    else {
      rb_raise(rb_eArgError, "Argument must be an Integer!");
    }

    /* Compiler Appreciation Project */
    return -1;
  }

  /** Shares impl. with the other NUM2*, change all if modifying. */
  long int NUM2LONG(VALUE num_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* number = env->get_object(num_handle);

    if(Fixnum* fix = try_as<Fixnum>(number)) {
      return fix->to_long();
    }
    else if(Bignum* big = try_as<Bignum>(number)) {
      return big->to_long();
    }
    else {
      rb_raise(rb_eArgError, "Argument must be an Integer!");
    }

    /* Compiler Appreciation Project */
    return -1;
  }

  /** Shares impl. with the other NUM2*, change all if modifying. */
  unsigned int NUM2UINT(VALUE num_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* number = env->get_object(num_handle);

    if(Fixnum* fix = try_as<Fixnum>(number)) {
      return fix->to_uint();
    }
    else if(Bignum* big = try_as<Bignum>(number)) {
      return big->to_uint();
    }
    else {
      rb_raise(rb_eArgError, "Argument must be an Integer!");
    }

    /* Compiler Appreciation Project */
    return -1;
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
}
