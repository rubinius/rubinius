#ifndef RBX_RESPOND_TO_CACHE_HPP
#define RBX_RESPOND_TO_CACHE_HPP

#include "dispatch.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"
#include "builtin/call_site.hpp"
#include "builtin/exception.hpp"
#include "type_info.hpp"

namespace rubinius {
  struct CallFrame;
  class Arguments;
  class Module;

  class RespondToCache : public CallSite {
  public:
    const static object_type type = RespondToCacheType;

  private:
    ClassData receiver_;
    Class*  receiver_class_; // slot
    Object* message_;        // slot
    Object* visibility_;     // slot
    Object* responds_;       // slot
    CallSite* fallback_;     // slot

  public:
    attr_accessor(receiver_class, Class);
    attr_accessor(message, Object);
    attr_accessor(visibility, Object);
    attr_accessor(responds, Object);
    attr_accessor(fallback, CallSite);

    void clear_receiver_data() {
      receiver_.raw = 0;
    }

    void set_receiver_data(uint64_t data) {
      receiver_.raw = data;
    }

    uint64_t receiver_data() {
      return receiver_.raw;
    }

    uint32_t receiver_class_id() {
      return receiver_.f.class_id;
    }

    uint32_t receiver_serial_id() {
      return receiver_.f.serial_id;
    }

  public:
    static void init(STATE);
    static RespondToCache* empty(STATE, CallSite* fallback, CallSite** location);

    static Object* check_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                               Arguments& args);

    void update(STATE, Object* recv, Symbol* msg, Object* priv, Object* res);

  public: // Rubinius Type stuff
    class Info : public CallSite::Info {
    public:
      BASIC_TYPEINFO(CallSite::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };
}

#endif

