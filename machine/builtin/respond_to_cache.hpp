#ifndef RBX_RESPOND_TO_CACHE_HPP
#define RBX_RESPOND_TO_CACHE_HPP

#include "object_utils.hpp"

#include "builtin/call_site.hpp"
#include "builtin/class.hpp"
#include "builtin/object.hpp"

namespace rubinius {
  struct CallFrame;
  class Arguments;
  class Module;

  class RespondToCache : public CallSite {
  public:
    const static object_type type = RespondToCacheType;

    attr_accessor(receiver_class, Class);
    attr_accessor(message, Object);
    attr_accessor(visibility, Object);
    attr_accessor(responds, Object);
    attr_accessor(fallback_call_site, CallSite);

  private:
    attr_field(receiver_data, ClassData);
    attr_field(hits, int);

  public:
    uint64_t receiver_data_raw() {
      return _receiver_data_.raw;
    }

    uint32_t receiver_class_id() {
      return _receiver_data_.f.class_id;
    }

    uint32_t receiver_serial_id() {
      return _receiver_data_.f.serial_id;
    }

    void hit() {
      ++_hits_;
    }

    // Rubinius.primitive+ :respond_to_cache_hits
    Integer* hits_prim(STATE);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, RespondToCache* obj) {
      CallSite::initialize(state, obj);

      obj->_receiver_data_.raw = 0;
      obj->receiver_class(nil<Class>());
      obj->message(nil<Object>());
      obj->visibility(nil<Object>());
      obj->responds(nil<Object>());
      obj->fallback_call_site(nil<CallSite>());
      obj->hits(0);
    }

    static RespondToCache* create(STATE, CallSite* fallback, Object* recv,
                                  Symbol* msg, Object* priv, Object* res, int hits);

    static Object* check_cache(STATE, CallSite* call_site,
                               Arguments& args);

  public: // Rubinius Type stuff
    class Info : public CallSite::Info {
    public:
      BASIC_TYPEINFO(CallSite::Info)
      virtual void mark(Object* t, memory::ObjectMark& mark);
    };

  };
}

#endif

