#ifndef RBX_BUILTIN_STATICSCOPE_HPP
#define RBX_BUILTIN_STATICSCOPE_HPP

namespace rubinius {
  class StaticScope : public BuiltinType {
  public:
    const static size_t fields = 3;
    const static object_type type = StaticScopeType;

    OBJECT __ivars__; // slot
    Module* module; // slot
    StaticScope* parent; // slot

    static StaticScope* create(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };

  };
}

#endif
