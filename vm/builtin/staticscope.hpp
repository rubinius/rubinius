#ifndef RBX_BUILTIN_STATICSCOPE_HPP
#define RBX_BUILTIN_STATICSCOPE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class Module;

  class StaticScope : public Object {
  public:
    const static object_type type = StaticScopeType;

  private:
    Module* module_;      // slot
    StaticScope* parent_; // slot

  public:
    /* accessors */

    attr_accessor(module, Module);
    attr_accessor(parent, StaticScope);

    /* interface */

    static void init(STATE);
    static StaticScope* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
}

#endif
