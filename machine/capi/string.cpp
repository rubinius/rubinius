#include "capi/capi.hpp"
#include "capi/ruby.h"
#include "capi/ruby/encoding.h"

#include "thread_state.hpp"
#include "memory.hpp"

#include "class/byte_array.hpp"
#include "class/fixnum.hpp"
#include "class/integer.hpp"
#include "class/native_method.hpp"
#include "class/object.hpp"
#include "class/string.hpp"

#include "util/vsnprintf.h"

#include <string.h>
#include <sys/mman.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  RString* MemoryHandle::get_rstring(STATE) {
    if(rstring_p()) {
      return reinterpret_cast<RString*>(data());
    } else if(object_type_p()) {
      String* string = c_as<String>(object());
      string->set_type_specific(state, String::eRString);
      string->unshare(state);

      ByteArray* byte_array = string->data();

      if(!byte_array->pinned_p()) {
        byte_array = ByteArray::copy_pinned(state, byte_array);
        string->data(byte_array);
      }

      char* ptr = reinterpret_cast<char*>(byte_array->raw_bytes());

      RString* rstring = new RString;

      rstring->dmwmb = rstring->ptr = ptr;
      rstring->len = string->byte_size();
      rstring->aux.capa = byte_array->size();
      rstring->aux.shared = Qfalse;

      rstring->ptr[string->byte_size()] = 0;

      set_rstring(rstring);
      return rstring;
    } else {
      Exception::raise_runtime_error(state, "C-API handle invalid reference as RString");
    }
  }

  void MemoryHandle::read_rstring(STATE) {
    if(rstring_p()) {
      String* string = as<String>(object());
      RString* rstring = reinterpret_cast<RString*>(data());

      if(string->get_byte_size() != rstring->len) {
        string->num_bytes(Fixnum::from(rstring->len));
      }
    }
  }

  void MemoryHandle::write_rstring(STATE) {
    if(rstring_p()) {
      String* string = as<String>(object());
      RString* rstring = reinterpret_cast<RString*>(data());

      ByteArray* byte_array = string->data();
      if(!byte_array->pinned_p()) byte_array->set_pinned(state);

      char* ptr = reinterpret_cast<char*>(byte_array->raw_bytes());

      rstring->dmwmb = rstring->ptr = ptr;
      rstring->len = string->get_byte_size();
      rstring->aux.capa = byte_array->size();
      rstring->aux.shared = Qfalse;

      rstring->ptr[string->get_byte_size()] = 0;
    }
  }
}

