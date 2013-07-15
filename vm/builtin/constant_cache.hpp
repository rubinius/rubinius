#ifndef RBX_BUILTIN_CONSTANT_CACHE_HPP
#define RBX_BUILTIN_CONSTANT_CACHE_HPP

#include "builtin/object.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"

namespace rubinius {
  class ConstantScope;

  class ConstantCache : public Object {
  public:
    const static object_type type = ConstantCacheType;

  private:
    Symbol* name_;           // slot
    Object* value_;          // slot
    Module* under_;          // slot
    ConstantScope* scope_;   // slot

    Executable* executable_; // slot
    int ip_;
    int serial_;

  public:
    attr_accessor(name, Symbol);
    attr_accessor(value, Object);
    attr_accessor(under, Module);
    attr_accessor(scope, ConstantScope);
    attr_accessor(executable, Executable);

    int ip() const { return ip_; }
    int serial() const { return serial_; }

    // Rubinius.primitive+ :constant_cache_ip
    Integer* ip_prim(STATE);

    // Rubinius.primitive+ :constant_cache_serial
    Integer* serial_prim(STATE);

    static void init(STATE);
    static ConstantCache* create(STATE, ConstantCache* cache, Object* value, ConstantScope* scope);
    static ConstantCache* create(STATE, ConstantCache* cache, Object* value, Module* under, ConstantScope* scope);
    static ConstantCache* empty(STATE, Symbol* name, Executable* executable, int ip);

    Object* retrieve(STATE, ConstantScope* scope) {
      if(serial_ == state->shared().global_serial() &&
         scope_ == scope) {
        return value_;
      }
      return NULL;
    }

    Object* retrieve(STATE, Module* under, ConstantScope* scope) {
      if(serial_ == state->shared().global_serial() &&
         scope_ == scope &&
         under_ == under) {
        return value_;
      }
      return NULL;
    }

    void update_constant_cache(STATE, ConstantCache* other) {
      if(this != other) {
        if(CompiledCode* ccode = try_as<CompiledCode>(executable_)) {
          ccode->machine_code()->store_constant_cache(state, ccode, ip_, other);
        }
      }
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
