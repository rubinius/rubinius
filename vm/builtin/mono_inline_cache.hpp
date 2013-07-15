#ifndef RBX_MONO_INLINE_CACHE_HPP
#define RBX_MONO_INLINE_CACHE_HPP

#include "builtin/class.hpp"
#include "builtin/call_site.hpp"

namespace rubinius {
  struct CallFrame;
  class Dispatch;
  class Arguments;
  class CallUnit;
  class Module;

  class MonoInlineCache : public CallSite {
  public:
    const static object_type type = MonoInlineCacheType;

  private:
    ClassData receiver_;

    Class*  receiver_class_; // slot
    Module* stored_module_;  // slot
    Executable* method_;     // slot

    MethodMissingReason method_missing_;
    int hits_;

  public:
    attr_accessor(receiver_class, Class);
    attr_accessor(stored_module, Module);
    attr_accessor(method, Executable);

    static void init(STATE);
    static MonoInlineCache* create(STATE, CallSite* call_site, Class* klass, Dispatch& dis);

    ClassData receiver_data() {
      return receiver_;
    }

    uint64_t receiver_data_raw() {
      return receiver_.raw;
    }

    MethodMissingReason method_missing() {
      return method_missing_;
    }

    int hits() {
      return hits_;
    }

    // Rubinius.primitive+ :mono_inline_cache_hits
    Integer* hits_prim(STATE);

    // Rubinius.primitive+ :mono_inline_cache_method_missing
    Symbol* method_missing_prim(STATE);

    friend class CompiledCode::Info;

    static Object* check_cache(STATE, CallSite* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_mm(STATE, CallSite* cache, CallFrame* call_frame,
                                  Arguments& args);

    static void mono_cache_updater(STATE, CallSite* call_site, Class* klass, Dispatch& dispatch);

  public: // Rubinius Type stuff
    class Info : public CallSite::Info {
    public:
      BASIC_TYPEINFO(CallSite::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };
}

#endif

