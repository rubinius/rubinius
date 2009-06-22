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
}
