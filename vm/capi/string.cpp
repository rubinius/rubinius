#include "capi/capi.hpp"
#include "capi/ruby.h"
#include "capi/ruby/encoding.h"

#include "vm.hpp"
#include "object_memory.hpp"

#include "builtin/byte_array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/native_method.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

#include "util/vsnprintf.h"

#include <string.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
    String* capi_get_string(NativeMethodEnvironment* env, VALUE string) {
      Handle* handle = Handle::from(string);
      String* str = c_as<String>(handle->object());
      handle->flush(env);

      return str;
    }

    void capi_update_string(NativeMethodEnvironment* env, VALUE string) {
      Handle* handle = Handle::from(string);
      handle->update(env);
    }

    /* We use the garbage collector's feature to "pin" an Object at a
     * particular memory location to allow C code to write directly into the
     * contets of a Ruby String (actually, a ByteArray, which provides the
     * storage for String). Since any method on String that mutates self may
     * cause a new ByteArray to be created, we always check whether the
     * String is pinned and update the RString structure unconditionally.
     */
    void ensure_pinned(NativeMethodEnvironment* env, String* string, RString* rstring) {
      ByteArray* ba = string->data();
      size_t byte_size = ba->size();

      if(!ba->pinned_p()) {
        ByteArray* nba = ByteArray::create_pinned(env->state(), byte_size);
        memcpy(nba->raw_bytes(), ba->raw_bytes(), byte_size);
        string->data(env->state(), nba);
        ba = nba;
      }

      char* ptr = reinterpret_cast<char*>(ba->raw_bytes());

      ptr[byte_size-1] = 0;
      rstring->dmwmb = rstring->ptr = ptr;
      rstring->len = string->byte_size();
      rstring->aux.capa = byte_size;
      rstring->aux.shared = Qfalse;
    }

    /* We are in C-land returning to Ruby-land. The value of RString.len
     * may have changed. We raise an exception if the value of len exceeds
     * the capacity of the underlying ByteArray or if the value of ptr has
     * changed.
     */
    void flush_cached_rstring(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rstring()) {
        String* string = c_as<String>(handle->object());
        RString* rstring = handle->get_rstring();

        if(rstring->ptr != rstring->dmwmb) {
          rb_raise(rb_eRuntimeError,
              "changing the value of RSTRING(obj)->ptr is not supported");
        }

        if(rstring->len > c_as<ByteArray>(string->data())->size()) {
          rb_raise(rb_eRuntimeError,
              "RSTRING(obj)->len must be <= capacity of the String");
        } else if(rstring->len != string->num_bytes()->to_native()) {
          // TODO: encoding support will need to define whether ->len
          // means bytes or characters.
          string->num_bytes(env->state(), Fixnum::from(rstring->len));
          string->hash_value(env->state(), nil<Fixnum>());
        }
      }
    }

    /* We were in Ruby-land and we are heading into C-land. In Ruby-land, we
     * may have updated the existing String bytes, appended more, shifted the
     * start of the String, or replaced the ByteArray so we ensure that the
     * RString structure contents are update.
     */
    void update_cached_rstring(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rstring()) {
        String* string = c_as<String>(handle->object());
        RString* rstring = handle->get_rstring();

        ensure_pinned(env, string, rstring);
      }
    }

    RString* Handle::as_rstring(NativeMethodEnvironment* env, int cache_level) {
      String* string = c_as<String>(object());
      bool unset = type_ != cRString;

      if(unset) {
        env->shared().capi_ds_lock().lock();

        // Gotta double check since we're now lock and it might
        // have changed since we asked for the lock.

        unset = type_ != cRString;
        if(unset) {

          string->unshare(env->state());

          RString* str = new RString;
          as_.rstring = str;

          if(cache_level == RSTRING_CACHE_UNSAFE) {
            flush_ = flush_cached_rstring;
            update_ = update_cached_rstring;
            env->check_tracked_handle(this, true);
          } else {
            env->check_tracked_handle(this, false);
          }
          // The underlying String may have changed since we last got the
          // associated RString structure.
          ensure_pinned(env, string, as_.rstring);

          atomic::memory_barrier();
          type_ = cRString;
        }

        env->shared().capi_ds_lock().unlock();
      } else {
        // The underlying String may have changed since we last got the
        // associated RString structure.
        ensure_pinned(env, string, as_.rstring);
      }

      /* In Ruby, regardless of the contents in the ByteArray for a String,
       * the String's size is the authority on the length of the String.
       * However, for the C-API, we cannot assume this because C code may be
       * writing to the cache behind our backs, so we faithfully keep the full
       * contents of the ByteArray and the C cache in sync. If there has never
       * been a cache created for a string, however, we must set put a null
       * byte in the cache based on the String size.
       */
      if(unset) as_.rstring->ptr[string->byte_size()] = 0;

      return as_.rstring;
    }
  }
}

