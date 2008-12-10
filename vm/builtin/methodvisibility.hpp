#ifndef RBX_BUILTIN_METHODVISIBILITY_HPP
#define RBX_BUILTIN_METHODVISIBILITY_HPP

#include "builtin/executable.hpp"
#include "vm.hpp"

namespace rubinius {

  class MethodVisibility : public Object {
  public:
    const static object_type type = MethodVisibilityType;

  private:
    Symbol* visibility_;  // slot
    Executable* method_; // slot

  public:
    /* accessors */

    attr_accessor(visibility, Symbol);
    attr_accessor(method, Executable);

    /* interface */

    static MethodVisibility* create(STATE);

    bool public_p(STATE) {
      return visibility_ == G(sym_public);
    }

    bool private_p(STATE) {
      return visibility_ == G(sym_private);
    }

    bool protected_p(STATE) {
      return visibility_ == G(sym_protected);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };

};

#endif
