#ifndef RBX_BUILTIN_CACHE_HPP
#define RBX_BUILTIN_CACHE_HPP

#include "vm.hpp"
#include "builtin/object.hpp"
#include "builtin/tuple.hpp"
#include "builtin/integer.hpp"
#include "builtin/class.hpp"

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

    ClassData receiver_;

    MethodMissingReason method_missing_;
    bool super_;

  public:
    attr_accessor(stored_module, Module);
    attr_accessor(receiver_class, Class);
    attr_accessor(method, Executable);

    uint64_t receiver_data() {
      return receiver_.raw;
    }

    uint32_t receiver_class_id() {
      return receiver_.f.class_id;
    }

    uint32_t receiver_serial_id() {
      return receiver_.f.serial_id;
    }

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
