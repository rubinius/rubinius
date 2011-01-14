#include "builtin/bytearray.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include <string.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    String* capi_get_string(NativeMethodEnvironment* env, VALUE str_handle) {
      if(!env) env = NativeMethodEnvironment::get();

      Handle* handle = Handle::from(str_handle);
      String* string = c_as<String>(handle->object());
      handle->flush(env);

      return string;
    }

    void capi_update_string(NativeMethodEnvironment* env, VALUE ary_handle) {
      if(!env) env = NativeMethodEnvironment::get();

      Handle* handle = Handle::from(ary_handle);
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
        ba = ByteArray::create_pinned(env->state(), byte_size);
        memcpy(ba->raw_bytes(), string->byte_address(), byte_size);
        string->data(env->state(), ba);
      }

      char* ptr = reinterpret_cast<char*>(ba->raw_bytes());

      ptr[byte_size-1] = 0;
      rstring->dmwmb = rstring->ptr = ptr;
      rstring->len = string->size();
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

        if(rstring->len > string->data()->size()) {
          rb_raise(rb_eRuntimeError,
              "RSTRING(obj)->len must be <= capacity of the String");
        } else if(rstring->len != string->num_bytes()->to_native()) {
          // TODO: encoding support will need to define whether ->len
          // means bytes or characters.
          string->num_bytes(env->state(), Fixnum::from(rstring->len));
          string->characters(env->state(), Fixnum::from(rstring->len));
          string->hash_value(env->state(), reinterpret_cast<Fixnum*>(RBX_Qnil));
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

      if(type_ != cRString) {
        type_ = cRString;

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
      }

      // The underlying String may have changed since we last got the
      // associated RString structure.
      ensure_pinned(env, string, as_.rstring);

      return as_.rstring;
    }
  }
}

extern "C" {
  struct RString* capi_rstring_struct(VALUE str_handle, int cache_level) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(str_handle);

    return handle->as_rstring(env, cache_level);
  }

  VALUE rb_String(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "String", "to_s");
  }

  void rb_str_modify(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = capi_get_string(env, self_handle);
    self->unshare(env->state());
  }

  VALUE rb_str_freeze(VALUE str) {
    return rb_obj_freeze(str);
  }

  VALUE rb_str_append(VALUE self_handle, VALUE other_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = capi_get_string(env, self_handle);
    self->append(env->state(), capi_get_string(env, other_handle));
    capi_update_string(env, self_handle);

    return self_handle;
  }

  VALUE rb_str_buf_new(long capacity) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), Fixnum::from(capacity));
    str->num_bytes(env->state(), Fixnum::from(0));

    return env->get_handle(str);
  }

  VALUE rb_str_buf_append(VALUE self_handle, VALUE other_handle) {
    return rb_str_append(self_handle, other_handle);
  }

  VALUE rb_str_buf_cat(VALUE self_handle, const char* other, size_t size) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self_handle);
    string->append(env->state(), other, size);
    capi_update_string(env, self_handle);

    return self_handle;
  }

  VALUE rb_str_buf_cat2(VALUE self_handle, const char* other) {
    return rb_str_buf_cat(self_handle, other, strlen(other));
  }

  VALUE rb_str_cat(VALUE self_handle, const char* other, size_t length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = capi_get_string(env, self_handle);
    self->append(env->state(), other, length);
    capi_update_string(env, self_handle);

    return self_handle;
  }

  VALUE rb_str_cat2(VALUE self_handle, const char* other) {
    return rb_str_cat(self_handle, other, strlen(other));
  }

  int rb_str_cmp(VALUE self_handle, VALUE other_handle) {
    return NUM2INT(rb_funcall(self_handle, rb_intern("<=>"), 1, other_handle));
  }

  VALUE rb_str_concat(VALUE self_handle, VALUE other_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* other = env->get_object(other_handle);

    /* Could be a character code. Only up to 256 supported. */
    if(Fixnum* character = try_as<Fixnum>(other)) {
      char byte = character->to_uint();

      return rb_str_cat(self_handle, &byte, 1);
    }

    return rb_str_append(self_handle, other_handle);
  }

  VALUE rb_str_dup(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = capi_get_string(env, self_handle);
    return env->get_handle(self->string_dup(env->state()));
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

  VALUE rb_str_new3(VALUE string) {
    return rb_str_dup(string);
  }

  VALUE rb_str_new4(VALUE string) {
    if(rb_obj_frozen_p(string))
      return string;

    VALUE str = rb_obj_freeze(rb_str_dup(string));

    return str;
  }

  VALUE rb_str_plus(VALUE self_handle, VALUE other_handle) {
    return rb_str_append(rb_str_dup(self_handle), other_handle);
  }

  VALUE rb_str_resize(VALUE self_handle, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self_handle);

    size_t size = string->data()->size();
    if(size != len) {
      if(size < len) {
        ByteArray* ba = ByteArray::create_pinned(env->state(), len+1);
        memcpy(ba->raw_bytes(), string->byte_address(), size);
        string->data(env->state(), ba);
      }

      string->byte_address()[len] = 0;
      string->num_bytes(env->state(), Fixnum::from(len));
      string->characters(env->state(), Fixnum::from(len));
      string->hash_value(env->state(), reinterpret_cast<Fixnum*>(RBX_Qnil));
    }
    capi_update_string(env, self_handle);

    return self_handle;
  }

  VALUE rb_str_split(VALUE self_handle, const char* separator) {
    return rb_funcall(self_handle, rb_intern("split"), 1, rb_str_new2(separator));
  }

  VALUE rb_str_substr(VALUE self_handle, size_t starting_index, size_t length) {
    return rb_funcall(self_handle, rb_intern("slice"), 2,
                      LONG2NUM(starting_index), LONG2NUM(length) );
  }

  VALUE rb_str_times(VALUE self_handle, VALUE times) {
    return rb_funcall(self_handle, rb_intern("*"), 1, times);
  }

  VALUE rb_str2inum(VALUE self_handle, int base) {
    return rb_funcall(self_handle, rb_intern("to_i"), 1, INT2NUM(base));
  }

  VALUE rb_str_to_str(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "String", "to_str");
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

    if(string->size() != (native_int)strlen(string->c_str())) {
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

  char* rb_str2cstr(VALUE str_handle, long* len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    StringValue(str_handle);
    String* string = capi_get_string(env, str_handle);
    char *ptr = RSTRING_PTR(str_handle);
    if(len) {
      *len = string->size();
    } else if(RTEST(ruby_verbose) && string->size() != (native_int)strlen(ptr)) {
      rb_warn("string contains NULL character");
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

    char* data = (char*)malloc(sizeof(char) * str->size() + 1);
    memcpy(data, str->c_str(), str->size());
    data[str->size()] = 0;

    return data;
  }
#define HAVE_RB_STR_COPIED_PTR 1

  void rb_str_flush(VALUE self) {
    // Using pinned ByteArray, we don't need this anymore.
  }

  void rb_str_update(VALUE self) {
    // Using pinned ByteArray, we don't need this anymore.
  }

  char* rb_str_ptr_readonly(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    RString* rstr = Handle::from(self)->as_rstring(env, RSTRING_CACHE_SAFE);

    return rstr->ptr;
  }

  size_t rb_str_len(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);
    return string->size();
  }

  void rb_str_set_len(VALUE self, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);

    size_t byte_size = string->data()->size();
    if(len > byte_size) len = byte_size - 1;

    string->byte_address()[len] = 0;
    string->num_bytes(env->state(), Fixnum::from(len));
    capi_update_string(env, self);
  }

  long rb_str_hash(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self);

    return string->hash_string(env->state());
  }
}
