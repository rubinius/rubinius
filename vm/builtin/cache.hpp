#ifndef RBX_BUILTIN_CACHE_HPP
#define RBX_BUILTIN_CACHE_HPP

#include "vm.hpp"
#include "builtin/object.hpp"
#include "builtin/tuple.hpp"
#include "builtin/integer.hpp"

#include "type_info.hpp"
#include "object_utils.hpp"

namespace rubinius {

  class Tuple;
  class Array;

  class MethodCacheEntry : public Object {
  public:
    const static object_type type = MethodCacheEntryType;

  private:
    Module* stored_module_;  // slot
    Class*  receiver_class_; // slot
    Executable* method_;     // slot

    MethodMissingReason method_missing_;
    bool super_;

  public:
    attr_accessor(stored_module, Module);
    attr_accessor(receiver_class, Class);
    attr_accessor(method, Executable);

    MethodMissingReason method_missing() {
      return method_missing_;
    }

    bool super() {
      return super_;
    }

  public:
    static MethodCacheEntry* create(STATE, Class* klass, Module* mod,
                                    Executable* method, MethodMissingReason method_missing,
                                    bool super);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo);
    };
  };
}

#endif