extern "C" {
  struct RString* capi_rstring_struct(VALUE string, int cache_level) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return MemoryHandle::from(string)->get_rstring(env->state());
  }

  void* rb_alloc_tmp_buffer(VALUE* store, long len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE s =  MemoryHandle::value(String::create_pinned(env->state(), Fixnum::from(len)));
    *store = s;

    return RSTRING_PTR(s);
  }

  void rb_free_tmp_buffer(VALUE* store)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE s = *store;
    *store = 0;

    String* str = MemoryHandle::object<String>(s);
    str->num_bytes(env->state(), Fixnum::from(0));
    str->data(env->state(), ByteArray::create(env->state(), 1));
  }

  uint64_t rb_memhash(const void *ptr, long len) {
    return String::siphash(reinterpret_cast<const unsigned char*>(ptr), len, 0);
  }

  void rb_str_modify(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);
    string->unshare(env->state());
  }

  void rb_str_modify_expand(VALUE self, long expand) {
    if (expand < 0) {
      rb_raise(rb_eArgError, "negative expanding string size");
    }

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* string = MemoryHandle::object<String>(self);

    string->unshare(env->state());

    size_t new_len = string->byte_size() + expand;

    if(new_len > FIXNUM_MAX) {
      rb_raise(rb_eArgError, "string size too big");
    }

    ByteArray* bytes = c_as<ByteArray>(string->data());
    size_t capacity = bytes->size();
    if(capacity >= new_len) return;

    rb_encoding* enc = rb_enc_get(self);

    ByteArray* new_bytes = ByteArray::create_pinned(env->state(),
        new_len + rb_enc_mbminlen(enc));

    memcpy(new_bytes->raw_bytes(), bytes->raw_bytes(), string->byte_size() + 1);
    string->data(env->state(), new_bytes);
  }

  VALUE rb_str_freeze(VALUE string) {
    return rb_obj_freeze(string);
  }

  VALUE rb_str_append(VALUE self, VALUE other) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    StringValue(other);
    String* string = MemoryHandle::object<String>(self);
    String* another = MemoryHandle::object<String>(other);
    string->append(env->state(), another);

    return self;
  }

  VALUE rb_str_buf_new(long capacity) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create_pinned(env->state(), Fixnum::from(capacity));
    str->num_bytes(env->state(), Fixnum::from(0));

    return MemoryHandle::value(str);
  }

  VALUE rb_str_buf_new2(const char* string) {
    return rb_str_new2(string);
  }

  VALUE rb_str_buf_append(VALUE self, VALUE other) {
    return rb_str_append(self, other);
  }

  VALUE rb_str_buf_cat(VALUE self, const char* other, size_t size) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);
    string->append(env->state(), other, size);

    return self;
  }

  VALUE rb_str_buf_cat2(VALUE self, const char* other) {
    return rb_str_buf_cat(self, other, strlen(other));
  }

  VALUE rb_str_cat(VALUE self, const char* other, size_t length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);
    string->append(env->state(), other, length);

    return self;
  }

  VALUE rb_str_cat2(VALUE self, const char* other) {
    return rb_str_cat(self, other, strlen(other));
  }

  int rb_str_cmp(VALUE self, VALUE other) {
    return NUM2INT(rb_funcall(self, rb_intern("<=>"), 1, other));
  }

  VALUE rb_str_concat(VALUE self, VALUE other) {
    /* Could be a character code. Only up to 256 supported. */
    if(Fixnum* character = MemoryHandle::try_as<Fixnum>(other)) {
      char byte = character->to_uint();

      return rb_str_cat(self, &byte, 1);
    }

    return rb_str_append(self, other);
  }

  VALUE rb_str_dup(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);
    return MemoryHandle::value(string->string_dup(env->state()));
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

    return MemoryHandle::value(String::create_pinned(env->state(), string, length));
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

    String* new_string = MemoryHandle::object<String>(str);
    String* template_string = MemoryHandle::object<String>(object);

    new_string->klass(env->state(), template_string->klass());

    return str;
  }

  VALUE rb_str_plus(VALUE self, VALUE other) {
    return rb_str_append(rb_str_dup(self), other);
  }

  VALUE rb_str_resize(VALUE self, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);

    size_t capacity = c_as<ByteArray>(string->data())->size();
    // We make sure we have at least a byte for a trailing NULL
    if(capacity <= len) {
      ByteArray* ba = ByteArray::create_pinned(env->state(), len+1);
      memcpy(ba->raw_bytes(), string->byte_address(), capacity);
      string->data(env->state(), ba);
    }

    string->hash_value(env->state(), nil<Fixnum>());
    string->num_bytes(env->state(), Fixnum::from(len));
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
    return rb_convert_type(object, T_STRING, "String", "to_str");
  }

  VALUE rb_string_value(volatile VALUE* object_variable) {
    if(!MemoryHandle::try_as<String>(*object_variable)) {
      *object_variable = rb_str_to_str(*object_variable);
    }

    return *object_variable;
  }

  char* rb_string_value_ptr(volatile VALUE* object_variable) {
    VALUE str = rb_string_value(object_variable);

    return RSTRING_PTR(str);
  }

  char* rb_string_value_cstr(volatile VALUE* object_variable) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE str = rb_string_value(object_variable);
    String* string = MemoryHandle::object<String>(str);

    if(string->byte_size() != (intptr_t)strlen(string->c_str(env->state()))) {
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

    String* str = String::create_pinned(env->state(), string, size);
    str->taint(env->state());

    return MemoryHandle::value(str);
  }

  char* rb_str2cstr(VALUE string, long* len) {
    StringValue(string);
    String* str = MemoryHandle::object<String>(string);
    char *ptr = RSTRING_PTR(string);
    if(len) {
      *len = str->byte_size();
    }
    return ptr;
  }

  char* rb_str_ptr(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return MemoryHandle::from(self)->get_rstring(env->state())->ptr;
  }

  char* rb_str_copied_ptr(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = MemoryHandle::object<String>(self);

    char* data = (char*)malloc(sizeof(char) * str->byte_size() + 1);
    if(!data) {
      rb_raise(rb_eSystemCallError, "unable to allocate memory");
    }

    memcpy(data, str->c_str(env->state()), str->byte_size());
    data[str->byte_size()] = 0;

    return data;
  }
