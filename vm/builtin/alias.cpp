#include "builtin/alias.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void Alias::init(STATE) {
    GO(alias).set(state->new_class("Alias", G(executable), G(rubinius)));
    G(alias)->set_object_type(state, AliasType);
  }

  // Create a new Alias object
  Alias* Alias::create(STATE, Symbol* name, Module* mod, Executable* exec) {
    Alias* alias = state->new_object<Alias>(G(alias));
    alias->inliners_ = 0;
    alias->prim_index_ = -1;
    alias->set_executor(exec->execute);
    alias->original_name(state, name);
    alias->original_module(state, mod);
    alias->original_exec(state, exec);

    return alias;
  }
}
