#ifndef RBX_BUILTIN_CONSTANT_CACHE_HPP
#define RBX_BUILTIN_CONSTANT_CACHE_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  class ConstantScope;


  class ConstantCacheEntry: public Object {
  public:
    const static object_type type = ConstantCacheEntryType;

  private:
    Object* value_;  // slot
    Module* under_; // slot
    ConstantScope* scope_; // slot

  public:
    attr_accessor(value, Object);
    attr_accessor(under, Module);
    attr_accessor(scope, ConstantScope);

    static ConstantCacheEntry* create(STATE, Object* value, Module* under, ConstantScope* scope);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };


  class ConstantCache : public Object {
  public:
    const static object_type type = ConstantCacheType;

  private:
    Symbol* name_; // slot
    ConstantCacheEntry* entry_; // slot
    int serial_;

  public:
    attr_accessor(name, Symbol);
    attr_accessor(entry, ConstantCacheEntry);

    int serial() { return serial_; }

    static void init(STATE);
    static ConstantCache* create(STATE, Symbol* name, Object* value, ConstantScope* scope);
    static ConstantCache* create(STATE, Symbol* name, Object* value, Module* under, ConstantScope* scope);
    static ConstantCache* empty(STATE, Symbol* name);

    void update(STATE, Object* value, ConstantScope* scope);
    void update(STATE, Object* value, Module* under, ConstantScope* scope);

    Object* retrieve(STATE, ConstantScope* scope) {
      ConstantCacheEntry* cache_entry = entry_;
      if(serial_ == state->shared().global_serial() &&
         cache_entry->scope() == scope) {
        return cache_entry->value();
      }
      return NULL;
    }

    Object* retrieve(STATE, Module* under, ConstantScope* scope) {
      ConstantCacheEntry* cache_entry = entry_;
      if(serial_ == state->shared().global_serial() &&
         cache_entry->scope() == scope &&
         cache_entry->under() == under) {
        return cache_entry->value();
      }
      return NULL;
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
