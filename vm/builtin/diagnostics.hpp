#ifndef RBX_BUILTIN_DIAGNOSTICS_HPP
#define RBX_BUILTIN_DIAGNOSTICS_HPP

#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/object.hpp"

#include "object_utils.hpp"
#include "diagnostics.hpp"

namespace rubinius {
  class LookupTable;

  class Diagnostics : public Module {
  public:
    const static object_type type = DiagnosticsType;

  private:
    LookupTable* map_;  // slot

  public:
    attr_accessor(map, LookupTable);

  public:
    static void init(STATE);


    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };
}

#endif
