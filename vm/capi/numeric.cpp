#include "builtin/bignum.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include <gdtoa.h>

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
    } else if(try_as<Float>(object)) {
      return (long)capi_get_float(env, obj)->val;
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

  long long rb_num2ll(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(Bignum* big = try_as<Bignum>(object)) {
      return big->to_long_long();
    }

    return (long long)rb_num2long(obj);
  }

  unsigned long long rb_num2ull(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(obj);

    if(Bignum* big = try_as<Bignum>(object)) {
      return big->to_ulong_long();
    }

    return (unsigned long long)rb_num2long(obj);
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

  VALUE rb_cstr_to_inum(const char* str, int base, int badcheck) {
    // TODO don't ignore badcheck
    return rb_cstr2inum(str, base);
  }

  VALUE rb_ll2inum(long long val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Integer::from(env->state(), val));
  }

  VALUE rb_ull2inum(unsigned long long val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Integer::from(env->state(), val));
  }

  VALUE rb_num_coerce_bin(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_cNumeric, rb_intern("num_coerce_bin"), 3, x, y, func);
  }

  VALUE rb_num_coerce_cmp(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_cNumeric, rb_intern("num_coerce_cmp"), 3, x, y, func);
  }

  double rb_num2dbl(VALUE val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    if(try_as<Float>(env->get_object(val))) {
      return capi_get_float(env, val)->val;
    }

    // @todo should coerce other types

    return 0.0;
  }

  // Imported from MRI
  double rb_cstr_to_dbl(const char *p, int badcheck) {

    const char *q;
    char *end;
    double d;
    const char *ellipsis = "";
    int w;
#define OutOfRange() (((w = end - p) > 20) ? (w = 20, ellipsis = "...") : (ellipsis = ""))

    if (!p) return 0.0;
    q = p;
    while (ISSPACE(*p)) p++;
    d = ::ruby_strtod(p, &end);
    if (errno == ERANGE) {
      OutOfRange();
      rb_warning("Float %.*s%s out of range", w, p, ellipsis);
      errno = 0;
    }
    if (p == end) {
      if (badcheck) {
        bad:
          rb_invalid_str(q, "Float()");
      }
      return d;
    }
    if (*end) {
      char buf[DBL_DIG * 4 + 10];
      char *n = buf;
      char *e = buf + sizeof(buf) - 1;
      char prev = 0;

      while (p < end && n < e) prev = *n++ = *p++;
      while (*p) {
        if (*p == '_') {
          /* remove underscores between digits */
          if (badcheck) {
            if (n == buf || !ISDIGIT(prev)) goto bad;
            ++p;
            if (!ISDIGIT(*p)) goto bad;
          } else {
            while (*++p == '_');
            continue;
          }
        }
        prev = *p++;
        if (n < e) *n++ = prev;
      }
      *n = '\0';
      p = buf;
      d = ::ruby_strtod(p, &end);
      if (errno == ERANGE) {
        OutOfRange();
        rb_warning("Float %.*s%s out of range", w, p, ellipsis);
        errno = 0;
      }
      if (badcheck) {
        if (!end || p == end) goto bad;
        while (*end && ISSPACE(*end)) end++;
        if (*end) goto bad;
      }
    }
    if (errno == ERANGE) {
      errno = 0;
      OutOfRange();
      rb_raise(rb_eArgError, "Float %.*s%s out of range", w, q, ellipsis);
    }
    return d;
  }
  
  VALUE rb_Integer(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "Integer", "to_i");
  }
}
