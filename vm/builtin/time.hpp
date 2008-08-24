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

    // Ruby.primitive :time_witch
    Time* time_switch(STATE, OBJECT gmt);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
