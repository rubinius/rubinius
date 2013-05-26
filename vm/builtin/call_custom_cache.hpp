#ifndef RBX_CALL_CUSTOM_CACHE_HPP
#define RBX_CALL_CUSTOM_CACHE_HPP

#include "builtin/call_site.hpp"

namespace rubinius {
  struct CallFrame;
  class CallUnit;

  class CallCustomCache : public CallSite {
  public:
    const static object_type type = CallCustomCacheType;

  private:
    CallUnit* call_unit_; // slot
    int hits_;

  public:
    attr_accessor(call_unit, CallUnit);

    static void init(STATE);
    static CallCustomCache* create(STATE, CallSite* call_site, CallUnit* call_unit);

    int hits() {
      return hits_;
    }

    static Object* check_cache(STATE, CallSite* cache, CallFrame* call_frame,
                               Arguments& args);

  public: // Rubinius Type stuff
    class Info : public CallSite::Info {
    public:
      BASIC_TYPEINFO(CallSite::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };
}

#endif
