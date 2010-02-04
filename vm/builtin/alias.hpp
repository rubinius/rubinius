#ifndef RBX_BUILTIN_ALIAS_HPP
#define RBX_BUILTIN_ALIAS_HPP

#include "builtin/executable.hpp"

namespace rubinius {
  class Alias : public Executable {
  public:
    const static object_type type = AliasType;

  private:
    Symbol* original_name_; // slot
    Module* original_module_; // slot
    Executable* original_exec_; // slot

  public:
    attr_accessor(original_name, Symbol);
    attr_accessor(original_module, Module);
    attr_accessor(original_exec, Executable);

    static void init(STATE);
    static Alias* create(STATE, Symbol* name, Module* mod, Executable* exec);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
