#ifndef RBX_BUILTIN_SELECTOR_HPP
#define RBX_BUILTIN_SELECTOR_HPP

#include "objects.hpp"
#include "builtin_sendsite.hpp"

namespace rubinius {
  class SendSite;

  class Selector : public BuiltinType {
    public:

    static const size_t fields = 3;
    static const object_type type = SelectorType;

    OBJECT __ivars__; // slot
    SYMBOL name; // slot
    Array* send_sites; // slot

    static void init(STATE);
    static Selector* create(STATE, OBJECT name);
    static Selector* lookup(STATE, OBJECT name);
    static void      clear_by_name(STATE,  OBJECT name);

    OBJECT associate(STATE, SendSite* ss);
    void   clear(STATE);
    bool   includes_p(STATE, SendSite* ss);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };

  };
};

#endif
