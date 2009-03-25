#include "builtin/bytearray.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_String(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "String", "to_s");
  }

  VALUE rb_str_append(VALUE self_handle, VALUE other_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = c_as<String>(env->get_object(self_handle));
    self->append(env->state(), c_as<String>(env->get_object(other_handle)));

    return self_handle;
  }

  VALUE rb_str_buf_new(long capacity) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), Fixnum::from(capacity));

    return env->get_handle(str);
  }

  VALUE rb_str_buf_append(VALUE self_handle, VALUE other_handle) {
    return rb_str_append(self_handle, other_handle);
  }

  VALUE rb_str_buf_cat(VALUE string_handle, const char* other, size_t size) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = c_as<String>(env->get_object(string_handle));
    string->append(env->state(), other, size);

    return string_handle;
  }

  VALUE rb_str_buf_cat2(VALUE string_handle, const char* other) {
    return rb_str_buf_cat(string_handle, other, std::strlen(other));
  }

  VALUE rb_str_cat(VALUE self_handle, const char* other, size_t length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = c_as<String>(env->get_object(self_handle));
    String* combo = self->string_dup(env->state());

    return env->get_handle(combo->append(env->state(), other, length));
  }

  VALUE rb_str_cat2(VALUE string_handle, const char* other) {
    return rb_str_cat(string_handle, other, std::strlen(other));
  }

  int rb_str_cmp(VALUE first_handle, VALUE second_handle) {
    return NUM2INT(rb_funcall(first_handle, rb_intern("<=>"), 1, second_handle));
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

    String* self = c_as<String>(env->get_object(self_handle));
    return env->get_handle(self->string_dup(env->state()));
  }

  /** @todo Refactor into a String::replace(). --rue */
  void rb_str_flush_char_ptr(VALUE string_handle, char* c_string, size_t length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    ByteArray* data = ByteArray::create(env->state(), (length + 1));
    std::memcpy(data->bytes, c_string, length);
    data->bytes[length] = '\0';

    Integer* bytes = Integer::from(env->state(), length);

    String* string = c_as<String>(env->get_object(string_handle));
    string->num_bytes(env->state(), bytes);
    string->characters(env->state(), bytes);
    string->hash_value(env->state(), reinterpret_cast<Integer*>(Qnil));
    /* Assume the encoding stays the same. */

    string->data(env->state(), data);
  }

  char rb_str_get_char(VALUE self_handle, int offset) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* self = c_as<String>(env->get_object(self_handle));

    if (offset < 0 || (unsigned)offset >= self->size()) {
      return '\0';
    }

    return self->c_str()[offset];
  }

  size_t rb_str_get_char_len(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    /* @todo Is this correct? Is assuming no wide characters valid? */
    return c_as<String>(env->get_object(self_handle))->size();
  }

  char* rb_str_get_char_ptr(VALUE str_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = c_as<String>(env->get_object(str_handle));
    size_t length = string->size();

    char* buffer = ALLOC_N(char, (length + 1));
    std::memcpy(buffer, string->c_str(), length);
    buffer[length] = '\0';

    return buffer;
  }

  VALUE rb_str_new(const char* string, size_t length) {
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
    return rb_String(object_handle);
  }

  VALUE rb_string_value(VALUE* object_variable) {
    *object_variable = rb_obj_as_string(*object_variable);
    return *object_variable;
  }

  char* rb_string_value_cstr(VALUE* object_variable) {
    return rb_str_get_char_ptr(rb_string_value(object_variable));
  }

  VALUE rb_tainted_str_new2(const char* string) {
    if(string == NULL) {
      rb_raise(rb_eArgError, "NULL pointer given");
    }

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), string, std::strlen(string));
    str->taint(env->state());

    return env->get_handle(str);

  }
}
