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
    const static object_type type = CompactLookupTableType;

    static void init(STATE);

    // Ruby.primitive :compactlookuptable_allocate
    static CompactLookupTable* create(STATE);

    // Ruby.primitive :compactlookuptable_fetch
    Object* fetch(STATE, Object* key);

    Object* fetch(STATE, Object* key, bool* found);

    Object* remove(STATE, Object* key, bool* removed = 0);

    // Ruby.primitive :compactlookuptable_remove
    Object* remove_prim(STATE, Object* key) {
      return remove(state, key);
    }

    // Ruby.primitive :compactlookuptable_store
    Object* store(STATE, Object* key, Object* val);

    // Ruby.primitive :compactlookuptable_has_key
    Object* has_key(STATE, Object* key);

    // Ruby.primitive :compactlookuptable_keys
    Array* keys(STATE);

    Array* filtered_keys(STATE, ObjectMatcher& matcher, Array* ary=0);

    // Ruby.primitive :compactlookuptable_values
    Array* values(STATE);

    LookupTable* to_lookuptable(STATE);

    class Info : public Tuple::Info {
    public:
      Info(object_type type, bool cleanup = false)
        : Tuple::Info(type, cleanup)
      {}

      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif
