#ifndef RBX_INSTRUMENTS_RBXTI_INTERNALS_HPP
#define RBX_INSTRUMENTS_RBXTI_INTERNALS_HPP

#include "capi/18/include/rbxti.hpp"

namespace rubinius {
  class VM;
  class Module;
  class Symbol;
  class CompiledMethod;
  class String;
  class Integer;
  class LookupTable;
  class Array;
  class Object;
}

namespace rbxti {
  Env* create_env(rubinius::VM*);

  static inline rubinius::Module* i(rmodule mod) {
    return (rubinius::Module*)mod;
  }

  static inline rubinius::Symbol* i(rsymbol sym) {
    return (rubinius::Symbol*)sym;
  }

  static inline rubinius::CompiledMethod* i(rmethod m) {
    return (rubinius::CompiledMethod*)m;
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

  static inline rmethod o(rubinius::CompiledMethod* m) {
    return (rmethod)m;
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
