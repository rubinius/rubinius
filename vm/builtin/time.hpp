#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Array;

  class Time : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = TimeType;

    Array* timeval; // slot
    Array* tm; // slot
    OBJECT is_gmt; // slot

    static void init(STATE);

    // Ruby.primitive :time_allocate
    static Time* create(STATE);

    // Ruby.primitive :time_gettimeofday
    Time* gettimeofday(STATE);

    // Ruby.primitive :time_switch
    Time* time_switch(STATE, OBJECT gmt);

    // Ruby.primitive :time_mktime
    static Array* mktime(STATE, FIXNUM sec, FIXNUM min, FIXNUM hour, FIXNUM mday, FIXNUM mon, FIXNUM year, FIXNUM usec, FIXNUM isdst, OBJECT from_gmt);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
