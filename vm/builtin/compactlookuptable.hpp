#ifndef RBX_BUILTIN_COMPACTLOOKUPTABLE_HPP
#define RBX_BUILTIN_COMPACTLOOKUPTABLE_HPP

#include "builtin/object.hpp"
#include "builtin/tuple.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Array;
  class LookupTable;

  // This value must be 2n
  #define COMPACTLOOKUPTABLE_SIZE 12U
  class CompactLookupTable : public Tuple {
  public:
    const static size_t fields = 0;
    const static object_type type = CompactLookupTableType;

    static void init(STATE);

    // Ruby.primitive :compactlookuptable_allocate
    static CompactLookupTable* create(STATE);

    // Ruby.primitive :compactlookuptable_fetch
    OBJECT fetch(STATE, OBJECT key);

    // Ruby.primitive :compactlookuptable_store
    OBJECT store(STATE, OBJECT key, OBJECT val);

    // Ruby.primitive :compactlookuptable_has_key
    OBJECT has_key(STATE, OBJECT key);

    // Ruby.primitive :compactlookuptable_keys
    Array* keys(STATE);

    // Ruby.primitive :compactlookuptable_values
    Array* values(STATE);

    LookupTable* to_lookuptable(STATE);

    class Info : public Tuple::Info {
    public:
      Info(object_type type) : Tuple::Info(type) { }
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
