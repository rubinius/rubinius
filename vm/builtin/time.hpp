#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "builtin/tuple.hpp"
#include "builtin/integer.hpp"

#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
# define HAVE_STRUCT_TM_TM_GMTOFF
# define HAVE_STRUCT_TM_TM_ZONE
#endif

namespace rubinius {
  class Array;
  class String;

  typedef time_t int64_t;

  class Time : public Object {
  public:
    const static object_type type = TimeType;

  private:
    time_t seconds_;
    time_t microseconds_;

    Array* decomposed_; // slot
    Object* is_gmt_;  // slot

  public:
    /* accessors */
    attr_accessor(decomposed, Array);
    attr_accessor(is_gmt, Object);

    /* interface */

    static void init(STATE);

    // Ruby.primitive :time_s_specific
    static Time* specific(STATE, Object* self, Integer* sec, Integer* usec, Object* gmt);

    // Ruby.primitive :time_s_now
    static Time* now(STATE, Object* self);

    // Ruby.primitive :time_s_from_array
    static Time* from_array(STATE, Object* self, Fixnum* sec, Fixnum* min, Fixnum* hour, Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* usec, Fixnum* isdst, Object* from_gmt);

    // Ruby.primitive :time_dup
    Time* dup(STATE);

    // Ruby.primitive :time_seconds
    Integer* seconds(STATE) {
      return Integer::from(state, seconds_);
    }

    // Ruby.primitive :time_useconds
    Integer* useconds(STATE) {
      return Integer::from(state, microseconds_);
    }

    // Ruby.primitive :time_decompose
    Array* calculate_decompose(STATE, Object* gmt);

    // Ruby.primitive :time_strftime
    String* strftime(STATE, String* format);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
