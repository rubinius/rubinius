#ifndef RBX_BULITIN_GLOBAL_CACHE_ENTRY_HPP
#define RBX_BULITIN_GLOBAL_CACHE_ENTRY_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  class ConstantScope;

  class GlobalCacheEntry : public Object {
  public:
    const static object_type type = GlobalCacheEntryType;

  private:
    Object* value_;  // slot
    ConstantScope* scope_; // slot
    Module* under_; // slot
    int serial_;

  public:
    attr_accessor(value, Object);
    attr_accessor(scope, ConstantScope);
    attr_accessor(under, Module);

    int serial() { return serial_; }

    int* serial_location() {
      return &serial_;
    }

    Object** value_location() {
      return &value_;
    }

    Module** under_location() {
      return &under_;
    }

    static void init(STATE);
    static GlobalCacheEntry* create(STATE, Object* value, ConstantScope* scope);
    static GlobalCacheEntry* create(STATE, Object* value, Module* under, ConstantScope* scope);
    static GlobalCacheEntry* empty(STATE);

    void update(STATE, Object* value, ConstantScope* scope);
    void update(STATE, Object* value, Module* under, ConstantScope* scope);
    bool valid_p(STATE, ConstantScope* scope);
    bool valid_p(STATE, Module* under, ConstantScope* scope);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
