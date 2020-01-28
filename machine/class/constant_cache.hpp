#ifndef RBX_BUILTIN_CONSTANT_CACHE_HPP
#define RBX_BUILTIN_CONSTANT_CACHE_HPP

#include "machine_code.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "class/lexical_scope.hpp"
#include "class/executable.hpp"
#include "class/module.hpp"
#include "class/object.hpp"
#include "class/symbol.hpp"

namespace rubinius {
  class LexicalScope;

  class ConstantCache : public Object {
  public:
    const static object_type type = ConstantCacheType;

    attr_accessor(name, Symbol);
    attr_accessor(value, Object);
    attr_accessor(module, Module);
    attr_accessor(lexical_scope, LexicalScope);
    attr_accessor(executable, Executable);

  private:
    attr_field(ip, int);
    attr_field(serial, int);

  public:
    // Rubinius.primitive+ :constant_cache_ip
    Integer* ip_prim(STATE);

    // Rubinius.primitive+ :constant_cache_serial
    Integer* serial_prim(STATE);

    static void bootstrap(STATE);
    static void initialize(STATE, ConstantCache* obj) {
      obj->name(nil<Symbol>());
      obj->value(nil<Object>());
      obj->module(nil<Module>());
      obj->lexical_scope(nil<LexicalScope>());
      obj->executable(nil<Executable>());
      obj->ip(0);
      obj->serial(0);
    }

    static ConstantCache* create(STATE, ConstantCache* cache, Object* value, LexicalScope* scope);
    static ConstantCache* create(STATE, ConstantCache* cache, Object* value, Module* mod, LexicalScope* scope);
    static ConstantCache* empty(STATE, Symbol* name, Executable* executable, int ip);

    Object* retrieve(STATE, LexicalScope* scope) {
      if(serial() == state->memory()->global_serial() &&
         lexical_scope()->equal_p(scope)) {
        return value();
      }
      return NULL;
    }

    Object* retrieve(STATE, Module* mod, LexicalScope* scope) {
      if(serial() == state->memory()->global_serial() &&
         lexical_scope()->equal_p(scope) &&
         module()->equal_p(mod)) {
        return value();
      }
      return NULL;
    }

    void update_constant_cache(STATE, ConstantCache* other) {
      if(this != other) {
        if(CompiledCode* ccode = try_as<CompiledCode>(executable())) {
          ccode->machine_code()->store_constant_cache(state, ccode, ip(), other);
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
