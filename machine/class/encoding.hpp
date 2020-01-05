#ifndef RBX_ENCODING_HPP
#define RBX_ENCODING_HPP

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/object.hpp"

#include "util/utf8.h"

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

  public:
    attr_accessor(name, String);
    attr_accessor(dummy, Object);

  private:
    attr_field(encoding, OnigEncodingType*);
    attr_field(index, int);
    attr_field(cache_index, int);
    attr_field(managed, bool);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Encoding* obj);

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
    static Encoding* filesystem_encoding(STATE);

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
    static intptr_t find_non_ascii_index(const uint8_t* start, const uint8_t* end);
    static intptr_t find_byte_character_index(const uint8_t* start, const uint8_t* end, intptr_t index, OnigEncodingType* enc);
    static intptr_t find_byte_character_index_utf8(const uint8_t* start, const uint8_t* end, intptr_t index) {
      return utf8_find_byte_character_index(start, end, index);
    }

    static intptr_t find_character_byte_index(const uint8_t* start, const uint8_t* end, intptr_t index, OnigEncodingType* enc);

    static intptr_t find_character_byte_index_utf8(const uint8_t* start, const uint8_t* end, intptr_t index) {
      return utf8_find_character_byte_index(start, end, index);
    }

    static intptr_t string_character_length(const uint8_t* start, const uint8_t* end, OnigEncodingType* enc);

    static intptr_t string_character_length_utf8(const uint8_t* start, const uint8_t* end) {
      return utf8_string_length(start, end);
    }

    static void string_reverse(uint8_t* start, uint8_t* end, OnigEncodingType* enc);

    static int mbclen(const uint8_t* start, const uint8_t* end, OnigEncodingType* enc);
    static int precise_mbclen(const uint8_t* start, const uint8_t* end, OnigEncodingType* enc);

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

      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
      virtual void before_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
      virtual void after_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
      virtual void visit(STATE, Object* o, std::function<void (STATE, Object**)> f);
      virtual void show(STATE, Object* self, int level);
    };
  };

  class Converter : public Object {
  public:
    const static object_type type = ConverterType;

    attr_accessor(source_encoding, Encoding);
    attr_accessor(destination_encoding, Encoding);
    attr_accessor(replacement, String);
    attr_accessor(convpath, Array);
    attr_accessor(converters, Array);
    attr_accessor(replacement_converters, Array);

  private:
    attr_field(converter, rb_econv_t*);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Converter* obj);

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

    attr_accessor(source, String);
    attr_accessor(target, String);

  private:
    attr_field(transcoder, OnigTranscodingType*);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Transcoding* obj);

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
