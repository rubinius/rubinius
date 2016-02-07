#ifndef RBX_BUILTIN_DIAGNOSTICS_HPP
#define RBX_BUILTIN_DIAGNOSTICS_HPP

#include "builtin/class.hpp"
#include "builtin/object.hpp"

#include "object_utils.hpp"
#include "diagnostics.hpp"

namespace rubinius {
  class LookupTable;

  class Diagnostics : public Class {
  public:
    const static object_type type = DiagnosticsType;

  private:
    LookupTable* map_;  // slot

  public:
    attr_accessor(map, LookupTable);

  public:
    static void bootstrap(STATE);


    class Info : public Class::Info {
    public:
      BASIC_TYPEINFO(Class::Info)
    };
  };
}

#endif
