#ifndef RBX_BUILTIN_INTEGER_HPP
#define RBX_BUILTIN_INTEGER_HPP

#include "builtin/object.hpp"

namespace rubinius {

  class Numeric : public Object {
  public:
    static const object_type type = NumericType;

    static void init(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

  class Integer : public Numeric {
  private:
    native_int bignum_to_native();
  public:
    static const object_type type = IntegerType;

    static void init(STATE);

    static Integer* from(STATE, int i);
    static Integer* from(STATE, unsigned int i);
    static Integer* from(STATE, long i);
    static Integer* from(STATE, unsigned long i);
    static Integer* from(STATE, long long i);
    static Integer* from(STATE, unsigned long long i);

    static Integer* from_cstr(STATE, const char* str, const char* end, int base, Object* strict);
    static Integer* from_cppstr(STATE, std::string str, int base) {
      return from_cstr(state, str.data(), str.data() + str.size(), base, cTrue);
    }

    unsigned int        to_uint();
    long                to_long();
    unsigned long       to_ulong();
    long long           to_long_long();
    unsigned long long  to_ulong_long();

    bool positive_p();

    native_int to_native() {
      if(fixnum_p()) return STRIP_FIXNUM_TAG(this);
      return bignum_to_native();
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif

