#ifndef RBX_BUILTIN_DIAGNOSTICS_HPP
#define RBX_BUILTIN_DIAGNOSTICS_HPP

#include "class/class.hpp"
#include "class/object.hpp"

#include "object_utils.hpp"
#include "diagnostics.hpp"

namespace rubinius {
  class LookupTable;

  class Diagnostics : public Class {
  public:
    const static object_type type = DiagnosticsType;

    attr_accessor(map, LookupTable);

    static void bootstrap(STATE);

    class Info : public Class::Info {
    public:
      BASIC_TYPEINFO(Class::Info)
    };
  };
}

#endif
