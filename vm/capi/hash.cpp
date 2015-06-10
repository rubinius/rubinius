#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_hash_new() {
    return capi_fast_call(rb_cHash, rb_intern("new"), 0);
  }

  VALUE rb_hash_dup(VALUE self) {
    return capi_fast_call(self, rb_intern("dup"), 0);
  }

  VALUE rb_hash_freeze(VALUE self) {
    return capi_fast_call(self, rb_intern("freeze"), 0);
  }

  VALUE rb_hash_aref(VALUE self, VALUE key) {
    return capi_fast_call(self, rb_intern("[]"), 1, key);
  }

  VALUE rb_hash_aset(VALUE self, VALUE key, VALUE value) {
    return capi_fast_call(self, rb_intern("[]="), 2, key, value);
  }

  VALUE rb_hash_delete(VALUE self, VALUE key) {
    return capi_fast_call(self, rb_intern("delete"), 1, key);
  }

  VALUE rb_hash_delete_if(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE block_handle = env->get_handle(env->block());

    return rb_funcall2b(self, rb_intern("delete_if"), 0, 0, block_handle);
  }

  VALUE rb_hash_size(VALUE self) {
    return capi_fast_call(self, rb_intern("size"), 0);
  }

  VALUE rb_hash_lookup(VALUE self, VALUE key) {
    return rb_hash_lookup2(self, key, Qnil);
  }

  VALUE rb_hash_lookup2(VALUE hash, VALUE key, VALUE def) {
    VALUE entry = capi_fast_call(hash, rb_intern("find_item"), 1, key);

    if(entry != Qnil) {
      return capi_fast_call(entry, rb_intern("value"), 0);
    } else {
      return def;
    }
  }

  void rb_hash_foreach(VALUE self,
                       int (*func)(ANYARGS),
                       VALUE farg)
  {
    VALUE iter = capi_fast_call(self, rb_intern("to_iter"), 0);
    VALUE entry = Qnil;
    VALUE to_delete = rb_ary_new();

    while(RTEST(entry = capi_fast_call(iter, rb_intern("next"), 1, entry))) {
      VALUE key = capi_fast_call(entry, rb_intern("key"), 0);
      VALUE val = capi_fast_call(entry, rb_intern("value"), 0);

      int ret = (*func)(key, val, farg);
      switch(ret) {
      case 0: // ST_CONTINUE:
        continue;
      case 1: // ST_STOP:
        return;
      case 2: // ST_DELETE:
        rb_ary_push(to_delete, key);
        continue;
      default:
        rubinius::bug("unsupported hash_foreach value");
      }
    }

    if(rb_ary_size(to_delete) > 0) {
      for (long i = 0; i < rb_ary_size(to_delete); i++) {
        capi_fast_call(self, rb_intern("delete"), 1, rb_ary_entry(to_delete, i));
      }
    }
  }

  VALUE rb_hash_set_ifnone(VALUE hash, VALUE def) {
    capi_fast_call(hash, rb_intern("default="), 1, def);

    return hash;
  }
}
