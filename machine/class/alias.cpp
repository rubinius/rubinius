#include "memory.hpp"

#include "class/alias.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"

namespace rubinius {
  void Alias::bootstrap(STATE) {
    GO(alias).set(state->memory()->new_class<Class, Alias>(
          state, G(executable), G(rubinius), "Alias"));
  }

  Object* Alias::executor(STATE, Executable* exe,
                         Module* mod, Arguments& args)
  {
    Exception::type_error(state, "Unable to directly execute a Alias");
    return 0;
  }

  // Create a new Alias object
  Alias* Alias::create(STATE, Symbol* name, Module* mod, Executable* exec) {
    Alias* alias = state->memory()->new_object<Alias>(state, G(alias));

    alias->original_name(state, name);
    alias->original_module(state, mod);
    alias->original_exec(state, exec);

    return alias;
  }
}
