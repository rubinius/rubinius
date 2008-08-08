#ifndef RBX_BUILTIN_COMPACTLOOKUPTABLE_HPP
#define RBX_BUILTIN_COMPACTLOOKUPTABLE_HPP

#include "builtin/object.hpp"
#include "builtin/tuple.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;

  // This value must be 2n
  #define COMPACTLOOKUPTABLE_SIZE 12U
  class CompactLookupTable : public Tuple {
  public:
    const static size_t fields = 0;
    const static object_type type = CompactLookupTableType;

    static CompactLookupTable* create(STATE);
    OBJECT fetch(STATE, OBJECT key);
    OBJECT store(STATE, OBJECT key, OBJECT val);
    LookupTable* to_lookuptable(STATE);
  };
};

#endif
