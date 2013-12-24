#include "vm.hpp"
#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "builtin/regexp.hpp"
#include "builtin/string.hpp"
#include "object_memory.hpp"
#include "primitives.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "util/spinlock.hpp"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_reg_new(const char *source, long len, int options) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String *pat = String::create(env->state(), source, len);
    return rb_funcall(rb_cRegexp, rb_intern("new"), 2, env->get_handle(pat), Fixnum::from(options));
  }

  VALUE rb_reg_nth_match(long nth, VALUE match_data) {
    if(NIL_P(match_data)) {
      return Qnil;
    }
    return rb_funcall(match_data, rb_intern("[]"), 1, Fixnum::from(nth));
  }

  VALUE rb_reg_source(VALUE r) {
    return rb_String(rb_funcall(r, rb_intern("source"), 0));
  }

  int rb_reg_options(VALUE r) {
    return FIX2INT(rb_funcall(r, rb_intern("options"), 0));
  }

  VALUE rb_reg_regcomp(VALUE str) {
    return rb_funcall(rb_cRegexp, rb_intern("new"), 1, str);
  }

  VALUE rb_reg_match(VALUE re, VALUE str) {
    return rb_funcall(re, rb_intern("=~"), 1, str);
  }

  VALUE rb_backref_get(void) {
    return rb_gv_get("$~");
  }

  static int onig_lock = RBX_SPINLOCK_UNLOCKED;

  void capi_reg_lock() {
    rbx_spinlock_lock(&onig_lock);
  }

  void capi_reg_unlock() {
    rbx_spinlock_unlock(&onig_lock);
  }
}
