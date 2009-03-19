#ifndef RBX_BUILTIN_INTEGER_HPP
#define RBX_BUILTIN_INTEGER_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class Numeric : public Object {
  public:
    static const object_type type = NumericType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

  class Integer : public Numeric {
  public:
    static const object_type type = IntegerType;

    static Integer* from(STATE, int i);
    static Integer* from(STATE, unsigned int i);
    static Integer* from(STATE, long i);
    static Integer* from(STATE, unsigned long i);
    static Integer* from(STATE, long long i);
    static Integer* from(STATE, unsigned long long i);

    native_int          to_native();
    long long           to_long_long();
    unsigned long long  to_ulong_long();


//    template< typename To >
//      To to() {
//        if(fixnum_p()) {
//          return (reinterpret_cast<Fixnum*>(this))->to<To>();
//        }
//
//        return as<Bignum>(this)->to<To>();
//    }

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif

