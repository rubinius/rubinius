#ifndef RBX_BULITIN_GLOBAL_CACHE_ENTRY_HPP
#define RBX_BULITIN_GLOBAL_CACHE_ENTRY_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {
  class GlobalCacheEntry : public Object {
  public:
    const static object_type type = GlobalCacheEntryType;

  private:
    Object* value_;  // slot
    Fixnum* serial_; // slot

  public:
    attr_accessor(value, Object);
    attr_accessor(serial, Fixnum);

    static void init(STATE);
    static GlobalCacheEntry* create(STATE, Object* value);

    void update(STATE, Object* value);
    bool valid_p(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
