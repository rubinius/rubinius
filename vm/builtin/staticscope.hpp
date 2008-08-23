#ifndef RBX_BUILTIN_STATICSCOPE_HPP
#define RBX_BUILTIN_STATICSCOPE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class Module;

  class StaticScope : public Object {
  public:
    const static size_t fields = 2;
    const static object_type type = StaticScopeType;

    Module* module; // slot
    StaticScope* parent; // slot

    static void init(STATE);
    static StaticScope* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
}

#endif
