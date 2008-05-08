#ifndef RBX_FLOAT_HPP
#define RBX_FLOAT_HPP

/* Begin borrowing from MRI 1.8.6 stable */
#if defined(__FreeBSD__) && __FreeBSD__ < 4
#include <floatingpoint.h>
#endif

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
    double to_double() { return val; }
    void into_string(STATE, char* buf, size_t sz);
    OBJECT compare(STATE, Float* other);

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
    };
  };
}

#endif
