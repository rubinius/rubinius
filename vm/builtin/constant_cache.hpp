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
    ConstantCacheEntry* entry_; // slot
    int serial_;

  public:
    attr_accessor(entry, ConstantCacheEntry);

    int serial() { return serial_; }

    static void init(STATE);
    static ConstantCache* create(STATE, Object* value, ConstantScope* scope);
    static ConstantCache* create(STATE, Object* value, Module* under, ConstantScope* scope);
    static ConstantCache* empty(STATE);

    void update(STATE, Object* value, ConstantScope* scope);
    void update(STATE, Object* value, Module* under, ConstantScope* scope);

    Object* retrieve(STATE, ConstantScope* scope);
    Object* retrieve(STATE, Module* under, ConstantScope* scope);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
