#ifndef RBX_FLOAT_HPP
#define RBX_FLOAT_HPP

/* Begin borrowing from MRI 1.8.6 stable */
#if defined(__FreeBSD__) && __FreeBSD__ < 4
#include <floatingpoint.h>
#endif

#include <math.h>
#include <float.h>

namespace rubinius {
  class Float : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = FloatType;

    static bool is_a(OBJECT obj) {
      return obj->obj_type == FloatType;
    }

    double val;

    static Float* create(STATE, double val);
    static Float* coerce(STATE, OBJECT value);
    double to_double(STATE) { return val; }
    void into_string(STATE, char* buf, size_t sz);

    // Ruby.primitive! :float_add
    Float* add(STATE, Float* other);
    // Ruby.primitive! :float_add
    Float* add(STATE, INTEGER other);

    // Ruby.primitive! :float_sub
    Float* sub(STATE, Float* other);
    // Ruby.primitive! :float_sub
    Float* sub(STATE, INTEGER other);

    // Ruby.primitive! :float_mul
    Float* mul(STATE, Float* other);
    // Ruby.primitive! :float_mul
    Float* mul(STATE, INTEGER other);

    // Ruby.primitive! :float_pow
    Float* fpow(STATE, Float* other);
    // Ruby.primitive! :float_pow
    Float* fpow(STATE, INTEGER other);

    // Ruby.primitive! :float_div
    Float* div(STATE, Float* other);
    // Ruby.primitive! :float_div
    Float* div(STATE, INTEGER other);

    // Ruby.primitive! :float_mod
    Float* mod(STATE, Float* other);
    // Ruby.primitive! :float_mod
    Float* mod(STATE, INTEGER other);
    
    // Ruby.primitive! :float_divmod
    Array* divmod(STATE, Float* other);
    // Ruby.primitive! :float_divmod
    Array* divmod(STATE, INTEGER other);

    // Ruby.primitive :float_neg
    Float* neg(STATE);

    // Ruby.primitive! :float_equal
    OBJECT equal(STATE, Float* other);
    // Ruby.primitive! :float_equal
    OBJECT equal(STATE, INTEGER other);

    // Ruby.primitive! :float_compare
    FIXNUM compare(STATE, Float* other);
    // Ruby.primitive! :float_compare
    FIXNUM compare(STATE, INTEGER other);

    // Ruby.primitive! :float_gt
    OBJECT gt(STATE, Float* other);
    // Ruby.primitive! :float_gt
    OBJECT gt(STATE, INTEGER other);

    // Ruby.primitive! :float_ge
    OBJECT ge(STATE, Float* other);
    // Ruby.primitive! :float_ge
    OBJECT ge(STATE, INTEGER other);

    // Ruby.primitive! :float_lt
    OBJECT lt(STATE, Float* other);
    // Ruby.primitive! :float_lt
    OBJECT lt(STATE, INTEGER other);

    // Ruby.primitive! :float_le
    OBJECT le(STATE, Float* other);
    // Ruby.primitive! :float_le
    OBJECT le(STATE, INTEGER other);

    // Ruby.primitive :float_isinf
    OBJECT fisinf(STATE);

    // Ruby.primitive :float_isnan
    OBJECT fisnan(STATE);

    // Ruby.primitive :float_round
    INTEGER fround(STATE);

    // Ruby.primitive :float_to_i
    INTEGER to_i(STATE);

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

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
}

#endif