extern "C" {
  struct RString* capi_rstring_struct(VALUE string, int cache_level) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(string);

    return handle->as_rstring(env, cache_level);
  }

  VALUE rb_String(VALUE object) {
    return rb_convert_type(object, -1, "String", "to_s");
  }

  void rb_str_modify(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    string->unshare(env->state());
  }

  VALUE rb_str_freeze(VALUE string) {
    return rb_obj_freeze(string);
  }

  VALUE rb_str_append(VALUE self, VALUE other) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    StringValue(other);
    String* string = capi_get_string(env, self);
    string->append(env->state(), capi_get_string(env, other));
    capi_update_string(env, self);

    return self;
  }

  VALUE rb_str_buf_new(long capacity) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), Fixnum::from(capacity));
    str->num_bytes(env->state(), Fixnum::from(0));

    return env->get_handle(str);
  }

  VALUE rb_str_buf_new2(const char* string) {
    return rb_str_new2(string);
  }

  VALUE rb_str_buf_append(VALUE self, VALUE other) {
    return rb_str_append(self, other);
  }

  VALUE rb_str_buf_cat(VALUE self, const char* other, size_t size) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    string->append(env->state(), other, size);
    capi_update_string(env, self);

    return self;
  }

  VALUE rb_str_buf_cat2(VALUE self, const char* other) {
    return rb_str_buf_cat(self, other, strlen(other));
  }

  VALUE rb_str_cat(VALUE self, const char* other, size_t length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    string->append(env->state(), other, length);
    capi_update_string(env, self);

    return self;
  }

  VALUE rb_str_cat2(VALUE self, const char* other) {
    return rb_str_cat(self, other, strlen(other));
  }

  int rb_str_cmp(VALUE self, VALUE other) {
    return NUM2INT(rb_funcall(self, rb_intern("<=>"), 1, other));
  }

  VALUE rb_str_concat(VALUE self, VALUE other) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(other);

    /* Could be a character code. Only up to 256 supported. */
    if(Fixnum* character = try_as<Fixnum>(object)) {
      char byte = character->to_uint();

      return rb_str_cat(self, &byte, 1);
    }

    return rb_str_append(self, other);
  }

  VALUE rb_str_dup(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    return env->get_handle(string->string_dup(env->state()));
  }

  VALUE rb_str_inspect(VALUE self) {
    return rb_funcall(self, rb_intern("inspect"), 0);
  }

  VALUE rb_str_intern(VALUE self) {
    return rb_funcall(self, rb_intern("to_sym"), 0);
  }

  VALUE rb_str_new(const char* string, long length) {
    if(length < 0) rb_raise(rb_eArgError, "invalid string size");
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return env->get_handle(String::create(env->state(), string, length));
  }

  VALUE rb_str_new2(const char* string) {
    if(string == NULL) {
      rb_raise(rb_eArgError, "NULL pointer given");
    }

    return rb_str_new(string, strlen(string));
  }

  VALUE rb_str_new_cstr(const char* string) {
    return rb_str_new2(string);
  }

  VALUE rb_str_new3(VALUE string) {
    return rb_str_dup(string);
  }

  VALUE rb_str_new4(VALUE string) {
    if(rb_obj_frozen_p(string)) {
      return string;
    }

    VALUE str = rb_obj_freeze(rb_str_dup(string));

    return str;
  }

  VALUE rb_str_new5(VALUE object, const char* ptr, long len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE str = rb_str_new(ptr, len);

    String* new_string = capi_get_string(env, str);
    String* template_string = capi_get_string(env, object);

    new_string->klass(env->state(), template_string->klass());

    return str;
  }

  VALUE rb_str_plus(VALUE self, VALUE other) {
    return rb_str_append(rb_str_dup(self), other);
  }

  VALUE rb_str_resize(VALUE self, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);

    size_t capacity = c_as<ByteArray>(string->data())->size();
    // We make sure we have at least a byte for a trailing NULL
    if(capacity <= len) {
      ByteArray* ba = ByteArray::create_pinned(env->state(), len+1);
      memcpy(ba->raw_bytes(), string->byte_address(), capacity);
      string->data(env->state(), ba);
    }

    string->hash_value(env->state(), nil<Fixnum>());
    string->num_bytes(env->state(), Fixnum::from(len));
    capi_update_string(env, self);
    return self;
  }

  VALUE rb_str_split(VALUE self, const char* separator) {
    return rb_funcall(self, rb_intern("split"), 1, rb_str_new2(separator));
  }

  VALUE rb_str_subseq(VALUE self, size_t starting_index, size_t length) {
    return rb_funcall(self, rb_intern("byteslice"), 2,
                      LONG2NUM(starting_index), LONG2NUM(length) );
  }

  VALUE rb_str_substr(VALUE self, size_t starting_index, size_t length) {
    return rb_funcall(self, rb_intern("slice"), 2,
                      LONG2NUM(starting_index), LONG2NUM(length) );
  }

  VALUE rb_str_times(VALUE self, VALUE times) {
    return rb_funcall(self, rb_intern("*"), 1, times);
  }

  VALUE rb_str2inum(VALUE self, int base) {
    return rb_funcall(self, rb_intern("to_i"), 1, INT2NUM(base));
  }

  VALUE rb_str_to_str(VALUE object) {
    return rb_convert_type(object, -1, "String", "to_str");
  }

  VALUE rb_string_value(volatile VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(!kind_of<String>(env->get_object(*object_variable))) {
      *object_variable = rb_str_to_str(*object_variable);
    }

    return *object_variable;
  }

  char* rb_string_value_ptr(volatile VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE str = rb_string_value(object_variable);
    // Type check.
    (void)c_as<String>(env->get_object(str));

    return RSTRING_PTR(str);
  }

  char* rb_string_value_cstr(volatile VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE str = rb_string_value(object_variable);
    String* string = capi_get_string(env, str);

    if(string->byte_size() != (native_int)strlen(string->c_str(env->state()))) {
      rb_raise(rb_eArgError, "string contains NULL byte");
    }

    return RSTRING_PTR(str);
  }

  VALUE rb_tainted_str_new2(const char* string) {
    if(string == NULL) {
      rb_raise(rb_eArgError, "NULL pointer given");
    }

    return rb_tainted_str_new(string, strlen(string));
  }

  VALUE rb_tainted_str_new(const char* string, long size) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), string, size);
    str->taint(env->state());

    return env->get_handle(str);
  }

  char* rb_str2cstr(VALUE string, long* len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    StringValue(string);
    String* str = capi_get_string(env, string);
    char *ptr = RSTRING_PTR(string);
    if(len) {
      *len = str->byte_size();
    }
    return ptr;
  }

  char* rb_str_ptr(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(self);
    RString* rstring = handle->as_rstring(env, RSTRING_CACHE_SAFE);

    return rstring->ptr;
  }

  char* rb_str_copied_ptr(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = c_as<String>(env->get_object(self));

    char* data = (char*)malloc(sizeof(char) * str->byte_size() + 1);
    memcpy(data, str->c_str(env->state()), str->byte_size());
    data[str->byte_size()] = 0;

    return data;
  }
