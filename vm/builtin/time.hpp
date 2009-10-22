#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "builtin/tuple.hpp"

#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
# define HAVE_STRUCT_TM_TM_GMTOFF
# define HAVE_STRUCT_TM_TM_ZONE
#endif

namespace rubinius {
  class Array;
  class String;

  class Time : public Object {
  public:
    const static object_type type = TimeType;

  private:
    Integer* sec_; // slot
    Integer* usec_; // slot
    Array* tm_;      // slot
    Object* is_gmt_;  // slot

  public:
    /* accessors */

    attr_accessor(sec, Integer);
    attr_accessor(usec, Integer);
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
    Time* time_switch(STATE, Object* gmt);

    // Ruby.primitive :time_mktime
    static Tuple* mktime(STATE, Fixnum* sec, Fixnum* min, Fixnum* hour, Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* usec, Fixnum* isdst, Object* from_gmt);

    // Ruby.primitive :time_strftime
    String* strftime(STATE, Array* ary, String* format);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
