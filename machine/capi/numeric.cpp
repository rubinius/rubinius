#include "class/bignum.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/object.hpp"
#include "class/integer.hpp"
#include "class/string.hpp"

#include "object_utils.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  char rb_num2chr(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = MemoryHandle::object(obj);

    String* str;
    char chr;

    if((str = try_as<String>(object)) && str->byte_size() >= 1) {
      chr = str->c_str(env->state())[0];
    } else {
      chr = (char)(NUM2INT(obj) & 0xff);
    }

    return chr;
  }

  long rb_num2int(VALUE obj) {
    long num = rb_num2long(obj);
    if((int)num != num) {
      rb_raise(rb_eRangeError, "integer too big to convert into int");
    }
    return num;
  }

  long rb_num2long(VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = MemoryHandle::object(value);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to Integer");
    } else if(Fixnum* fix = try_as<Fixnum>(object)) {
      return fix->to_long();
    } else if(try_as<Bignum>(object)) {
      return rb_big2long(value);
    } else if(try_as<Float>(object)) {
      return (long)MemoryHandle::from(value)->get_rfloat(env->state())->value;
    } else if(object->true_p()) {
      rb_raise(rb_eTypeError, "can't convert true to Integer");
    } else if(object->false_p()) {
      rb_raise(rb_eTypeError, "can't convert false to Integer");
    }

    ID to_int_id = rb_intern("to_int");

    if(!rb_respond_to(value, to_int_id)) {
	    rb_raise(rb_eTypeError, "can't convert %s into Integer",
		     rb_obj_classname(value));
    }

    value = rb_funcall(value, to_int_id, 0);

    return rb_num2long(value);
  }

  unsigned long rb_num2uint(VALUE obj) {
    long sign = rb_num2long(obj);
    bool negative = sign < 0;

    unsigned long num = (unsigned long)sign;
    if(negative) {
      if(num < (unsigned long)INT_MIN) {
        rb_raise(rb_eRangeError, "integer %ld too small to convert into int", sign);
      }
    } else {
      if(num > UINT_MAX) {
        rb_raise(rb_eRangeError, "integer %lu too big to convert into int", num);
      }
    }
    return num;
  }

  unsigned long rb_num2ulong(VALUE value) {
    if(MemoryHandle::try_as<Bignum>(value)) {
      return rb_big2ulong(value);
    }

    return (unsigned long)rb_num2long(value);
  }

  long long rb_num2ll(VALUE value) {
    if(Bignum* big = MemoryHandle::try_as<Bignum>(value)) {
      return big->to_long_long();
    }

    return (long long)rb_num2long(value);
  }

  unsigned long long rb_num2ull(VALUE value) {
    if(Bignum* big = MemoryHandle::try_as<Bignum>(value)) {
      return big->to_ulong_long();
    }

    return (unsigned long long)rb_num2long(value);
  }

  VALUE rb_int2big(long number) {
    return capi_native2num<long>(number);
  }

  VALUE rb_uint2big(unsigned long number) {
    return capi_native2num<unsigned long>(number);
  }

  VALUE rb_int2inum(int number) {
    return capi_native2num<int>(number);
  }

  VALUE INT2NUM(int number) {
    return capi_native2num<int>(number);
  }

  VALUE LONG2NUM(long int number) {
    return capi_native2num<long int>(number);
  }

  VALUE rb_uint2inum(unsigned long number) {
    return capi_native2num<unsigned long>(number);
  }

  VALUE UINT2NUM(unsigned long number) {
    return capi_native2num<unsigned long>(number);
  }

  VALUE ULONG2NUM(unsigned long number) {
    return capi_native2num<unsigned long>(number);
  }

  VALUE rb_cstr2inum(const char* str, int base) {
    return rb_cstr_to_inum(str, base, base == 0);
  }

  VALUE rb_cstr_to_inum(const char* str, int base, int badcheck) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Integer* i = Integer::from_cstr(env->state(), str, str + strlen(str),
                                    base, RBOOL(badcheck));
    if(i->nil_p()) {
      rb_raise(rb_eArgError, "invalid string for Integer");
    }
    return MemoryHandle::value(i);
  }

  VALUE rb_ll2inum(long long val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return MemoryHandle::value(Integer::from(env->state(), val));
  }

  VALUE rb_ull2inum(unsigned long long val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return MemoryHandle::value(Integer::from(env->state(), val));
  }

  VALUE rb_num_coerce_bin(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_mCAPI, rb_intern("rb_num_coerce_bin"), 3, x, y, ID2SYM(func));
  }

  VALUE rb_num_coerce_cmp(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_mCAPI, rb_intern("rb_num_coerce_cmp"), 3, x, y, ID2SYM(func));
  }

  VALUE rb_num_coerce_relop(VALUE x, VALUE y, ID func) {
    return rb_funcall(rb_mCAPI, rb_intern("rb_num_coerce_relop"), 3, x, y, ID2SYM(func));
  }

  double rb_num2dbl(VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = MemoryHandle::object(value);

    if(object->nil_p()) {
      rb_raise(rb_eTypeError, "no implicit conversion from nil to Float");
    } else if(try_as<String>(object)) {
      rb_raise(rb_eTypeError, "no implicit conversion from String to Float");
    } else if(!try_as<Float>(object)) {
      value = rb_Float(value);
    }

    return MemoryHandle::from(value)->get_rfloat(env->state())->value;
  }

  // Imported from MRI
  double rb_cstr_to_dbl(const char *p, int badcheck) {

    const char *q;
    char *end;
    double d;

    if(!p) return 0.0;
    q = p;
    while (ISSPACE(*p)) p++;
    d = Float::string_to_double(p, strlen(p), badcheck, &end);
    if(p == end) {
      if(badcheck) {
        bad:
          rb_invalid_str(q, "Float()");
      }
      return d;
    }
    if(*end) {
      char buf[DBL_DIG * 4 + 10];
      char *n = buf;
      char *e = buf + sizeof(buf) - 1;
      char prev = 0;

      while (p < end && n < e) prev = *n++ = *p++;
      while (*p) {
        if(*p == '_') {
          /* remove underscores between digits */
          if(badcheck) {
            if(n == buf || !ISDIGIT(prev)) goto bad;
            ++p;
            if(!ISDIGIT(*p)) goto bad;
          } else {
            while (*++p == '_');
            continue;
          }
        }
        prev = *p++;
        if(n < e) *n++ = prev;
      }
      *n = '\0';
      p = buf;
      d = Float::string_to_double(p, strlen(p), badcheck, &end);
      if(badcheck) {
        if(!end || p == end) goto bad;
        while (*end && ISSPACE(*end)) end++;
        if(*end) goto bad;
      }
    }
    return d;
  }

  void rb_num_zerodiv(void) {
    rb_raise(rb_eZeroDivError, "divided by 0");
  }

  int rb_cmpint(VALUE val, VALUE a, VALUE b) {
    if(NIL_P(val)) rb_cmperr(a, b);
    if(FIXNUM_P(val)) return FIX2INT(val);
    if(TYPE(val) == T_BIGNUM) {
      if(RBIGNUM_SIGN(val)) return 1;
      return -1;
    }

    if(RTEST(rb_funcall(val, rb_intern(">"), 1, INT2FIX(0)))) return 1;
    if(RTEST(rb_funcall(val, rb_intern("<"), 1, INT2FIX(0)))) return -1;
    return 0;
  }

  void rb_cmperr(VALUE x, VALUE y) {
    const char *classname;

    if(SPECIAL_CONST_P(y)) {
      y = rb_inspect(y);
      classname = StringValuePtr(y);
    } else {
      classname = rb_obj_classname(y);
    }

    rb_raise(rb_eArgError, "comparison of %s with %s failed",
        rb_obj_classname(x), classname);
  }
}
