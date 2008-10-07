#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Array;
  class String;

  class Time : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = TimeType;

  private:
    Array* timeval_; // slot
    Array* tm_;      // slot
    OBJECT is_gmt_;  // slot

  public:
    /* accessors */

    attr_accessor(timeval, Array);
    attr_accessor(tm, Array);
    attr_accessor(is_gmt, Object);

    /* interface */

    static void init(STATE);

    // Ruby.primitive :time_allocate
    static Time* create(STATE);

    Time* initialize_copy(STATE, Time* other);

    // Ruby.primitive :time_gettimeofday
    Time* gettimeofday(STATE);

    // Ruby.primitive :time_switch
    Time* time_switch(STATE, OBJECT gmt);

    // Ruby.primitive :time_mktime
    static Array* mktime(STATE, FIXNUM sec, FIXNUM min, FIXNUM hour, FIXNUM mday, FIXNUM mon, FIXNUM year, FIXNUM usec, FIXNUM isdst, OBJECT from_gmt);

    // Ruby.primitive :time_strftime
    String* strftime(STATE, Array* ary, String* format);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
