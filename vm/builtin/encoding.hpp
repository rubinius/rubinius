#ifndef RBX_ENCODING_HPP
#define RBX_ENCODING_HPP

#include "builtin/object.hpp"

// See comment in regexp.hpp
#ifndef ONIGURUMA_H
struct OnigEncodingType;
struct OnigTranscodingType;
struct rb_econv_t;
#endif

namespace rubinius {
  class Array;
  class LookupTable;
  class Symbol;
  class String;
  class LookupTable;

  class Encoding : public Object {
  public:
    const static object_type type = EncodingType;

    enum Index {
      eBinary = 0,
      eUtf8,
      eAscii
    };

  private:
    String* name_;    // slot
    Object* dummy_;   // slot

    OnigEncodingType* encoding_;
    int cache_index_;
    bool managed_;

  public:
    attr_accessor(name, String);
    attr_accessor(dummy, Object);

    int cache_index() const {
      return cache_index_;
    }

    static void init(STATE);

    static Class* transcoding_class(STATE);
    static Class* converter_class(STATE);
    static LookupTable* encoding_map(STATE);
    static LookupTable* transcoding_map(STATE);
    static Array* encoding_list(STATE);
    static void add_constant(STATE, const char* name, Encoding* enc);

    static Encoding* usascii_encoding(STATE) {
      return G(usascii_encoding);
    }

    static Encoding* utf8_encoding(STATE) {
      return G(utf8_encoding);
    }

    static Encoding* ascii8bit_encoding(STATE) {
      return G(ascii8bit_encoding);
    }

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

    // Rubinius.primitive+ :encoding_get_object_encoding
    static Encoding* get_object_encoding(STATE, Object* obj);
    static void set_object_encoding(STATE, Object* obj, Encoding* enc);

    /**
     * Encoding handling methods that can are used by string but can
     * also be used for implementing certain C-API methods
     */
    static native_int find_non_ascii_index(const uint8_t* start, const uint8_t* end);
    static native_int find_byte_character_index(const uint8_t* start, const uint8_t* end, native_int index, OnigEncodingType* enc);
    static native_int find_character_byte_index(const uint8_t* start, const uint8_t* end, native_int index, OnigEncodingType* enc);

    static int mbclen(const uint8_t* start, const uint8_t* end, OnigEncodingType* enc);
    static int precise_mbclen(const uint8_t* start, const uint8_t* end, OnigEncodingType* enc);

    OnigEncodingType* get_encoding() const {
      return encoding_;
    }

    void set_encoding(OnigEncodingType* enc) {
      encoding_ = enc;
    }

    void make_managed(STATE, const char* name, OnigEncodingType* enc);

    // Rubinius.primitive+ :encoding_compatible_p
    static Encoding* compatible_p(STATE, Object* a, Object* b);

    // Rubinius.primitive :encoding_replicate
    Encoding* replicate(STATE, String* name);

    bool ascii_compatible();
    // Rubinius.primitive+ :encoding_ascii_compatible_p
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
    Encoding* source_encoding_;       // slot
    Encoding* destination_encoding_;  // slot
    String* replacement_;             // slot
    Array* convpath_;                 // slot
    Array* converters_;               // slot
    Array* replacement_converters_;   // slot

    rb_econv_t* converter_;

  public:
    attr_accessor(source_encoding, Encoding);
    attr_accessor(destination_encoding, Encoding);
    attr_accessor(replacement, String);
    attr_accessor(convpath, Array);
    attr_accessor(converters, Array);
    attr_accessor(replacement_converters, Array);

    void set_converter(rb_econv_t* c) {
      converter_ = c;
    }

    rb_econv_t* get_converter() const {
      return converter_;
    }

    static void init(STATE);
    static void finalize(STATE, Converter* converter);

    // Rubinius.primitive :encoding_converter_allocate
    static Converter* allocate(STATE, Object* self);

    // Rubinius.primitive :encoding_converter_primitive_convert
    Symbol* primitive_convert(STATE, Object* source, String* target, Fixnum* offset, Fixnum* size, Fixnum* options);

    // Rubinius.primitive :encoding_converter_finish
    String* finish(STATE);

    // Rubinius.primitive :encoding_converter_last_error
    LookupTable* last_error(STATE);

    // Rubinius.primitive :encoding_converter_primitive_errinfo
    Array* primitive_errinfo(STATE);

    // Rubinius.primitive :encoding_converter_putback
    String* putback(STATE, Object* maxbytes);

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

    OnigTranscodingType* get_transcoder() const {
      return transcoder_;
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
