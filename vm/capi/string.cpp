#include "builtin/bytearray.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

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
          atomic::memory_barrier();
          type_ = cRString;
        }

        env->shared().capi_ds_lock().unlock();
      }

      // The underlying String may have changed since we last got the
      // associated RString structure.
      ensure_pinned(env, string, as_.rstring);

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
    return rb_convert_type(object, 0, "String", "to_s");
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
    return rb_convert_type(object, 0, "String", "to_str");
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
}
