#ifndef RBX_CLASS_UNWIND_SITE_HPP
#define RBX_CLASS_UNWIND_SITE_HPP

#include "class/object.hpp"
#include "thread_state.hpp"

#include <atomic>
#include <stdint.h>

namespace rubinius {
  class UnwindSite : public Object {
  public:
    struct InlineCache;

    const static object_type type = UnwindSiteType;

    enum UnwindType {
      eRescue = 0,
      eEnsure = 1,
      eNone = 2
    };

    attr_field(ip, int);
    attr_field(stack_depth, int);
    attr_field(unwind_type, UnwindType);
    attr_field(previous, UnwindSite*);

    typedef std::atomic<InlineCache*> InlineCachePtr;

    InlineCachePtr* _caches_;

  public:
    static int max_caches;

    struct InlineCache {
      attr_field(exception_class, Class*);
      attr_field(hits, int);
    };

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, UnwindSite* obj) {
      obj->ip(0);
      obj->stack_depth(0);
      obj->unwind_type(eNone);
      obj->previous(nullptr);

      obj->_caches_ = new InlineCachePtr[max_caches];

      for(int i = 0; i < max_caches; i++) {
        obj->_caches_[i] = nullptr;
      }
    }

    static UnwindSite* create(STATE, int ip, UnwindType unwind_type);

    void finalize(STATE) {
      delete[] _caches_;
      _caches_ = nullptr;
    }

    class Info : public TypeInfo {
      public:
        Info(object_type type)
          : TypeInfo(type)
        {
          allow_user_allocate = false;
        }

        void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) { }
        void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) { }
        void set_field(STATE, Object* target, size_t index, Object* val) { }
        Object* get_field(STATE, Object* target, size_t index) { return cNil; }
        void populate_slot_locations() { }
    };
  };
}

#endif
