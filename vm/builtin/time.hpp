#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "builtin/tuple.hpp"
#include "builtin/integer.hpp"

namespace rubinius {
  class Array;
  class String;

  typedef time_t int64_t;

  class Time : public Object {
  public:
    const static object_type type = TimeType;

  private:
    time_t seconds_;
    time_t nanoseconds_;

    Array* decomposed_; // slot
    Object* is_gmt_;  // slot

  public:
    /* accessors */
    attr_accessor(decomposed, Array);
    attr_accessor(is_gmt, Object);

    /* interface */

    static void init(STATE);

    // Rubinius.primitive :time_s_specific
    static Time* specific(STATE, Object* self, Integer* sec, Integer* nsec, Object* gmt);

    // Rubinius.primitive :time_s_now
    static Time* now(STATE, Object* self);

    // Rubinius.primitive :time_s_from_array
    static Time* from_array(STATE, Object* self, Fixnum* sec, Fixnum* min, Fixnum* hour, Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* nsec, Fixnum* isdst, Object* from_gmt);

    // Rubinius.primitive :time_s_dup
    static Time* dup(STATE, Object* self, Time* other);

    // Rubinius.primitive :time_seconds
    Integer* seconds(STATE) {
      return Integer::from(state, seconds_);
    }

    // Rubinius.primitive :time_useconds
    Integer* useconds(STATE) {
      return Integer::from(state, nanoseconds_ / 1000);
    }

    // Rubinius.primitive :time_nseconds
    Integer* nseconds(STATE) {
      return Integer::from(state, nanoseconds_);
    }

    // Rubinius.primitive :time_decompose
    Array* calculate_decompose(STATE, Object* gmt);

    // Rubinius.primitive :time_strftime
    String* strftime(STATE, String* format);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
