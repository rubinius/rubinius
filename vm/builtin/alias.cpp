#include "builtin/alias.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "ontology.hpp"

namespace rubinius {
  void Alias::init(STATE) {
    GO(alias).set(ontology::new_class(state, "Alias",
                    G(executable), G(rubinius)));
    G(alias)->set_object_type(state, AliasType);
  }

  namespace {
    Object* alias_executor(STATE, CallFrame* call_frame, Executable* exe,
                           Module* mod, Arguments& args)
    {
      Exception::type_error(state, "Unable to directly execute a Alias", call_frame);
      return 0;
    }
  }

  // Create a new Alias object
  Alias* Alias::create(STATE, Symbol* name, Module* mod, Executable* exec) {
    Alias* alias = state->new_object<Alias>(G(alias));
    alias->inliners_ = 0;
    alias->prim_index_ = -1;
    alias->custom_call_site_ = false;
    alias->set_executor(alias_executor);
    alias->original_name(state, name);
    alias->original_module(state, mod);
    alias->original_exec(state, exec);

    return alias;
  }
}
