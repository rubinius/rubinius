#ifndef RBX_ENCODING_HPP
#define RBX_ENCODING_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

// See comment in regexp.hpp
#ifndef ONIGURUMA_H
struct OnigEncodingType;
struct OnigTranscodingType;
#endif

namespace rubinius {
  class Array;
  class LookupTable;
  class Symbol;
  class String;

  class Encoding : public Object {
  public:
    const static object_type type = EncodingType;

    enum Index {
      eAscii = 0,
      eBinary,
      eUtf8
    };

  private:
    String* name_;    // slot
    Object* dummy_;   // slot

    bool managed_;
    OnigEncodingType* encoding_;

  public:
    attr_accessor(name, String);
    attr_accessor(dummy, Object);

    static void init(STATE);

    static Class* internal_class(STATE);
    static Class* transcoding_class(STATE);
    static LookupTable* encoding_map(STATE);
    static LookupTable* transcoding_map(STATE);
    static Array* encoding_list(STATE);
    static void add_constant(STATE, const char* name, Encoding* enc);

    static Encoding* usascii_encoding(STATE);
    static Encoding* utf8_encoding(STATE);
    static Encoding* ascii8bit_encoding(STATE);
    static Encoding* default_external(STATE);
    static Encoding* default_internal(STATE);

    static Encoding* create_bootstrap(STATE, const char* name,
                                      Index index, OnigEncodingType* enc);
    static void create_internal(STATE, const char* name, int index);
    static Encoding* create(STATE, OnigEncodingType* enc, Object* dummy = cFalse);

    static Encoding* define(STATE, const char* name, OnigEncodingType* enc,
                            Object* dummy = cFalse);
    static Encoding* define_dummy(STATE, const char* name);
    static Encoding* replicate(STATE, const char* name, Encoding* enc);
    static Encoding* replicate(STATE, const char* name, const char* original);
    static Encoding* alias(STATE, const char* name, const char* original);

    static Encoding* from_index(STATE, int index);
    static int find_index(STATE, const char* name);
    static Encoding* find(STATE, const char* name);

    // Rubinius.primitive :encoding_get_object_encoding
    static Encoding* get_object_encoding(STATE, Object* obj);
    static void set_object_encoding(STATE, Object* obj, Encoding* enc);

    OnigEncodingType* get_encoding() {
      return encoding_;
    }

    void set_encoding(OnigEncodingType* enc) {
      encoding_ = enc;
    }

    void make_managed(STATE, const char* name, OnigEncodingType* enc);

    bool get_managed() {
      return managed_;
    }

    void set_managed() {
      managed_ = true;
    }

    // Rubinius.primitive :encoding_compatible_p
    static Encoding* compatible_p(STATE, Object* a, Object* b);

    // Rubinius.primitive :encoding_replicate
    Encoding* replicate(STATE, String* name);

    // Rubinius.primitive :encoding_ascii_compatible_p
    Object* ascii_compatible_p(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
      virtual void show(STATE, Object* self, int level);
    };
  };

  class Converter : public Object {
  public:
    const static object_type type = ConverterType;

  private:
    String* replacement_; // slot

  public:
    attr_accessor(replacement, String);

    static void init(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class Transcoding : public Object {
  public:
    const static object_type type = TranscodingType;

  private:
    String* source_;  // slot
    String* target_;  // slot

    OnigTranscodingType* transcoder_;

  public:
    attr_accessor(source, String);
    attr_accessor(target, String);

    static void init(STATE);

    static Transcoding* create(STATE, OnigTranscodingType* tr);
    static void declare(STATE, const char* from, const char* to, const char* lib);
    static void define(STATE, OnigTranscodingType* tr);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
