#ifndef RBX_BUILTIN_TIME_HPP
#define RBX_BUILTIN_TIME_HPP

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/object.hpp"
#include "class/integer.hpp"

#include "util/time64.h"

namespace rubinius {
  class Array;
  class String;
  class Tuple;

  class Time : public Object {
  public:
    const static object_type type = TimeType;

  private:
    attr_field(sec, time64_t);
    attr_field(nsec, long);

  public:
    attr_accessor(decomposed, Array);
    attr_accessor(is_gmt, Object);
    attr_accessor(offset, Object);
    attr_accessor(zone, Object);

  private:
    String* locale_string(STATE, const char* data);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Time* obj) {
      obj->sec(0);
      obj->nsec(0);
      obj->decomposed(nil<Array>());
      obj->is_gmt(cFalse);
      obj->offset(nil<Object>());
      obj->zone(nil<Object>());
    }

    static Time* at(STATE, time64_t seconds, long nanoseconds = 0);

    bool gmt_p() {
      return CBOOL(is_gmt());
    }

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
      return Integer::from(state, sec());
    }

    // Rubinius.primitive+ :time_useconds
    Integer* useconds(STATE) {
      return Integer::from(state, nsec() / 1000);
    }

    // Rubinius.primitive+ :time_nseconds
    Integer* nseconds(STATE) {
      return Integer::from(state, nsec());
    }

    // Rubinius.primitive+ :time_set_nseconds
    Integer* set_nseconds(STATE, Integer* nanoseconds) {
      this->nsec(nanoseconds->to_long());
      return nanoseconds;
    }

    // Rubinius.primitive :time_utc_offset
    Object* utc_offset(STATE);

    // Rubinius.primitive :time_decompose
    Array* calculate_decompose(STATE);

    // Rubinius.primitive :time_strftime
    String* strftime(STATE, String* format);

    // Rubinius.primitive :time_env_zone
    String* env_zone(STATE);

    struct tm64 get_tm();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
