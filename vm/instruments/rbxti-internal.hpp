#ifndef RBX_INSTRUMENTS_RBXTI_INTERNALS_HPP
#define RBX_INSTRUMENTS_RBXTI_INTERNALS_HPP

#include "capi/18/include/rbxti.hpp"

namespace rubinius {
  class VM;
  class Module;
  class Symbol;
  class CompiledCode;
  class MachineCode;
  class String;
  class Integer;
  class LookupTable;
  class Array;
  class Object;
}

namespace rbxti {
  Env* create_env(rubinius::VM*);
  void destroy_env(Env*);

  static inline rubinius::Module* i(rmodule mod) {
    return (rubinius::Module*)mod;
  }

  static inline rubinius::Symbol* i(rsymbol sym) {
    return (rubinius::Symbol*)sym;
  }

  static inline rubinius::CompiledCode* i(rcompiled_code code) {
    return (rubinius::CompiledCode*)code;
  }

  static inline rubinius::MachineCode* i(rmachine_code code) {
    return (rubinius::MachineCode*)code;
  }

  static inline rubinius::String* i(rstring s) {
    return (rubinius::String*)s;
  }

  static inline rubinius::Integer* i(rinteger i) {
    return (rubinius::Integer*)i;
  }

  static inline rubinius::LookupTable* i(rtable i) {
    return (rubinius::LookupTable*)i;
  }

  static inline rubinius::Array* i(rarray a) {
    return (rubinius::Array*)a;
  }

  static inline rmodule o(rubinius::Module* mod) {
    return (rmodule)mod;
  }

  static inline rsymbol o(rubinius::Symbol* sym) {
    return (rsymbol)sym;
  }

  static inline rcompiled_code o(rubinius::CompiledCode* code) {
    return (rcompiled_code)code;
  }

  static inline rmachine_code o(rubinius::MachineCode* mcode) {
    return (rmachine_code)mcode;
  }

  static inline rstring o(rubinius::String* s) {
    return (rstring)s;
  }

  static inline rinteger o(rubinius::Integer* i) {
    return (rinteger)i;
  }

  static inline rtable o(rubinius::LookupTable* i) {
    return (rtable)i;
  }

  static inline rarray o(rubinius::Array* a) {
    return (rarray)a;
  }

  static inline robject s(rubinius::Object* obj) {
    return (robject)obj;
  }

  static inline rubinius::Object* s(robject obj) {
    return (rubinius::Object*)obj;
  }
}

#endif
