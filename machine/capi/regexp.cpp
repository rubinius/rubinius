#include "thread_state.hpp"
#include "object_utils.hpp"
#include "spinlock.hpp"

#include "class/array.hpp"
#include "class/fixnum.hpp"
#include "class/io.hpp"
#include "class/regexp.hpp"
#include "class/string.hpp"
#include "memory.hpp"
#include "primitives.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_reg_new(const char *source, long len, int options) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String *pat = String::create_pinned(env->state(), source, len);
    return rb_funcall(rb_cRegexp, rb_intern("new"), 2,
        MemoryHandle::value(pat), Fixnum::from(options));
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

  static locks::spinlock_mutex onig_lock;

  void capi_reg_lock() {
    onig_lock.lock();
  }

  void capi_reg_unlock() {
    onig_lock.unlock();
  }
}
