#ifndef RBX_BULITIN_GLOBAL_CACHE_ENTRY_HPP
#define RBX_BULITIN_GLOBAL_CACHE_ENTRY_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  class StaticScope;

  class GlobalCacheEntry : public Object {
  public:
    const static object_type type = GlobalCacheEntryType;

  private:
    Object* value_;  // slot
    StaticScope* scope_; // slot
    int serial_;

  public:
    attr_accessor(value, Object);
    attr_accessor(scope, StaticScope);

    int serial() { return serial_; }

    int* serial_location() {
      return &serial_;
    }

    Object** value_location() {
      return &value_;
    }

    static void init(STATE);
    static GlobalCacheEntry* create(STATE, Object* value, StaticScope* scope);

    void update(STATE, Object* value, StaticScope* scope);
    bool valid_p(STATE, StaticScope* scope);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
