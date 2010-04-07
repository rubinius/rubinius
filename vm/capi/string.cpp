#include "builtin/bytearray.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include <cstring>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    String* capi_get_string(NativeMethodEnvironment* env, VALUE str_handle) {
      if(!env) env = NativeMethodEnvironment::get();

      Handle* handle = Handle::from(str_handle);
      String* string = c_as<String>(handle->object());

      return string;
    }

    void repin_string(NativeMethodEnvironment* env, String* string, RString* str) {
      size_t size = string->size();

      ByteArray* ba = string->data();

      char* ptr = 0;
      if(ba->pinned_p()) {
        ptr = reinterpret_cast<char*>(ba->raw_bytes());
      } else {
        ByteArray* new_ba = ByteArray::create_pinned(env->state(), size + 1);
        std::memcpy(new_ba->raw_bytes(), string->byte_address(), size);
        string->data(env->state(), new_ba);

        ptr = reinterpret_cast<char*>(new_ba->raw_bytes());
      }

      ptr[size] = 0;

      str->dmwmb = str->ptr = ptr;
      str->aux.capa = str->len = size;
    }

    void capi_update_string(NativeMethodEnvironment* env, VALUE str_handle) {
      if(!env) env = NativeMethodEnvironment::get();

      Handle* handle = Handle::from(str_handle);

      RString* str = handle->as_rstring(env);

      String* string = c_as<String>(handle->object());

      repin_string(env, string, str);
    }

    RString* Handle::as_rstring(NativeMethodEnvironment* env) {
      String* string = c_as<String>(object());

      if(type_ != cRString) {
        string->unshare(env->state());

        RString* str = new RString;
        str->aux.shared = Qfalse;

        type_ = cRString;
        as_.rstring = str;
      }

      // We always repin because the ByteArray could have changed between
      // the last time we used this handle and now.
      repin_string(env, string, as_.rstring);

      return as_.rstring;
    }
  }
}

extern "C" {
  struct RString* capi_rstring_struct(VALUE str_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(str_handle);
    RString* rstring = handle->as_rstring(env);

    return rstring;
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
    return rb_str_buf_cat(self_handle, other, std::strlen(other));
  }

  VALUE rb_str_cat(VALUE self_handle, const char* other, size_t length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = capi_get_string(env, self_handle);
    self->append(env->state(), other, length);
    capi_update_string(env, self_handle);

    return self_handle;
  }

  VALUE rb_str_cat2(VALUE self_handle, const char* other) {
    return rb_str_cat(self_handle, other, std::strlen(other));
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

    return rb_str_new(string, std::strlen(string));
  }

  VALUE rb_str_plus(VALUE self_handle, VALUE other_handle) {
    return rb_str_append(rb_str_dup(self_handle), other_handle);
  }

  VALUE rb_str_resize(VALUE self_handle, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = capi_get_string(env, self_handle);

    size_t size = string->size();
    if(size != len) {
      if(size < len) {
        ByteArray* ba = ByteArray::create_pinned(env->state(), len+1);
        std::memcpy(ba->raw_bytes(), string->byte_address(), size);
        string->data(env->state(), ba);
      }

      string->byte_address()[len] = 0;
      string->num_bytes(env->state(), Fixnum::from(len));
      string->characters(env->state(), Fixnum::from(len));
      string->hash_value(env->state(), reinterpret_cast<Integer*>(RBX_Qnil));
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

  VALUE rb_string_value(VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(!kind_of<String>(env->get_object(*object_variable))) {
      *object_variable = rb_str_to_str(*object_variable);
    }

    return *object_variable;
  }

  char* rb_string_value_ptr(VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE str = rb_string_value(object_variable);
    // Type check.
    (void)c_as<String>(env->get_object(str));

    return RSTRING_PTR(str);
  }

  char* rb_string_value_cstr(VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE str = rb_string_value(object_variable);
    String* string = capi_get_string(env, str);

    if(string->size() != strlen(string->c_str())) {
      rb_raise(rb_eArgError, "string contains NULL byte");
    }

    return RSTRING_PTR(str);
  }

  VALUE rb_tainted_str_new2(const char* string) {
    if(string == NULL) {
      rb_raise(rb_eArgError, "NULL pointer given");
    }

    return rb_tainted_str_new(string, std::strlen(string));
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
    } else if(RTEST(ruby_verbose) && string->size() != strlen(ptr)) {
      rb_warn("string contains \\0 character");
    }
    return ptr;
  }

  char* rb_str_ptr(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(self);
    RString* rstring = handle->as_rstring(env);

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

    RString* rstr = Handle::from(self)->as_rstring(env);

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
    if(string->size() > len) {
      string->byte_address()[len] = 0;
      string->num_bytes(env->state(), Fixnum::from(len));
    }
  }
}
