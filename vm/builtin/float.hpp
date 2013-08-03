#ifndef RBX_FLOAT_HPP
#define RBX_FLOAT_HPP

#include "builtin/object.hpp"
#include "builtin/integer.hpp"

/* Begin borrowing from MRI 1.8.6 stable */
#if defined(__FreeBSD__) && __FreeBSD__ < 4
#include <floatingpoint.h>
#endif

#include <math.h>
#include <float.h>

namespace rubinius {
  class Array;
  class String;

  class Float : public Numeric {
  public:
    const static object_type type = FloatType;

    double val;

    static void init(STATE);
    static Float* create(STATE, double val);
    static Float* create(STATE, float val);
    static Float* create(STATE, native_int val);
    static Float* coerce(STATE, Object* value);
    double to_double(STATE) { return val; }
    void into_string(STATE, char* buf, size_t sz);

    static Float* from_cstr(STATE, const char* str, Object* strict);

    // Rubinius.primitive! :float_add
    Float* add(STATE, Float* other);
    // Rubinius.primitive! :float_add
    Float* add(STATE, Integer* other);

    // Rubinius.primitive! :float_sub
    Float* sub(STATE, Float* other);
    // Rubinius.primitive! :float_sub
    Float* sub(STATE, Integer* other);

    // Rubinius.primitive! :float_mul
    Float* mul(STATE, Float* other);
    // Rubinius.primitive! :float_mul
    Float* mul(STATE, Integer* other);

    // Rubinius.primitive! :float_pow
    Object* fpow(STATE, Float* other);
    // Rubinius.primitive! :float_pow
    Float* fpow(STATE, Integer* other);

    // Rubinius.primitive! :float_div
    Float* div(STATE, Float* other);
    // Rubinius.primitive! :float_div
    Float* div(STATE, Integer* other);

    // Rubinius.primitive! :float_mod
    Float* mod(STATE, Float* other);
    // Rubinius.primitive! :float_mod
    Float* mod(STATE, Integer* other);

    // Rubinius.primitive! :float_divmod
    Array* divmod(STATE, Float* other);
    // Rubinius.primitive! :float_divmod
    Array* divmod(STATE, Integer* other);

    // Rubinius.primitive :float_neg
    Float* neg(STATE);

    // Rubinius.primitive! :float_equal
    Object* equal(STATE, Float* other);
    // Rubinius.primitive! :float_equal
    Object* equal(STATE, Integer* other);

    // Rubinius.primitive! :float_eql
    Object* eql(STATE, Float* other);
    // Rubinius.primitive! :float_eql
    Object* eql(STATE, Integer* other);

    // Rubinius.primitive! :float_compare
    Object* compare(STATE, Float* other);
    // Rubinius.primitive! :float_compare
    Object* compare(STATE, Integer* other);

    // Rubinius.primitive! :float_gt
    Object* gt(STATE, Float* other);
    // Rubinius.primitive! :float_gt
    Object* gt(STATE, Integer* other);

    // Rubinius.primitive! :float_ge
    Object* ge(STATE, Float* other);
    // Rubinius.primitive! :float_ge
    Object* ge(STATE, Integer* other);

    // Rubinius.primitive! :float_lt
    Object* lt(STATE, Float* other);
    // Rubinius.primitive! :float_lt
    Object* lt(STATE, Integer* other);

    // Rubinius.primitive! :float_le
    Object* le(STATE, Float* other);
    // Rubinius.primitive! :float_le
    Object* le(STATE, Integer* other);

    // Rubinius.primitive+ :float_isinf
    Object* fisinf(STATE);

    // Rubinius.primitive+ :float_isnan
    Object* fisnan(STATE);

    // Rubinius.primitive :float_round
    Integer* fround(STATE);

    // Rubinius.primitive :float_to_i
    Integer* to_i(STATE);

    // Rubinius.primitive :float_to_s_formatted
    String* to_s_formatted(STATE, String* format);

    // Rubinius.primitive :float_to_s_minimal
    String* to_s_minimal(STATE);

    // Rubinius.primitive :float_dtoa
    Tuple* dtoa(STATE);

    // Rubinius.primitive :float_to_packed
    String* to_packed(STATE, Object* want_double);

    static int radix()      { return FLT_RADIX; }
    static int rounds()     { return FLT_ROUNDS; }
    static double min()     { return DBL_MIN; }
    static double max()     { return DBL_MAX; }
    static int min_exp()    { return DBL_MIN_EXP; }
    static int max_exp()    { return DBL_MAX_EXP; }
    static int min_10_exp() { return DBL_MIN_10_EXP; }
    static int max_10_exp() { return DBL_MAX_10_EXP; }
    static int dig()        { return DBL_DIG; }
    static int mant_dig()   { return DBL_MANT_DIG; }
    static double epsilon() { return DBL_EPSILON; }

    static double string_to_double(const char* buf, size_t len, bool strict, char** end);
    static int double_to_string(char* buf, size_t len, double val);
    static int double_to_ascii(char* buf, size_t len, double val, bool* sign, int* decpt);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* t, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif
