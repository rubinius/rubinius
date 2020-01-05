#ifndef RBX_BUILTIN_INTEGER_HPP
#define RBX_BUILTIN_INTEGER_HPP

#include "class/object.hpp"

namespace rubinius {

  class Numeric : public Object {
  public:
    static const object_type type = NumericType;

    static void bootstrap(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
    };
  };

  class Integer : public Numeric {
  private:
    intptr_t bignum_to_native();
  public:
    static const object_type type = IntegerType;

    static void bootstrap(STATE);

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

    intptr_t to_native() {
      if(fixnum_p()) return STRIP_FIXNUM_TAG(this);
      return bignum_to_native();
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
    };
  };
}

#endif