#define HAVE_RB_STR_COPIED_PTR 1

  char* rb_str_ptr_readonly(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return MemoryHandle::from(self)->get_rstring(env->state())->ptr;
  }

  char* rb_str_ptr_readonly_end(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    RString* rstr = MemoryHandle::from(self)->get_rstring(env->state());

    return rstr->ptr + rstr->len;
  }

  long rb_str_len(VALUE self) {
    String* string = MemoryHandle::object<String>(self);
    return string->byte_size();
  }

  VALUE rb_str_length(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);
    return LONG2FIX(string->char_size(env->state()));
  }

  void rb_str_set_len(VALUE self, size_t len) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);

    size_t capacity = c_as<ByteArray>(string->data())->size();
    if(len > capacity) len = capacity - 1;

    string->byte_address()[len] = 0;
    string->num_bytes(env->state(), Fixnum::from(len));
  }

  long rb_str_hash(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);

    return string->hash_string(env->state());
  }

  void rb_str_update(VALUE self, long beg, long end, VALUE replacement) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = MemoryHandle::object<String>(self);
    Class* klass = Module::mirror(env->state(), string);

    VALUE mirror = capi_fast_call(MemoryHandle::value(klass), rb_intern("new"), 1, self);
    capi_fast_call(mirror, rb_intern("splice"), 3, LONG2FIX(beg), LONG2FIX(end), replacement);
  }

  VALUE rb_str_equal(VALUE self, VALUE other) {
    if(self == other) {
      return Qtrue;
    }
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* string = MemoryHandle::object<String>(self);
    String* other_str = MemoryHandle::object<String>(other);

    return MemoryHandle::value(string->equal(env->state(), other_str));
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

    String* str = String::create_pinned(env->state(), string, size);
    str->taint(env->state());

    Encoding* enc = Encoding::find(env->state(), encoding->name);
    if(enc == Encoding::usascii_encoding(env->state())
       && !CBOOL(str->ascii_only_p(env->state()))) {
      str->encoding(env->state(), Encoding::ascii8bit_encoding(env->state()));
    } else {
      str->encoding(env->state(), enc);
    }

    return rb_str_conv_enc(MemoryHandle::value(str), enc->encoding(),
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

    String *str = MemoryHandle::object<String>(string);

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

#define RBX_RB_VSPRINTF_LEN 0x10000

  VALUE rb_vsprintf(const char *format, va_list varargs) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    void* buffer = mmap(NULL, RBX_RB_VSPRINTF_LEN,
        PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if(buffer == MAP_FAILED) {
      return MemoryHandle::value(String::create_pinned(env->state(),
            "rb_vsprintf failed to allocate space for result"));
    }

    intptr_t length = vsnprintf((char*)buffer, RBX_RB_VSPRINTF_LEN, format, varargs);
    String* str = String::create_pinned(env->state(), (const char*)buffer, length);

    munmap(buffer, RBX_RB_VSPRINTF_LEN);

    return MemoryHandle::value(str);
  }
}
