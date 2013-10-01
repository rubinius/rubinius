#ifndef RBX_BUILTIN_STATICSCOPE_HPP
#define RBX_BUILTIN_STATICSCOPE_HPP

#include "builtin/object.hpp"

namespace rubinius {

  class Module;

  class ConstantScope : public Object {
  public:
    const static object_type type = ConstantScopeType;

  private:
    Module* module_;      // slot

    // This is used like the ruby_class MRI variable. It lets
    // manipulate this aspect of the class lexical enclosure
    // without having to change module also.
    Module* current_module_;   // slot

    ConstantScope* parent_; // slot

  public:
    /* accessors */

    attr_accessor(module, Module);
    attr_accessor(current_module, Module);
    attr_accessor(parent, ConstantScope);

    /* interface */

    static void init(STATE);
    static void bootstrap_methods(STATE);
    static ConstantScope* create(STATE);

    // Rubinius.primitive+ :constant_scope_of_sender
    static ConstantScope* of_sender(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :constant_scope_const_set
    Object* const_set(STATE, Symbol* name, Object* value);

    // The module to use when adding and removing methods
    Module* for_method_definition();

    // Rubinius.primitive :constant_scope_cvar_defined
    Object* cvar_defined(STATE, Symbol* name);

    // Rubinius.primitive :constant_scope_cvar_get
    Object* cvar_get(STATE, Symbol* name);

    // Rubinius.primitive :constant_scope_cvar_set
    Object* cvar_set(STATE, Symbol* name, Object* value);

    // Rubinius.primitive :constant_scope_cvar_get_or_set
    Object* cvar_get_or_set(STATE, Symbol* name, Object* value);

    bool top_level_p(STATE) const {
      return parent_->nil_p();
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
}

#endif