#define HAVE_RB_STR_COPIED_PTR 1

  char* rb_str_ptr_readonly(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    RString* rstr = Handle::from(self)->as_rstring(env, RSTRING_CACHE_SAFE);

    return rstr->ptr;
  }

  char* rb_str_ptr_readonly_end(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    RString* rstr = Handle::from(self)->as_rstring(env, RSTRING_CACHE_SAFE);

    return rstr->ptr + rstr->len;
  }

  long rb_str_len(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    return string->byte_size();
  }

  VALUE rb_str_length(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    return LONG2FIX(string->char_size(env->state()));
  }

  void rb_str_set_len(VALUE self, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);

    size_t capacity = c_as<ByteArray>(string->data())->size();
    if(len > capacity) len = capacity - 1;

    string->byte_address()[len] = 0;
    string->num_bytes(env->state(), Fixnum::from(len));
    capi_update_string(env, self);
  }

  long rb_str_hash(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);

    return string->hash_string(env->state());
  }

  void rb_str_update(VALUE self, long beg, long end, VALUE replacement) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    Class* klass = Module::mirror(env->state(), string);

    VALUE mirror = capi_fast_call(env->get_handle(klass), rb_intern("new"), 1, self);
    capi_fast_call(mirror, rb_intern("splice"), 3, LONG2FIX(beg), LONG2FIX(end), replacement);
  }

  VALUE rb_str_equal(VALUE self, VALUE other) {
    if(self == other) {
      return Qtrue;
    }
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* string = capi_get_string(env, self);
    String* other_str = capi_get_string(env, other);

    return env->get_handle(string->equal(env->state(), other_str));
  }

  void rb_str_free(VALUE str) {}

  VALUE rb_enc_str_new(const char *ptr, long len, rb_encoding *enc)
  {
    VALUE str = rb_str_new(ptr, len);
    rb_enc_associate(str, enc);
    return str;
  }

  VALUE rb_usascii_str_new(const char* ptr, long len) {
    return rb_enc_str_new(ptr, len, rb_usascii_encoding());
  }

  VALUE rb_usascii_str_new2(const char* ptr) {
    return rb_enc_str_new(ptr, strlen(ptr), rb_usascii_encoding());
  }

  VALUE rb_external_str_new_with_enc(const char* string, long size, rb_encoding* encoding) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), string, size);
    str->taint(env->state());

    Encoding* enc = Encoding::find(env->state(), encoding->name);
    if(enc == Encoding::usascii_encoding(env->state())
       && !CBOOL(str->ascii_only_p(env->state()))) {
      str->encoding(env->state(), Encoding::ascii8bit_encoding(env->state()));
    } else {
      str->encoding(env->state(), enc);
    }

    return rb_str_conv_enc(env->get_handle(str), enc->get_encoding(),
                           rb_default_internal_encoding());
  }

  VALUE rb_external_str_new(const char* string, long size) {
    return rb_external_str_new_with_enc(string, size, rb_default_external_encoding());
  }

  VALUE rb_external_str_new_cstr(const char* string) {
    return rb_external_str_new_with_enc(string, strlen(string), rb_default_external_encoding());
  }

  VALUE rb_str_encode(VALUE str, VALUE to, int ecflags, VALUE ecopts) {
    return rb_funcall(rb_mCAPI, rb_intern("rb_str_encode"), 4,
                      str, to, INT2FIX(ecflags), ecopts);
  }

  int rb_enc_str_coderange(VALUE string) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String *str = c_as<String>(env->get_object(string));

    bool valid = CBOOL(str->valid_encoding_p(env->state()));
    bool ascii = CBOOL(str->ascii_only_p(env->state()));

    if(valid && ascii) {
      return ENC_CODERANGE_7BIT;
    } else if(valid) {
      return ENC_CODERANGE_VALID;
    } else {
      return ENC_CODERANGE_BROKEN;
    }
  }

  VALUE rb_locale_str_new_cstr(const char *ptr) {
    return rb_external_str_new_with_enc(ptr, strlen(ptr), rb_locale_encoding());
  }

  VALUE rb_locale_str_new(const char* ptr, long len) {
    return rb_external_str_new_with_enc(ptr, len, rb_locale_encoding());
  }

  VALUE rb_str_conv_enc_opts(VALUE str, rb_encoding* from, rb_encoding* to,
                             int ecflags, VALUE ecopts)
  {
    VALUE f = rb_enc_from_encoding(from);
    VALUE t = to ? rb_enc_from_encoding(to) : Qnil;

    return rb_funcall(rb_mCAPI, rb_intern("rb_str_conv_enc_opts"), 5,
                      str, f, t, INT2FIX(ecflags), ecopts);
  }

  VALUE rb_str_conv_enc(VALUE str, rb_encoding *from, rb_encoding *to) {
    return rb_str_conv_enc_opts(str, from, to, 0, Qnil);
  }

  VALUE rb_str_export_to_enc(VALUE str, rb_encoding *enc) {
    return rb_str_conv_enc(str, rb_enc_get(str), enc);
  }

  VALUE rb_str_export(VALUE str) {
    return rb_str_conv_enc(str, rb_enc_get(str), rb_default_external_encoding());
  }

  VALUE rb_str_export_locale(VALUE str) {
    return rb_str_conv_enc(str, rb_enc_get(str), rb_locale_encoding());
  }

  VALUE rb_sprintf(const char* format, ...) {
    va_list varargs;

    ssize_t length = strlen(format) << 1;
    ssize_t err = 0;
    VALUE result = Qnil;

    do {
      result = rb_str_buf_new(length);
      char* buf = RSTRING_PTR(result);
      va_start(varargs, format);
      err = ruby_vsnprintf(buf, length, format, varargs);
      va_end(varargs);
      if(err >= length) {
        length = err + 1;
        err = -1;
      } else if(err < 0) {
        length <<= 1;
      }
    } while(err < 0);

    rb_str_set_len(result, err);
    return result;
  }
}
