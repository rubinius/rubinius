#ifndef RBX_BUILTIN_STATICSCOPE_HPP
#define RBX_BUILTIN_STATICSCOPE_HPP

#include "object_utils.hpp"
#include "memory.hpp"

#include "class/module.hpp"
#include "class/object.hpp"

namespace rubinius {

  class Module;
  class LookupTable;

  class LexicalScope : public Object {
  public:
    const static object_type type = LexicalScopeType;

    attr_accessor(functions, LookupTable);
    attr_accessor(module, Module);

    /* This is used like the ruby_class MRI variable. It lets us manipulate
     * this aspect of the class lexical enclosure without having to change
     * module also.
     */
    attr_accessor(current_module, Module);
    attr_accessor(parent, LexicalScope);

    static void bootstrap(STATE);
    static void bootstrap_methods(STATE);
    static void initialize(STATE, LexicalScope* obj) {
      obj->functions(nil<LookupTable>());
      obj->module(nil<Module>());
      obj->current_module(nil<Module>());
      obj->parent(nil<LexicalScope>());
    }

    static LexicalScope* create(STATE);

    // Rubinius.primitive+ :lexical_scope_of_sender
    static LexicalScope* of_sender(STATE);

    // Rubinius.primitive :lexical_scope_const_set
    Object* const_set(STATE, Symbol* name, Object* value);

    // The module to use when adding and removing methods
    Module* for_method_definition() {
      if(current_module()->nil_p()) {
        return module();
      }

      return current_module();
    }

    // Rubinius.primitive :lexical_scope_cvar_defined
    Object* cvar_defined(STATE, Symbol* name);

    // Rubinius.primitive :lexical_scope_cvar_get
    Object* cvar_get(STATE, Symbol* name);

    // Rubinius.primitive :lexical_scope_cvar_set
    Object* cvar_set(STATE, Symbol* name, Object* value);

    // Rubinius.primitive :lexical_scope_cvar_get_or_set
    Object* cvar_get_or_set(STATE, Symbol* name, Object* value);

    bool top_level_p(STATE) const {
      return parent()->nil_p();
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };

  };
}

#endif
