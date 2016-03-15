#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  const char *rb_id2name(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return reinterpret_cast<Symbol*>(sym)->cpp_str(env->state()).c_str();
  }

  VALUE rb_id2str(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* str = reinterpret_cast<Symbol*>(sym)->to_str(env->state());
    return env->get_handle(str);
  }

  int rb_is_const_id(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* p = reinterpret_cast<Symbol*>(sym)->is_constant_p(env->state());
    return CBOOL(p) ? Qtrue : Qfalse;
  }

  int rb_is_instance_id(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* p = reinterpret_cast<Symbol*>(sym)->is_ivar_p(env->state());
    return CBOOL(p) ? Qtrue : Qfalse;
  }

  int rb_is_class_id(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* p = reinterpret_cast<Symbol*>(sym)->is_cvar_p(env->state());
    return CBOOL(p) ? Qtrue : Qfalse;
  }

  VALUE rb_sym2str(VALUE sym) {
    return rb_id2str(SYM2ID(sym));
  }
}
