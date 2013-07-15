#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "util/time64.h"

#include "builtin/object.hpp"
#include "builtin/integer.hpp"

namespace rubinius {
  class Array;
  class String;
  class Tuple;

  class Time : public Object {
  public:
    const static object_type type = TimeType;

  private:
    time64_t seconds_;
    long nanoseconds_;

    Array* decomposed_; // slot
    Object* is_gmt_;  // slot
    Object* offset_; // slot

  public:
    /* accessors */
    attr_accessor(decomposed, Array);
    attr_accessor(is_gmt, Object);
    attr_accessor(offset, Object);

    /* interface */

    static void init(STATE);

    static Time* at(STATE, time64_t seconds, long nanoseconds = 0);

    // Rubinius.primitive+ :time_s_specific
    static Time* specific(STATE, Object* self, Integer* sec, Integer* nsec, Object* gmt, Object* offset);

    // Rubinius.primitive+ :time_s_now
    static Time* now(STATE, Object* self);

    // Rubinius.primitive :time_s_from_array
    static Time* from_array(STATE, Object* self, Fixnum* sec, Fixnum* min, Fixnum* hour, Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* nsec, Fixnum* isdst, Object* from_gmt, Object* offset, Fixnum* offset_sec, Fixnum* offset_nsec);

    // Rubinius.primitive :time_s_dup
    static Time* dup(STATE, Object* self, Time* other);

    // Rubinius.primitive+ :time_seconds
    Integer* seconds(STATE) {
      return Integer::from(state, seconds_);
    }

    // Rubinius.primitive+ :time_useconds
    Integer* useconds(STATE) {
      return Integer::from(state, nanoseconds_ / 1000);
    }

    // Rubinius.primitive+ :time_nseconds
    Integer* nseconds(STATE) {
      return Integer::from(state, nanoseconds_);
    }

    // Rubinius.primitive :time_utc_offset
    Object* utc_offset(STATE);

    // Rubinius.primitive :time_decompose
    Array* calculate_decompose(STATE);

    // Rubinius.primitive :time_strftime
    String* strftime(STATE, String* format);

    struct tm64 get_tm();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
