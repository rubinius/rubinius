#ifndef RBX_BUILTIN_METHODVISIBILITY_HPP
#define RBX_BUILTIN_METHODVISIBILITY_HPP

#include "builtin/executable.hpp"
#include "vm.hpp"

namespace rubinius {

  class MethodVisibility : public Object {
  public:
    const static size_t fields = 2;
    const static object_type type = CMVisibilityType;

    SYMBOL visibility; // slot
    Executable* method; // slot

    static MethodVisibility* create(STATE);

    bool public_p(STATE) {
      return visibility == G(sym_public);
    }

    bool private_p(STATE) {
      return visibility == G(sym_private);
    }

    bool protected_p(STATE) {
      return visibility == G(sym_protected);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
    };
  };

};

#endif
