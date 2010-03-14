#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_hash_new() {
    return rb_funcall(rb_cHash, rb_intern("new"), 0);
  }

  VALUE rb_hash_aref(VALUE self, VALUE key) {
    return rb_funcall(self, rb_intern("[]"), 1, key);
  }

  VALUE rb_hash_aset(VALUE self, VALUE key, VALUE value) {
    return rb_funcall(self, rb_intern("[]="), 2, key, value);
  }

  VALUE rb_hash_delete(VALUE self, VALUE key) {
    return rb_funcall(self, rb_intern("delete"), 1, key);
  }

  VALUE rb_hash_size(VALUE self) {
    return rb_funcall(self, rb_intern("size"), 0);
  }

  VALUE rb_hash_lookup(VALUE self, VALUE key) {
    VALUE entry = rb_funcall(self, rb_intern("find_entry"), 1, key);
    if(entry != Qnil) {
      return rb_funcall(entry, rb_intern("value"), 0);
    } else {
      return Qnil;
    }
  }

  void rb_hash_foreach(VALUE self,
                       int (*func)(ANYARGS),
                       VALUE farg)
  {
    VALUE iter = rb_funcall(self, rb_intern("to_iter"), 0);
    VALUE entry = Qnil;

    while(RTEST(entry = rb_funcall(iter, rb_intern("next"), 1, entry))) {
      VALUE key = rb_funcall(entry, rb_intern("key"), 0);
      VALUE val = rb_funcall(entry, rb_intern("value"), 0);

      int ret = (*func)(key, val, farg);
      switch(ret) {
      case 0: // ST_CONTINUE:
        continue;
      case 1: // ST_STOP:
        return;
      default:
        rubinius::bug("unsupported hash_foreach value");
      }
    }
  }
}
