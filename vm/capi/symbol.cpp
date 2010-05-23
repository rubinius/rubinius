#include "builtin/symbol.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  const char *rb_id2name(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return reinterpret_cast<Symbol*>(sym)->c_str(env->state());
  }

  int rb_is_const_id(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return RBX_RTEST(reinterpret_cast<Symbol*>(sym)->is_constant_p(env->state())) ? Qtrue : Qfalse;
  }

  int rb_is_instance_id(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return RBX_RTEST(reinterpret_cast<Symbol*>(sym)->is_ivar_p(env->state())) ? Qtrue : Qfalse;
  }

  int rb_is_class_id(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return RBX_RTEST(reinterpret_cast<Symbol*>(sym)->is_cvar_p(env->state())) ? Qtrue : Qfalse;
  }
}
