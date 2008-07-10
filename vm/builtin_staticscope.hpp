#ifndef RBX_BUILTIN_STATICSCOPE_HPP
#define RBX_BUILTIN_STATICSCOPE_HPP

namespace rubinius {
  class StaticScope : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = StaticScopeType;

    OBJECT __ivars__; // slot
    Module* module; // slot
    StaticScope* parent; // slot

    static StaticScope* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
}

#endif
